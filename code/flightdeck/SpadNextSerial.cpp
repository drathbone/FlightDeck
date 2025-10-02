#include "ui_capi.h"
#include "ui_generated_c.h"
#include "SpadNextSerial.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "debug.h"
#include <math.h>  // fabsf, roundf

// Adjust to cover your highest DataId value (401 fits easily)
static const uint16_t DID_TRACK_CAP = 1024;
static uint32_t s_lastEmitMs[DID_TRACK_CAP] = {0};

bool SpadNextSerial::_emitIfChangedThrottled(DataId id,
                                             float& lastRef,
                                             float val,
                                             float epsilon,
                                             uint16_t minPeriodMs,
                                             float quantStep,
                                             float zeroClamp)
{
    // 0) Optional "stopped" clamp (e.g., <0.25 m/s = 0)
    if (zeroClamp > 0.0f && fabsf(val) < zeroClamp) {
        val = 0.0f;
    }

    // 1) Optional quantization (e.g., 0.1 steps)
    if (quantStep > 0.0f) {
        val = roundf(val / quantStep) * quantStep;
    }

    // 2) Deadband vs lastRef
    if (fabsf(val - lastRef) < epsilon) {
        return false;
    }

    // 3) Per-DID rate limit
    const uint32_t now = millis();
    unsigned idx = static_cast<unsigned>(id);
    if (idx >= DID_TRACK_CAP) idx = 0;     // safety bucket
    uint32_t& lastMs = s_lastEmitMs[idx];
    if (now - lastMs < minPeriodMs) {
        return false;
    }
    lastMs = now;

    // 4) Reuse existing emitter to store & forward (epsilon already applied)
    return _emitIfChanged(id, lastRef, val, 0.0f);
}



// ---------------- ctor / begin ----------------
SpadNextSerial::SpadNextSerial(Stream& io, uint32_t baud)
: _io(io), _baud(baud) {}

void SpadNextSerial::begin(const char* vendor,
                           const char* product,
                           const char* serial) {
  _vendor  = vendor;
  _product = product;  // appears as device "Name" in SPAD UI
  _serial  = serial;   // sent as an optional INIT attribute
  // NOTE: you must call Serial.begin(_baud) (or your chosen stream) in your .ino.
}

// ---------------- optional identity setters ----------------
void SpadNextSerial::setDeviceGuid(const char* guid)     { if (guid && *guid)   _deviceGuid = guid; }
void SpadNextSerial::setDeviceVersion(const char* ver)   { if (ver && *ver)     _deviceVer  = ver; }

// ---------------- public loop hook ----------------
void SpadNextSerial::poll() {
  while (_io.available()) {
    char c = (char)_io.read();
    if (c == '\r') continue;         // <<< ignore CR so tokens don't carry \r
    if (_esc) { if (_rxPos < RX_SIZE - 1) _rx[_rxPos++] = c; _esc = false; continue; }
    if (c == '/') { _esc = true; continue; }
    if (c == ';') {
      if (_rxPos < RX_SIZE) _rx[_rxPos] = '\0';
      _processMessage(_rx);
      _rxPos = 0; _esc = false;
      continue;
    }
    if (_rxPos < RX_SIZE - 1) _rx[_rxPos++] = c;
  }

  // If we ACKed SCANSTATE but haven't seen START soon, re-ACK once or twice.
  if (_scanAckMs && !_started) {
    uint32_t now = millis();
    if ((uint32_t)(now - _scanAckMs) >= 700) {     // try ~0.7s; tune 500–1000ms if needed
      if (_scanAckRetry < 2) {
        _scanAckRetry++;
        _scanAckMs = now;
        DBG.printf("[WARN] No START yet, re-ACKing SCANSTATE (retry %u)\n", _scanAckRetry);
        _sendBegin(0);
        _io.print("STATESCAN");
        _sendEnd();
        _io.flush();
      } else {
        DBG.println("[ERROR] No START after SCANSTATE ack retries.");
        _scanAckMs = 0; // stop retrying
      }
    }
  }

  // Do deferred SCANSTATE dump only after we've seen START,
  // so we never block receiving the START handshake.
  if (_needStateDump && _started) {
    _emitAllStatePaced();   // make sure this ends with _io.flush()
    _needStateDump = false;
  }

  // Retry SUBSCRIBE burst if we started but haven't seen first data yet.
  // This stays quiet once _subsAcked flips to true (on first chan-5 value).
  if (_started && _subsSent && !_subsAcked) {
    uint32_t now = millis();
    if ((uint32_t)(now - _subsSentMs) >= 1000) {   // 1s timeout
      if (_subsRetry < 2) {
        _subsRetry++;
        _subsSentMs = now;
        DBG.printf("[WARN] No first data after subscribe, retry %u\n", _subsRetry);
        _sendSubscriptionsPaced();   // re-send your _doSubscriptions() list
      } else {
        DBG.println("[ERROR] Subscriptions not acknowledged after retries.");
      }
    }
  }


}

// -- Function to stop sending repeated events
bool SpadNextSerial::_allowSend(uint32_t intervalMs) {
  uint32_t now = millis();
  if (now - _lastSendMs < intervalMs) return false;
  _lastSendMs = now;
  return true;
}

// ---------------- change callback ----------------
void SpadNextSerial::onValueChanged(ChangeHandler cb) { _onChange = cb; }

