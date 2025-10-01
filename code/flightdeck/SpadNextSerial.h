#pragma once
#include <Arduino.h>
#include <math.h>

class SpadNextSerial {
public:
  // ---- Data IDs (also used as SUBSCRIBE indices) ----
  enum DataId : uint16_t {
    DID_COM1_ACT = 101, DID_COM1_STB = 102,
    DID_COM2_ACT = 103, DID_COM2_STB = 104,
    DID_NAV1_ACT = 105, DID_NAV1_STB = 106,
    DID_NAV2_ACT = 107, DID_NAV2_STB = 108,
    DID_SIM_RATE = 109, DID_ACTIVE_PAUSE = 110,
    DID_SLEW_ACTIVE = 111,  DID_FULL_PAUSE  = 112,
    DID_AP_HDG = 201,   DID_AP_ALT = 202,
    DID_AP_VS  = 203,   DID_AP_IAS = 204,
    DID_AP_MASTER_STATE = 205,
    //DID_FD_STATE        = 206,
    DID_AP_HDG_MODE     = 207,
    DID_AP_NAV_MODE     = 208,
    DID_AP_ALT_MODE     = 209,
    DID_AP_VS_MODE      = 210,
    DID_APR_ACTIVE      = 211,
    DID_APR_ARMED       = 212,
    DID_FD_STATE_1      = 213,
    DID_FD_STATE_2      = 214,
    DID_AP_BC_MODE      = 215,  // back course (bool)
    DID_AP_YD_MODE      = 216,  // yaw damper (bool)
    DID_AP_LVL_MODE     = 217,  // wing leveler (bool)
    DID_AP_VNAV_MODE    = 218,   // VNAV (optional; may not exist on all aircraft)
    DID_AP_VNAV_ACTIVE = 219,
    DID_AP_VNAV_ARMED  = 220,

  };

  using ChangeHandler = void (*)(DataId id, float value);

  // ---- Lifecycle ----
  // Pass whichever Stream you use to talk to SPAD.neXt (Serial, Serial1, etc)
  explicit SpadNextSerial(Stream& io, uint32_t baud = 115200);

  // Does NOT call Serial.begin() for you.
  // 'product' is the device name shown in SPAD; 'serial' is optional info.
  void begin(const char* vendor  = "DIY",
             const char* product = "RadioPanel",
             const char* serial  = "SN-001");

  // Call every loop() to parse/process traffic
  void poll();

  // ---- Change notification ----
  void onValueChanged(ChangeHandler cb);

  // ---- Getters (latest values pushed from SPAD.neXt) ----
  float com1ActiveMHz()  const { return _com1ActiveMHz; }
  float com1StandbyMHz() const { return _com1StandbyMHz; }
  float com2ActiveMHz()  const { return _com2ActiveMHz; }
  float com2StandbyMHz() const { return _com2StandbyMHz; }
  float nav1ActiveMHz()  const { return _nav1ActiveMHz; }
  float nav1StandbyMHz() const { return _nav1StandbyMHz; }
  float nav2ActiveMHz()  const { return _nav2ActiveMHz; }
  float nav2StandbyMHz() const { return _nav2StandbyMHz; }
//  float simRate()        const { return _simRate; }
//  bool  activePause()     const { return _activePause != 0; }
  float apHeadingDeg()    const { return _apHdgDeg; }     // degrees
  float apAltitudeFt()    const { return _apAltFt; }      // feet
  float apVSFPM()         const { return _apVSFPM; }      // ft/min
  float apIASKnots()      const { return _apIASkt; }      // knots

  // ---- Radio actions (call from your own handlers) ----
  void swapCOM1(); void swapCOM2(); void swapNAV1(); void swapNAV2();

  void com1StandbyIncMHz(); void com1StandbyDecMHz();
  void com1StandbyIncKHz(); void com1StandbyDecKHz();

  void com2StandbyIncMHz(); void com2StandbyDecMHz();
  void com2StandbyIncKHz(); void com2StandbyDecKHz();

  void nav1StandbyIncMHz(); void nav1StandbyDecMHz();
  void nav1StandbyIncKHz(); void nav1StandbyDecKHz();

  void nav2StandbyIncMHz(); void nav2StandbyDecMHz();
  void nav2StandbyIncKHz(); void nav2StandbyDecKHz();


