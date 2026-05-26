//
// Created by robo on 12.04.26.
//
#include "ui.h"

lv_obj_t * generateLabel(const String& text, int x, int y, lv_style_t& style, lv_obj_t * parent, lv_align_t align) {
  lv_obj_t * label = lv_label_create(parent);
  lv_label_set_text(label, text.c_str());
  lv_obj_add_style(label, &style, 0);

  lv_obj_align(label, align, x, y);
  return label;
}


lv_obj_t * generateRect(int w, int h , int x, int y, lv_style_t& style) {
  lv_obj_t * rect = lv_obj_create(lv_scr_act());

  // 2. Größe und Position festlegen
  lv_obj_set_size(rect, w, h);
  lv_obj_add_style(rect, &style, 0);
  lv_obj_align(rect, LV_ALIGN_TOP_LEFT, x, y);
  lv_obj_set_scrollbar_mode(rect, LV_SCROLLBAR_MODE_OFF); // Scrollbars erzwingt aus
  lv_obj_clear_flag(rect, LV_OBJ_FLAG_SCROLLABLE);       // Scrollen komplett deaktivieren

  // Optional: Padding auf 0 setzen, falls du später Dinge darin platzieren willst
  lv_obj_set_style_pad_all(rect, 0, 0);
  return rect;
}