bool SpadNextSerial::_emitIfChanged(DataId id, float& storage, float v, float tol) {
  if (isnan(storage) || fabsf(storage - v) > tol) {
    storage = v;
    if (_onChange) _onChange(id, v);
    return true;
  }
  return false;
}

// ---------------- SPAD message handlers ----------------
void SpadNextSerial::_processMessage(char* msg) {
  const uint8_t MAXTOK = 10;
  const char* tok[MAXTOK] = {0};
  uint8_t t = 0;

  // tokenize (comma-separated, already unescaped)
  for (char* p = msg; *p && t < MAXTOK; ++p) {
    if (p == msg || *(p-1) == '\0') tok[t++] = p;
    if (*p == ',') *p = '\0';
  }
  if (t == 0) return;

  // NEW: trim tokens (remove \r, \n, spaces, tabs at end)
  auto rtrim_tok = [](char* s) {
    size_t n = strlen(s);
    while (n && (s[n-1] == '\r' || s[n-1] == '\n' || s[n-1] == ' ' || s[n-1] == '\t')) s[--n] = '\0';
  };
  for (uint8_t i = 0; i < t; ++i) rtrim_tok((char*)tok[i]);

  int chan = atoi(tok[0]);

  // 0,INIT,...
  if (chan == 0 && t >= 2 && strncmp(tok[1], "INIT", 4) == 0) {
    _scanAckMs = 0;
    _scanAckRetry = 0;
    _needStateDump = false;   // avoid stale deferred dumps after a reconnect
    DBG.println("[HANDSHAKE] INIT received");
    _seenInit   = true;
    _started    = false;
    _subsSent   = false;
    _subsAcked  = false;
    _subsRetry  = 0;
    _subsSentMs = 0;
    _initSeenMs = millis();
    _sendInitReply();
    return;
  }

  // 0,CONFIG[,...]  -> must ACK with 0,CONFIG; then we may subscribe
  if (chan == 0 && t >= 2 && strncmp(tok[1], "CONFIG", 6) == 0) {
    _handleConfig();
    return;
  }

  // 2,START
  if (chan == 2 && t >= 2 && strncmp(tok[1], "START", 5) == 0) {
    if (!_started) {
      DBG.println("[HANDSHAKE] START received");
      _started = true;
      _scheduleSubscribeBurst();
    } else {
      DBG.println("[HANDSHAKE] START (duplicate) ignored");
    }

    _scanAckMs = 0;           // <-- stop re-ACK attempts now

    return;
  }

  // 0,SCANSTATE / STATESCAN
  if (chan == 0 && t >= 2 && (strncmp(tok[1], "SCANSTATE", 9) == 0 || strncmp(tok[1], "STATESCAN", 9) == 0)) {
    DBG.println("[HANDSHAKE] SCANSTATE received");

    // ACK FIRST so SPAD can move on to START immediately
    _sendBegin(0);
    _io.print("STATESCAN");
    _sendEnd();
    _io.flush();
    DBG.println("[HANDSHAKE] SCANSTATE ack sent (awaiting START)");

    // Defer the heavy dump to poll()
    _needStateDump = true;

      // Start re-ACK timer
    _scanAckMs = millis();
    _scanAckRetry = 0;

    return;
  }

  // 0,PING,<ticket>  ->  0,PONG,<ticket>
  if (chan == 0 && t >= 2 && strncmp(tok[1], "PING", 4) == 0) {
    _sendBegin(0);
    _io.print("PONG,");
    if (t >= 3) _printEscaped(tok[2]); // echo ticket if present
    _sendEnd();
    return;
  }

  // ---- Channel 2 housekeeping (post-start). If we haven't seen START,
  // treat these as an implied START so we don't stall on a dropped line.
  if (chan == 2 && t >= 2) {
    // exact START stays as you already have (strncmp "START", 5)
    if (strncmp(tok[1], "PROVIDER", 8) == 0)            { _markStartedIfNeeded("PROVIDER");         return; }
    if (strncmp(tok[1], "PROFILECHANGED", 14) == 0)     { _markStartedIfNeeded("PROFILECHANGED");   return; }
    if (strncmp(tok[1], "AIRCRAFTCHANGED", 15) == 0)    { _markStartedIfNeeded("AIRCRAFTCHANGED");  return; }
    // ignore other chan-2 items you don't care about
  }

  if (chan == 5) {
    if (!_subsAcked) {
      _subsAcked = true;
      DBG.println("[INFO] First value received; subscriptions confirmed.");
    }
    // ... your existing channel-5 handling ...
  }

  // --- 5,SUBSCRIBE-OK,<id>  (SPAD.neXt ack) ---
  if (chan == 5 && t >= 2 && strncmp(tok[1], "SUBSCRIBE-OK", 12) == 0) {
    _subsAcked = true;
    return;
  }

  // 5,<index>,<value>
  if (chan == 5 && t >= 3) {
    // NEW: confirm subs on first data
    if (!_subsAcked) {
      _subsAcked = true;
      _subsRetry = 0;
      DBG.println("[INFO] First data received; subscriptions confirmed.");
    }

    uint16_t idx = (uint16_t)atoi(tok[1]);
    float val = atof(tok[2]);
    switch (idx) {
      case DID_COM1_ACT: _emitIfChanged(DID_COM1_ACT, _com1ActiveMHz,  val); break;
      case DID_COM1_STB: _emitIfChanged(DID_COM1_STB, _com1StandbyMHz, val); break;
      case DID_COM2_ACT: _emitIfChanged(DID_COM2_ACT, _com2ActiveMHz,  val); break;
      case DID_COM2_STB: _emitIfChanged(DID_COM2_STB, _com2StandbyMHz, val); break;
      case DID_NAV1_ACT: _emitIfChanged(DID_NAV1_ACT, _nav1ActiveMHz,  val); break;
      case DID_NAV1_STB: _emitIfChanged(DID_NAV1_STB, _nav1StandbyMHz, val); break;
      case DID_NAV2_ACT: _emitIfChanged(DID_NAV2_ACT, _nav2ActiveMHz,  val); break;
      case DID_NAV2_STB: _emitIfChanged(DID_NAV2_STB, _nav2StandbyMHz, val); break;
      case DID_AP_HDG:       _emitIfChanged(DID_AP_HDG, _apHdgDeg,  val, 0.1f); break;
      case DID_AP_ALT:       _emitIfChanged(DID_AP_ALT, _apAltFt,   val, 1.0f); break;
      case DID_AP_VS:        _emitIfChanged(DID_AP_VS,  _apVSFPM,   val, 10.0f); break;
      case DID_AP_IAS:       _emitIfChanged(DID_AP_IAS, _apIASkt,   val, 0.5f); break;
      case DID_AP_MASTER_STATE: _emitIfChanged(DID_AP_MASTER_STATE, _apMaster,  val, 0.5f); break;
      //case DID_FD_STATE:     _emitIfChanged(DID_FD_STATE, _fdState, val, 0.5f); break;
      case DID_FD_STATE_1: _emitIfChanged(DID_FD_STATE_1, _fd1, val, 0.5f); break;
      case DID_FD_STATE_2: _emitIfChanged(DID_FD_STATE_2, _fd2, val, 0.5f); break;
      case DID_AP_HDG_MODE:     _emitIfChanged(DID_AP_HDG_MODE,     _apHdgMode, val, 0.5f); break;
      case DID_AP_NAV_MODE:     _emitIfChanged(DID_AP_NAV_MODE,     _apNavMode, val, 0.5f); break;
      case DID_AP_ALT_MODE:     _emitIfChanged(DID_AP_ALT_MODE,     _apAltMode, val, 0.5f); break;
      case DID_AP_VS_MODE:      _emitIfChanged(DID_AP_VS_MODE,      _apVsMode,  val, 0.5f); break;
      case DID_APR_ACTIVE:      _emitIfChanged(DID_APR_ACTIVE,      _aprActive, val, 0.5f); break;
      case DID_APR_ARMED:       _emitIfChanged(DID_APR_ARMED,       _aprArmed,  val, 0.5f); break;
      case DID_AP_BC_MODE:   _emitIfChanged(DID_AP_BC_MODE,   _apBC,   val, 0.5f); break;
      case DID_AP_YD_MODE:   _emitIfChanged(DID_AP_YD_MODE,   _apYD,   val, 0.5f); break;
      case DID_AP_LVL_MODE:  _emitIfChanged(DID_AP_LVL_MODE,  _apLVL,  val, 0.5f); break;
      // case DID_AP_VNAV_MODE: _emitIfChanged(DID_AP_VNAV_MODE, _apVNAV, val, 0.5f); break; // optional
      case DID_AP_VNAV_ACTIVE: _emitIfChanged(DID_AP_VNAV_ACTIVE, _apVnavActive, val, 0.5f); break;
      case DID_AP_VNAV_ARMED:  _emitIfChanged(DID_AP_VNAV_ARMED,  _apVnavArmed,  val, 0.5f); break;
      case DID_AP_VNAV_MODE:
        _emitIfChanged(DID_AP_VNAV_MODE, _apVNAV, val, 0.5f);  // treat >=0.5 as ON
        break;
      case DID_SIM_RATE:     _emitIfChanged(DID_SIM_RATE,     _simRate,     val, 0.0f); break;
      case DID_ACTIVE_PAUSE: _emitIfChanged(DID_ACTIVE_PAUSE, _activePause, val, 0.5f); break;
      case DID_SLEW_ACTIVE:  _emitIfChanged(DID_SLEW_ACTIVE,  _slewActive,  val, 0.5f); break;
      case DID_FULL_PAUSE:   _emitIfChanged(DID_FULL_PAUSE,   _fullPause,   val, 0.5f); break;
      // Lights
      case DID_LIGHT_LANDING: _emitIfChanged(DID_LIGHT_LANDING, _lightLanding, val, 0.5f); break;
      //case DID_LIGHT_TAXI:    _emitIfChanged(DID_LIGHT_TAXI,    _lightTaxi,    val, 0.5f); break;
      case DID_LIGHT_TAXI: {
          _emitIfChanged(DID_LIGHT_TAXI, _lightTaxi, val, 0.5f);
          _taxiState = (val > 0.5f) ? 1 : 0;
          break;
      }
      //case DID_LIGHT_NAV:     _emitIfChanged(DID_LIGHT_NAV,     _lightNav,     val, 0.5f); break;
              case DID_LIGHT_NAV: {
            _emitIfChanged(DID_LIGHT_NAV, _lightNav, val, 0.5f);
            _navState = (val > 0.5f) ? 1 : 0;
            break;
        }
      //case DID_LIGHT_STROBE:  _emitIfChanged(DID_LIGHT_STROBE,  _lightStrobe,  val, 0.5f); break;
      case DID_LIGHT_STROBE: {
          _emitIfChanged(DID_LIGHT_STROBE, _lightStrobe, val, 0.5f);
          _strobeState = (val > 0.5f) ? 1 : 0;
          break;
      }
      //case DID_LIGHT_BEACON:  _emitIfChanged(DID_LIGHT_BEACON,  _lightBeacon,  val, 0.5f); break;
      case DID_LIGHT_BEACON: {
          _emitIfChanged(DID_LIGHT_BEACON, _lightBeacon, val, 0.5f);
          _beaconState = (val > 0.5f) ? 1 : 0;
          break;
      }
      case DID_LIGHT_PANEL:   _emitIfChanged(DID_LIGHT_PANEL,   _lightPanel,   val, 0.5f); break;
      case DID_LIGHT_RECOG:   _emitIfChanged(DID_LIGHT_RECOG,   _lightRecog,   val, 0.5f); break;
      case DID_LIGHT_WING:    _emitIfChanged(DID_LIGHT_WING,    _lightWing,    val, 0.5f); break;
      case DID_LIGHT_LOGO:    _emitIfChanged(DID_LIGHT_LOGO,    _lightLogo,    val, 0.5f); break;
      case DID_LIGHT_CABIN:   _emitIfChanged(DID_LIGHT_CABIN,   _lightCabin,   val, 0.5f); break;

      // Ground controls
      //case DID_GROUNDSPEED:      _emitIfChanged(DID_GROUNDSPEED,      _groundSpeed_mps, val, -999.0f); break;
      case DID_GROUNDSPEED: {
          // epsilon=0.1 m/s, minPeriod=200 ms, quant=0.1 m/s, clamp <0.25 m/s to 0
          _emitIfChangedThrottled(DID_GROUNDSPEED, _groundSpeed_mps, val,
                                  0.1f, 200, 0.1f, 0.25f);
          break;
      }
      case DID_PARK_BRAKE:       _emitIfChanged(DID_PARK_BRAKE,       _parkBrake_pos,   val, -1.0f);   break;
      case DID_FUEL_TOTAL_LBS:   _emitIfChanged(DID_FUEL_TOTAL_LBS,   _fuelTotal_lbs,   val, -1.0f);   break;
      case DID_BATT_VOLTAGE:     _emitIfChanged(DID_BATT_VOLTAGE,     _battVoltage_v,   val, -1.0f);   break;
      case DID_TOTAL_WEIGHT_LBS: _emitIfChanged(DID_TOTAL_WEIGHT_LBS, _totalWeight_lbs, val, -1.0f);   break;
      case DID_OAT_C:            _emitIfChanged(DID_OAT_C,            _oat_c,           val, -999.0f); break;
      case DID_GPU_ON:           _emitIfChanged(DID_GPU_ON,           _gpu_on,          val, -1.0f);   break;


      default: break;
    }
    return;
  }

  // (Optional) handle other channels/errors if needed
}

