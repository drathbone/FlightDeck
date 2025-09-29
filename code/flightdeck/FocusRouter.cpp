#include <Arduino.h>
#include "FocusRouter.h"
#include "debug.h"


// --------- utils ----------
static inline int adiff(int a, int b){ int d=a-b; return d<0?-d:d; }
static inline bool nearRect(lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
                            lv_coord_t X, lv_coord_t Y, lv_coord_t W, lv_coord_t H,
                            lv_coord_t tol=6) {
  return adiff((int)x,(int)X)<=tol && adiff((int)y,(int)Y)<=tol &&
         adiff((int)w,(int)W)<=tol && adiff((int)h,(int)H)<=tol;
}

// ---- Screen 1 (RADIOS) mapping table: LOCAL coords from screens.c ----
RectMap RADIOS_MAP[] = {
  { "COM1", RID_COM1, 100, 120, 360, 160 },
  { "COM2", RID_COM2, 540, 120, 360, 160 },  // ← from your print
  { "NAV1", RID_NAV1, 100, 320, 360, 160 },  // ← from your print
  { "NAV2", RID_NAV2, 540, 320, 360, 160 },  // ← from your print
  // { "XPDR", RID_XPDR,  ...,  ...,  ...,  ... },
  // { "ADF",  RID_ADF,   ...,  ...,  ...,  ... },
};
const size_t RADIOS_MAP_N = sizeof(RADIOS_MAP) / sizeof(RADIOS_MAP[0]);

// ---- Screen 2 (AUTOPILOT) mapping table: LOCAL coords ----
static RectMap AUTOPILOT_MAP[] = {
  { "HDG", AP_HDG, 100, 210, 185, 150 },
  { "ALT", AP_ALT, 305, 210, 185, 150 },
  { "VS",  AP_VS,  510, 210, 185, 150 },
  { "IAS", AP_IAS, 715, 210, 185, 150 },
};
static const size_t AUTOPILOT_MAP_N = sizeof(AUTOPILOT_MAP) / sizeof(AUTOPILOT_MAP[0]);

// --------- focus finding (no groups required) ----------

// Walk the active screen and return the deepest focused child (never the root)
static lv_obj_t* find_focused_on_screen() {
  lv_obj_t *root = lv_scr_act();
  if (!root) return nullptr;

  lv_obj_t *stack[256]; int sp = 0;
  lv_obj_t *best = nullptr;  // keep the last focused child we see
  stack[sp++] = root;

  while (sp) {
    lv_obj_t *o = stack[--sp];

    // prefer children: record a candidate but keep scanning
    if (o != root && lv_obj_has_state(o, LV_STATE_FOCUSED)) best = o;

    uint32_t cnt = lv_obj_get_child_cnt(o);
    for (uint32_t i = 0; i < cnt && sp < (int)(sizeof(stack)/sizeof(stack[0])); i++) {
      stack[sp++] = lv_obj_get_child(o, i);
    }
  }
  return best;  // may be nullptr if only the root is focused
}

// Climb to the nearest "panel-sized" ancestor, but never return the root
static lv_obj_t* climb_to_panel(lv_obj_t *o) {
  lv_obj_t *root = lv_scr_act();
  while (o && o != root) {
    lv_coord_t w = lv_obj_get_width(o);
    lv_coord_t h = lv_obj_get_height(o);
    // thresholds: adjust if your panels are smaller/larger
    if (w >= 260 && h >= 100) return o;   // nearest qualifying ancestor
    o = lv_obj_get_parent(o);
  }
  return nullptr;  // no suitable ancestor; caller will use the original focus
}

// No-op now (kept for API compatibility)
void Focus_Init() {
  // intentionally empty; we now scan the screen on demand
}

// --------- main resolver ----------
FocusKey get_focus_key() {
  FocusKey out{ ui_getCurrentScreen(), -1, nullptr, false };

  // Find focused object anywhere on the current screen
  lv_obj_t *f = find_focused_on_screen();
  if (!f) {
    // Uncomment to debug:
    // DBG.print("[Map] No LV_STATE_FOCUSED on screen "); DBG.println(out.screen);
    return out;
  }

  // Climb to a panel-sized ancestor so rects match the container from screens.c
  lv_obj_t *panel = climb_to_panel(f);
  lv_obj_t *target = panel ? panel : f;

  // LOCAL rect (matches lv_obj_set_pos/size you see in screens.c)
  lv_coord_t lx = lv_obj_get_x(target);
  lv_coord_t ly = lv_obj_get_y(target);
  lv_coord_t lw = lv_obj_get_width(target);
  lv_coord_t lh = lv_obj_get_height(target);

  switch (out.screen) {
    case 1: { // RADIOS
      for (size_t i=0; i<RADIOS_MAP_N; ++i) {
        if (nearRect(lx, ly, lw, lh,
                     RADIOS_MAP[i].x, RADIOS_MAP[i].y, RADIOS_MAP[i].w, RADIOS_MAP[i].h)) {
          out.element = RADIOS_MAP[i].id;
          out.name    = RADIOS_MAP[i].name;
          out.valid   = true;
          return out;
        }
      }
    }
    
    case 2: { // AUTOPILOT
      for (size_t i = 0; i < AUTOPILOT_MAP_N; ++i) {
        if (nearRect(lx, ly, lw, lh,
                    AUTOPILOT_MAP[i].x, AUTOPILOT_MAP[i].y,
                    AUTOPILOT_MAP[i].w, AUTOPILOT_MAP[i].h)) {
          out.element = AUTOPILOT_MAP[i].id;
          out.name    = AUTOPILOT_MAP[i].name;
          out.valid   = true;
          return out;
        }
      }
      // Help you collect rectangles the first time:
      DBG.print("[Map:AP] miss local=");
      DBG.print((int)lx); DBG.print(","); DBG.print((int)ly); DBG.print(",");
      DBG.print((int)lw); DBG.print(","); DBG.println((int)lh);
      return out;
    }

    default:
      return out; // add more screens with their own map arrays when you’re ready
  }
}
