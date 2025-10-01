#pragma once
#include "lvgl.h"

// Keep only what ui_post_init.cpp needs:
typedef enum {
    MODE_DISABLED = 0,
    MODE_OFF      = 1,
    MODE_ARMED    = 2,
    MODE_ACTIVE   = 3
} mode_state_t;

// Implemented in flightdeck.ino
void set_ap_button(lv_obj_t* container, lv_obj_t* label, mode_state_t state);