// ---------------- phase helpers ----------------
static inline void _yieldPaced(uint16_t i, uint16_t every = 6) {
  if ((i % every) == 0) delay(2); // let USB drain a little
}

void SpadNextSerial::_emitAllStatePaced() {
  // TEMP: no-op so we compile. You can paste your existing STATESCAN
  // state-dump here and sprinkle _yieldPaced(++i, 8) inside its loop.
    _io.flush();
}

void SpadNextSerial::_handleConfig() {
  // End the CONFIG phase (we're not declaring a pin-map etc.)
  _sendBegin(0);
  _io.print("CONFIG");
  _sendEnd();
}

void SpadNextSerial::_handleScanState() {
  // Simple one-shot: we don't send any initial states, just signal "done"
  _sendBegin(0);
  _io.print("STATESCAN");
  _sendEnd();
}

void SpadNextSerial::_scheduleSubscribeBurst() {
  if (_subsSent) return;
  _sendSubscriptionsPaced();
  _subsRetry  = 0;
}

void SpadNextSerial::_sendSubscriptionsPaced() {
  DBG.println("[INFO] SUBSCRIBE burst starting...");
  uint32_t t0 = millis();

  _subPaceCount = 0;          // << reset pacing counter at the start of the burst

  // IMPORTANT: emit using your existing list
  _doSubscriptions();        // <-- your function

  // ensure the USB buffer is pushed
  _io.flush();

  // Treat as acknowledged; SPAD.neXt doesn’t send SUBSCRIBE-OK on Serial V2
  _subsSent   = true;
  //_subsAcked  = true;           // <— add this line
  //_subsSentMs = millis();
  _subsSentMs = t0;
  _subsRetry  = 0;

  DBG.printf("[INFO] SUBSCRIBE burst sent. dt=%lums\n", (unsigned long)(millis() - t0));
}

