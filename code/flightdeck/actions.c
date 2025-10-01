#include "actions.h"
#include "ui.h"
#include "spad_actions.h"

// Functions for Home Page buttons
void action_e_home(lv_event_t * e) {
  loadScreen(SCREEN_ID_PG_HOME);
}

void action_e_radios(lv_event_t * e) {
  loadScreen(SCREEN_ID_PG_RADIOS);
}

void action_e_autopilot(lv_event_t * e) {
  loadScreen(SCREEN_ID_PG_AUTOPILOT);
}
void action_e_sim_controls(lv_event_t * e) {
  loadScreen(SCREEN_ID_PG_SIM_CONTROLS);
}
void action_e_ground(lv_event_t * e) {}
void action_e_engine(lv_event_t * e) {}
void action_e_lighting(lv_event_t * e) {}

// Functions for AP buttons
void action_ap_pressed (lv_event_t * e) { spad_action_ap_toggle(e); }
void action_fd_pressed (lv_event_t * e) { spad_action_fd_toggle(e); }
void action_hdg_pressed(lv_event_t * e) { spad_action_hdg_toggle(e); }
void action_nav_pressed(lv_event_t * e) { spad_action_nav_toggle(e); }
void action_alt_pressed(lv_event_t * e) { spad_action_alt_toggle(e); }
void action_vs_pressed (lv_event_t * e) { spad_action_vs_toggle(e); }
void action_apr_pressed(lv_event_t * e) { spad_action_apr_toggle(e); }
void action_bc_pressed  (lv_event_t * e) { spad_action_bc_toggle(e); }
void action_yd_pressed  (lv_event_t * e) { spad_action_yd_toggle(e); }
void action_lvl_pressed (lv_event_t * e) { spad_action_lvl_toggle(e); }
void action_vnav_pressed(lv_event_t * e) { spad_action_vnav_toggle(e); } // optional

// Functions for Sim Control buttons
void action_btn_sim_rate_inc(lv_event_t * e) { spad_action_simrate_inc(e); }
void action_btn_sim_rate_dec(lv_event_t * e) { spad_action_simrate_dec(e); }
void action_btn_sim_rate_rst(lv_event_t * e) { spad_action_simrate_1x(e); }
void action_e_pause_pressed(lv_event_t * e) { spad_action_pause_toggle(e); }
void action_e_active_pause_pressed(lv_event_t * e) { spad_action_activepause_toggle(e); }

// Slew
//void action_slew_toggle (lv_event_t * e){ spad_action_slew_toggle(e); }