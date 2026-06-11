//
// Created by robo on 28.05.26.
//

#include "ui.h"
#include <lvgl.h>
#include <cstring>
#include <Arduino.h>

// ---------------------------
// Styles
// ---------------------------
lv_style_t text_14;
lv_style_t text_20;
lv_style_t text_24;
lv_style_t text_24_black;
lv_style_t text_32;

lv_style_t style_rect_gold;
lv_style_t style_rect_gold_filled;
lv_style_t style_rect_red;
lv_style_t style_rect_red_filled;
lv_style_t style_rect_grey;
lv_style_t style_rect_grey_filled;

// ---------------------------
// UI Objects
// ---------------------------
lv_obj_t* f_sections[5][2];
lv_obj_t* b_sections[5][3];

LabelData l_sections[5][16];

// ---------------------------
// Helper: safe copy
// ---------------------------
static void safe_copy(char* dst, const char* src, size_t len)
{
    strncpy(dst, src, len);
    dst[len - 1] = '\0';
}

// ---------------------------
// Label creation
// ---------------------------
static void generateLabel(LabelData* section,
                          int x, int y,
                          lv_style_t& style,
                          lv_obj_t* parent,
                          lv_align_t align)
{
    lv_obj_t* label = lv_label_create(parent);

    lv_label_set_text_static(label, section->text);
    lv_obj_add_style(label, &style, 0);
    lv_obj_align(label, align, x, y);

    section->label = label;
    section->dirty = false;
}

