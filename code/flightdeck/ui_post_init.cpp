// ui_post_init.cpp
#include "ui_post_init.h"
#include "ui_generated_c.h"
#include "ui_capi.h"
#include "lvgl.h"

// Include your generated EEZ header that declares `objects.*`.
// Adjust this include to whatever your project uses (examples below):
//   #include "pages.h"
//   //   #include "eez-project.h"
// TODO: change if your generated header has a different name
// Include the header that declares ui_set_state(...) and ui_state_t / MODE_*.
#include "ui_helpers.h"  // TODO: replace with your actual header

// Safety: only touch objects if they exist.
static inline bool _ok(lv_obj_t* a, lv_obj_t* b) { return a && b; }

void ui_post_init() {
  // Disable the bulk lights buttons for now (grey + non-clickable)
  if (_ok(objects.btn_all_on, objects.lbl_all_on)) {
    ui_set_state(objects.btn_all_on,  objects.lbl_all_on,  ui_state_t::Disabled);
    lv_obj_clear_flag(objects.btn_all_on,  LV_OBJ_FLAG_CLICKABLE);
  }

  if (_ok(objects.btn_all_off, objects.lbl_all_off)) {
    ui_set_state(objects.btn_all_off, objects.lbl_all_off, ui_state_t::Disabled);
    lv_obj_clear_flag(objects.btn_all_off, LV_OBJ_FLAG_CLICKABLE);
  }

  // ⤵ Place any other post-init UI tweaks here as you add pages/features.
  // e.g., pre-colour certain indicators, hide dev controls, etc.
}