void SpadNextSerial::_doSubscriptions() {
  _subscribe(DID_COM1_ACT, "SIMCONNECT:COM ACTIVE FREQUENCY:1", "MHz");
  _subscribe(DID_COM1_STB, "SIMCONNECT:COM STANDBY FREQUENCY:1", "MHz");
  _subscribe(DID_COM2_ACT, "SIMCONNECT:COM ACTIVE FREQUENCY:2", "MHz");
  _subscribe(DID_COM2_STB, "SIMCONNECT:COM STANDBY FREQUENCY:2", "MHz");

  _subscribe(DID_NAV1_ACT, "SIMCONNECT:NAV ACTIVE FREQUENCY:1", "MHz");
  _subscribe(DID_NAV1_STB, "SIMCONNECT:NAV STANDBY FREQUENCY:1", "MHz");
  _subscribe(DID_NAV2_ACT, "SIMCONNECT:NAV ACTIVE FREQUENCY:2", "MHz");
  _subscribe(DID_NAV2_STB, "SIMCONNECT:NAV STANDBY FREQUENCY:2", "MHz");

  // Autopilot selected/target values
  _subscribe(DID_AP_HDG, "SIMCONNECT:AUTOPILOT HEADING LOCK DIR", "Degrees");
  _subscribe(DID_AP_ALT, "SIMCONNECT:AUTOPILOT ALTITUDE LOCK VAR", "Feet");
  _subscribe(DID_AP_VS,  "SIMCONNECT:AUTOPILOT VERTICAL HOLD VAR", "Feet per minute");
  _subscribe(DID_AP_IAS, "SIMCONNECT:AUTOPILOT AIRSPEED HOLD VAR", "Knots");

  // AP Toggles
  _subscribe(DID_AP_MASTER_STATE, "SIMCONNECT:AUTOPILOT MASTER");
  _subscribe(DID_AP_HDG_MODE,     "SIMCONNECT:AUTOPILOT HEADING LOCK");
  _subscribe(DID_AP_NAV_MODE,     "SIMCONNECT:AUTOPILOT NAV1 LOCK");
  _subscribe(DID_AP_ALT_MODE,     "SIMCONNECT:AUTOPILOT ALTITUDE LOCK");
  _subscribe(DID_AP_VS_MODE,      "SIMCONNECT:AUTOPILOT VERTICAL HOLD");
  _subscribe(DID_APR_ACTIVE,      "SIMCONNECT:AUTOPILOT APPROACH HOLD");
  _subscribe(DID_APR_ARMED,       "SIMCONNECT:AUTOPILOT APPROACH ARM");
  //_subscribe(DID_FD_STATE,        "SIMCONNECT:AUTOPILOT FLIGHT DIRECTOR ACTIVE");
  _subscribe(DID_FD_STATE_1, "SIMCONNECT:AUTOPILOT FLIGHT DIRECTOR ACTIVE:1");
  _subscribe(DID_FD_STATE_2, "SIMCONNECT:AUTOPILOT FLIGHT DIRECTOR ACTIVE:2");
  // Extra AP mode states
  _subscribe(DID_AP_BC_MODE,   "SIMCONNECT:AUTOPILOT BACKCOURSE HOLD");
  _subscribe(DID_AP_YD_MODE,   "SIMCONNECT:AUTOPILOT YAW DAMPER");
  _subscribe(DID_AP_LVL_MODE,  "SIMCONNECT:AUTOPILOT WING LEVELER");

  // VNAV state is not guaranteed across aircraft; add only if you want to try:
  // _subscribe(DID_AP_VNAV_MODE, "SIMCONNECT:AUTOPILOT VNAV");   // optional
  _subscribe(DID_AP_VNAV_ACTIVE, "SIMCONNECT:AP_VNAV_ACTIVE");
  _subscribe(DID_AP_VNAV_ARMED,  "SIMCONNECT:AP_VNAV_ARMED");
  _subscribe(DID_AP_VNAV_MODE, "LVAR:XMLVAR_VNAVButtonValue");

  // --- Sim controls ---
  _subscribe(DID_SIM_RATE,     "SIMCONNECT:SIMULATION RATE");
  _subscribe(DID_ACTIVE_PAUSE, "SIMCONNECT:IS IN ACTIVE PAUSE");
  _subscribe(DID_SLEW_ACTIVE,  "SIMCONNECT:IS SLEW ACTIVE");
  // _subscribe(DID_FULL_PAUSE,   "SIMCONNECT:PAUSED");   // optional (classic pause)

  // Lights
  _subscribe(DID_LIGHT_LANDING, "SIMCONNECT:LIGHT LANDING");
  _subscribe(DID_LIGHT_TAXI,    "SIMCONNECT:LIGHT TAXI");
  _subscribe(DID_LIGHT_NAV,     "SIMCONNECT:LIGHT NAV");
  _subscribe(DID_LIGHT_STROBE,  "SIMCONNECT:LIGHT STROBE");
  _subscribe(DID_LIGHT_BEACON,  "SIMCONNECT:LIGHT BEACON");
  _subscribe(DID_LIGHT_PANEL,   "SIMCONNECT:LIGHT PANEL");
  _subscribe(DID_LIGHT_RECOG,   "SIMCONNECT:LIGHT RECOGNITION");
  _subscribe(DID_LIGHT_WING,    "SIMCONNECT:LIGHT WING");
  _subscribe(DID_LIGHT_LOGO,    "SIMCONNECT:LIGHT LOGO");
  _subscribe(DID_LIGHT_CABIN,   "SIMCONNECT:LIGHT CABIN");

  // Ground Controls
  _subscribe(DID_GROUNDSPEED,      "SIMCONNECT:GROUND VELOCITY");
  _subscribe(DID_PARK_BRAKE,       "SIMCONNECT:BRAKE PARKING POSITION");
  _subscribe(DID_FUEL_TOTAL_LBS,   "SIMCONNECT:FUEL TOTAL QUANTITY WEIGHT");
  _subscribe(DID_BATT_VOLTAGE,     "SIMCONNECT:ELECTRICAL MAIN BUS VOLTAGE");
  _subscribe(DID_TOTAL_WEIGHT_LBS, "SIMCONNECT:TOTAL WEIGHT");
  _subscribe(DID_OAT_C,            "SIMCONNECT:AMBIENT TEMPERATURE");
  _subscribe(DID_GPU_ON,           "SIMCONNECT:EXTERNAL POWER ON");

}

