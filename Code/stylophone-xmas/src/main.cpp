#include <Arduino.h>
#include "songs.h"

int keys[] = {
  PIN_PC2, PIN_PC1, PIN_PC0, PIN_PB5, PIN_PB4,
  PIN_PB3, PIN_PB2, PIN_PB1, PIN_PB0, PIN_PA7, PIN_PA6, PIN_PA5,
  PIN_PF2, PIN_PF1, PIN_PF0, PIN_PE3, PIN_PE2,
  PIN_PE1, PIN_PE0, PIN_PD7, PIN_PD6, PIN_PD5, PIN_PD4, PIN_PD3
};

// frequencies from C4 to B5
unsigned int freqz[] = {
  261, 277, 293, 311, 329, 349, 369, 391, 415, 440, 466, 493,
  523, 554, 587, 622, 659, 698, 739, 783, 830, 880, 932, 987
};

uint32_t waitTime = 0;

#define KEY_COUNT 24
#define BUZZER_PIN PIN_PA2
#define BUTTON_PIN PIN_PF3

#define OE_PIN PIN_PC7
#define STORAGE_CLOCK_PIN PIN_PD0
#define SHIFT_CLOCK_PIN PIN_PD1
#define SHIFT_NRST_PIN PIN_PD2
#define DATA_PIN PIN_PC6

void shift_clock() {
  digitalWrite(SHIFT_CLOCK_PIN,   HIGH);
  //digitalWrite probably has enough delay to clock correctly 
  digitalWrite(SHIFT_CLOCK_PIN,   LOW);
}

void LED_show() {
  digitalWrite(STORAGE_CLOCK_PIN, LOW);
  digitalWrite(STORAGE_CLOCK_PIN, HIGH);
}

void LED_clear() {
  digitalWrite(SHIFT_NRST_PIN, LOW);
  digitalWrite(SHIFT_NRST_PIN, HIGH);

  LED_show();
}

void LED_on(int key) {
  LED_clear();
  digitalWrite(DATA_PIN, HIGH);
  shift_clock();
  digitalWrite(DATA_PIN, LOW);

  for (int i = 0; i < key; i++) {
    shift_clock();
  }

  LED_show();
}

int readKeys(){
  for (int i = 0; i < KEY_COUNT; i++) {
    if (digitalRead(keys[i]) == LOW) {
      return i;
    }
  }
  return -1;
}

void playSong(const note_t * song, int length){

  for (int i = 0; i < length; i++) {
    if (song[i].note == 0) {
      noTone(BUZZER_PIN);
      LED_clear();
      delay(song[i].duration);
      continue;
    }

    uint8_t note = song[i].note-1;

    tone(BUZZER_PIN, freqz[note]);
    LED_on(note);
    delay(song[i].duration);
  }

  noTone(BUZZER_PIN);
}

void learnSong(const note_t * song, int length){

  // start learning mode
  for (int i = 0; i < length; i++) {
    if(song[i].note == 0){
      noTone(BUZZER_PIN);
      LED_clear();
      delay(song[i].duration);
      continue;
    }

    uint8_t note = song[i].note-1;

    // turn on LED
    LED_on(note);

    // wait for note to be played
    while (readKeys() != note) { delay(10); }

    //start LED timer
    unsigned long start = millis();

    // wait for duration note to end
    while (millis() - start < song[i].duration)
    {

      if (readKeys() == note) {
        tone(BUZZER_PIN, freqz[note]);
      }else{
        noTone(BUZZER_PIN);
      }

      delay(10);
    }

    LED_clear();

    // wait for note to be released
    while (readKeys() == note) { delay(10); }

    noTone(BUZZER_PIN);

  }
}


void setup() {
  // init keys as input with pullup
  for (int i = 0; i < KEY_COUNT; i++) {
    pinMode(keys[i], INPUT_PULLUP);
  }

  // init buzzer as output
  pinMode(BUZZER_PIN, OUTPUT);

  // init button as input with pullup
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // init shift register pins as output
  pinMode(OE_PIN, OUTPUT);
  pinMode(STORAGE_CLOCK_PIN, OUTPUT);
  pinMode(SHIFT_CLOCK_PIN, OUTPUT);
  pinMode(SHIFT_NRST_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  // init shift registers
  digitalWrite(STORAGE_CLOCK_PIN, LOW);
  digitalWrite(SHIFT_CLOCK_PIN, LOW);
  LED_clear();
  digitalWrite(OE_PIN, LOW);

  // test all leds and buzzer
  for (int i = 0; i < KEY_COUNT; i++) {
    LED_on(i);
    tone(BUZZER_PIN, freqz[i]);
    delay(60);
  }

  //test buzzer
  tone(BUZZER_PIN, 440, 1000);

}

void loop() {
  // put your main code here, to run repeatedly:

  int key = readKeys();
  int songButton = digitalRead(BUTTON_PIN);

  if (key != -1 && songButton == LOW) {
    if(key<12)
      learnSong(songs[key], song_lengths[key]);
    else
      playSong(songs[key-12], song_lengths[key-12]);

    waitTime = 0;

  } else if (key != -1) {
    LED_on(key);
    tone(BUZZER_PIN, freqz[key]);
    waitTime = 0;
  } else {
    LED_clear();
    noTone(BUZZER_PIN);
  }

  // play snippet of song after 30 seconds
  waitTime += 10;
  if (waitTime > 30000) {
    playSong(songs[4], 10);
    waitTime = 0;
  }

  delay(10);
}