#include <Arduino.h>
/**
 * NeoClock
 *
 * Clock using 60 WS2812B/Neopixel LEDs and DS3231 RTC
 *
 * Libraries needed:
 *   * Adafruit NeoPixel (Library Manager) - Phil Burgess / Paint Your Dragon for Adafruit Industries - LGPL3
 *   * Rtc by Makuna (Library Manager) - Michael C. Miller
 *   * Arduino Timezone Library (https://github.com/JChristensen/Timezone) - Jack Christensen - CC-BY-SA
 *   * Time Library (https://github.com/PaulStoffregen/Time) - Paul Stoffregen, Michael Margolis - LGPL2.1
 */

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

#include <Timezone.h>    //https://github.com/JChristensen/Timezone   Timezone_master

//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone CE(CEST, CET);

TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
time_t utc;

#define PIN 8              //  Din     WS2812B/Neopixel                                                                                                                                      // Änderung Pin 8

unsigned long lastMillis = millis();
byte dimmer = 0x88;  // 0x88
byte hmark = 0;

byte ohour=0;
byte ominute=0;
byte osecond=0;

boolean fader=true;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

double long faketime = 41400;                                       // pseudo uhr start in sekunden 11:30 Uhr  = 11x3600 +  1800 sec. = 41400
int rounds = 0;                                                     // Änderung  +

void calcTime(void) {

      //utc = now();
        utc = (faketime++);                                         // Änderung  von now

        CE.toLocal(utc, &tcr);
        ohour = hour(utc);
        ominute = minute(utc);
        if (osecond != second(utc)) {
                osecond = second(utc);
                lastMillis = millis();
                if (ominute == 0 && osecond == 0) {
                }
        }
}
void addPixelColor(byte pixel, byte color, byte brightness) {
        color *= 8;
        uint32_t acolor = brightness;
        acolor <<= color;
        uint32_t ocolor = strip.getPixelColor(pixel);
        ocolor |= acolor;
        strip.setPixelColor(pixel, ocolor);
}

void drawClock(byte h, byte m, byte s) {
        strip.clear();
        addPixelColor(m, 1, dimmer);
        if (hmark > 0) {
                for (byte i = 0; i < 12; i++) {
                        addPixelColor((5 * i), 2, hmark);
                }
        }
        h %= 12;
        h *= 5;
        h += (m / 12);
        addPixelColor(h, 2, dimmer);
        // 0x RR GG BB

        if (fader) {
                byte dim_s1 = dimmer;
                byte dim_s2 = 0;
                byte px_s2 = s + 1;
                if (px_s2 >= 60)
                        px_s2 = 0;
                unsigned long curMillis = millis() - lastMillis;
                if (curMillis < 250) {
                        dim_s2 = 0;
                        dim_s1 = dimmer;
                } else {
                        dim_s2 = map(curMillis, 250, 1000, 0, dimmer);
                        dim_s1 = dimmer - map(curMillis, 250, 1000, 0, dimmer);
                }

                addPixelColor(s, 0, dim_s1);
                addPixelColor(px_s2, 0, dim_s2);
        } else {
                addPixelColor(s, 0, dimmer);
        }
        strip.show();
}
void setBrightness(byte brightness) {
        dimmer = brightness;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
        strip.begin();
        strip.show(); // Initialize all pixels to 'off'
}
void loop() {
        calcTime();
        dimmer = 200;                                                                       // Änderung  +
        hmark = 15;                                                                         // Änderung +
        drawClock(ohour, ominute, osecond);
        delay(50);                                                                          // Änderung  von 10
}
