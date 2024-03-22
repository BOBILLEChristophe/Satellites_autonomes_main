/*


*/

#ifndef __CONFIG__
#define __CONFIG__

#include <Arduino.h>
#include <ACAN_ESP32.h>

#define DEBUG

#define NO_ID      255       
#define NO_PIN     255

#define NB_SAT     30
#define NB_LOCOS   7

/* ----- Debug   -------------------*/
#define DEBUG
#define debug Serial


/* ----- Wifi --------------------*/

#define CONFIG 2 // Selection du mode

#if CONFIG == 0 // WiFi en mode point d’accès
#define WIFI_AP_MODE
#define WIFI_SSID "digital"
#define WIFI_PSW "digital" // Password facultatif

#elif CONFIG == 1
#define WIFI_SSID "Freebox-5C00B0"
#define WIFI_PSW  "relat@@-apiarius3-evitandam#-resolvatis9"

#elif CONFIG == 2
#define WIFI_SSID "Livebox-BC90"
#define WIFI_PSW "V9b7qzKFxdQfbMT4Pa"

#elif CONFIG == 3
#define WIFI_SSID "Starlink Olivier"
#define WIFI_PSW "VF4Ba.C-9M9FWprX"

#endif

#endif
