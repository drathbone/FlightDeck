#pragma once
#include <lvgl.h>
#include "ui.h"

// Screen 1 (Radios) element IDs
enum RadiosIds {
  RID_COM1 = 0,
  RID_COM2 = 1,
  RID_NAV1 = 2,
  RID_NAV2 = 3
};

// Screen 2 (Autopilot) element IDs
enum AutopilotIds {
  AP_HDG = 0,
  AP_ALT = 1,
  AP_VS  = 2,
  AP_IAS = 3,
};

// Rect mapping entry (ABS coords from lv_obj_get_coords)
struct RectMap {
  const char *name;
  int id;                // screen-local element id (e.g., RID_COM1)
  lv_coord_t x, y, w, h; // absolute rectangle
};

// Result of focus resolution
struct FocusKey {
  int screen;            // ui_getCurrentScreen()
  int element;           // screen-local element id
  const char *name;      // optional friendly name
  bool valid;
};

// Provided by FocusRouter.cpp (you edit/extend there)
//extern RectMap RADIOS_MAP[];
//extern const size_t RADIOS_MAP_N;
//extern RectMap AUTOPILOT_MAP[];
//extern const size_t AUTOPILOT_MAP_N;

// API
void Focus_Init();       // call once in setup() after LVGL/indev + first screen
FocusKey get_focus_key(); // use in your rotary handlers
