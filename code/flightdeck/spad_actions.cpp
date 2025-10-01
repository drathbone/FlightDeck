// spad_actions.cpp  (C++ file)
extern "C" {
  #include "lvgl.h"
  #include "spad_actions.h"
}

#include "SpadNextSerial.h"

// Use the same global instance you defined in your .ino:
//   SpadNextSerial spad(Serial);
extern SpadNextSerial spad;

extern "C" {

// Core 7
void spad_action_ap_toggle  (lv_event_t * e) { (void)e; spad.apMasterToggle(); }
void spad_action_fd_toggle  (lv_event_t * e) { (void)e; 
  // If you’re using FD :1/:2 reads for state, you still only send one event:
  spad.flightDirectorToggle(); 
}
void spad_action_hdg_toggle (lv_event_t * e) { (void)e; spad.apHeadingToggle(); }
void spad_action_nav_toggle (lv_event_t * e) { (void)e; spad.apNavToggle(); }
void spad_action_alt_toggle (lv_event_t * e) { (void)e; spad.apAltHoldToggle(); }
void spad_action_vs_toggle  (lv_event_t * e) { (void)e; spad.apVsHoldToggle(); }
void spad_action_apr_toggle (lv_event_t * e) { (void)e; spad.apApproachToggle(); }

// Optional extras
void spad_action_bc_toggle  (lv_event_t * e) { (void)e; spad.apBackcourseToggle(); }
void spad_action_yd_toggle  (lv_event_t * e) { (void)e; spad.yawDamperToggle(); }
void spad_action_lvl_toggle (lv_event_t * e) { (void)e; spad.apWingLevelerToggle(); }
void spad_action_vnav_toggle(lv_event_t * e) { (void)e; spad.vnavToggle(); }

// Sim rate
extern "C" void spad_action_simrate_inc(lv_event_t * e){ (void)e; spad.simRateInc(); }
extern "C" void spad_action_simrate_dec(lv_event_t * e){ (void)e; spad.simRateDec(); }
extern "C" void spad_action_simrate_1x (lv_event_t * e){ (void)e; spad.simRateSet1x(); }

// Active Pause
extern "C" void spad_action_activepause_toggle(lv_event_t * e){ (void)e; spad.activePauseToggle(); }
extern "C" void spad_action_activepause_on    (lv_event_t * e){ (void)e; spad.activePauseOn(); }
extern "C" void spad_action_activepause_off   (lv_event_t * e){ (void)e; spad.activePauseOff(); }

// Full Pause (optional)
extern "C" void spad_action_pause_toggle(lv_event_t * e){ (void)e; spad.pauseToggle(); }

// Slew
extern "C" void spad_action_slew_toggle (lv_event_t * e){ (void)e; spad.slewToggle(); }

// Lights
void spad_action_light_landing_toggle(lv_event_t * e){ (void)e; spad.lightLandingToggle(); }
void spad_action_light_taxi_toggle   (lv_event_t * e){ (void)e; spad.lightTaxiToggle(); }
void spad_action_light_nav_toggle    (lv_event_t * e){ (void)e; spad.lightNavToggle(); }
void spad_action_light_strobe_toggle (lv_event_t * e){ (void)e; spad.lightStrobeToggle(); }
void spad_action_light_beacon_toggle (lv_event_t * e){ (void)e; spad.lightBeaconToggle(); }
void spad_action_light_panel_toggle  (lv_event_t * e){ (void)e; spad.lightPanelToggle(); }
void spad_action_light_recog_toggle  (lv_event_t * e){ (void)e; spad.lightRecogToggle(); }
void spad_action_light_wing_toggle   (lv_event_t * e){ (void)e; spad.lightWingToggle(); }
void spad_action_light_logo_toggle   (lv_event_t * e){ (void)e; spad.lightLogoToggle(); }
void spad_action_light_cabin_toggle  (lv_event_t * e){ (void)e; spad.lightCabinToggle(); }

} // extern "C"
