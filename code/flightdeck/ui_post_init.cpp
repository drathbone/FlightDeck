// ui_post_init.cpp
#include "ui_post_init.h"
#include "lvgl.h"

// Include your generated EEZ header that declares `objects.*`.
// Adjust this include to whatever your project uses (examples below):
//   #include "pages.h"
//   #include "ui.h"
//   #include "eez-project.h"
#include "screens.h"  // TODO: change if your generated header has a different name

// Include the header that declares set_ap_button(...) and mode_state_t / MODE_*.
#include "ui_helpers.h"  // TODO: replace with your actual header

// Safety: only touch objects if they exist.
static inline bool _ok(lv_obj_t* a, lv_obj_t* b) { return a && b; }

void ui_post_init() {
  // Disable the bulk lights buttons for now (grey + non-clickable)
  if (_ok(objects.btn_all_on, objects.lbl_all_on)) {
    set_ap_button(objects.btn_all_on,  objects.lbl_all_on,  MODE_DISABLED);
    lv_obj_clear_flag(objects.btn_all_on,  LV_OBJ_FLAG_CLICKABLE);
  }

  if (_ok(objects.btn_all_off, objects.lbl_all_off)) {
    set_ap_button(objects.btn_all_off, objects.lbl_all_off, MODE_DISABLED);
    lv_obj_clear_flag(objects.btn_all_off, LV_OBJ_FLAG_CLICKABLE);
  }

  // ⤵ Place any other post-init UI tweaks here as you add pages/features.
  // e.g., pre-colour certain indicators, hide dev controls, etc.
}
