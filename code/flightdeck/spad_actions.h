// spad_actions.h  (C header)
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"   // for lv_event_t; okay to include from C

// One wrapper per AP button
void spad_action_ap_toggle(lv_event_t * e);
void spad_action_fd_toggle(lv_event_t * e);
void spad_action_hdg_toggle(lv_event_t * e);
void spad_action_nav_toggle(lv_event_t * e);
void spad_action_alt_toggle(lv_event_t * e);
void spad_action_vs_toggle(lv_event_t * e);
void spad_action_apr_toggle(lv_event_t * e);

// (Optional extras for your bottom row)
void spad_action_bc_toggle(lv_event_t * e);
void spad_action_yd_toggle(lv_event_t * e);
void spad_action_lvl_toggle(lv_event_t * e);
void spad_action_vnav_toggle(lv_event_t * e);

// Sim rate
void spad_action_simrate_inc(lv_event_t * e);
void spad_action_simrate_dec(lv_event_t * e);
void spad_action_simrate_1x (lv_event_t * e);

// Active Pause
void spad_action_activepause_toggle(lv_event_t * e);
void spad_action_activepause_on    (lv_event_t * e);
void spad_action_activepause_off   (lv_event_t * e);

// Full Pause (optional)
void spad_action_pause_toggle(lv_event_t * e);

// Slew
void spad_action_slew_toggle (lv_event_t * e);

#ifdef __cplusplus
}
#endif
