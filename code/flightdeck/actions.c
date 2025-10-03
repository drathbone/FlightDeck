#include "spad_actions.h"
#include "ui_generated_c.h"
// actions.c
#include "lvgl.h"
#include <stdbool.h>

// --- Universal LVGL event guard (dedupe + stop bubbling) ---
#include "config.h"

extern void spad_force_rescan_caps(void);

typedef struct {
    lv_obj_t*       last_target;
    lv_event_code_t last_code;
    uint32_t        last_tick;
} ui_guard_state_t;
static ui_guard_state_t __ui_guard = {0};

static inline bool ui_guard(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    uint32_t now = lv_tick_get();

    if (target == __ui_guard.last_target &&
        code   == __ui_guard.last_code) {
        uint32_t dt = lv_tick_elaps(__ui_guard.last_tick);
        if (dt < UI_DEDUP_WINDOW_MS) {
            return false; // drop duplicate within debounce window
        }
    }
    __ui_guard.last_target = target;
    __ui_guard.last_code   = code;
    __ui_guard.last_tick   = now;

    lv_event_stop_bubbling(e); // prevent parent handlers re-firing
    return true;
}
// --- End guard ---

// Tunables
#ifndef UI_DEDUP_WINDOW_MS
#define UI_DEDUP_WINDOW_MS 180   // ignore repeats within this window per target
#endif

// Define colours for changing YES/NO highlights for aircraft capabilities
#define CAP_COL_YES lv_color_hex(0x22D3EE)  // cyan (highlight)
#define CAP_COL_NO  lv_color_hex(0x0F172A)  // slate (dim)

// Global deduper state (per-target)
typedef struct {
    lv_obj_t*       last_target;
    lv_event_code_t last_code;
    uint32_t        last_tick;
} ui_dedupe_t;

static ui_dedupe_t g_ui = {0};

static inline bool ui_event_guard(lv_event_t* e) {
    // Only dedupe "action" events; other events pass through.
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_CLICKED && code != LV_EVENT_VALUE_CHANGED) {
        return true;
    }

    lv_obj_t* target = lv_event_get_target(e);
    uint32_t  now    = lv_tick_get();

    // If same target + same event code within window → drop
    if (target == g_ui.last_target &&
        code   == g_ui.last_code &&
        lv_tick_elaps(g_ui.last_tick) < UI_DEDUP_WINDOW_MS) {
        return false;
    }

    // Update dedupe state
    g_ui.last_target = target;
    g_ui.last_code   = code;
    g_ui.last_tick   = now;

    // Stop propagation so parents/containers don’t re-trigger handlers
    lv_event_stop_bubbling(e);
    lv_event_stop_processing(e);
    return true;
}

