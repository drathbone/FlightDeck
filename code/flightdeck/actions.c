#include "actions.h"
#include "ui.h"

void action_e_home(lv_event_t * e) {
  loadScreen(SCREEN_ID_PG_HOME);
}

void action_e_radios(lv_event_t * e) {
  loadScreen(SCREEN_ID_PG_RADIOS);
}

void action_e_autopilot(lv_event_t * e) {
  loadScreen(SCREEN_ID_PG_AUTOPILOT);
}
void action_e_sim_controls(lv_event_t * e) {}
void action_e_ground(lv_event_t * e) {}
void action_e_engine(lv_event_t * e) {}
void action_e_lighting(lv_event_t * e) {}
