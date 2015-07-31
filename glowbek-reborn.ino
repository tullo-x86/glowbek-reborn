
#include "fix_fft.h"

#include <FastLED.h>

#define LED_COUNT 4
#define SAMPLE_COUNT 128
#define USED_BIN_COUNT 50


int x = 0;
char im[SAMPLE_COUNT], data[SAMPLE_COUNT];
unsigned short magnitudeSquared[USED_BIN_COUNT];

#define AUDIOPIN A0

struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];

char gradient[10] = {' ', '·','~','¢','c','»','¤','X','M','¶'};

//------------------SETUP------------------
void setup() {
    Serial.begin(115200);
    Serial.print("Initialising...");

    LEDS.addLeds<WS2811, 7, GRB>(leds, 0, LED_COUNT);
    fill_solid(leds, LED_COUNT, CRGB(16,16,0));
    LEDS.show();
    Serial.println(" Done.");
}

void printValues() {
    Serial.println();
    char buf[3];
    for (int j = 0; j < USED_BIN_COUNT; j++) {
        sprintf(buf, "%4x", magnitudeSquared[j]);
        Serial.print(buf);
        Serial.print(' ');
    }
}

//------------------MAIN LOOP------------------
void loop() {
    listen();

    printValues();

    for (uint8_t i = 0; i < LED_COUNT; i++) {
        leds[i] = CHSV(i * 85, 255, magnitudeSquared[i] >> 8);
    }

    LEDS.show();
}

void listen() {
    memset8(im, 0, SAMPLE_COUNT);

    // Read some samples
    for (uint8_t i = 0; i < SAMPLE_COUNT; i++) {
        data[i] = analogRead(AUDIOPIN);
        _delay_us(125);
    };

    // The FFT provides vectors in the form of { data[i], im[i] } vectors.
    // The amplitude of a bin is encoded as the magnitude of this vector
    // (ie. sqrt(x^2 + y^2) ), and the phase is encoded as the angle
    // (ie. atan2(y,x) ).
    fix_fft(data, im, 7, 0);

    // Don't calculate magnitude for every bin; there's a lot of data we probably won't use.
    for (uint8_t i = 0; i < USED_BIN_COUNT; i++) {
        magnitudeSquared[i] = data[i] * data[i] + im[i] * im[i];
        //data[i] = sqrt16(magnitudeSquared[i]);
    };

}
