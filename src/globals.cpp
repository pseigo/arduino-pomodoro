#include "../include/globals.h"

SevSeg sevseg; // seven segment object for display
Timer timer; // pomodoro timer

// button debounce
int button1_pressed = LOW;
int button2_pressed = LOW;
unsigned long button1_pressed_time = 0;
unsigned long button2_pressed_time = 0;
unsigned long button1_last_debounced_time = 0;
unsigned long button2_last_debounced_time = 0;

// pause loop
byte pause_led_pin = pin::led_statusG; // system starts in work mode
byte pause_led_value = 0;
unsigned long pause_timer = 0;

// state switching
bool next_state_pending = false; // cue for next state
bool skip_next_tick = false;
