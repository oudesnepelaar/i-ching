#include <MD_MAX72xx.h>
#include <SPI.h>
#include <MD_Parola.h>
#include "chords.h"
#include "hexagram.h"
/* 
  I-Ching oracle consultation device.
  by Ferrie Bank - Amsterdam, 8 July 2020
*/

#define BUTTON_PIN 8
boolean but;

/* 8x8 display init */

#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
#define MAX_DEVICES 1

#define DATA_PIN  10
#define CS_PIN    11
#define CLK_PIN   12

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_MAX72XX *MAX = P.getGraphicObject();

#define PAUSE_TIME 1000

uint8_t OPEN = B01100110;
uint8_t CLOSED = B01111110;

uint8_t hexagram[6];
uint8_t hexaValue = 0;
uint8_t transHexagram[6];
uint8_t transHexaValue = 0;
uint8_t segIndex = 0;

char today[64];
char tomorrow[64];

boolean busyRolling = false;

uint8_t ledPattern = 0;
uint8_t blinkIndex = 0;
uint16_t blinkCount = 0;

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

  Serial.begin(9600);
  digitalWrite(LED_BUILTIN, LOW);

  P.begin();
  P.setSpeed(100);
  P.setPause(2000);
  P.setIntensity(0);
  P.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  initDisplay();

  randomSeed(analogRead(0));

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode (LED6_PIN, OUTPUT);
  pinMode (LED5_PIN, OUTPUT);
  pinMode (LED4_PIN, OUTPUT);
  pinMode (LED3_PIN, OUTPUT);
  pinMode (LED2_PIN, OUTPUT);
  pinMode (LED1_PIN, OUTPUT);
  setLEDs(0);

  play_complete();
}

void initDisplay() {
    
  MAX->clear();
}

void loop() {

  but = digitalRead(BUTTON_PIN);
  if (!but && !busyRolling) {
    
    rollSegment();
    delay(800);
  }

  updateLEDs();
}

void reset() {

  segIndex = 0;
  hexaValue = 0;
  transHexaValue = 0;
  ledPattern = 0;
  blinkIndex = 0;

  updateLEDs();
  printHexagram(hexagram);
  
  busyRolling = false;

  play_complete();
}

void rollSegment() {

  busyRolling = true;
  
  if (segIndex > 5) {

    reset();
    return;
  }

  uint8_t segment = random(0, 64);
  boolean isClosed;
  
  if (segment >= 32) {
    
    hexaValue = setBit (hexaValue, segIndex, false);
    hexagram[segIndex] = OPEN;
    isClosed = false;
    
  } else {
    
    hexaValue = setBit (hexaValue, segIndex, true);
    hexagram[segIndex] = CLOSED;
    isClosed = true;
  }

  uint8_t transcendence = random(0,64);
  
  if (transcendence >= 32) {

    ledPattern = (ledPattern | (1 << segIndex));
    transHexaValue = setBit (transHexaValue, segIndex, !isClosed);
    transHexagram[segIndex] = ((isClosed) ? OPEN : CLOSED);
    play_transient();
    
  } else {
    
    transHexaValue = setBit (transHexaValue, segIndex, isClosed);
    transHexagram[segIndex] = ((isClosed) ? CLOSED : OPEN);
    play_stable();
  }

  segIndex++;
  blinkIndex++;

  updateLEDs();
  
  printHexagram(hexagram);
  busyRolling = false;

  if (segIndex > 5) {

    delay(800);
    setDescription();
    play_iching();
    scrollDescription();
    printHexagram(transHexagram);
    play_iching();
    scrollTransientDescription();
    cycleHexagrams();
  }
}

uint8_t setBit (uint8_t origValue, uint8_t index, boolean setToOne) {

  uint8_t result;

  if (setToOne) {
    result = origValue | (1 << index);
  } else {
    result = origValue & ~(1 << index);
  }

  return result;
}

void printHexagram(uint8_t hexagram[]) {

  initDisplay();
  
  for (uint8_t i = 0; i < segIndex; i++) {
    MAX->setRow(0, 0, (7 - (i + 1)), hexagram[i]);  
  }
}

void setDescription() {

  strcpy_P(today, (char *) pgm_read_word(&(descriptions[hexaValue])));
  strcpy_P(tomorrow, (char *) pgm_read_word(&(descriptions[transHexaValue])));
}

void scrollDescription() {

  initDisplay();
  
  char text[75] = " NOW: ";
  strcat(text, today);
  
  P.setTextBuffer(text);
  P.displayReset();
  while (!P.displayAnimate()){;};
}

void scrollTransientDescription() {

  initDisplay();

  char text[75] = " LATER: ";
  strcat(text, tomorrow);

  P.setTextBuffer(text);
  P.displayReset();
  while (!P.displayAnimate()){;};
}

void cycleHexagrams() {

  but = digitalRead(BUTTON_PIN);

  char kwhex[5];
  kwhex[0] = ' ';
  strncat(kwhex, today, 4);
  kwhex[3] = ' ';
  kwhex[4] = '\0';
  
  char kwtrans[5];
  kwtrans[0] = ' ';
  strncat(kwtrans, tomorrow, 4);
  kwtrans[3] = ' ';
  kwtrans[4] = '\0';

  while(but) {
    
    printHexagram(hexagram);
    delay(2000);

    if(!digitalRead(BUTTON_PIN)) {
      reset;
      return;
    }

    showText(kwhex);

    if(!digitalRead(BUTTON_PIN)) {
      reset;
      return;
    }
    
    printHexagram(transHexagram);
    delay(2000);

    if(!digitalRead(BUTTON_PIN)) {
      reset;
      return;
    }

    showText(kwtrans);
    but = digitalRead(BUTTON_PIN);
  }
}

void showText(char text[]) {
  
  P.setTextBuffer(text);
  P.displayReset();
  while (!P.displayAnimate()){;};
}

void updateLEDs() {

  uint8_t burnPattern = ledPattern;
  
  if (blinkCount > 10000) {
    burnPattern = burnPattern | (1 << blinkIndex);
  } else {
    burnPattern = burnPattern & ~(1 << blinkIndex);
  }
  
  if (blinkCount > 16000) blinkCount = 0;
  blinkCount++;
  
  setLEDs(burnPattern);
}

void setLEDs (byte pattern) {

  digitalWrite (LED6_PIN, (pattern & 32));
  digitalWrite (LED5_PIN, (pattern & 16));
  digitalWrite (LED4_PIN, (pattern & 8));
  digitalWrite (LED3_PIN, (pattern & 4));
  digitalWrite (LED2_PIN, (pattern & 2));
  digitalWrite (LED1_PIN, (pattern & 1));
}

void play_iching () {
  play (music_iching, sizeof(music_iching), score_note_millis, BUZZER_PIN);
}

void play_stable () {
  play (effect_roll_stable, sizeof(effect_roll_stable), score_note_millis, BUZZER_PIN);
}

void play_transient () {
  play (effect_roll_transient, sizeof(effect_roll_transient), score_note_millis, BUZZER_PIN);
}

void play_complete () {
  play (effect_complete, sizeof(effect_complete), score_note_millis, BUZZER_PIN);
}
