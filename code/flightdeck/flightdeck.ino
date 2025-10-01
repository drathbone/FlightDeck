#pragma GCC push_options
#pragma GCC optimize("O3")

#include "FocusRouter.h"
#include <Arduino.h>
#include "lvgl.h"
#include "pins_config.h"
#include "src/lcd/jd9165_lcd.h"
#include "src/touch/gt911_touch.h"
#include "ui.h"
#include "vars.h"
#include <string.h>
#include "SpadNextSerial.h"
#include "debug.h"
#include "ui_helpers.h"
#include "ui_post_init.h"

SpadNextSerial spad(Serial);


jd9165_lcd lcd = jd9165_lcd(LCD_RST);
gt911_touch touch = gt911_touch(TP_I2C_SDA, TP_I2C_SCL, TP_RST, TP_INT);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_color_t *buf1;

// START code for detecting focused item

// END code for focused item


void RotaryTop_TurnLeft();
void RotaryTop_TurnRight();
void RotaryTop_ButtonPressed();

void RotaryBottom_TurnLeft();
void RotaryBottom_TurnRight();
void RotaryBottom_ButtonPressed();

// This block is a helper to change label/text colours depending on off/standby/active status
#define COL_DISABLED lv_color_hex(0x94A3B8)  // grey - not available
#define COL_OFF      lv_color_hex(0x22D3EE)  // cyan - available/off
#define COL_ARM      lv_color_hex(0xEF4444)  // red - armed
#define COL_ACT      lv_color_hex(0xF59E0B)  // amber - active/on

void set_ap_button(lv_obj_t* container, lv_obj_t* label, mode_state_t state) {
    lv_color_t c = (state == MODE_ACTIVE)   ? COL_ACT :
                   (state == MODE_ARMED)    ? COL_ARM :
                   (state == MODE_OFF)      ? COL_OFF : COL_DISABLED;
    lv_obj_set_style_border_color(container, c, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, c, LV_PART_MAIN);
}
// ---END---

// ---- Encoder helper ----
struct RotaryEncoder {
  uint8_t clkPin;
  uint8_t dtPin;
  uint8_t swPin;

  // decode state
  uint8_t lastAB = 0;           // last 2-bit AB state
  int8_t  accum = 0;            // accumulate quarter-steps to one detent
  unsigned long lastABTime = 0; // debounce for AB changes

  // button debounce
  bool lastSW = true;           // with INPUT_PULLUP, idle is HIGH
  unsigned long lastSWTime = 0;

  // tuning (adjust as needed)
  const uint16_t abDebounceMs = 1;   // very short; encoder needs snappy reads
  const uint16_t detentTicks  = 4;   // 4 transitions per detent on KY-040
  const uint16_t swDebounceMs = 25;  // 20–30ms is typical

  // callbacks
  void (*onLeft)()  = nullptr;
  void (*onRight)() = nullptr;
  void (*onClick)() = nullptr;

  void begin() {
    pinMode(clkPin, INPUT_PULLUP);
    pinMode(dtPin,  INPUT_PULLUP);
    pinMode(swPin,  INPUT_PULLUP);
    // initialize lastAB to current physical state
    uint8_t a = (uint8_t)digitalRead(clkPin);
    uint8_t b = (uint8_t)digitalRead(dtPin);
    lastAB = ((a ? 1 : 0) << 1) | (b ? 1 : 0);
    lastSW = digitalRead(swPin); // HIGH (not pressed)
    lastABTime = millis();
    lastSWTime = millis();
  }

  // call this as often as possible in loop()
  void update() {
    unsigned long now = millis();

    // ---- AB (rotation) decoding ----
    uint8_t a = (uint8_t)digitalRead(clkPin);
    uint8_t b = (uint8_t)digitalRead(dtPin);
    uint8_t ab = ((a ? 1 : 0) << 1) | (b ? 1 : 0);

    if (ab != lastAB && (now - lastABTime) >= abDebounceMs) {
      // 4-bit history index: previous AB (2 bits) + current AB (2 bits)
      uint8_t idx = (lastAB << 2) | ab;
      // Gray-code transition table:
      //  0  - no move / invalid
      // +1  - step CW
      // -1  - step CCW
      static const int8_t dirTable[16] = {
        0, -1, +1,  0,
        +1,  0,  0, -1,
        -1,  0,  0, +1,
         0, +1, -1,  0
      };

      int8_t d = dirTable[idx];
      if (d != 0) {
        accum += d;
        // When accum reaches +4 or -4 we consider it one "detent"
        if (accum >= detentTicks) {
          if (onRight) onRight();
          accum = 0;
        } else if (accum <= -detentTicks) {
          if (onLeft) onLeft();
          accum = 0;
        }
      }

      lastAB = ab;
      lastABTime = now;
    }

    // ---- SW (button) ----
    bool sw = digitalRead(swPin); // HIGH = not pressed, LOW = pressed
    if (sw != lastSW && (now - lastSWTime) >= swDebounceMs) {
      lastSWTime = now;
      // trigger on press (falling edge)
      if (sw == LOW) {
        if (onClick) onClick();
      }
      lastSW = sw;
    }
  }
};

