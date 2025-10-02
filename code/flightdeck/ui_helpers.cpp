#include "ui_capi.h"
#include "ui_generated_c.h"
#include "ui_helpers.h"

static inline lv_color_t col_disabled(){ return lv_color_hex(0x94A3B8); } // grey - not available
static inline lv_color_t col_off()     { return lv_color_hex(0x22D3EE); } // cyan - available/off
static inline lv_color_t col_armed()   { return lv_color_hex(0xEF4444); } // red - armed
static inline lv_color_t col_active()  { return lv_color_hex(0xF59E0B); } // amber - active/on

void ui_set_state(lv_obj_t* container, lv_obj_t* label, ui_state_t state) {
    lv_color_t c =
        (state == ui_state_t::Active) ? col_active() :
        (state == ui_state_t::Armed)  ? col_armed()  :
        (state == ui_state_t::Off)    ? col_off()    :
                                          col_disabled();

    if (container) lv_obj_set_style_border_color(container, c, LV_PART_MAIN);
    if (label)     lv_obj_set_style_text_color(label, c, LV_PART_MAIN);
}