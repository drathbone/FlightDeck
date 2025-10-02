#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_e_radios(lv_event_t * e);
extern void action_e_autopilot(lv_event_t * e);
extern void action_e_sim_controls(lv_event_t * e);
extern void action_e_ground(lv_event_t * e);
extern void action_e_engine(lv_event_t * e);
extern void action_e_lighting(lv_event_t * e);
extern void action_e_home(lv_event_t * e);
extern void action_ap_pressed(lv_event_t * e);
extern void action_fd_pressed(lv_event_t * e);
extern void action_hdg_pressed(lv_event_t * e);
extern void action_nav_pressed(lv_event_t * e);
extern void action_alt_pressed(lv_event_t * e);
extern void action_vs_pressed(lv_event_t * e);
extern void action_apr_pressed(lv_event_t * e);
extern void action_bc_pressed(lv_event_t * e);
extern void action_yd_pressed(lv_event_t * e);
extern void action_lvl_pressed(lv_event_t * e);
extern void action_vnav_pressed(lv_event_t * e);
extern void action_btn_sim_rate_inc(lv_event_t * e);
extern void action_btn_sim_rate_dec(lv_event_t * e);
extern void action_btn_sim_rate_rst(lv_event_t * e);
extern void action_e_pause_pressed(lv_event_t * e);
extern void action_e_active_pause_pressed(lv_event_t * e);
extern void action_btn_landing_lights_pressed(lv_event_t * e);
extern void action_btn_taxi_lights_pressed(lv_event_t * e);
extern void action_btn_nav_lights_pressed(lv_event_t * e);
extern void action_btn_strobe_lights_pressed(lv_event_t * e);
extern void action_btn_beacon_lights_pressed(lv_event_t * e);
extern void action_btn_panel_lights_pressed(lv_event_t * e);
extern void action_btn_recog_lights_pressed(lv_event_t * e);
extern void action_btn_wing_lights_pressed(lv_event_t * e);
extern void action_btn_logo_lights_pressed(lv_event_t * e);
extern void action_btn_cabin_lights_pressed(lv_event_t * e);
extern void action_btn_all_on_lights_pressed(lv_event_t * e);
extern void action_btn_all_off_lights_pressed(lv_event_t * e);
extern void action_btn_pushback(lv_event_t * e);
extern void action_btn_refuel(lv_event_t * e);
extern void action_btn_gpu(lv_event_t * e);
extern void action_btn_doors(lv_event_t * e);
extern void action_btn_deice(lv_event_t * e);
extern void action_btn_autostart(lv_event_t * e);
extern void action_e_aircraft(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/