// ---- Instantiate two encoders with your pin mapping ----
RotaryEncoder Rotary_Top    = {2, 3, 4};
RotaryEncoder Rotary_Bottom = {47, 46, 45};

// ---- Setup: attach callbacks and begin ----
void setupRotaries() {
  // Top callbacks
  Rotary_Top.onLeft   = RotaryTop_TurnLeft;
  Rotary_Top.onRight  = RotaryTop_TurnRight;
  Rotary_Top.onClick  = RotaryTop_ButtonPressed;

  // Bottom callbacks
  Rotary_Bottom.onLeft   = RotaryBottom_TurnLeft;
  Rotary_Bottom.onRight  = RotaryBottom_TurnRight;
  Rotary_Bottom.onClick  = RotaryBottom_ButtonPressed;

  Rotary_Top.begin();
  Rotary_Bottom.begin();
}


// 显示刷新
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  const int offsetx1 = area->x1;
  const int offsetx2 = area->x2;
  const int offsety1 = area->y1;
  const int offsety2 = area->y2;
  lcd.lcd_draw_bitmap(offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, &color_p->full);
  lv_disp_flush_ready(disp); // 告诉lvgl刷新完成
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  bool touched;
  uint16_t touchX, touchY;

  touched = touch.getTouch(&touchX, &touchY);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    // 设置坐标
    data->point.x = touchX;
    data->point.y = touchY;
    // DBG.printf("x=%d,y=%d \r\n",touchX,touchY);
  }
}

static void lvgl_port_update_callback(lv_disp_drv_t *drv)
{
    switch (drv->rotated) {
    case LV_DISP_ROT_NONE:
        touch.set_rotation(0);
        break;
    case LV_DISP_ROT_90:
        touch.set_rotation(1);
        break;
    case LV_DISP_ROT_180:
        touch.set_rotation(2);
        break;
    case LV_DISP_ROT_270:
        touch.set_rotation(3);
        break;
    }
}

void setup()
{
  Serial.begin(115200);
  Serial0.begin(115200);
  
  debugBegin(Serial0);       // bind DBG to COM15

  DBG.println("Serial Debug Started at 115200 baud");
  lcd.begin();
  touch.begin();

  spad.begin("DarrensWorkshop", "FlightDeck", "SN-001");
  spad.onValueChanged(&onSpadChange);

  lv_init();
  size_t buffer_size = sizeof(int32_t) * LCD_H_RES * LCD_V_RES;
  // buf = (int32_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  // buf1 = (int32_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  buf = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  assert(buf);
  assert(buf1);

  lv_disp_draw_buf_init(&draw_buf, buf, buf1, LCD_H_RES * LCD_V_RES);

  static lv_disp_drv_t disp_drv;
  /*Initialize the display*/
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_H_RES;
  disp_drv.ver_res = LCD_V_RES;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.full_refresh = true;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  setupRotaries();

  ui_init();

  Focus_Init();
 
  // Call my ui_post_init function
  ui_post_init();
}

void loop()
{
  spad.poll();
  lv_timer_handler();
  delay(5);
  ui_tick();
  // Service encoders frequently
  Rotary_Top.update();
  Rotary_Bottom.update();
}


// User functions