// ---------------- send helpers ----------------
void SpadNextSerial::_printEscaped(const char* s) {
  for (const char* p = s; *p; ++p) {
    if (*p == ',' || *p == ';' || *p == '/') _io.write('/');
    _io.write(*p);
  }
}
void SpadNextSerial::_sendBegin(uint8_t channel) { _io.print(channel); _io.print(','); }
void SpadNextSerial::_sendEnd() { _io.print(';'); }

// 0,SPAD,<Guid>,<Name>,<SerialVersion>,<DeviceVersion>[,<options>...];
void SpadNextSerial::_sendInitReply() {
  _sendBegin(0);
  _io.print("SPAD,");
  _printEscaped(_deviceGuid);  _io.print(',');
  _printEscaped(_product);     _io.print(',');
  _io.print(_serialVersion);   _io.print(',');
  _printEscaped(_deviceVer);

  // Optional attributes (key=value style). Informational only.
  _io.print(',');
  _io.print("AUTHOR=");  _printEscaped(_vendor);
  _io.print(',');
  _io.print("SERIAL=");  _printEscaped(_serial);

  _sendEnd();
}

// 1,SUBSCRIBE,<index>,<path>[,<unit>];
void SpadNextSerial::_subscribe(uint16_t index, const char* path, const char* unit) {
  _sendBegin(1);
  _io.print("SUBSCRIBE,");
  _io.print(index);
  _io.print(',');
  _printEscaped(path);
  if (unit && *unit) { _io.print(','); _printEscaped(unit); }
  _sendEnd();

  // --- micro-pacing every 6 lines to keep CDC happy ---
  if ((++_subPaceCount % 6) == 0) {
    delay(2);
  }
}

