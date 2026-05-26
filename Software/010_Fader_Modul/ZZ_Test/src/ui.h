//
// Created by robo on 12.04.26.
//

#ifndef ZZ_TEST_UI_H
#define ZZ_TEST_UI_H

#include <WString.h>
#include <lvgl.h>

#define SECTION_WIDTH 136
#define SECTION_SPACING 4

// Label indices
#define L_ENC_0     0
#define L_ENC_1     1
#define L_FAD_NR    2
#define L_FAD_LAB   3
#define L_FAD_TYPE  4
#define L_FAD_EXCL  5
#define L_FAD_SOL   6
#define L_BUT1_0    7
#define L_BUT1_1    8
#define L_BUT2_0    9
#define L_BUT2_1  10

lv_obj_t * generateLabel(const String& text, int x, int y, lv_style_t& style, lv_obj_t * parent);
lv_obj_t * generateRect(int w, int h , int x, int y, lv_style_t& style);

void setup_simple_ui();

#endif //ZZ_TEST_UI_H