void RotaryTop_TurnLeft() {
  DBG.println("Rotary Top turned LEFT");
  FocusKey fk = get_focus_key();
  if (!fk.valid) { DBG.print("[TOP L] Unmapped focus on screen "); DBG.println(fk.screen); return; }

  switch (fk.screen) {
    case 1:  // RADIOS
      switch (fk.element) {
        case RID_COM1: DBG.println("[TOP L] COM1 --"); spad.com1StandbyDecMHz();/* do increment */ break;
        case RID_COM2: DBG.println("[TOP L] COM2 --"); spad.com2StandbyDecMHz(); break;
        case RID_NAV1: DBG.println("[TOP L] NAV1 --"); spad.nav1StandbyDecMHz(); break;
        case RID_NAV2: DBG.println("[TOP L] NAV2 --"); spad.nav2StandbyDecMHz(); break;
        default:       DBG.println("[TOP L] RADIOS: unmapped element"); break;
      }
      break;

    case 2: // Autopilot
      switch (fk.element) {
        case AP_HDG: DBG.println("[TOP L] AP HDG --"); spad.apHeadingDec(); break;
        case AP_ALT: DBG.println("[TOP L] AP ALT --"); spad.apAltitudeDec(); break;
        case AP_VS:  DBG.println("[TOP L] AP VS  --"); spad.apVSDec(); break;
        case AP_IAS: DBG.println("[TOP L] AP IAS --"); spad.apIASDec(); break;
        default:     DBG.println("[TOP L] AP: unmapped element"); break;
      }
      break;

    default:
      DBG.print("[TOP L] No handler for screen "); DBG.println(fk.screen);
      break;
  }
  // Your logic here
}

void RotaryTop_TurnRight() {
  DBG.println("Rotary Top turned RIGHT");
  FocusKey fk = get_focus_key();
  if (!fk.valid) { DBG.print("[TOP R] Unmapped focus on screen "); DBG.println(fk.screen); return; }

  switch (fk.screen) {
    case 1:  // RADIOS
      switch (fk.element) {
        case RID_COM1: DBG.println("[TOP R] COM1 ++"); spad.com1StandbyIncMHz();/* do increment */ break;
        case RID_COM2: DBG.println("[TOP R] COM2 ++"); spad.com2StandbyIncMHz(); break;
        case RID_NAV1: DBG.println("[TOP R] NAV1 ++"); spad.nav1StandbyIncMHz(); break;
        case RID_NAV2: DBG.println("[TOP R] NAV2 ++"); spad.nav2StandbyIncMHz(); break;
        default:       DBG.println("[TOP R] RADIOS: unmapped element"); break;
      }
      break;

    case 2: // Autopilot
      switch (fk.element) {
        case AP_HDG: DBG.println("[TOP R] AP HDG ++"); spad.apHeadingInc(); break;
        case AP_ALT: DBG.println("[TOP R] AP ALT ++"); spad.apAltitudeInc(); break;
        case AP_VS:  DBG.println("[TOP R] AP VS  ++"); spad.apVSInc(); break;
        case AP_IAS: DBG.println("[TOP R] AP IAS ++"); spad.apIASInc(); break;
        default:     DBG.println("[TOP R] AP: unmapped element"); break;
      }
      break;

    default:
      DBG.print("[TOP R] No handler for screen "); DBG.println(fk.screen);
      break;
  }
  // Your logic here
}

void RotaryTop_ButtonPressed() {
  DBG.println("Rotary Top BUTTON pressed");
  DBG.print("Current screen ID = ");
  DBG.println(ui_getCurrentScreen());
  FocusKey fk = get_focus_key();
  if (!fk.valid) { DBG.print("[TOP BTN] Unmapped focus on screen "); DBG.println(fk.screen); return; }

  switch (fk.screen) {
    case 1:  // RADIOS
      switch (fk.element) {
        case RID_COM1: DBG.println("[TOP BTN] COM1 SWITCH"); spad.swapCOM1(); break;
        case RID_COM2: DBG.println("[TOP BTN] COM2 SWITCH"); spad.swapCOM2(); break;
        case RID_NAV1: DBG.println("[TOP BTN] NAV1 SWITCH"); spad.swapNAV1(); break;
        case RID_NAV2: DBG.println("[TOP BTN] NAV2 SWITCH"); spad.swapNAV2(); break;
        default:       DBG.println("[TOP BTN] RADIOS: unmapped element"); break;
      }
      break;

    case 2:  // AUTOPILOT
      switch (fk.element) {
        case AP_HDG:  DBG.println("[TOP BTN] AP HDG SWITCH"); /* do increment */ break;
        case AP_ALT:  DBG.println("[TOP BTN] AP ALT SWITCH"); break;
        case AP_VS:   DBG.println("[TOP BTN] AP VS SWITCH"); break;
        case AP_IAS:  DBG.println("[TOP BTN] AP IAS SWITCH"); break;
        default:      DBG.println("[TOP BTN] AP: unmapped element"); break;
      }
      break;

    default:
      DBG.print("[TOP BTN] No handler for screen "); DBG.println(fk.screen);
      break;
  }
  // Your logic here
}