// 4,<EVENT>[,<VALUE>];
void SpadNextSerial::_sendEvent(const char* name) {
  _sendBegin(4);
  _io.print("SIMCONNECT:");         // be explicit
  _printEscaped(name);
  _io.print(",1");                  // non-zero pulse
  _sendEnd();
}

void SpadNextSerial::_sendEvent(const char* name, long value) {
  _sendBegin(4);
  _io.print("SIMCONNECT:");
  _printEscaped(name);
  _io.print(',');
  _io.print(value);
  _sendEnd();
}

void SpadNextSerial::_sendEventParam(const char* evt, int32_t param) {
  _sendBegin(4);
  _io.print("SIMCONNECT:");
  _io.print(evt);
  _io.print(",");
  _io.print(param);
  _sendEnd();
}

// ---------------- public control API ----------------
void SpadNextSerial::swapCOM1() { _sendEvent("COM_STBY_RADIO_SWAP"); }
void SpadNextSerial::swapCOM2() { _sendEvent("COM2_RADIO_SWAP"); }
void SpadNextSerial::swapNAV1() { _sendEvent("NAV1_RADIO_SWAP"); }
void SpadNextSerial::swapNAV2() { _sendEvent("NAV2_RADIO_SWAP"); }

void SpadNextSerial::com1StandbyIncMHz() { _sendEvent("COM_RADIO_WHOLE_INC"); }
void SpadNextSerial::com1StandbyDecMHz() { _sendEvent("COM_RADIO_WHOLE_DEC"); }
void SpadNextSerial::com1StandbyIncKHz() { _sendEvent("COM_RADIO_FRACT_INC"); }
void SpadNextSerial::com1StandbyDecKHz() { _sendEvent("COM_RADIO_FRACT_DEC"); }

void SpadNextSerial::com2StandbyIncMHz() { _sendEvent("COM2_RADIO_WHOLE_INC"); }
void SpadNextSerial::com2StandbyDecMHz() { _sendEvent("COM2_RADIO_WHOLE_DEC"); }
void SpadNextSerial::com2StandbyIncKHz() { _sendEvent("COM2_RADIO_FRACT_INC"); }
void SpadNextSerial::com2StandbyDecKHz() { _sendEvent("COM2_RADIO_FRACT_DEC"); }

void SpadNextSerial::nav1StandbyIncMHz() { _sendEvent("NAV1_RADIO_WHOLE_INC"); }
void SpadNextSerial::nav1StandbyDecMHz() { _sendEvent("NAV1_RADIO_WHOLE_DEC"); }
void SpadNextSerial::nav1StandbyIncKHz() { _sendEvent("NAV1_RADIO_FRACT_INC"); }
void SpadNextSerial::nav1StandbyDecKHz() { _sendEvent("NAV1_RADIO_FRACT_DEC"); }

