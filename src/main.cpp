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
    tick();
}

Timer timer;
int count = 0;
int button1_pressed = LOW;
int button2_pressed = LOW;
unsigned long button1_pressed_time = 0;
unsigned long button2_pressed_time = 0;
unsigned long button1_last_debounced_time = 0;
unsigned long button2_last_debounced_time = 0;
int pause_rgb_value = 0;

void to_next_state()
{
    switch(timer.state())
    {
        case Timer::Work:
            timer.set_state(timer.pomodoros_completed() < timer.pomodoro_goal() ?
                Timer::BreakShort :
                Timer::BreakLong);
            timer.pomodoro_complete();
            break;

        case Timer::BreakLong:
            timer.reset();
            break;

        case Timer::BreakShort:
            timer.set_state(Timer::Work);
            timer.set_state(Timer::Pause); // start paused
            break;

        // theoretically will never occur due to check inside tick()
        case Timer::Pause:
            break;
    }
}

// tips: do not mess with memory + keep as short as possible
void tick() {
    if (timer.state() == Timer::Pause) {
        return;
    }

    const float time = timer.current_time();
    if (time <= 0) {
        to_next_state();
    }

    tone(pin::audio_ticker, 330, 5); // play tone
    timer.tick(); // decrement time
    sevseg.setNumber(time, 2); // update display
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
                    pause_rgb_value = 255; // reset RGB value
                    timer.set_state(Timer::Pause);
                }
            } else {
                // right button
                if (timer.state() == Timer::Work) {
                    timer.set_state(Timer::BreakShort);
                } else if (timer.state() == Timer::BreakShort
                        || timer.state() == Timer::BreakLong) {
                    timer.set_state(Timer::Work);
                }

                // update the display immediately after switching states
                tick();
            }
        // } else if (pressed_duration >= 500 && pressed_duration < 1000) {
        //     // medium press
        // } else {
        //     // long press
            }
        pressed_time = 0;
    }
}

void set_rgb_led(int pin, int value)
{
    analogWrite(pin, value);
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

void pause_loop()
{
    // select colour of previous state
    const int pin = timer.state_previous() == Timer::Work ?
        pin::led_statusG : pin::led_statusB;

    // set opposite colour to 0
    set_rgb_led(timer.state_previous() == Timer::Work ?
        pin::led_statusB : pin::led_statusG, 0);

    static unsigned long timer = millis();
    static int deci_seconds = 0;
    static const int delay_time = 5;
    static int step = 1;

    if (millis() - timer >= delay_time) {
        timer += delay_time;

        // equal to 100 ms
        ++deci_seconds;

        // reset to 0 after counting for 10000 deciseconds
        if (deci_seconds >= 10000) {
            deci_seconds = 0;
        }

        pause_rgb_value += step;
        set_rgb_led(pin, pause_rgb_value);

        if (pause_rgb_value >= 255) {
            step = -1;
        } else if (pause_rgb_value <= 0) {
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

void loop()
{
    // button 1 handler
    debounced_press(pin::in_button1, button1_pressed, button1_pressed_time, button1_last_debounced_time, 1);

    // button 2 handler
    debounced_press(pin::in_button2, button2_pressed, button2_pressed_time, button2_last_debounced_time, -1);

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