void RotaryBottom_TurnLeft() {
  DBG.println("Rotary Bottom turned LEFT");
  FocusKey fk = get_focus_key();
  if (!fk.valid) { DBG.print("[BTM L] Unmapped focus on screen "); DBG.println(fk.screen); return; }

  switch (fk.screen) {
    case 1:  // RADIOS
      switch (fk.element) {
        case RID_COM1: DBG.println("[BTM L] COM1 --"); spad.com1StandbyDecKHz();/* do increment */ break;
        case RID_COM2: DBG.println("[BTM L] COM2 --"); spad.com2StandbyDecKHz(); break;
        case RID_NAV1: DBG.println("[BTM L] NAV1 --"); spad.nav1StandbyDecKHz(); break;
        case RID_NAV2: DBG.println("[BTM L] NAV2 --"); spad.nav2StandbyDecKHz(); break;
        default:       DBG.println("[BTM L] RADIOS: unmapped element"); break;
      }
      break;

    default:
      DBG.print("[BTM L] No handler for screen "); DBG.println(fk.screen);
      break;
  }
  // Your logic here
}

void RotaryBottom_TurnRight() {
  DBG.println("Rotary Bottom turned RIGHT");
  FocusKey fk = get_focus_key();
  if (!fk.valid) { DBG.print("[BTM R] Unmapped focus on screen "); DBG.println(fk.screen); return; }

  switch (fk.screen) {
    case 1:  // RADIOS
      switch (fk.element) {
        case RID_COM1: DBG.println("[BTM R] COM1 ++"); spad.com1StandbyIncKHz();/* do increment */ break;
        case RID_COM2: DBG.println("[BTM R] COM2 ++"); spad.com2StandbyIncKHz(); break;
        case RID_NAV1: DBG.println("[BTM R] NAV1 ++"); spad.nav1StandbyIncKHz(); break;
        case RID_NAV2: DBG.println("[BTM R] NAV2 ++"); spad.nav2StandbyIncKHz(); break;
        default:       DBG.println("[BTM R] RADIOS: unmapped element"); break;
      }
      break;

    default:
      DBG.print("[BTM R] No handler for screen "); DBG.println(fk.screen);
      break;
  }
  // Your logic here
}

void RotaryBottom_ButtonPressed() {
  DBG.println("Rotary Bottom BUTTON pressed");
  FocusKey fk = get_focus_key();
  if (!fk.valid) { DBG.print("[BTM BTN] Unmapped focus on screen "); DBG.println(fk.screen); return; }

  switch (fk.screen) {
    case 1:  // RADIOS
      switch (fk.element) {
        case RID_COM1: DBG.println("[BTM BTN] COM1 SWITCH"); spad.swapCOM1(); break;
        case RID_COM2: DBG.println("[BTM BTN] COM2 SWITCH"); spad.swapCOM2(); break;
        case RID_NAV1: DBG.println("[BTM BTN] NAV1 SWITCH"); spad.swapNAV1(); break;
        case RID_NAV2: DBG.println("[BTM BTN] NAV2 SWITCH"); spad.swapNAV2(); break;
        default:       DBG.println("[BTM BTN] RADIOS: unmapped element"); break;
      }
      break;

    default:
      DBG.print("[BTM BTN] No handler for screen "); DBG.println(fk.screen);
      break;
  }
  // Your logic here
}

// little formmater the the SimRate to x1/x2/x4 etc
static void formatSimRate(float simRateRaw, char* out, size_t outsz) {
  int mult = (int)roundf(simRateRaw / 256.0f);
  if (mult < 1) mult = 1;
  snprintf(out, outsz, "x%d", mult);
}