void SpadNextSerial::nav2StandbyIncMHz() { _sendEvent("NAV2_RADIO_WHOLE_INC"); }
void SpadNextSerial::nav2StandbyDecMHz() { _sendEvent("NAV2_RADIO_WHOLE_DEC"); }
void SpadNextSerial::nav2StandbyIncKHz() { _sendEvent("NAV2_RADIO_FRACT_INC"); }
void SpadNextSerial::nav2StandbyDecKHz() { _sendEvent("NAV2_RADIO_FRACT_DEC"); }

// Heading
void SpadNextSerial::apHeadingInc()         { _sendEvent("HEADING_BUG_INC"); }
void SpadNextSerial::apHeadingDec()         { _sendEvent("HEADING_BUG_DEC"); }
void SpadNextSerial::apHeadingSet(int deg)  { _sendEvent("HEADING_BUG_SET", deg % 360); }

// Altitude (feet)
void SpadNextSerial::apAltitudeInc()        { _sendEvent("AP_ALT_VAR_INC"); }
void SpadNextSerial::apAltitudeDec()        { _sendEvent("AP_ALT_VAR_DEC"); }
void SpadNextSerial::apAltitudeSetFeet(long ft) { _sendEvent("AP_ALT_VAR_SET_ENGLISH", ft); }

// Vertical Speed (ft/min)
void SpadNextSerial::apVSInc()              { _sendEvent("AP_VS_VAR_INC"); }
void SpadNextSerial::apVSDec()              { _sendEvent("AP_VS_VAR_DEC"); }
void SpadNextSerial::apVSSetFPM(long fpm)   { _sendEvent("AP_VS_VAR_SET_ENGLISH", fpm); }

// IAS (knots)
void SpadNextSerial::apIASInc()             { _sendEvent("AP_SPD_VAR_INC"); }
void SpadNextSerial::apIASDec()             { _sendEvent("AP_SPD_VAR_DEC"); }
void SpadNextSerial::apIASSetKnots(long kts){ _sendEvent("AP_SPD_VAR_SET", kts); }

// AP Toggles
void SpadNextSerial::apMasterToggle()       {_sendEvent("AP_MASTER"); }
void SpadNextSerial::flightDirectorToggle() { _sendEvent("TOGGLE_FLIGHT_DIRECTOR"); }
void SpadNextSerial::apHeadingToggle()      { _sendEvent("AP_HDG_HOLD"); }
void SpadNextSerial::apNavToggle()          { _sendEvent("AP_NAV1_HOLD"); }
void SpadNextSerial::apAltHoldToggle()      { _sendEvent("AP_ALT_HOLD"); }
void SpadNextSerial::apVsHoldToggle()       { _sendEvent("AP_VS_HOLD"); }
void SpadNextSerial::apApproachToggle()     { _sendEvent("AP_APR_HOLD"); }
void SpadNextSerial::apBackcourseToggle()  { _sendEvent("AP_BC_HOLD"); }
void SpadNextSerial::yawDamperToggle()     { _sendEvent("YAW_DAMPER_TOGGLE"); }
void SpadNextSerial::apWingLevelerToggle() { _sendEvent("AP_WING_LEVELER"); }
void SpadNextSerial::vnavToggle()          { _sendEvent("VNV_BUTTON"); }

// --- Sim Rate ---
void SpadNextSerial::simRateInc()   { _sendEvent("SIM_RATE_INCR"); }
void SpadNextSerial::simRateDec()   { _sendEvent("SIM_RATE_DECR"); }


void SpadNextSerial::simRateSet1x() {
    // Table of valid sim rate notches
    const float notches[] = {0.25f, 0.50f, 1.00f, 2.00f, 4.00f, 8.00f,
                             16.00f, 32.00f, 64.00f, 128.00f};
    const int notchCount = sizeof(notches) / sizeof(notches[0]);
    const int targetIdx  = 2; // index of 1.00f in the table

    // Find nearest notch to current _simRate
    int curIdx = 0;
    float bestErr = fabsf(_simRate - notches[0]);
    for (int i = 1; i < notchCount; ++i) {
        float err = fabsf(_simRate - notches[i]);
        if (err < bestErr) {
            curIdx = i;
            bestErr = err;
        }
    }

    int delta = curIdx - targetIdx;   // +ve means above 1x, -ve means below
    int steps = abs(delta);

    DBG.print("SimRate reset requested from ");
    DBG.print(_simRate, 2);
    DBG.print(" (nearest notch idx ");
    DBG.print(curIdx);
    DBG.print(") -> 1.00x (idx ");
    DBG.print(targetIdx);
    DBG.print("), steps needed: ");
    DBG.println(steps);

    if (steps == 0) {
        DBG.println("Already at 1.00x, nothing to do.");
        return;
    }

    // Delay slightly longer than your event throttle (200 ms)
    const unsigned stepDelayMs = 240;

    if (delta > 0) {
        // Above 1x → step down
        for (int i = 0; i < steps; ++i) {
            _sendEvent("SIM_RATE_DECR");
            DBG.println("  -> Sent SIM_RATE_DECR");
            delay(stepDelayMs);
        }
    } else {
        // Below 1x → step up
        for (int i = 0; i < steps; ++i) {
            _sendEvent("SIM_RATE_INCR");
            DBG.println("  -> Sent SIM_RATE_INCR");
            delay(stepDelayMs);
        }
    }

    DBG.println("Sim rate reset sequence complete.");
}

