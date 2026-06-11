//
// Created by robo on 12.04.26.
//

#ifndef ZZ_TEST_UI_H
#define ZZ_TEST_UI_H

#include <lvgl.h>

#define SECTION_WIDTH 136
#define SECTION_SPACING 4

#define NUM_LABELS      80
#define MAX_LABEL_LEN   24

// Label indices
#define L_ENC_0     0
#define L_ENC_1     1
#define L_FAD_NR    2
#define L_FAD_0     3
#define L_FAD_1     4
#define L_FAD_2     5
#define L_FAD_3     6
#define L_FAD_4     7
#define L_FAD_5     8
#define L_FAD_6     9
#define L_FAD_7     10
#define L_FAD_8     11
#define L_BUT1_0    12
#define L_BUT1_1    13
#define L_BUT2_0    14
#define L_BUT2_1    15

struct LabelData {
    lv_obj_t *label;
    char text[MAX_LABEL_LEN];
    bool dirty;
};

void generateLabel(struct LabelData *section, int x, int y, lv_style_t& style, lv_obj_t * parent);
static lv_obj_t * generateRect(int w, int h , int x, int y, lv_style_t& style);

void setup_simple_ui();
void update_label(uint8_t id, const char* newText);
void update_screen();

#endif //ZZ_TEST_UI_H