/* 
  I-Ching oracle consultation device.
  by Ferrie Bank - Amsterdam, 8 July 2020
*/

#define BUTTON_PIN 8
boolean but;
boolean loopForever = true;

/* 8x8 display init */

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
#define MAX_DEVICES 1

#define DATA_PIN  10
#define CS_PIN    11
#define CLK_PIN   12

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_MAX72XX MAX = MD_MAX72XX (HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
#define PAUSE_TIME 1000

uint8_t iching[] = {8, 0, 126, 126, 84, 84, 126, 126, 0};
uint8_t yinyang[] = {8,
  B00111100,
  B01111110,
  B11011111,
  B11111011,
  B10110001,
  B10000101,
  B01000010,
  B00111100
};

/* side LEDs init */

byte pat = 0;

const byte LED6_PIN = 7;
const byte LED5_PIN = 6;
const byte LED4_PIN = 5;
const byte LED3_PIN = 4;
const byte LED2_PIN = 3;
const byte LED1_PIN = 2;

/* music library init */

#include "chords.h"

const int whole_note_millis = 1000;
const int score_element_resolution = 16; // each element in the score array represents a 1/16 (= 1/resolution value) note duration.
const int score_note_millis = (whole_note_millis / score_element_resolution);

const int BUZZER_PIN = 9;

const byte effect_complete[] = {

    C, CS, D, DS, R, C, CS, D, DS, R, C, CS, D, DS
};

const byte effect_roll_stable[] = {

    C, C
};

const byte effect_roll_transient[] = {

    DS, DS
};

const byte music_iching[] = {
  
    E, E, E, E, R, R,
    CS, CS, CS, CS, CS, CS, R, R,
    CS, CS, CS, CS, CS, CS, CS, CS,
    R, R, R, R, R, R, R, R,
    CS, CS, CS, R, R, E,
    E, E, R, R, FS, FS, FS,
    R, R, FS, FS, FS, FS, R, R,
    E, E, E, E, E, E, E, E
};

void setup() {
  
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  P.begin();
  P.setSpeed(100);
  P.setPause(2000);
  P.setIntensity(0);
  P.setTextEffect(PA_GROW_UP, PA_GROW_DOWN);

  randomSeed(analogRead(0));

  pinMode (LED6_PIN, OUTPUT);
  pinMode (LED5_PIN, OUTPUT);
  pinMode (LED4_PIN, OUTPUT);
  pinMode (LED3_PIN, OUTPUT);
  pinMode (LED2_PIN, OUTPUT);
  pinMode (LED1_PIN, OUTPUT);

  P.addChar('#', yinyang);
  P.addChar('$', iching);
  
  P.write("#");

//  play_iching_tune();
  play_stable_roll();

//  P.write("$");
//  MAX.clear();
//  MAX.setRow(0, 3, B011010);

}

void loop() {

  but = digitalRead(BUTTON_PIN);
  if (!but) {
    pat = random(0, 64);
//    MAX.setRow(0, 4, pat);
    set_LEDs(pat);
    play_transient_roll();
  }

//  
//  if (loopForever && P.displayAnimate()) {
//
//    uint8_t hexagram = random(0, 64);
//    set_LEDs(hexagram);
//    
//    iching[4] = hexagram << 1;
//    iching[5] = hexagram << 1;
//    
//    P.addChar('$', iching);
////    play_stable_roll();
//    
//    P.setTextBuffer("#");
//    P.setCharSpacing(1);
//    P.displayReset();
//  }
}

void play_iching_tune () {
  play (music_iching, sizeof(music_iching), score_note_millis, BUZZER_PIN);
}

void play_stable_roll () {
  play (effect_roll_stable, sizeof(effect_roll_stable), score_note_millis, BUZZER_PIN);
}

void play_transient_roll () {
  play (effect_roll_transient, sizeof(effect_roll_transient), score_note_millis, BUZZER_PIN);
}

void play_complete_effect () {
  play (effect_complete, sizeof(effect_complete), score_note_millis, BUZZER_PIN);
}

void set_LEDs (byte pattern) {

  digitalWrite (LED6_PIN, (pattern & 32));
  digitalWrite (LED5_PIN, (pattern & 16));
  digitalWrite (LED4_PIN, (pattern & 8));
  digitalWrite (LED3_PIN, (pattern & 4));
  digitalWrite (LED2_PIN, (pattern & 2));
  digitalWrite (LED1_PIN, (pattern & 1));
}
