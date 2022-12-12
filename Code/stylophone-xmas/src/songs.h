#include <Arduino.h>

// Song data as 2D array of [note, duration]
// Note: 1 is a C4, 24 is a B6, etc. 0 is a rest (no sound) 
// Duration: note/rest time in milliseconds

typedef struct {
  uint16_t note;
  uint16_t duration;
} note_t;

// Last christmas:
note_t last_christmas[] = {{8, 200}, {0, 200}, {6, 300}};

note_t * songs[] = {(note_t *) last_christmas};