// spad.next functions
void onSpadChange(SpadNextSerial::DataId id, float v) {
  static float fdA = NAN;  // AUTOPILOT FLIGHT DIRECTOR ACTIVE
  static float fdB = NAN;  // AUTOPILOT FLIGHT DIRECTOR
  static bool fd1 = false, fd2 = false;     // only needed if you subscribed to FD :1 and :2
  static bool aprArmed = false;             // needs DID_APR_ARMED subscription
  static bool aprActive = false;            // needs DID_APR_ACTIVE subscription
  static bool vnavArmed=false, vnavActive=false;

  switch (id) {
    case SpadNextSerial::DID_COM1_ACT: {
      static char com1ActStr[16];
      snprintf(com1ActStr, sizeof(com1ActStr), "%.3f", v);
      set_var_v_com1_active(com1ActStr);
      break;
    }
    case SpadNextSerial::DID_COM1_STB: {
      static char com1StbStr[16];
      snprintf(com1StbStr, sizeof(com1StbStr), "%.3f", v);
      set_var_v_com1_standby(com1StbStr);
      break;
    }
    case SpadNextSerial::DID_COM2_ACT: {
      static char com2ActStr[16];
      snprintf(com2ActStr, sizeof(com2ActStr), "%.3f", v);
      set_var_v_com2_active(com2ActStr);
      break;
    }
    case SpadNextSerial::DID_COM2_STB: {
      static char com2StbStr[16];
      snprintf(com2StbStr, sizeof(com2StbStr), "%.3f", v);
      set_var_v_com2_standby(com2StbStr);
      break;
    }
    case SpadNextSerial::DID_NAV1_ACT: {
      static char nav1ActStr[16];
      snprintf(nav1ActStr, sizeof(nav1ActStr), "%.3f", v);
      set_var_v_nav1_active(nav1ActStr);
      break;
    }
    case SpadNextSerial::DID_NAV1_STB: {
      static char nav1StbStr[16];
      snprintf(nav1StbStr, sizeof(nav1StbStr), "%.3f", v);
      set_var_v_nav1_standby(nav1StbStr);
      break;
    }
    case SpadNextSerial::DID_NAV2_ACT: {
      static char nav2ActStr[16];
      snprintf(nav2ActStr, sizeof(nav2ActStr), "%.3f", v);
      set_var_v_nav2_active(nav2ActStr);
      break;
    }
    case SpadNextSerial::DID_NAV2_STB: {
      static char nav2StbStr[16];
      snprintf(nav2StbStr, sizeof(nav2StbStr), "%.3f", v);
      set_var_v_nav2_standby(nav2StbStr);
      break;
    }
    case SpadNextSerial::DID_AP_HDG: {
      static char hdgStr[16];
      snprintf(hdgStr, sizeof(hdgStr), "%.0f", v);   // e.g., "273"
      set_var_v_aphdg(hdgStr);
      break;
    }
    case SpadNextSerial::DID_AP_ALT: {
      static char altStr[16];
      snprintf(altStr, sizeof(altStr), "%.0f", v);   // feet
      set_var_v_apalt(altStr);
      break;
    }
    case SpadNextSerial::DID_AP_VS: {
      static char vsStr[16];
      snprintf(vsStr, sizeof(vsStr), "%.0f", v);     // fpm
      set_var_v_apvs(vsStr);
      break;
    }
    case SpadNextSerial::DID_AP_IAS: {
      static char iasStr[16];
      snprintf(iasStr, sizeof(iasStr), "%.0f", v);   // knots
      set_var_v_apias(iasStr);
      break;
    }
    
    // -------- AP master (ON/OFF -> ACTIVE/OFF) --------
    case SpadNextSerial::DID_AP_MASTER_STATE:
      set_ap_button(objects.b_ap,  objects.obj18, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;

    // -------- Flight Director (pick one of these sections) --------
    // If you subscribed to indexed FD :1 and :2 (recommended for broad compatibility):
    case SpadNextSerial::DID_FD_STATE_1:
      fd1 = (v >= 0.5f);
      set_ap_button(objects.b_fd,  objects.obj19, (fd1 || fd2) ? MODE_ACTIVE : MODE_OFF);
      break;
    case SpadNextSerial::DID_FD_STATE_2:
      fd2 = (v >= 0.5f);
      set_ap_button(objects.b_fd,  objects.obj19, (fd1 || fd2) ? MODE_ACTIVE : MODE_OFF);
      break;

    // -------- Lateral / vertical modes (ACTIVE/OFF) --------
    case SpadNextSerial::DID_AP_HDG_MODE:
      set_ap_button(objects.b_hdg, objects.obj20, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;

    case SpadNextSerial::DID_AP_NAV_MODE:
      set_ap_button(objects.b_nav, objects.obj21, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;

    case SpadNextSerial::DID_AP_ALT_MODE:
      set_ap_button(objects.b_alt, objects.obj22, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;

    case SpadNextSerial::DID_AP_VS_MODE:
      set_ap_button(objects.b_vs,  objects.obj23,  (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;

    // -------- APR has ARMED (amber) vs ACTIVE (cyan) --------
    case SpadNextSerial::DID_APR_ARMED:
      aprArmed = (v >= 0.5f);
      // show amber only when not active
      set_ap_button(objects.b_apr, objects.obj24,
                    aprActive ? MODE_ACTIVE : (aprArmed ? MODE_ARMED : MODE_OFF));
      break;

    case SpadNextSerial::DID_APR_ACTIVE:
      aprActive = (v >= 0.5f);
      set_ap_button(objects.b_apr, objects.obj24,
                    aprActive ? MODE_ACTIVE : (aprArmed ? MODE_ARMED : MODE_OFF));
      break;

    case SpadNextSerial::DID_AP_BC_MODE:
      set_ap_button(objects.b_bc,  objects.obj33,  (v>=0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;

    case SpadNextSerial::DID_AP_YD_MODE:
      set_ap_button(objects.b_yd,  objects.obj34,  (v>=0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;

    case SpadNextSerial::DID_AP_LVL_MODE:
      set_ap_button(objects.b_lvl, objects.obj35, (v>=0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;

    case SpadNextSerial::DID_AP_VNAV_MODE:
      set_ap_button(objects.b_vnav, objects.obj36, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;

    case SpadNextSerial::DID_AP_VNAV_ARMED:
      vnavArmed = (v >= 0.5f);
      set_ap_button(objects.b_vnav, objects.obj36,
                    vnavActive ? MODE_ACTIVE : (vnavArmed ? MODE_ARMED : MODE_OFF));
      break;

    case SpadNextSerial::DID_AP_VNAV_ACTIVE:
      vnavActive = (v >= 0.5f);
      set_ap_button(objects.b_vnav, objects.obj36,
                    vnavActive ? MODE_ACTIVE : (vnavArmed ? MODE_ARMED : MODE_OFF));
      break;

    case SpadNextSerial::DID_SIM_RATE: 
      char s[8];
      formatSimRate(v, s, sizeof(s));
      set_var_v_simrate(s);   // your label setter for rate text (e.g., “x1”, “x2”)
      // Optional: highlight rate pill when > x1
      set_ap_button(objects.sim_rate, objects.l_simrate, (v > 256.0f) ? MODE_ACTIVE : MODE_OFF);
      break;
    
    case SpadNextSerial::DID_ACTIVE_PAUSE:
      set_ap_button(objects.active_pause, objects.obj38, (v>=0.5f)?MODE_ACTIVE:MODE_OFF);
      break;

    //case SpadNextSerial::DID_SLEW_ACTIVE:
    //  set_ap_button(objects.b_slew, objects.l_slew, (v>=0.5f)?MODE_ACTIVE:MODE_OFF);
    //  break;

    // Optional full pause indicator if you want it separate from active pause:
    case SpadNextSerial::DID_FULL_PAUSE:
      set_ap_button(objects.pause, objects.pause, (v>=0.5f)?MODE_ACTIVE:MODE_OFF);
      break;

    case SpadNextSerial::DID_LIGHT_LANDING:
      set_ap_button(objects.btn_landing, objects.lbl_landing, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;
    case SpadNextSerial::DID_LIGHT_TAXI:
      set_ap_button(objects.btn_taxi, objects.lbl_taxi, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;
    case SpadNextSerial::DID_LIGHT_NAV:
      set_ap_button(objects.btn_nav, objects.lbl_nav, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;
    case SpadNextSerial::DID_LIGHT_STROBE:
      set_ap_button(objects.btn_strobe, objects.lbl_strobe, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;
    case SpadNextSerial::DID_LIGHT_BEACON:
      set_ap_button(objects.btn_beacon, objects.lbl_beacon, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;
    case SpadNextSerial::DID_LIGHT_PANEL:
      set_ap_button(objects.btn_panel, objects.lbl_panel, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;
    case SpadNextSerial::DID_LIGHT_RECOG:
      set_ap_button(objects.btn_recog, objects.lbl_recog, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;
    case SpadNextSerial::DID_LIGHT_WING:
      set_ap_button(objects.btn_wing, objects.lbl_wing, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;
    case SpadNextSerial::DID_LIGHT_LOGO:
      set_ap_button(objects.btn_logo, objects.lbl_logo, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;
    case SpadNextSerial::DID_LIGHT_CABIN:
      set_ap_button(objects.btn_cabin, objects.lbl_cabin, (v >= 0.5f) ? MODE_ACTIVE : MODE_OFF);
      break;

    default: break;
  }
}