// ---------------------------
// Rectangle helper
// ---------------------------
static lv_obj_t* generateRect(int w, int h, int x, int y, lv_style_t& style)
{
    lv_obj_t* rect = lv_obj_create(lv_scr_act());

    lv_obj_set_size(rect, w, h);
    lv_obj_add_style(rect, &style, 0);
    lv_obj_align(rect, LV_ALIGN_TOP_LEFT, x, y);

    lv_obj_clear_flag(rect, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(rect, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(rect, 0, 0);

    return rect;
}

// ---------------------------
// UI Setup
// ---------------------------
void setup_simple_ui()
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0);

    // Fonts
    lv_style_init(&text_20);
    lv_style_set_text_font(&text_20, &lv_font_montserrat_20);
    lv_style_set_text_color(&text_20, lv_color_hex(0xFFFFFF));

    lv_style_init(&text_24);
    lv_style_set_text_font(&text_24, &lv_font_montserrat_24);
    lv_style_set_text_color(&text_24, lv_color_hex(0xFFFFFF));

    lv_style_init(&text_24_black);
    lv_style_set_text_font(&text_24_black, &lv_font_montserrat_24);
    lv_style_set_text_color(&text_24_black, lv_color_hex(0x000000));

    // Rechtecke
    lv_style_init(&style_rect_gold);
    lv_style_set_bg_color(&style_rect_gold, lv_color_hex(0x000000));
    lv_style_set_radius(&style_rect_gold, 0);
    lv_style_set_border_width(&style_rect_gold, 2);
    lv_style_set_border_color(&style_rect_gold, lv_color_hex(0xffe522));

    lv_style_init(&style_rect_gold_filled);
    lv_style_set_bg_color(&style_rect_gold_filled, lv_color_hex(0xffe522));
    lv_style_set_radius(&style_rect_gold_filled, 0);
    lv_style_set_border_width(&style_rect_gold_filled, 2);
    lv_style_set_border_color(&style_rect_gold_filled, lv_color_hex(0xffe522));

    lv_style_init(&style_rect_red);
    lv_style_set_bg_color(&style_rect_red, lv_color_hex(0x000000));
    lv_style_set_radius(&style_rect_red, 0);
    lv_style_set_border_width(&style_rect_red, 2);
    lv_style_set_border_color(&style_rect_red, lv_color_hex(0xd40004));

    lv_style_init(&style_rect_red_filled);
    lv_style_set_bg_color(&style_rect_red_filled, lv_color_hex(0xd40004));
    lv_style_set_radius(&style_rect_red_filled, 0);
    lv_style_set_border_width(&style_rect_red_filled, 2);
    lv_style_set_border_color(&style_rect_red_filled, lv_color_hex(0xd40004));

    lv_style_init(&style_rect_grey);
    lv_style_set_bg_color(&style_rect_grey, lv_color_hex(0x000000));
    lv_style_set_radius(&style_rect_grey, 0);
    lv_style_set_border_width(&style_rect_grey, 2);
    lv_style_set_border_color(&style_rect_grey, lv_color_hex(0x5f5f5f));

    lv_style_init(&style_rect_grey_filled);
    lv_style_set_bg_color(&style_rect_grey_filled, lv_color_hex(0x5f5f5f));
    lv_style_set_radius(&style_rect_grey_filled, 0);
    lv_style_set_border_width(&style_rect_grey_filled, 2);
    lv_style_set_border_color(&style_rect_grey_filled, lv_color_hex(0x5f5f5f));

    const char texts_nr[5][4] = {"#01", "#02", "#03", "#04", "#05"};

    for (int i = 0; i < 5; i++)
    {
        // Draw the section rects
        f_sections[i][0] = generateRect(SECTION_WIDTH, 318,800-SECTION_WIDTH*(5-i)-SECTION_SPACING*(4-i),54, style_rect_grey);
        f_sections[i][1] = generateRect(SECTION_WIDTH, 28,800-SECTION_WIDTH*(5-i)-SECTION_SPACING*(4-i),54, style_rect_grey_filled);
        b_sections[i][0] = generateRect(SECTION_WIDTH, 54,800-SECTION_WIDTH*(5-i)-SECTION_SPACING*(4-i),0, style_rect_grey);
        b_sections[i][1] = generateRect(SECTION_WIDTH, 54,800-SECTION_WIDTH*(5-i)-SECTION_SPACING*(4-i),372, style_rect_grey);
        b_sections[i][2] = generateRect(SECTION_WIDTH, 54,800-SECTION_WIDTH*(5-i)-SECTION_SPACING*(4-i),426, style_rect_grey);

        // Encoder          0xX0, 0xX1
        safe_copy(l_sections[i][L_ENC_0].text, "Encoder", 16);
        generateLabel(&l_sections[i][L_ENC_0], 1,1,text_20, b_sections[i][0], LV_ALIGN_TOP_LEFT);
        safe_copy(l_sections[i][L_ENC_1].text, "2nd Line", 16);
        generateLabel(&l_sections[i][L_ENC_1], 0, 23, text_20, b_sections[i][0], LV_ALIGN_TOP_MID);

        // Fader nr         0xX2
        safe_copy(l_sections[i][L_FAD_NR].text, texts_nr[i], 4);
        generateLabel(&l_sections[i][L_FAD_NR], 0, -1, text_24_black, f_sections[i][1], LV_ALIGN_TOP_LEFT);
        lv_obj_center(l_sections[i][L_FAD_NR].label);

        // Fader content    0xX3, 0xX4, 0xX5, 0xX6, 0xX7, 0xX8, 0xX9, 0xXA, 0xXB
        safe_copy(l_sections[i][L_FAD_0].text, "0xX3", 16);
        generateLabel(&l_sections[i][L_FAD_0], 1,25,text_20, f_sections[i][0], LV_ALIGN_TOP_LEFT);

        safe_copy(l_sections[i][L_FAD_1].text, "0xX4", 16);
        generateLabel(&l_sections[i][L_FAD_1], 1,25+1*22,text_20, f_sections[i][0], LV_ALIGN_TOP_LEFT);

        safe_copy(l_sections[i][L_FAD_2].text, "0xX5", 16);
        generateLabel(&l_sections[i][L_FAD_2], 1,25+2*22,text_20, f_sections[i][0], LV_ALIGN_TOP_LEFT);

        safe_copy(l_sections[i][L_FAD_3].text, "0xX6", 16);
        generateLabel(&l_sections[i][L_FAD_3], 1,25+3*22,text_20, f_sections[i][0], LV_ALIGN_TOP_LEFT);

        safe_copy(l_sections[i][L_FAD_4].text, "0xX7", 16);
        generateLabel(&l_sections[i][L_FAD_4], 1,25+4*22,text_20, f_sections[i][0], LV_ALIGN_TOP_LEFT);

        safe_copy(l_sections[i][L_FAD_5].text, "0xX8", 16);
        generateLabel(&l_sections[i][L_FAD_5], 1,25+5*22,text_20, f_sections[i][0], LV_ALIGN_TOP_LEFT);

        safe_copy(l_sections[i][L_FAD_6].text, "0xX9", 16);
        generateLabel(&l_sections[i][L_FAD_6], 1,25+6*22,text_20, f_sections[i][0], LV_ALIGN_TOP_LEFT);

        safe_copy(l_sections[i][L_FAD_7].text, "0xXA", 16);
        generateLabel(&l_sections[i][L_FAD_7], 1,25+10*22,text_20, f_sections[i][0], LV_ALIGN_TOP_LEFT);

        safe_copy(l_sections[i][L_FAD_8].text, "0xXB", 16);
        generateLabel(&l_sections[i][L_FAD_8], 0,25+11*22,text_20, f_sections[i][0], LV_ALIGN_TOP_MID);

        // Button 1         0xXC, 0xXD
        safe_copy(l_sections[i][L_BUT1_0].text, "Button", 16);
        generateLabel(&l_sections[i][L_BUT1_0], 1, 1, text_20, b_sections[i][1], LV_ALIGN_TOP_LEFT);
        safe_copy(l_sections[i][L_BUT1_1].text, "Fire", 16);
        generateLabel(&l_sections[i][L_BUT1_1], 0, 23, text_20, b_sections[i][1], LV_ALIGN_TOP_MID);

        // Button 2         0xXE, 0xXF
        safe_copy(l_sections[i][L_BUT2_0].text, "Button", 16);
        generateLabel(&l_sections[i][L_BUT2_0], 1, 1, text_20, b_sections[i][2], LV_ALIGN_TOP_LEFT);
        safe_copy(l_sections[i][L_BUT2_1].text, "Stop", 16);
        generateLabel(&l_sections[i][L_BUT2_1], 0, 23, text_20, b_sections[i][2], LV_ALIGN_TOP_MID);
    }

    lv_obj_add_style(f_sections[1][0], &style_rect_gold, 0);
    lv_obj_add_style(f_sections[1][1], &style_rect_gold_filled, 0);
    lv_obj_add_style(f_sections[2][0], &style_rect_red, 0);
    lv_obj_add_style(f_sections[2][1], &style_rect_red_filled, 0);

}

// ---------------------------
// Update single label
// ---------------------------
void update_label(uint8_t id, const char* newText)
{
    if (id > 0x4F) return;

    size_t section = id / 16;
    size_t row = id % 16;

    safe_copy(l_sections[section][row].text, newText, MAX_LABEL_LEN);

    l_sections[section][row].dirty = true;
}

// ---------------------------
// Screen update
// ---------------------------
void update_screen()
{
    for (int i = 0; i < 0x50; i++)
    {
        auto& l = l_sections[i / 16][i % 16];

        if (l.dirty)
        {
            lv_label_set_text_static(l.label, l.text);
            l.dirty = false;
        }
    }
}