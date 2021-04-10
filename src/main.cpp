#include <Arduino.h>
#include <FastLED.h>

#ifdef ARDUINO_ARCH_ESP32
	#include <WiFi.h>
	#include <AsyncTCP.h>
#else
	#include <ESP8266WiFi.h>
	#include <ESPAsyncTCP.h>
#endif

#define ESPALEXA_ASYNC
#include <Espalexa.h>

#include "wifiCredentials.h"


#define SERIAL_BAUDRATE     115200
//#define DEBUG
#ifdef DEBUG	// Test LED strip only has 21 LEDs so use that value for testing & debug.
	#define NUM_LEDS		21
	#define DEVICE_NAME		"Test Light"
#else
	#define NUM_LEDS		606
	#define DEVICE_NAME		"Ring Light"
#endif

#define DATA_PIN			15
#define LED_TYPE			WS2811
#define COLOR_ORDER			GRB
#define FRAMES_PER_SECOND	120


CRGB leds[NUM_LEDS];
Espalexa espalexa;


//callback function prototype
void wifiSetup();
void colorLightChanged(uint8_t brightness, uint32_t rgb);
void setBrightness(uint8_t brightness);
void setSolidColour(CRGB Colour, uint8_t brightness);


void setup()
{
	delay(3000); // 3 second delay for recovery
	
	Serial.begin(SERIAL_BAUDRATE);

	FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);	// tell FastLED about the LED strip configuration
	setBrightness(0);	// Set brightness to 0, so LED array will default to off.

	wifiSetup();

	espalexa.addDevice(DEVICE_NAME, colorLightChanged);
	espalexa.begin();
}

void loop()
{
	espalexa.loop();

	FastLED.show();	// send the 'leds' array out to the actual LED strip
	FastLED.delay(1000/FRAMES_PER_SECOND);	// insert a delay to keep the framerate modest
}

void wifiSetup()
{
	// Set WIFI module to STA mode
	WiFi.mode(WIFI_STA);

	// Connect
	Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
	WiFi.begin(WIFI_SSID, WIFI_PASS);

	// Wait
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(100);
	}
	Serial.println();
	Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void colorLightChanged(uint8_t brightness, uint32_t rgb)
{
	setSolidColour(CRGB((rgb >> 16) & 0xFF, (rgb >>  8) & 0xFF, rgb & 0xFF), brightness);
}

void setBrightness(uint8_t brightness)
{
	// set master brightness control	//	255 maximum value	127 half
	FastLED.setBrightness(brightness);
}

void setSolidColour(CRGB colour, uint8_t brightness)
{
	fadeToBlackBy(leds, NUM_LEDS, 10);
	setBrightness(brightness);
	fill_solid(leds, NUM_LEDS, colour);
}