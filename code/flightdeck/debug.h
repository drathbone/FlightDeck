#pragma once
#include <Arduino.h>

// A lightweight debug stream wrapper that you can point at any Stream
// Usage after setup():   debugBegin(Serial0);   // or Debug::begin(Serial0);
// Then anywhere:         DBG.println("hello");  DBG.printf("v=%.2f\n", v);

class DebugStream : public Print {
public:
  explicit DebugStream(Stream* s = nullptr) : _s(s) {}

  void setStream(Stream* s) { _s = s; }
  bool enabled() const      { return _s != nullptr; }

  // Print/println via Print base:
  size_t write(uint8_t b) override {
    return _s ? _s->write(b) : 1;    // pretend success when disabled
  }
  size_t write(const uint8_t* buf, size_t size) override {
    return _s ? _s->write(buf, size) : size;
  }

  // printf-style helper
  int printf(const char* fmt, ...) __attribute__((format(printf, 2, 3)));

private:
  Stream* _s;  // nullptr = disabled
};

namespace Debug {
  // Point DBG at a concrete Stream (e.g., Serial0). Pass nullptr to disable.
  void begin(Stream& s);
  void begin(Stream* s);
  void end();                     // disables logging (DBG becomes a no-op)
  DebugStream& ref();             // the global DBG object
}

// Nice aliases so your loop reads cleanly:
inline void debugBegin(Stream& s)  { Debug::begin(s); }
inline void debugBegin(Stream* s)  { Debug::begin(s); }
inline void debugEnd()             { Debug::end(); }

// Use this in code: DBG.print(...), DBG.println(...), DBG.printf(...)
#define DBG Debug::ref()
