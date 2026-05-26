#include <Arduino.h>
#include <lvgl.h>
#include "display.hpp"

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
void setup() {
    Serial.begin(115200);
    delay(1000); // Zeit für Serial Monitor
    Serial.println("Initialisiere CrowPanel...");


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
    delay(5);
}



