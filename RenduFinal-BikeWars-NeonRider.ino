#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <WiFi.h>
#include <WiFiUdp.h>


// Paramètres OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 5
#define SCL_PIN 6
#define OLED_RESET -1

// Paramètres WiFi
const char* softAP_ssid = "ESP32-Access-Point";
const char* softAP_password = "12345678";

IPAddress serverIP;
// Paramètres UDP
WiFiUDP udp;
unsigned int localPort = 9999; // Port UDP local
char packetBuffer[255];


// Création de l'affichage OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Paramètres de la clé
#define O_1 20
#define O_2 10
#define O_3 0
#define I_1 7
#define I_2 8
#define I_3 9
const byte ROWS = 3;
const byte COLS = 3;
byte rowPins[ROWS] = {O_1, O_2, O_3};
byte colPins[COLS] = {I_1, I_2, I_3};
char keys[ROWS][COLS] = {
  {'U', 'L', 'D'},
  {'R', 'S', 'X'},
  {'A', 'B', 'Y'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
// 'NEON RIDERS', 116x40px
const uint8_t epd_bitmap_NEON_RIDERS [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
	0x87, 0xcf, 0x9f, 0x7f, 0x83, 0xe7, 0xef, 0xdf, 0xe3, 0xfe, 0x07, 0xe0, 0x00, 0x00, 0xff, 0xe7, 
	0xcf, 0xbf, 0x7f, 0x83, 0xe7, 0xef, 0xdf, 0xe3, 0xff, 0x1f, 0xf8, 0x00, 0x00, 0xff, 0xe7, 0xcf, 
	0xbe, 0x7f, 0xc3, 0xe7, 0xef, 0xdf, 0xe3, 0xff, 0x9f, 0xf8, 0x00, 0x00, 0xfb, 0xe7, 0xcf, 0xbe, 
	0x7f, 0xc3, 0xef, 0xef, 0x9f, 0xe3, 0xef, 0xbf, 0xfc, 0x00, 0x00, 0xfb, 0xe7, 0xcf, 0xfe, 0x7c, 
	0x03, 0xef, 0xef, 0xbf, 0xe3, 0xef, 0xbe, 0x7c, 0x00, 0x00, 0xfb, 0xe7, 0xcf, 0xfc, 0x7c, 0x03, 
	0xef, 0xef, 0xbf, 0xf3, 0xef, 0xbf, 0x7c, 0x00, 0x00, 0xff, 0xe7, 0xcf, 0xfc, 0x7c, 0x03, 0xff, 
	0xef, 0xbf, 0xf3, 0xff, 0xbf, 0x00, 0x00, 0x00, 0xff, 0xc7, 0xcf, 0xfc, 0x7f, 0x81, 0xff, 0xef, 
	0xbf, 0xf3, 0xff, 0x9f, 0xc0, 0x00, 0x00, 0xff, 0xe7, 0xcf, 0xf8, 0x7f, 0x81, 0xff, 0xef, 0xbf, 
	0xf3, 0xff, 0x1f, 0xf0, 0x00, 0x00, 0xff, 0xe7, 0xcf, 0xf8, 0x7f, 0x81, 0xff, 0xff, 0xbf, 0xf3, 
	0xff, 0x8f, 0xf8, 0x00, 0x00, 0xfb, 0xf7, 0xcf, 0xfc, 0x7f, 0x81, 0xff, 0xff, 0xbf, 0xf3, 0xef, 
	0x83, 0xfc, 0x00, 0x00, 0xfb, 0xf7, 0xcf, 0xfc, 0x7c, 0x01, 0xff, 0xff, 0xbf, 0xf3, 0xef, 0x80, 
	0xfc, 0x00, 0x00, 0xfb, 0xf7, 0xcf, 0xfc, 0x7c, 0x01, 0xfe, 0xff, 0x7e, 0xfb, 0xef, 0xbf, 0x7c, 
	0x00, 0x00, 0xfb, 0xf7, 0xcf, 0xfe, 0x7c, 0x01, 0xfe, 0xff, 0x7f, 0xfb, 0xef, 0xbf, 0x7c, 0x00, 
	0x00, 0xfb, 0xf7, 0xcf, 0xbe, 0x7c, 0x01, 0xfe, 0xff, 0x7f, 0xfb, 0xef, 0x9f, 0x7c, 0x00, 0x00, 
	0xfb, 0xf7, 0xcf, 0xbe, 0x7f, 0xc0, 0xfe, 0xff, 0x7f, 0xfb, 0xef, 0x9f, 0x7c, 0x00, 0x00, 0xff, 
	0xe7, 0xcf, 0xbf, 0x7f, 0xc0, 0xfe, 0xff, 0x7c, 0xfb, 0xef, 0x9f, 0xfc, 0x00, 0x00, 0xff, 0xe7, 
	0xcf, 0xbf, 0x7f, 0xc0, 0xfe, 0x7f, 0x7c, 0xfb, 0xef, 0x8f, 0xf8, 0x00, 0x00, 0xff, 0xc7, 0xcf, 
	0x9f, 0x7f, 0xc0, 0xfe, 0x7f, 0x7c, 0xff, 0xef, 0x87, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x7e, 0x3e, 0x31, 0x81, 0xf3, 0xef, 
	0xc7, 0xe7, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe6, 0x60, 0x73, 0x39, 0x81, 0x98, 0x8c, 0xe6, 
	0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf6, 0x60, 0x61, 0xbd, 0x81, 0x98, 0x8c, 0x26, 0x06, 
	0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf6, 0x60, 0xc1, 0xbd, 0x81, 0x98, 0x8c, 0x36, 0x06, 0x60, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0x7e, 0xc1, 0xb7, 0x81, 0xf0, 0x8c, 0x37, 0xe7, 0xc0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xde, 0x60, 0xc1, 0xb7, 0x81, 0xb0, 0x8c, 0x36, 0x06, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xce, 0x60, 0x61, 0xb3, 0x81, 0x98, 0x8c, 0x26, 0x06, 0x60, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xce, 0x60, 0x73, 0x33, 0x81, 0x8c, 0x8c, 0xe6, 0x06, 0x30, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xc6, 0x7e, 0x3e, 0x31, 0x81, 0x8f, 0xef, 0xc7, 0xe6, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 624)
const int epd_bitmap_allArray_LEN = 1;
const uint8_t* epd_bitmap_allArray[1] = {
	epd_bitmap_NEON_RIDERS
};
// 'Acceuil', 128x64px
const uint8_t epd_bitmap_Acceuil [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x07, 0xfc, 0x3e, 0x7c, 0xfb, 0xfc, 0x1f, 0x3f, 0x7e, 0xff, 0x1f, 0xf0, 0x3f, 0x00, 0x00, 
	0x00, 0x07, 0xff, 0x3e, 0x7d, 0xfb, 0xfe, 0x1f, 0x3f, 0x7e, 0xff, 0x1f, 0xf8, 0xff, 0xc0, 0x00, 
	0x00, 0x07, 0xff, 0x3e, 0x7d, 0xf3, 0xfe, 0x1f, 0x3f, 0x7e, 0xff, 0x1f, 0xfc, 0xff, 0xc0, 0x00, 
	0x00, 0x07, 0xdf, 0x3e, 0x7d, 0xf3, 0xfe, 0x1f, 0x7f, 0x7c, 0xff, 0x1f, 0x7d, 0xff, 0xe0, 0x00, 
	0x00, 0x07, 0xdf, 0x3e, 0x7d, 0xf3, 0xe0, 0x1f, 0x7f, 0x7c, 0xff, 0x1f, 0x7d, 0xfb, 0xe0, 0x00, 
	0x00, 0x07, 0xdf, 0x3e, 0x7f, 0xe3, 0xe0, 0x1f, 0x7f, 0x7d, 0xff, 0x9f, 0x7d, 0xfb, 0xe0, 0x00, 
	0x00, 0x07, 0xff, 0x3e, 0x7f, 0xe3, 0xe0, 0x1f, 0xff, 0x7d, 0xff, 0x9f, 0xfd, 0xf8, 0x00, 0x00, 
	0x00, 0x07, 0xfe, 0x3e, 0x7f, 0xe3, 0xfc, 0x0f, 0xff, 0x7d, 0xff, 0x9f, 0xfc, 0xfe, 0x00, 0x00, 
	0x00, 0x07, 0xff, 0x3e, 0x7f, 0xc3, 0xfc, 0x0f, 0xff, 0x7d, 0xff, 0x9f, 0xf8, 0xff, 0x80, 0x00, 
	0x00, 0x07, 0xff, 0x3e, 0x7f, 0xc3, 0xfc, 0x0f, 0xff, 0xfd, 0xff, 0x9f, 0xfc, 0x7f, 0xc0, 0x00, 
	0x00, 0x07, 0xdf, 0xbe, 0x7f, 0xe3, 0xfc, 0x0f, 0xff, 0xfd, 0xff, 0x9f, 0x7c, 0x1f, 0xe0, 0x00, 
	0x00, 0x07, 0xdf, 0xbe, 0x7f, 0xe3, 0xe0, 0x0f, 0xff, 0xf9, 0xff, 0x9f, 0x7c, 0x07, 0xe0, 0x00, 
	0x00, 0x07, 0xdf, 0xbe, 0x7f, 0xe3, 0xe0, 0x0f, 0xff, 0xfb, 0xf7, 0xdf, 0x7d, 0xfb, 0xe0, 0x00, 
	0x00, 0x07, 0xdf, 0xbe, 0x7f, 0xf3, 0xe0, 0x0f, 0xf7, 0xfb, 0xff, 0xdf, 0x7d, 0xfb, 0xe0, 0x00, 
	0x00, 0x07, 0xdf, 0xbe, 0x7d, 0xf3, 0xe0, 0x0f, 0xf7, 0xfb, 0xff, 0xdf, 0x7c, 0xfb, 0xe0, 0x00, 
	0x00, 0x07, 0xdf, 0xbe, 0x7d, 0xf3, 0xfe, 0x07, 0xf7, 0xfb, 0xff, 0xdf, 0x7c, 0xfb, 0xe0, 0x00, 
	0x00, 0x07, 0xff, 0x3e, 0x7d, 0xfb, 0xfe, 0x07, 0xf7, 0xfb, 0xe7, 0xdf, 0x7c, 0xff, 0xe0, 0x00, 
	0x00, 0x07, 0xff, 0x3e, 0x7d, 0xfb, 0xfe, 0x07, 0xf3, 0xfb, 0xe7, 0xdf, 0x7c, 0x7f, 0xc0, 0x00, 
	0x00, 0x07, 0xfe, 0x3e, 0x7d, 0xfb, 0xfe, 0x07, 0xf3, 0xfb, 0xe7, 0xff, 0x7c, 0x3f, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xe6, 0x7e, 0x3e, 0x39, 0x81, 0xf1, 0xef, 0xc7, 0xe7, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xe6, 0x60, 0x73, 0x39, 0x81, 0x98, 0x8c, 0xe6, 0x06, 0x60, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xf2, 0x60, 0x61, 0xbd, 0x81, 0x98, 0x8c, 0x26, 0x06, 0x60, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xf6, 0x60, 0xc1, 0xbd, 0x81, 0x98, 0x8c, 0x36, 0x06, 0x60, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xde, 0x7e, 0xc1, 0xb7, 0x81, 0xf0, 0x8c, 0x37, 0xe7, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xde, 0x60, 0xc1, 0xb7, 0x81, 0xb0, 0x8c, 0x36, 0x06, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xce, 0x60, 0x61, 0xb3, 0x81, 0x98, 0x8c, 0x26, 0x06, 0x60, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xce, 0x60, 0x73, 0x33, 0x81, 0x9c, 0x8c, 0xe6, 0x06, 0x70, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc6, 0x7e, 0x3e, 0x31, 0x81, 0x8f, 0xef, 0xc7, 0xe6, 0x30, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c, 0x00, 
	0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xf3, 0x00, 0x01, 0x98, 0x00, 
	0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x37, 0x00, 0x03, 0x98, 0x00, 
	0x00, 0x0f, 0x9f, 0xbf, 0x66, 0xcd, 0xf3, 0xe3, 0xec, 0xdf, 0x03, 0x8f, 0xbe, 0x7f, 0xc0, 0x00, 
	0x00, 0x1d, 0x9b, 0xb7, 0x66, 0xd9, 0xbb, 0xc7, 0x0c, 0xde, 0x03, 0xe6, 0x06, 0x7b, 0x00, 0x00, 
	0x00, 0x1f, 0x9b, 0xb7, 0xee, 0xfb, 0xfb, 0x03, 0xdd, 0xd8, 0x00, 0x76, 0x3e, 0x63, 0x00, 0x00, 
	0x00, 0x31, 0xbb, 0x76, 0xec, 0xf3, 0x87, 0x00, 0xfd, 0xb8, 0x06, 0x7e, 0x76, 0xe7, 0x00, 0x00, 
	0x00, 0x31, 0xbf, 0x7c, 0xfc, 0xe1, 0xf6, 0x07, 0xdf, 0xb0, 0x07, 0xe7, 0x7e, 0xc3, 0x80, 0x00, 
	0x00, 0x00, 0x30, 0x60, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x30, 0x60, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'VeloGameBoy', 27x28px
const uint8_t epd_bitmap_VeloGameBoy[] PROGMEM = {
  0x07, 0x80, 0x00, 0x02, 0x03, 0x80, 0x02, 0x02, 0x40, 0x01, 0xfc, 0x40, 0x01, 0x08, 0x80, 0x02, 
	0x04, 0x00, 0x1e, 0x07, 0x00, 0x22, 0x08, 0x80, 0x45, 0x14, 0x40, 0x88, 0xa2, 0x20, 0x88, 0xa2, 
	0x20, 0x80, 0xa0, 0x20, 0x41, 0x10, 0x40, 0x22, 0x08, 0x80, 0x1c, 0x07, 0x00
  };
// 'Velo19x15(B)', 19x15px
const unsigned char epd_bitmap_Velo19x15_B_ [] PROGMEM = {
	0x00, 0x3c, 0x00, 0x38, 0x08, 0x00, 0x48, 0x08, 0x00, 0x47, 0xf0, 0x00, 0x22, 0x10, 0x00, 0x04, 
	0x08, 0x00, 0x1c, 0x0f, 0x00, 0x22, 0x08, 0x80, 0x45, 0x14, 0x40, 0x88, 0xa2, 0x20, 0x88, 0xa2, 
	0x20, 0x80, 0xa0, 0x20, 0x41, 0x10, 0x40, 0x22, 0x08, 0x80, 0x1c, 0x07, 0x00
};


// Positions et mouvements
int x = SCREEN_WIDTH / 4;
int y = SCREEN_HEIGHT / 2;
int serverX = 5, serverY = 5; // Position du client
int radius = 5;
int deltaX = 0;
int deltaY = 0;

// Gestion de la direction
enum Direction { UP, DOWN, LEFT, RIGHT, NONE };
Direction currentDirection = NONE;

// Gestion de la traînée
const int trailLength = 100; // Longueur maximale de la traînée
int trailX[trailLength];
int trailY[trailLength];
int trailIndex = 0;

// Gestion de la trainée de l'adversaire
int trailX2[trailLength];
int trailY2[trailLength];
int trailIndex2 = 0;

// Ajouter les variables booléennes
bool aGagne = false;
bool aPerdu = false;

bool CheckCollisionServeur() {
  // Commencer à vérifier après un certain nombre de déplacements pour éviter une fausse collision au début
  if (trailIndex > 10) {
    for (int i = 0; i < trailIndex-10; i++) { // Ignorer les 10 derniers pixels de la traînée
      if (trailX[i] == x && trailY[i] == y) {
        return true; // Collision détectée
      }
    }
  }
  return false; // Aucune collision
}

bool CheckCollisionAdversaireServeur() {
    // Commencer à vérifier après un certain nombre de déplacements pour éviter une fausse collision au début
    if (trailIndex2 > 10) {
        for (int i = 0; i < trailIndex2 - 10; i++) { // Ignorer les 10 derniers pixels de la traînée de l'adversaire
            if (trailX2[i] == x && trailY2[i] == y) {
                return true; // Collision avec la traînée de l'adversaire détectée
            }
        }
    }
    return false; // Aucune collision avec la traînée de l'adversaire
}

bool CheckCollisionClient() {
    if (trailIndex2 > 10) {
        for (int i = 0; i < trailIndex2 - 10; i++) {
            if (trailX2[i] == serverX && trailY2[i] == serverY) {
              if (serverX != 5 && serverY != 5){
                return true; // Collision de l'adversaire avec sa propre traînée
              }
            }
        }
    }
    return false; // Aucune collision de l'adversaire avec sa propre traînée
}

bool CheckCollisionAdversaireClient() {
    if (trailIndex > 10) {
        for (int i = 0; i < trailIndex - 10; i++) {
            if (trailX[i] == serverX && trailY[i] == serverY) {
                if (serverX != 5 && serverY != 5){
                  return true; // Collision de l'adversaire avec sa propre traînée
              }
            }
        }
    }
    return false; // Aucune collision du client avec votre traînée
}


void EcranFinDeJeu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  
  
  // Afficher l'image NEON RIDERS en haut de l'écran
  int bitmapX = (SCREEN_WIDTH - 116) / 2; // Centrer l'image horizontalement
  display.drawBitmap(bitmapX, 0, epd_bitmap_NEON_RIDERS, 116, 40, SSD1306_WHITE);
  display.setCursor(16, 39);
  if (aPerdu) {
    display.println("Vous avez perdu");
  } else if (aGagne) {
    display.println("Vous avez gagne");
  }
  display.println("Bouton A: Recommencer");
  display.println("Bouton B: Quitter");
  display.display();

  while (true) {
    char key = keypad.getKey();
    if (key == 'A') {
      // Redémarrer le jeu
      setup(); // Appelle la fonction setup pour redémarrer
      break;
    } else if (key == 'B') {
      // Éteindre l'écran et arrêter le jeu
      display.clearDisplay();
      display.display();
      while(1); // Boucle infinie pour arrêter le jeu
    }
  }
}


enum Mode { MODE_SERVER, MODE_CLIENT };
Mode currentMode = MODE_SERVER; // Déclaration de la variable currentMode

void setup() {
    Wire.begin(SDA_PIN, SCL_PIN);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
        display.clearDisplay();

    
    // Afficher l'écran d'accueil
    display.drawBitmap(0, 0, epd_bitmap_Acceuil, 128, 64, SSD1306_WHITE);
    display.display();

    // Attendre l'appui sur 'Start'
    char key = 0;
    while (key != 'X') {
        key = keypad.getKey();
        delay(100); // Petite pause pour éviter la lecture en boucle trop rapide
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);

  bool modeSelected = false;
  while (!modeSelected) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Choisir mode:");
    display.println(currentMode == MODE_SERVER ? "> Serveur" : "  Serveur");
    display.println(currentMode == MODE_CLIENT ? "> Client" : "  Client");
    display.display();

    char key = keypad.getKey();
    if (key == 'U') {
      currentMode = MODE_SERVER;
    } else if (key == 'D') {
      currentMode = MODE_CLIENT;
    } else if (key == 'S') {
      modeSelected = true;
    }
    delay(100);
  }

  if (currentMode == MODE_SERVER) {
    // Configurer le point d'accès SoftAP
    WiFi.softAP(softAP_ssid, softAP_password);
    Serial.println("SoftAP démarré");
    IPAddress IP = WiFi.softAPIP();
    Serial.print("IP du SoftAP: ");
    Serial.println(IP);
    display.println("SoftAP OK");

    udp.begin(localPort);
    Serial.printf("UDP server : %s:%i \n", WiFi.localIP().toString().c_str(), localPort);


  } else if (currentMode == MODE_CLIENT) {
    WiFi.begin(softAP_ssid, softAP_password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    // Enregistrer l'adresse IP du serveur
    serverIP = WiFi.gatewayIP(); // Utilisez gatewayIP() car en mode SoftAP, le serveur agit en tant que passerelle
    Serial.println("WiFi connecté en tant que client");
    display.println("Client WiFi OK");
    udp.begin(localPort);
    Serial.printf("UDP Client : %s:%i \n", WiFi.localIP().toString().c_str(), localPort);
    int y = SCREEN_HEIGHT / 2 + 20;

  }

    display.clearDisplay();
    // Afficher l'image NEON RIDERS en haut de l'écran
    int bitmapX = (SCREEN_WIDTH - 116) / 2; // Centrer l'image horizontalement
    display.drawBitmap(bitmapX, 0, epd_bitmap_NEON_RIDERS, 116, 40, SSD1306_WHITE);

    display.setCursor(16, 52); // Laisser un espace de 10 pixels avant l'affichage de l'IP
    display.print("IP: ");
    display.println(WiFi.localIP());


    display.display();

    delay(3000); // Afficher l'adresse IP et l'image pendant 2 secondes

    

    for (int i = 0; i < trailLength; i++) {
        trailX[i] = -1;
        trailY[i] = -1;
    }

    memset(trailX2, -1, sizeof(trailX2));
    memset(trailY2, -1, sizeof(trailY2));

    // Initialiser la direction et le mouvement vers la droite
    currentDirection = RIGHT;
    deltaX = 1;
    deltaY = 0;
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    switch (key) {
      case 'U':
        if (currentDirection != DOWN) {
          deltaY = -1; deltaX = 0;
          currentDirection = UP;
        }
        break;
      case 'D':
        if (currentDirection != UP) {
          deltaY = 1; deltaX = 0;
          currentDirection = DOWN;
        }
        break;
      case 'L':
        if (currentDirection != RIGHT) {
          deltaX = -1; deltaY = 0;
          currentDirection = LEFT;
        }
        break;
      case 'R':
        if (currentDirection != LEFT) {
          deltaX = 1; deltaY = 0;
          currentDirection = RIGHT;
        }
        break;
      default:
        break;
    }
  }


  // Mise à jour de la position en vérifiant les limites de l'écran
  x += deltaX;
  y += deltaY;

  // Si le joueur atteint le bord droit, il réapparaît à gauche
  if (x > SCREEN_WIDTH - radius) {
    x = radius;
  }
  // Si le joueur atteint le bord gauche, il réapparaît à droite
  else if (x < radius) {
    x = SCREEN_WIDTH - radius;
  }

  // Si le joueur atteint le bord inférieur, il réapparaît en haut
  if (y > SCREEN_HEIGHT - radius) {
    y = radius;
  }
  // Si le joueur atteint le bord supérieur, il réapparaît en bas
  else if (y < radius) {
    y = SCREEN_HEIGHT - radius;
  }

  // Mettre à jour la traînée
  trailX[trailIndex] = x;
  trailY[trailIndex] = y;
  trailIndex = (trailIndex + 1) % trailLength;
  Serial.println(trailIndex); // Affiche la valeur de trailIndex dans le moniteur série

  String message = String(x) + " " + String(y);

  if (currentMode == MODE_SERVER) {
    // envoie et reception des données en mode serveur 
    server(message);

  }
  else{
    //envoie et receptioin des données en mode client 
    client(message, serverIP, localPort);
    
  }

  // Mettre à jour la traînée de l'adversaire
  trailX2[trailIndex2] = serverX;
  trailY2[trailIndex2] = serverY;
  trailIndex2 = (trailIndex2 + 1) % trailLength;
  
  if (CheckCollisionServeur()) {
    aPerdu = true;
    EcranFinDeJeu();
  }
  if (CheckCollisionAdversaireServeur()) {
    aPerdu = true;
    EcranFinDeJeu();
  }
    if (CheckCollisionClient()) {
    aGagne = true;
    EcranFinDeJeu();
  }
  if (CheckCollisionAdversaireClient()) {
    aGagne = true;
    EcranFinDeJeu();
  }  
  
  updateDisplay();
  delay(20);
}


void client (const String message, IPAddress serverIP, unsigned int localPort) {
 	int packetSize = udp.parsePacket();
 	//Serial.print(" Received packet from : "); Serial.println(udp.remoteIP());
 	//Serial.print(" Size : "); Serial.println(packetSize);
 	if (packetSize) {
 			int len = udp.read(packetBuffer, 255);
 			if (len > 0) packetBuffer[len - 1] = 0;
      int receivedX, receivedY;
      if(sscanf(packetBuffer, "%d %d", &receivedX, &receivedY) == 2) {
        serverX = receivedX;
        serverY = receivedY;
        Serial.print("Valeurs extraites - X: ");
        Serial.print(serverX);
        Serial.print(", Y: ");
        Serial.println(serverY);
 			//Serial.printf("Data : %s\n", packetBuffer);
    }
 	}
 	Serial.println("\n");
 	delay(10);
 	//Serial.print("[Client Connected] ");
 	//Serial.println(WiFi.localIP());
 	udp.beginPacket(serverIP, localPort);
 	udp.printf(message.c_str());

 	udp.printf("\r\n");
 	udp.endPacket();
}



void server(const String message) {
 	int packetSize = udp.parsePacket();
 	//Serial.print(" Received packet from : "); Serial.println(udp.remoteIP());
 	//Serial.print(" Size : "); Serial.println(packetSize);
 	if (packetSize) {
 			int len = udp.read(packetBuffer, 255);
 			if (len > 0) packetBuffer[len - 1] = 0;
      int receivedX, receivedY;
      if(sscanf(packetBuffer, "%d %d", &receivedX, &receivedY) == 2) {
        serverX = receivedX;
        serverY = receivedY;
        Serial.print("Valeurs extraites - X: ");
        Serial.print(serverX);
        Serial.print(", Y: ");
        Serial.println(serverY);
      }
 			//Serial.printf("Data : %s\n", packetBuffer);
 			udp.beginPacket(udp.remoteIP(), udp.remotePort());
 			udp.printf(message.c_str());
      udp.printf("\r\n");
 			udp.endPacket();
 	}
 	Serial.println("\n");
 	delay(10);
 	//Serial.print("[Server Connected] ");
 	//Serial.println (WiFi.localIP());
}




void updateDisplay() {
  display.clearDisplay();

  // Dessinez la traînée
  for (int i = 0; i < trailLength; i++) {
    if (trailX[i] != -1 && trailY[i] != -1) {
      display.drawPixel(trailX[i], trailY[i], SSD1306_WHITE);
    }
  }
  // Dessinez la traînée de l'adversaire
  for (int i = 0; i < trailLength; i++) {
    if (trailX2[i] != -1 && trailY2[i] != -1) {
      display.drawPixel(trailX2[i], trailY2[i], SSD1306_WHITE);
    }
  }

    // Dessinez l'image du joueur serveur
    int bitmapWidth = 19;  // Largeur de l'image en pixels
    int bitmapHeight = 15; // Hauteur de l'image en pixels

    // Calculez la position supérieure gauche pour centrer l'image sur la position (x, y)
    int bitmapX = x - bitmapWidth / 2;
    int bitmapY = y - bitmapHeight / 2;
    // Calculez la position supérieure gauche pour centrer l'image sur la position (x, y)
    int bitmapX2 = serverX - bitmapWidth / 2;
    int bitmapY2 = serverY - bitmapHeight / 2;

  display.drawBitmap(bitmapX, bitmapY, epd_bitmap_VeloGameBoy, bitmapWidth, bitmapHeight, SSD1306_WHITE);
  display.drawBitmap(bitmapX2, bitmapY2, epd_bitmap_Velo19x15_B_ ,bitmapWidth, bitmapHeight, SSD1306_WHITE);
  display.display();
}