static inline void _cap_label_toggle(lv_obj_t* yes, lv_obj_t* no, bool has) {
    lv_color_t colYes = has ? CAP_COL_YES : CAP_COL_NO;
    lv_color_t colNo  = has ? CAP_COL_NO  : CAP_COL_YES;

    lv_obj_set_style_outline_color(yes, colYes, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(no,  colNo,  LV_PART_MAIN | LV_STATE_DEFAULT);
    // If outline width isn’t set in EEZ, uncomment:
    // lv_obj_set_style_outline_width(yes, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_outline_width(no,  2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_invalidate(yes);
    lv_obj_invalidate(no);
}

// ---- C-callable bridges (called from C++)
void ui_set_capability_pushback(int has) {
    _cap_label_toggle(objects.lbl_cap_pushback_yes, objects.lbl_cap_pushback_no, has != 0);
}
void ui_set_capability_gpu(int has) {
    _cap_label_toggle(objects.lbl_cap_gpu_yes, objects.lbl_cap_gpu_no, has != 0);
}
void ui_set_capability_ground_deice(int has) {
    _cap_label_toggle(objects.lbl_cap_deice_yes, objects.lbl_cap_deice_no, has != 0);
}
void ui_set_capability_antiice(int has) {
    _cap_label_toggle(objects.lbl_cap_antiice_yes, objects.lbl_cap_antiice_no, has != 0);
}
void ui_set_capability_autostart(int has) {
    _cap_label_toggle(objects.lbl_cap_autostart_yes, objects.lbl_cap_autostart_no, has != 0);
}
void ui_set_capability_exits(int has) {
    _cap_label_toggle(objects.lbl_cap_exits_yes, objects.lbl_cap_exits_no, has != 0);
}

// Functions for Home Page buttons
void action_e_home(lv_event_t * e) {
  if (!ui_event_guard(e)) return;
  loadScreen(SCREEN_ID_PG_HOME);
}

void action_e_radios(lv_event_t * e) {
  if (!ui_event_guard(e)) return;
  loadScreen(SCREEN_ID_PG_RADIOS);
}

void action_e_autopilot(lv_event_t * e) {
  if (!ui_event_guard(e)) return;
  loadScreen(SCREEN_ID_PG_AUTOPILOT);
}
void action_e_sim_controls(lv_event_t * e) {
  if (!ui_event_guard(e)) return;
  loadScreen(SCREEN_ID_PG_SIM_CONTROLS);
}
void action_e_ground(lv_event_t * e) {
  if (!ui_event_guard(e)) return;
  loadScreen(SCREEN_ID_PG_GROUND_SERVICES);
}
void action_e_engine(lv_event_t * e) {}
void action_e_lighting(lv_event_t * e) {
  if (!ui_event_guard(e)) return;
  loadScreen(SCREEN_ID_PG_LIGHTS);
}
void action_e_aircraft(lv_event_t * e) {
  if (!ui_event_guard(e)) return;
  loadScreen(SCREEN_ID_PG_AIRCRAFT_INFO);
}


// Functions for AP buttons
void action_ap_pressed (lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_ap_toggle(e); }
void action_fd_pressed (lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_fd_toggle(e); }
void action_hdg_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_hdg_toggle(e); }
void action_nav_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_nav_toggle(e); }
void action_alt_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_alt_toggle(e); }
void action_vs_pressed (lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_vs_toggle(e); }
void action_apr_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_apr_toggle(e); }
void action_bc_pressed  (lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_bc_toggle(e); }
void action_yd_pressed  (lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_yd_toggle(e); }
void action_lvl_pressed (lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_lvl_toggle(e); }
void action_vnav_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_vnav_toggle(e); } // optional

// Functions for Sim Control buttons
void action_btn_sim_rate_inc(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_simrate_inc(e); }
void action_btn_sim_rate_dec(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_simrate_dec(e); }
void action_btn_sim_rate_rst(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_simrate_1x(e); }
void action_e_pause_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_pause_toggle(e); }
void action_e_active_pause_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_activepause_toggle(e); }

// Slew
//void action_slew_toggle (lv_event_t * e){ spad_action_slew_toggle(e); }

// Light page buttons
void action_btn_landing_lights_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_light_landing_toggle(e); }
void action_btn_taxi_lights_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_light_taxi_toggle(e); }
void action_btn_nav_lights_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_light_nav_toggle(e); }
void action_btn_strobe_lights_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_light_strobe_toggle(e); }
void action_btn_beacon_lights_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_light_beacon_toggle(e); }
void action_btn_panel_lights_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_light_panel_toggle(e); }
void action_btn_recog_lights_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_light_recog_toggle(e); }
void action_btn_wing_lights_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_light_wing_toggle(e); }
void action_btn_logo_lights_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_light_logo_toggle(e); }
void action_btn_cabin_lights_pressed(lv_event_t * e) { if (!ui_event_guard(e)) return; spad_action_light_cabin_toggle(e); }
void action_btn_all_on_lights_pressed(lv_event_t * e) {}
void action_btn_all_off_lights_pressed(lv_event_t * e) {}

// Sim controls page buttons
void action_btn_pushback(lv_event_t * e) {}
void action_btn_refuel(lv_event_t * e) {}
void action_btn_gpu(lv_event_t * e) {}
void action_btn_doors(lv_event_t * e) {}
void action_btn_deice(lv_event_t * e) {}
void action_btn_autostart(lv_event_t * e) {}

// Aircraft Info page buttons
void action_btn_rescan_pressed(lv_event_t * e) {
    (void)e;
    spad_force_rescan_caps();
    // (Optional UX) briefly disable the button or show a toast/spinner if you like
    // lv_obj_add_state(objects.btn_cap_rescan, LV_STATE_DISABLED);
    // ...re-enable later once you see fresh values in your DBG or after a short timer.

}
void action_btn_copy_debug_pressed(lv_event_t * e) {}
void action_btn_engine_mgmt_pressed(lv_event_t * e) {}