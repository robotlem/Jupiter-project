#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp> // Erzwingt das Laden der RGB-Klasse
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/touch/Touch_GT911.hpp> // GT911 liegt meist im allgemeinen touch-Ordner
#include <lvgl.h>
#define LGFX_USE_V1

class LGFX : public lgfx::LGFX_Device
{
public:

    lgfx::Bus_RGB     _bus_instance;
    lgfx::Panel_RGB   _panel_instance;

    LGFX(void)
    {


        {
            auto cfg = _bus_instance.config();
            cfg.panel = &_panel_instance;

            cfg.pin_d0  = GPIO_NUM_8; // B0
            cfg.pin_d1  = GPIO_NUM_3;  // B1
            cfg.pin_d2  = GPIO_NUM_46;  // B2
            cfg.pin_d3  = GPIO_NUM_9;  // B3
            cfg.pin_d4  = GPIO_NUM_1;  // B4

            cfg.pin_d5  = GPIO_NUM_5;  // G0
            cfg.pin_d6  = GPIO_NUM_6; // G1
            cfg.pin_d7  = GPIO_NUM_7;  // G2
            cfg.pin_d8  = GPIO_NUM_15;  // G3
            cfg.pin_d9  = GPIO_NUM_16; // G4
            cfg.pin_d10 = GPIO_NUM_4;  // G5

            cfg.pin_d11 = GPIO_NUM_45; // R0
            cfg.pin_d12 = GPIO_NUM_48; // R1
            cfg.pin_d13 = GPIO_NUM_47; // R2
            cfg.pin_d14 = GPIO_NUM_21; // R3
            cfg.pin_d15 = GPIO_NUM_14; // R4

            cfg.pin_henable = GPIO_NUM_40;
            cfg.pin_vsync   = GPIO_NUM_41;
            cfg.pin_hsync   = GPIO_NUM_39;
            cfg.pin_pclk    = GPIO_NUM_0;
            cfg.freq_write  = 15000000;

            cfg.hsync_polarity    = 0;
            cfg.hsync_front_porch = 8;
            cfg.hsync_pulse_width = 4;
            cfg.hsync_back_porch  = 43;

            cfg.vsync_polarity    = 0;
            cfg.vsync_front_porch = 8;
            cfg.vsync_pulse_width = 4;
            cfg.vsync_back_porch  = 12;

            cfg.pclk_active_neg   = 1;
            cfg.de_idle_high      = 0;
            cfg.pclk_idle_high    = 0;

            _bus_instance.config(cfg);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.memory_width  = 800;
            cfg.memory_height = 480;
            cfg.panel_width  = 800;
            cfg.panel_height = 480;
            cfg.offset_x = 0;
            cfg.offset_y = 0;



            _panel_instance.config(cfg);
        }
        _panel_instance.setBus(&_bus_instance);
        setPanel(&_panel_instance);

    }
};


LGFX tft;

static const uint32_t screenWidth  = 800;
static const uint32_t screenHeight = 480;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 40]; // Puffer für 40 Zeilen

/* Display-Flush: Übergibt LVGL-Daten an LovyanGFX */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.pushImage(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

/* Touch-Read: Übergibt Touch-Koordinaten an LVGL */
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY)) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
void setup_simple_ui() {
    // 1. Hintergrundfarbe setzen
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0);

    // 2. Text erstellen (Label)
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "CrowPanel 5.0 - Status");

    // Einen Style erstellen
    static lv_style_t style_gross;
    lv_style_init(&style_gross);

    // Schriftart setzen (Voraussetzung: in lv_conf.h aktiviert)
    lv_style_set_text_font(&style_gross, &lv_font_montserrat_48);
    lv_style_set_text_color(&style_gross, lv_color_hex(0xFFFFFF));
    // Style dem Label hinzufügen
    lv_obj_add_style(label, &style_gross, 0);

    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // 3. Eine Linie zeichnen
    static lv_point_t line_points[] = { {0, 0}, {400, 0} }; // Start- und Endpunkt
    lv_obj_t * line = lv_line_create(lv_scr_act());
    lv_line_set_points(line, line_points, 2);     // Punkte zuweisen
    lv_obj_set_style_line_width(line, 3, 0);      // 3px dick
    lv_obj_set_style_line_color(line, lv_color_hex(0x00FF00), 0); // Grün
    lv_obj_align(line, LV_ALIGN_CENTER, 0, 0);    // In der Mitte platzieren
}
void setup() {
    Serial.begin(115200);
    delay(1000); // Zeit für Serial Monitor
    Serial.println("Initialisiere CrowPanel...");


    if (!tft.begin()) Serial.println("LGFX Fehler!");
    tft.setBrightness(255);
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

    // Backlight Pins für 5.0" Modell
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);

    delay(2000);
    setup_simple_ui();
    Serial.println("Setup fertig!");
}

void loop() {
    lv_timer_handler();
    delay(5);
}



