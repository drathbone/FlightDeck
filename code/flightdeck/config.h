#include "ui_generated_c.h"
#pragma once

// UI debounce window in milliseconds for touch handlers
#ifndef UI_DEDUP_WINDOW_MS
#define UI_DEDUP_WINDOW_MS 180
#endif

// Serial / protocol config
#ifndef SPAD_BAUD
#define SPAD_BAUD 115200
#endif

// Feature flags (toggle per-aircraft support)
#ifndef FEATURE_VNAV
#define FEATURE_VNAV 0
#endif
