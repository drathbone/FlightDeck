#pragma once
#include "lvgl.h"
#include "ui_generated_c.h"
#include <stdint.h>

// General UI state used for coloring controls across the UI
enum class ui_state_t : uint8_t {
    Disabled = 0,
    Off      = 1,
    Armed    = 2,
    Active   = 3
};

// Backward-compat: old name used in earlier code
using mode_state_t = ui_state_t;

// Apply state styling to a container outline and label text color.
// Implementation is in ui_helpers.cpp
void ui_set_state(lv_obj_t* container, lv_obj_t* label, ui_state_t state);

// Backward-compat wrapper: prefer ui_set_state(...)
static inline void set_ap_button(lv_obj_t* container, lv_obj_t* label, ui_state_t state) {
    ui_set_state(container, label, state);
}
