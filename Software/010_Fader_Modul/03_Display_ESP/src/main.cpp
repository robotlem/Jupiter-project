#include <Arduino.h>
#include <lvgl.h>
#include "display.hpp"
#include "driver/twai.h"

#define CAN_TX_PIN GPIO_NUM_17
#define CAN_RX_PIN GPIO_NUM_18

void setup_simple_ui() {
    // 1. Hintergrundfarbe setzen
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x272727), 0);

    // 2. Text erstellen (Label)
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hallo Welt");

    // Einen Style erstellen
    static lv_style_t style_gross;
    lv_style_init(&style_gross);

    // Schriftart setzen (Voraussetzung: in lv_conf.h aktiviert)
    lv_style_set_text_font(&style_gross, &lv_font_montserrat_48);
    lv_style_set_text_color(&style_gross, lv_color_hex(0xFFFFFF));
    lv_obj_add_style(label, &style_gross, 0);

    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    // 3. Eine Linie zeichnen
    static lv_point_t line_points[] = { {0, 0}, {400, 0} }; // Start- und Endpunkt
    lv_obj_t * line = lv_line_create(lv_scr_act());
    lv_line_set_points(line, line_points, 2);     // Punkte zuweisen
    lv_obj_set_style_line_width(line, 3, 0);      // 3px dick
    lv_obj_set_style_line_color(line, lv_color_hex(0xffe522), 0); // Grün
    lv_obj_align(line, LV_ALIGN_CENTER, 0, 0);    // In der Mitte platzieren
}

void sendCan() {
    twai_message_t msg;
    msg.identifier = 0x123;      // CAN-ID
    msg.extd = 0;                // 0 = Standard Frame (11bit), 1 = Extended (29bit)
    msg.data_length_code = 4;    // Anzahl Datenbytes (0-8)
    msg.data[0] = 0xDE;
    msg.data[1] = 0xAD;
    msg.data[2] = 0xBE;
    msg.data[3] = 0xEF;

    twai_transmit(&msg, pdMS_TO_TICKS(1000));  // 1000ms Timeout
}

void setup() {
    Serial.begin(115200);
    delay(1000); // Zeit für Serial Monitor
    Serial.println("Initialisiere CrowPanel...");

    // Setup CAN

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();  // Baudrate anpassen
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
        Serial.println("TWAI install failed!");
        return;
    }
    if (twai_start() != ESP_OK) {
        Serial.println("TWAI start failed!");
        return;
    }
    Serial.println("CAN ready!");

    // Setup Display
    if (!tft.begin()) Serial.println("LGFX Fehler!");
    tft.setRotation(2);
    tft.fillScreen(TFT_RED); // Das MUSS das Display jetzt rot machen!

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 40);


    // Display-Treiber für LVGL registrieren
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);


    tft.setBrightness(255);
    delay(2000);
    setup_simple_ui();
    Serial.println("Setup fertig!");
}

void loop() {
    lv_timer_handler();

    // Periodisch can Senden
    static unsigned long lastCanMessage = 0;
    if (lastCanMessage + 2000 < millis()) {
        sendCan();
        lastCanMessage = millis();
    }

    // Can Empfangen
    twai_message_t msg;
    if (twai_receive(&msg, pdMS_TO_TICKS(1000)) == ESP_OK) {
        Serial.printf("ID: 0x%03X  DLC: %d  Data: ", msg.identifier, msg.data_length_code);
        for (int i = 0; i < msg.data_length_code; i++) {
            Serial.printf("%02X ", msg.data[i]);
        }
        Serial.println();
    }



    delay(5);
}



