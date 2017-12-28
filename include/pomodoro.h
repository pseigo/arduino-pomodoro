#pragma once

void restart_timer();
void update_display();

// LED modifiers
void reset_indicators();
void set_led_pwm(byte pin, int value);
void set_all_leds_pwm(int value);
void toggle_led(byte pin, bool on);
void toggle_leds(uint8_t count);
void update_pomodoro_leds();

void work();
void break_short();
void break_long();
void pause();

void to_next_break();
void to_next_state();

void toggle_pause();
void toggle_break();
