#include <Arduino.h>
#include "../include/constants.h"
#include "../include/timer.h"
#include "SevSeg.h"
#include "TimerOne.h"

SevSeg sevseg; // instantiate a seven segment object
static const int debounceDelay = 50;

// TEMP: remove these prototypes later when refactoring
void tick();
void set_rgb_led(int pin, int value);
void update_display();

void setup() {
    byte numDigits = 4;
    byte digitPins[] = {
        pin::disp_digit1,
        pin::disp_digit2,
        pin::disp_digit3,
        pin::disp_digit4
    };
    byte segmentPins[] = {
        pin::disp_segA,
        pin::disp_segB,
        pin::disp_segC,
        pin::disp_segD,
        pin::disp_segE,
        pin::disp_segF,
        pin::disp_segG,
        pin::disp_segP
    };
    bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
    byte hardwareConfig = COMMON_CATHODE; // See README.md for options
    sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);

    Timer1.initialize();
    Timer1.attachInterrupt(tick);

    pinMode(pin::in_button1, INPUT);
    pinMode(pin::in_button2, INPUT);
    pinMode(pin::led_statusR, OUTPUT);
    pinMode(pin::led_statusG, OUTPUT);
    pinMode(pin::led_statusB, OUTPUT);
    pinMode(pin::audio_ticker, OUTPUT);
    pinMode(pin::led_countA, OUTPUT);
    pinMode(pin::led_countB, OUTPUT);
    pinMode(pin::led_countC, OUTPUT);
    pinMode(pin::led_countD, OUTPUT);

    // TEMP
    digitalWrite(pin::led_countA, HIGH);
    digitalWrite(pin::led_countB, HIGH);
    digitalWrite(pin::led_countC, HIGH);
    digitalWrite(pin::led_countD, HIGH);

    // show the timer right away at startup
    update_display();
}

Timer timer;
int button1_pressed = LOW;
int button2_pressed = LOW;
unsigned long button1_pressed_time = 0;
unsigned long button2_pressed_time = 0;
unsigned long button1_last_debounced_time = 0;
unsigned long button2_last_debounced_time = 0;
int pause_led_value = 0;
unsigned long pause_timer = 0;
int pause_led_pin = pin::led_statusG; // system starts in work mode
bool skip_next_tick = false;
bool next_state_pending = false;

void set_rgb_led(int pin, int value)
{
    analogWrite(pin, value);
}

void set_all_leds(int value)
{
    set_rgb_led(pin::led_statusR, value);
    set_rgb_led(pin::led_statusG, value);
    set_rgb_led(pin::led_statusB, value);
}

void work_loop()
{
    set_rgb_led(pin::led_statusR, 0);
    set_rgb_led(pin::led_statusG, 255);
    set_rgb_led(pin::led_statusB, 0);
}

void break_short_loop()
{
    set_rgb_led(pin::led_statusR, 0);
    set_rgb_led(pin::led_statusG, 0);
    set_rgb_led(pin::led_statusB, 255);
}

void break_long_loop()
{
    set_rgb_led(pin::led_statusR, 0);
    set_rgb_led(pin::led_statusG, 0);
    set_rgb_led(pin::led_statusB, 255);
}

void work()
{
    timer.set_state(Timer::Work);
}

void break_short()
{
    timer.set_state(Timer::BreakShort);
}

void break_long()
{
    timer.set_state(Timer::BreakLong);
}

void to_next_break()
{
    if (timer.pomodoros_completed() < timer.pomodoro_goal()) {
        break_short();
    } else {
        break_long();
    }
}

void pause()
{
    pause_led_value = 255;
    pause_timer = millis();
    const bool is_working = timer.state() == Timer::Work;

    // set opposite pins to 0
    set_rgb_led(is_working ? pin::led_statusB : pin::led_statusG, 0);
    set_rgb_led(pin::led_statusR, 0);

    // select colour of previous state
    pause_led_pin = is_working ? pin::led_statusG : pin::led_statusB;

    timer.set_state(Timer::Pause);
}

