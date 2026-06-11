#include <Arduino.h>
#include "display.hpp"
#include <lvgl.h>
#include "ui.h"
#include "driver/twai.h"
#include <CAN_IDs.h>
#include "can_communication.h"



void setup() {
  Serial0.begin(115200);

  // Driver init
  lcd.init();
  lcd.setRotation(2);

  // Setup CAN

  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();  // Baudrate anpassen
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
    Serial0.println("TWAI install failed!");
    return;
  }
  if (twai_start() != ESP_OK) {
    Serial0.println("TWAI start failed!");
    return;
  }
  Serial0.println("CAN ready!");

  // LVGL Part
  setupLVGL();

  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0);

  LV_IMG_DECLARE(Jupiter);
  lv_obj_t * img = lv_img_create(lv_scr_act());
  lv_img_set_src(img, &Jupiter);

  lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

  lv_timer_handler();

  delay(1500);
  lv_obj_del(img);

  setup_simple_ui();

  lv_timer_handler();
}

void loop() {
  lv_timer_handler();


  // Periodisch can Senden
  static unsigned long lastCanMessage = 0;
  if (lastCanMessage + HEARTBEAT_INTERVALL < millis()) {
    sendHeartbeat();
    lastCanMessage = millis();
  }

  // Can Empfangen
  processIncomingCan();

}