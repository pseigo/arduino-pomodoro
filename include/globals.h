#pragma once

#include <Arduino.h>
#include "constants.h"
#include "timer.h"
#include "SevSeg.h"
#include "TimerOne.h"

// objects
extern SevSeg sevseg; // seven segment object for display
extern Timer timer; // pomodoro timer

// button debounce
extern int button1_pressed;
extern int button2_pressed;
extern unsigned long button1_pressed_time;
extern unsigned long button2_pressed_time;
extern unsigned long button1_last_debounced_time;
extern unsigned long button2_last_debounced_time;

// pause loop
extern byte pause_led_pin;
extern byte pause_led_value;
extern unsigned long pause_timer;

// state switching
extern bool next_state_pending; // cue for next state
extern bool skip_next_tick;