  // Autopilot controls
  void apHeadingInc();            void apHeadingDec();
  void apHeadingSet(int deg);     // 0..359
  void apAltitudeInc();           void apAltitudeDec();
  void apAltitudeSetFeet(long ft);
  void apVSInc();                 void apVSDec();
  void apVSSetFPM(long fpm);
  void apIASInc();                void apIASDec();
  void apIASSetKnots(long kts);

  void apMasterToggle();
  void flightDirectorToggle();
  void apHeadingToggle();
  void apNavToggle();
  void apAltHoldToggle();
  void apVsHoldToggle();
  void apApproachToggle();
  void apBackcourseToggle();     // BC
  void yawDamperToggle();        // YD
  void apWingLevelerToggle();    // LVL
  void vnavToggle();             // VNAV (stock support varies)

  // Sim Rate
  void simRateInc();
  void simRateDec();
  void simRateSet1x();   // hard reset to 1×

  // Active Pause
  void activePauseToggle();
  void activePauseOn();
  void activePauseOff();

  // Full Pause (optional, classic pause)
  void pauseToggle();
  void pauseOn();
  void pauseOff();

  // Slew
  void slewToggle();
  void slewOn();
  void slewOff();


  // ---- (Optional) Set your own fixed GUID and firmware version ----
  // If not set, defaults (constant GUID + "1.0") are used.
  void setDeviceGuid(const char* guid);        // e.g. "{12345678-90AB-CDEF-1234-567890ABCDEF}"
  void setDeviceVersion(const char* version);  // e.g. "1.2.3"

private:
  // --- internals ---
  void _processMessage(char* msg);
  
  uint32_t _lastSendMs = 0;   // shared timestamp
  bool _allowSend(uint32_t intervalMs);

  bool _didSubscribe = false;
  float _apHdgDeg       = NAN;
  float _apAltFt        = NAN;
  float _apVSFPM        = NAN;
  float _apIASkt        = NAN;
  float _apMaster = NAN, _fdState = NAN;
  float _apHdgMode = NAN, _apNavMode = NAN, _apAltMode = NAN, _apVsMode = NAN;
  float _aprActive = NAN, _aprArmed = NAN;
  float _apBC = NAN, _apYD = NAN, _apLVL = NAN, _apVNAV = NAN;
  float _apVnavActive = NAN, _apVnavArmed = NAN;
  float _simRate = NAN;
  float _activePause = NAN;
  float _slewActive = NAN;
  float _fullPause = NAN;   // optional

  // Handshake reply: 0,SPAD,<Guid>,<Name>,<SerialVersion>,<DeviceVersion>[,options...];
  void _sendInitReply();

  // Complete CONFIG phase: reply 0,CONFIG;  (and then subscribe)
  void _handleConfig();

  // Reply to SCANSTATE (simple single-shot)
  void _handleScanState();

  // Send the list of subscriptions we need
  void _doSubscriptions();

  // SUBSCRIBE + EVENT helpers
  void _subscribe(uint16_t index, const char* path, const char* unit = nullptr);
  void _sendEvent(const char* name);
  void _sendEvent(const char* name, long value);
  void _sendEventParam(const char* evt, int32_t param);  

  // wire helpers
  void _sendBegin(uint8_t channel);
  void _sendEnd();
  void _printEscaped(const char* s);

  // emit change callback if value changed beyond tolerance
  bool _emitIfChanged(DataId id, float& storage, float newVal, float tol = 0.0005f);

  Stream& _io;
  uint32_t _baud;
  const char* _vendor;
  const char* _product;
  const char* _serial;

  // device identity used in 0,SPAD reply
  const char* _deviceGuid   = "{A8AA15C5-7BB6-4AC6-A558-A88CAFB78729}"; // replace with your own fixed GUID if you want
  const char* _deviceVer    = "1.0";                                     // your firmware version string
  static constexpr uint8_t  _serialVersion = 2;                          // SPAD Serial protocol version

  // rx buffer / parser state
  static const size_t RX_SIZE = 128;
  char _rx[RX_SIZE];
  uint8_t _rxPos = 0;
  bool _esc = false;

  // pushed state (start as NAN so first update fires callback)
  float _com1ActiveMHz  = NAN, _com1StandbyMHz = NAN;
  float _com2ActiveMHz  = NAN, _com2StandbyMHz = NAN;
  float _nav1ActiveMHz  = NAN, _nav1StandbyMHz = NAN;
  float _nav2ActiveMHz  = NAN, _nav2StandbyMHz = NAN;

  float _fd1 = NAN, _fd2 = NAN;

  ChangeHandler _onChange = nullptr;
};