// --- Active Pause ---
void SpadNextSerial::activePauseToggle() { _sendEvent("ACTIVE_PAUSE_TOGGLE"); }
void SpadNextSerial::activePauseOn()     { _sendEvent("ACTIVE_PAUSE_ON"); }
void SpadNextSerial::activePauseOff()    { _sendEvent("ACTIVE_PAUSE_OFF"); }

// --- Full Pause (classic) ---
void SpadNextSerial::pauseToggle() { _sendEvent("PAUSE_TOGGLE"); }
void SpadNextSerial::pauseOn()     { _sendEvent("PAUSE_ON"); }
void SpadNextSerial::pauseOff()    { _sendEvent("PAUSE_OFF"); }

// --- Slew ---
void SpadNextSerial::slewToggle() { _sendEvent("SLEW_TOGGLE"); }
// If you prefer explicit on/off, SPAD accepts SLEW_SET with 1/0:
void SpadNextSerial::slewOn()     { _sendEventParam("SLEW_SET", 1); }
void SpadNextSerial::slewOff()    { _sendEventParam("SLEW_SET", 0); }

// Lights
void SpadNextSerial::lightLandingToggle() { _sendEvent("LANDING_LIGHTS_TOGGLE"); }
// void SpadNextSerial::lightTaxiToggle()    { if (!_allowSend(200)) return; _sendEvent("TAXI_LIGHTS_TOGGLE"); }
/* ===== TAXI ===== */
void SpadNextSerial::lightTaxiSet(bool on) {
    // if (!_allowSend(200)) return;
    _sendEventParam("TAXI_LIGHTS_SET", (int32_t)(on ? 1 : 0));
    // optional alias (some stacks listen to LIGHT_*):
    _sendEventParam("LIGHT_TAXI_SET",  (int32_t)(on ? 1 : 0));
}
void SpadNextSerial::lightTaxiToggle() {
    //if (!_allowSend(200)) return;
    if (_taxiState != -1) { lightTaxiSet(!_taxiState); return; }
    _sendEvent("TOGGLE_TAXI_LIGHTS");     // correct canonical toggle
    _sendEvent("TAXI_LIGHTS_TOGGLE");     // alt spelling, harmless
}
//void SpadNextSerial::lightNavToggle()     { if (!_allowSend(200)) return; _sendEvent("TOGGLE_NAV_LIGHTS"); }     // common name
/* ===== NAV ===== */
void SpadNextSerial::lightNavSet(bool on) {
    //if (!_allowSend(200)) return;
    _sendEventParam("NAV_LIGHTS_SET", (int32_t)(on ? 1 : 0));
    _sendEventParam("LIGHT_NAV_SET",  (int32_t)(on ? 1 : 0)); // optional alias
}
void SpadNextSerial::lightNavToggle() {
    if (!_allowSend(200)) return;
    if (_navState != -1) { lightNavSet(!_navState); return; }
    _sendEvent("TOGGLE_NAV_LIGHTS");
}
//void SpadNextSerial::lightStrobeToggle()  { if (!_allowSend(200)) return; _sendEvent("STROBES_TOGGLE"); }
/* ===== STROBE ===== */
void SpadNextSerial::lightStrobeSet(bool on) {
    //if (!_allowSend(200)) return;
    _sendEventParam("STROBES_SET",      (int32_t)(on ? 1 : 0));
    _sendEventParam("LIGHT_STROBE_SET", (int32_t)(on ? 1 : 0)); // optional alias
}
void SpadNextSerial::lightStrobeToggle() {
    if (!_allowSend(200)) return;
    if (_strobeState != -1) { lightStrobeSet(!_strobeState); return; }
    _sendEvent("STROBES_TOGGLE");
}
//void SpadNextSerial::lightBeaconToggle()  { if (!_allowSend(200)) return; _sendEvent("BEACON_LIGHTS_TOGGLE"); }
/* ===== BEACON ===== */
void SpadNextSerial::lightBeaconSet(bool on) {
    //if (!_allowSend(200)) return;
    _sendEventParam("BEACON_LIGHTS_SET", (int32_t)(on ? 1 : 0));
    _sendEventParam("LIGHT_BEACON_SET",  (int32_t)(on ? 1 : 0)); // optional alias
}
void SpadNextSerial::lightBeaconToggle() {
    if (!_allowSend(200)) return;
    if (_beaconState != -1) { lightBeaconSet(!_beaconState); return; }
    _sendEvent("TOGGLE_BEACON_LIGHTS");
    _sendEvent("BEACON_LIGHTS_TOGGLE");
}
void SpadNextSerial::lightPanelToggle()   { if (!_allowSend(200)) return; _sendEvent("PANEL_LIGHTS_TOGGLE"); }
void SpadNextSerial::lightRecogToggle()   { if (!_allowSend(200)) return; _sendEvent("RECOGNITION_LIGHTS_TOGGLE"); }
void SpadNextSerial::lightWingToggle()    { if (!_allowSend(200)) return; _sendEvent("WING_LIGHTS_TOGGLE"); }
void SpadNextSerial::lightLogoToggle()    { if (!_allowSend(200)) return; _sendEvent("LOGO_LIGHTS_TOGGLE"); }
void SpadNextSerial::lightCabinToggle()   { if (!_allowSend(200)) return; _sendEvent("CABIN_LIGHTS_TOGGLE"); }