void setup_simple_ui() {
  // 1. Hintergrundfarbe setzen
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0);

  static lv_style_t text_14;
  lv_style_init(&text_14);
  lv_style_set_text_font(&text_14, &lv_font_montserrat_14);
  lv_style_set_text_color(&text_14, lv_color_hex(0xFFFFFF));

  static lv_style_t text_20;
  lv_style_init(&text_20);
  lv_style_set_text_font(&text_20, &lv_font_montserrat_20);
  lv_style_set_text_color(&text_20, lv_color_hex(0xFFFFFF));

  static lv_style_t text_24;
  lv_style_init(&text_24);
  lv_style_set_text_font(&text_24, &lv_font_montserrat_24);
  lv_style_set_text_color(&text_24, lv_color_hex(0xFFFFFF));

  static lv_style_t text_24_black;
  lv_style_init(&text_24_black);
  lv_style_set_text_font(&text_24_black, &lv_font_montserrat_24);
  lv_style_set_text_color(&text_24_black, lv_color_hex(0x000000));

  static lv_style_t text_32;
  lv_style_init(&text_32);
  lv_style_set_text_font(&text_32, &lv_font_montserrat_32);
  lv_style_set_text_color(&text_32, lv_color_hex(0xFFFFFF));

  //generateLabel("14 px", 0, 48, text_14, lv_scr_act());
  //generateLabel("24 px", 0, 62, text_24, lv_scr_act());
  //generateLabel("32 px", 0, 86, text_32, lv_scr_act());

  // Rechtecke
  static lv_style_t style_rect_gold;
  lv_style_init(&style_rect_gold);
  lv_style_set_bg_color(&style_rect_gold, lv_color_hex(0x000000));
  lv_style_set_radius(&style_rect_gold, 0);
  lv_style_set_border_width(&style_rect_gold, 2);
  lv_style_set_border_color(&style_rect_gold, lv_color_hex(0xffe522));

  static lv_style_t style_rect_gold_filled;
  lv_style_init(&style_rect_gold_filled);
  lv_style_set_bg_color(&style_rect_gold_filled, lv_color_hex(0xffe522));
  lv_style_set_radius(&style_rect_gold_filled, 0);
  lv_style_set_border_width(&style_rect_gold_filled, 2);
  lv_style_set_border_color(&style_rect_gold_filled, lv_color_hex(0xffe522));

  static lv_style_t style_rect_red;
  lv_style_init(&style_rect_red);
  lv_style_set_bg_color(&style_rect_red, lv_color_hex(0x000000));
  lv_style_set_radius(&style_rect_red, 0);
  lv_style_set_border_width(&style_rect_red, 2);
  lv_style_set_border_color(&style_rect_red, lv_color_hex(0xd40004));

  static lv_style_t style_rect_red_filled;
  lv_style_init(&style_rect_red_filled);
  lv_style_set_bg_color(&style_rect_red_filled, lv_color_hex(0xd40004));
  lv_style_set_radius(&style_rect_red_filled, 0);
  lv_style_set_border_width(&style_rect_red_filled, 2);
  lv_style_set_border_color(&style_rect_red_filled, lv_color_hex(0xd40004));

  static lv_style_t style_rect_grey;
  lv_style_init(&style_rect_grey);
  lv_style_set_bg_color(&style_rect_grey, lv_color_hex(0x000000));
  lv_style_set_radius(&style_rect_grey, 0);
  lv_style_set_border_width(&style_rect_grey, 2);
  lv_style_set_border_color(&style_rect_grey, lv_color_hex(0x5f5f5f));

  static lv_style_t style_rect_grey_filled;
  lv_style_init(&style_rect_grey_filled);
  lv_style_set_bg_color(&style_rect_grey_filled, lv_color_hex(0x5f5f5f));
  lv_style_set_radius(&style_rect_grey_filled, 0);
  lv_style_set_border_width(&style_rect_grey_filled, 2);
  lv_style_set_border_color(&style_rect_grey_filled, lv_color_hex(0x5f5f5f));

  // Fader Sections
  static lv_obj_t * f_sections[2][5]; // Border, Top-Part (filled)
  // Button Sections
  static lv_obj_t * b_sections[3][5]; // Encoder, Fire, Stop
  // Labels
  static lv_obj_t * l_sections[11][5];

  char texts_nr[5][4] = {"#01", "#02", "#03", "#04", "#05" };

  for (int i = 0; i < 5; i++) {
    // Draw the section rects
    f_sections[0][i] = generateRect(SECTION_WIDTH, 318,800-SECTION_WIDTH*(5-i)-SECTION_SPACING*(4-i),54, style_rect_grey);
    f_sections[1][i] = generateRect(SECTION_WIDTH, 28,800-SECTION_WIDTH*(5-i)-SECTION_SPACING*(4-i),54, style_rect_grey_filled);
    b_sections[0][i] = generateRect(SECTION_WIDTH, 54,800-SECTION_WIDTH*(5-i)-SECTION_SPACING*(4-i),0, style_rect_grey);
    b_sections[1][i] = generateRect(SECTION_WIDTH, 54,800-SECTION_WIDTH*(5-i)-SECTION_SPACING*(4-i),372, style_rect_grey);
    b_sections[2][i] = generateRect(SECTION_WIDTH, 54,800-SECTION_WIDTH*(5-i)-SECTION_SPACING*(4-i),426, style_rect_grey);

    // Add Labels
    l_sections[L_ENC_0][i] = generateLabel("Encoder", 1,1,text_20, b_sections[0][i], LV_ALIGN_TOP_LEFT);
    l_sections[L_ENC_1][i] = generateLabel("2nd Line", 0,23,text_20, b_sections[0][i], LV_ALIGN_TOP_MID);

    l_sections[L_FAD_NR][i] = generateLabel(texts_nr[i], 0,-1,text_24_black, f_sections[1][i], LV_ALIGN_TOP_LEFT);
    lv_obj_center(l_sections[L_FAD_NR][i]);

    l_sections[L_BUT1_0][i] = generateLabel("Button", 1,1,text_20, b_sections[1][i], LV_ALIGN_TOP_LEFT);
    l_sections[L_BUT1_1][i] = generateLabel("Fire", 0,23,text_20, b_sections[1][i], LV_ALIGN_TOP_MID);

    l_sections[L_BUT2_0][i] = generateLabel("Button", 1,1,text_20, b_sections[2][i], LV_ALIGN_TOP_LEFT);
    l_sections[L_BUT2_1][i] = generateLabel("Stop", 0,23,text_20, b_sections[2][i], LV_ALIGN_TOP_MID);


  }

  lv_obj_add_style(f_sections[0][1], &style_rect_gold, 0);
  lv_obj_add_style(f_sections[1][1], &style_rect_gold_filled, 0);
  lv_obj_add_style(f_sections[0][2], &style_rect_red, 0);
  lv_obj_add_style(f_sections[1][2], &style_rect_red_filled, 0);

  lv_label_set_text(l_sections[L_BUT2_0][2], "");




}
