#include "SpadNextSerial.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

  int chan = atoi(tok[0]);

  // 0,INIT,<SerialVersion>,<AppVersion>,<AuthToken>
  if (chan == 0 && t >= 2 && strcmp(tok[1], "INIT") == 0) {
    _sendInitReply();   // reply with 0,SPAD,...
    return;
  }

  // 0,CONFIG[,...]  -> must ACK with 0,CONFIG; then we may subscribe
  if (chan == 0 && t >= 2 && strcmp(tok[1], "CONFIG") == 0) {
    _handleConfig();
    return;
  }

  // 2,START;  -> SPAD is ready; now it's safe to send device commands like SUBSCRIBE
  if (chan == 2 && t >= 2 && strcmp(tok[1], "START") == 0) {
    if (!_didSubscribe) { _doSubscriptions(); _didSubscribe = true; }
    return;
  }

  // 0,SCANSTATE; -> we can either do a 2-step scan or just end it
  if (chan == 0 && t >= 2 && strcmp(tok[1], "SCANSTATE") == 0) {
    _handleScanState();
    return;
  }

  // 0,PING,<ticket>  ->  0,PONG,<ticket>
  if (chan == 0 && t >= 2 && strcmp(tok[1], "PING") == 0) {
    _sendBegin(0);
    _io.print("PONG,");
    if (t >= 3) _printEscaped(tok[2]); // echo ticket if present
    _sendEnd();
    return;
  }

  // 5,<index>,<value>
  if (chan == 5 && t >= 3) {
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
      case DID_SIM_RATE: _emitIfChanged(DID_SIM_RATE, _simRate,        val, 0.01f); break; // coarser tolerance for rate
      case DID_ACTIVE_PAUSE: _emitIfChanged(DID_ACTIVE_PAUSE, *(float*)&_activePause, val, 0.5f); break;
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

      default: break;
    }
    return;
  }

  // (Optional) handle other channels/errors if needed
}

// ---------------- phase helpers ----------------
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

void SpadNextSerial::_doSubscriptions() {
  _subscribe(DID_COM1_ACT, "SIMCONNECT:COM ACTIVE FREQUENCY:1", "MHz");
  _subscribe(DID_COM1_STB, "SIMCONNECT:COM STANDBY FREQUENCY:1", "MHz");
  _subscribe(DID_COM2_ACT, "SIMCONNECT:COM ACTIVE FREQUENCY:2", "MHz");
  _subscribe(DID_COM2_STB, "SIMCONNECT:COM STANDBY FREQUENCY:2", "MHz");

  _subscribe(DID_NAV1_ACT, "SIMCONNECT:NAV ACTIVE FREQUENCY:1", "MHz");
  _subscribe(DID_NAV1_STB, "SIMCONNECT:NAV STANDBY FREQUENCY:1", "MHz");
  _subscribe(DID_NAV2_ACT, "SIMCONNECT:NAV ACTIVE FREQUENCY:2", "MHz");
  _subscribe(DID_NAV2_STB, "SIMCONNECT:NAV STANDBY FREQUENCY:2", "MHz");

  _subscribe(DID_SIM_RATE, "SIMCONNECT:SIMULATION RATE");

  // Active Pause state (boolean-like)
  _subscribe(DID_ACTIVE_PAUSE, "SIMCONNECT:ACTIVE PAUSE");

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

void SpadNextSerial::simRateInc()      { _sendEvent("SIM_RATE_INCR"); }
void SpadNextSerial::simRateDec()      { _sendEvent("SIM_RATE_DECR"); }
void SpadNextSerial::simRateReset1x()  { _sendEvent("SIM_RATE_SET", 256); }

void SpadNextSerial::activePauseOn()     { _sendEvent("ACTIVE_PAUSE_ON"); }
void SpadNextSerial::activePauseOff()    { _sendEvent("ACTIVE_PAUSE_OFF"); }
void SpadNextSerial::activePauseToggle() { _sendEvent("ACTIVE_PAUSE_TOGGLE"); }

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
void SpadNextSerial::apMasterToggle()       { _sendEvent("AP_MASTER"); }
void SpadNextSerial::flightDirectorToggle() { _sendEvent("TOGGLE_FLIGHT_DIRECTOR"); }
void SpadNextSerial::apHeadingToggle()      { _sendEvent("AP_HDG_HOLD"); }
void SpadNextSerial::apNavToggle()          { _sendEvent("AP_NAV1_HOLD"); }
void SpadNextSerial::apAltHoldToggle()      { _sendEvent("AP_ALT_HOLD"); }
void SpadNextSerial::apVsHoldToggle()       { _sendEvent("AP_VS_HOLD"); }
void SpadNextSerial::apApproachToggle()     { _sendEvent("AP_APR_HOLD"); }
