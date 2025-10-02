#include "ui_capi.h"
#include "ui_generated_c.h"
#include "debug.h"
#include <stdarg.h>

// Single global instance, default disabled (nullptr)
static DebugStream g_dbg;

namespace Debug {
  void begin(Stream& s)     { g_dbg.setStream(&s); }
  void begin(Stream* s)     { g_dbg.setStream(s);  }
  void end()                { g_dbg.setStream(nullptr); }
  DebugStream& ref()        { return g_dbg; }
}

int DebugStream::printf(const char* fmt, ...) {
  if (!_s || !fmt) return 0;

  char buf[256];
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  if (n <= 0) return n;

  if (n < (int)sizeof(buf)) {
    _s->write((const uint8_t*)buf, n);
    return n;
  }

  // Longer than stack buffer: allocate exact size and print
  char* big = (char*)malloc(n + 1);
  if (!big) { _s->write((const uint8_t*)buf, sizeof(buf)); return (int)sizeof(buf); }

  va_start(ap, fmt);
  vsnprintf(big, n + 1, fmt, ap);
  va_end(ap);
  _s->write((const uint8_t*)big, n);
  free(big);
  return n;
}