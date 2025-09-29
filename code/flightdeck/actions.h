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


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/