/* Display Klasse für das Elecrow 5" HMI Touchdisplay
 * https://www.elecrow.com/esp32-display-5-inch-hmi-display-rgb-tft-lcd-touch-screen-support-lvgl.html
 *
 *
 *
 */

#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp> // Erzwingt das Laden der RGB-Klasse
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/touch/Touch_GT911.hpp> // GT911 liegt meist im allgemeinen touch-Ordner
#define LGFX_USE_V1

// Display Class
class LGFX : public lgfx::LGFX_Device
{
public:

    lgfx::Bus_RGB     _bus_instance;
    lgfx::Panel_RGB   _panel_instance;
    lgfx::Light_PWM   _light_instance;

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
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = GPIO_NUM_2;
            _light_instance.config(cfg);
        }
        _panel_instance.light(&_light_instance);
        _panel_instance.setBus(&_bus_instance);
        setPanel(&_panel_instance);

    }
};

// Display instance
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