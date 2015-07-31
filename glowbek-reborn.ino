
#include "fix_fft.h"

#include <FastLED.h>
#include "SparksPattern.h"

#define LED_COUNT 4
#define SAMPLE_COUNT 128
#define USED_BIN_COUNT 50


int x = 0;
char im[SAMPLE_COUNT], data[SAMPLE_COUNT];
unsigned short magnitudeSquared[USED_BIN_COUNT];

#define AUDIOPIN A0

CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];

char gradient[10] = {' ', '·','~','¢','c','»','¤','X','M','¶'};

SparksPattern sparks(leds, LED_COUNT,
                     50, //framesPerSecond,
                     3, //sparkleTrailLength,
                     6, //valFalloffDistance,
                     32, //valMin,
                     150, //valMax,
                     10); //sparkDistance

#define FRAME_WINDOW 4
bool pastTriggers[FRAME_WINDOW];

void setup() {
    //Serial.begin(115200);
    //Serial.print("Initialising...");

    LEDS.addLeds<WS2811, 7, GRB>(leds, 0, LED_COUNT);
    fill_solid(leds, LED_COUNT, CRGB(16,16,0));
    LEDS.show();
    //Serial.println(" Done.");

    memset8(pastTriggers, 0, FRAME_WINDOW);
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

void listen();

void loop() {
    listen();

    //printValues();

    FastLED.show();

    bool isTriggered = false;

    for (int i = 0; i < USED_BIN_COUNT; ++i) {
        if (magnitudeSquared[i] > 0x0150)
        {
            isTriggered = true;
            break;
        }
    }

    bool canFireSpark = true;
    for (int j = 0; j < FRAME_WINDOW; ++j) {
        if (pastTriggers[j]) {
            canFireSpark = false;
            break;
        }
    }

    if (canFireSpark && isTriggered) {
        sparks.pushSparkToFront(random8());
    }

    memcpy8(pastTriggers, pastTriggers+1, FRAME_WINDOW-1);
    pastTriggers[FRAME_WINDOW-1] = isTriggered;


    sparks.update();
    sparks.draw(leds);
}

void listen() {
    memset8(im, 0, SAMPLE_COUNT);

    // Read some samples
    for (uint8_t i = 0; i < SAMPLE_COUNT; i++) {
        data[i] = analogRead(AUDIOPIN);
        _delay_us(50);
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