void to_next_state()
{
    switch(timer.state())
    {
        case Timer::Work:
            to_next_break();
            timer.pomodoro_complete();
            break;

        case Timer::BreakLong:
            timer.reset();
            break;

        case Timer::BreakShort:
            work();
            pause(); // start paused
            break;

        // theoretically will never occur due to check inside tick()
        case Timer::Pause:
            break;
    }
}

void restart_timer()
{
    // Timer1.start() immediately calls tick().. so one tick is skipped
    skip_next_tick = true;
    Timer1.start();
}

void update_display()
{
    sevseg.setNumber(timer.current_time(), 2);
}


void debounced_press(int pin, int& pressed, unsigned long& pressed_time, unsigned long& last_debounced_time, int value)
{
    const int reading = digitalRead(pin);

    // only record last pressed time when the button state changes
    if (reading != pressed) {
        last_debounced_time = millis();

        if (reading == HIGH) {
            pressed_time = millis();
        }

        pressed = reading;
    }

    if (millis() - last_debounced_time < debounceDelay) {
        return;
    }

    if (pressed == LOW && pressed_time > 0) {
        int pressed_duration = millis() - pressed_time;

        // NOTE: LONG PRESSES TEMPORARILY DISABLED
        if (pressed_duration > 0) {
            if (value > 0) {
                // left button
                if (timer.state() == Timer::Pause) {
                    timer.set_state(timer.state_previous());
                } else {
                    pause();
                }
            } else {
                // right button
                if (timer.state() == Timer::Work) {
                    break_short();
                } else if (timer.state() == Timer::BreakShort
                        || timer.state() == Timer::BreakLong) {
                    work();
                }

                // update the display immediately after switching states
                update_display(); // must be first because..
                restart_timer();
            }
        // } else if (pressed_duration >= 500 && pressed_duration < 1000) {
        //     // medium press
        // } else {
        //     // long press
            }
        pressed_time = 0;
    }
}

void pause_loop()
{
    static const int DELAY_TIME = 5;
    static int step = 1;

    if (millis() - pause_timer >= DELAY_TIME) {
        pause_timer = millis();

        pause_led_value += step;
        set_rgb_led(pause_led_pin, pause_led_value);

        if (pause_led_value >= 255) {
            step = -1;
        } else if (pause_led_value <= 0) {
            step = 1;
        }
    }
}

void toggle_led(int pin, bool on)
{
    digitalWrite(pin, on ? HIGH : LOW);
}

void toggle_leds(uint8_t count)
{
    toggle_led(pin::led_countA, count >= 1);
    toggle_led(pin::led_countB, count >= 2);
    toggle_led(pin::led_countC, count >= 3);
    toggle_led(pin::led_countD, count >= 4);
}

// tips: do not mess with memory + keep as short as possible
void tick() {
    if (skip_next_tick) {
        skip_next_tick = false;
        return;
    }

    if (timer.state() == Timer::Pause) {
        return;
    }

    if (timer.current_time() <= 0) {
        next_state_pending = true;
    } else {
        timer.tick(); // decrement time
    }

    if (timer.sound_tick()) {
        tone(pin::audio_ticker, 330, 5);
    }
    update_display();
}

void loop()
{
    // button 1 handler
    debounced_press(pin::in_button1, button1_pressed, button1_pressed_time, button1_last_debounced_time, 1);

    // button 2 handler
    debounced_press(pin::in_button2, button2_pressed, button2_pressed_time, button2_last_debounced_time, -1);

    if (next_state_pending) {
        to_next_state();
        next_state_pending = false;
    }

    // compiler warns if there is an unhandled enum state
    switch (timer.state())
    {
        case Timer::Work:
            work_loop();
            break;

        case Timer::BreakShort:
            break_short_loop();
            break;

        case Timer::BreakLong:
            break_long_loop();
            break;

        case Timer::Pause:
            pause_loop();
            break;
    }

    // TODO: choose better place to check LEDs?
    toggle_leds(timer.pomodoro_goal() - timer.pomodoros_completed());

    // must run repeatedly
    sevseg.refreshDisplay();
}
