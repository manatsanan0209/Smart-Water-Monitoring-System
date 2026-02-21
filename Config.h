#ifndef THEME_H
#define THEME_H

#include <TFT_eSPI.h>

// --- Colors ---
#define COLOR_BG        TFT_BLACK
#define COLOR_TEXT      TFT_WHITE
#define COLOR_HIGHLIGHT TFT_GREEN
#define COLOR_ALERT     TFT_RED

// --- Fonts ---
#define FMB9  &FreeMonoBold9pt7b
#define FMB12 &FreeMonoBold12pt7b
#define FMB18 &FreeMonoBold18pt7b 

// --- Layout Positions ---
#define NAV_H           50
#define LABEL_X         35
#define COLON_X         160
#define VALUE_X         185
#define TEMP_X          80

// --- Icon Positions ---
#define ICON_LORA_X     155
#define ICON_BELL_X     200
#define ICON_GEAR_X     245
#define ICON_WIFI_X     290
#define ICON_Y          10
#define ALARM_ICON_X    280

#endif