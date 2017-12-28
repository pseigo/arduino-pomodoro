#include <Arduino.h>
#include "../include/constants.h"
#include "../include/timer.h"
#include "SevSeg.h"
#include "TimerOne.h"

// objects
SevSeg sevseg; // seven segment object for display
Timer timer; // pomodoro timer

// button debounce
int button1_pressed = LOW;
int button2_pressed = LOW;
unsigned long button1_pressed_time = 0;
unsigned long button2_pressed_time = 0;
unsigned long button1_last_debounced_time = 0;
unsigned long button2_last_debounced_time = 0;
static const int debounce_threshold = 50;

// pause loop
byte pause_led_pin = pin::led_statusG; // system starts in work mode
byte pause_led_value = 0;
unsigned long pause_timer = 0;

// state switching
bool next_state_pending = false; // cue for next state
bool skip_next_tick = false;

// prototypes
void tick();
void update_pomodoro_leds();
void set_all_leds_pwm(int value);

void setup_display()
{
    byte hardware_config = COMMON_CATHODE; // See README.md for options
    byte num_digits = 4;
    bool resistors_on_segments = false; // 'false' means resistors are on digit pins

    byte digit_pins[] = {
        pin::disp_digit1,
        pin::disp_digit2,
        pin::disp_digit3,
        pin::disp_digit4
    };
    byte segment_pins[] = {
        pin::disp_segA,
        pin::disp_segB,
        pin::disp_segC,
        pin::disp_segD,
        pin::disp_segE,
        pin::disp_segF,
        pin::disp_segG,
        pin::disp_segP
    };

    sevseg.begin(hardware_config, num_digits, digit_pins, segment_pins, resistors_on_segments);
}

void setup_pins()
{
    // inputs
    pinMode(pin::in_button1, INPUT);
    pinMode(pin::in_button2, INPUT);

    // outputs
    pinMode(pin::led_statusR, OUTPUT);
    pinMode(pin::led_statusG, OUTPUT);
    pinMode(pin::led_statusB, OUTPUT);
    pinMode(pin::audio_ticker, OUTPUT);
    pinMode(pin::led_countA, OUTPUT);
    pinMode(pin::led_countB, OUTPUT);
    pinMode(pin::led_countC, OUTPUT);
    pinMode(pin::led_countD, OUTPUT);
}

void setup_indicators()
{
    update_pomodoro_leds();
    set_all_leds_pwm(0);
}

void setup_timer()
{
    Timer1.initialize();
    Timer1.attachInterrupt(tick);
}

void restart_timer()
{
    // Timer1.start() immediately calls tick(), causing some timers to 'lose' a second
    skip_next_tick = true;
    Timer1.start();
}

void update_display()
{
    sevseg.setNumber(timer.current_time(), 2);
}

// LED modifiers
void set_led_pwm(byte pin, int value)
{
    analogWrite(pin, value);
}

void set_all_leds_pwm(int value)
{
    set_led_pwm(pin::led_statusR, value);
    set_led_pwm(pin::led_statusG, value);
    set_led_pwm(pin::led_statusB, value);
}

void toggle_led(byte pin, bool on)
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

void update_pomodoro_leds()
{
    // toggle # of leds according to Pomodoros completed
    toggle_leds(timer.pomodoro_goal() - timer.pomodoros_completed());
}

// indicator LED loops
void work_loop()
{
    set_led_pwm(pin::led_statusR, 0);
    set_led_pwm(pin::led_statusG, 255);
    set_led_pwm(pin::led_statusB, 0);
}

void break_short_loop()
{
    set_led_pwm(pin::led_statusR, 0);
    set_led_pwm(pin::led_statusG, 0);
    set_led_pwm(pin::led_statusB, 255);
}

void break_long_loop()
{
    set_led_pwm(pin::led_statusR, 0);
    set_led_pwm(pin::led_statusG, 0);
    set_led_pwm(pin::led_statusB, 255);
}

void pause_loop()
{
    static const int DELAY_TIME = 5;
    static int8_t step = 1;

    if (millis() - pause_timer >= DELAY_TIME) {
        pause_timer = millis();

        pause_led_value += step;
        set_led_pwm(pause_led_pin, pause_led_value);

        if (pause_led_value == 255) {
            step = -1;
        } else if (pause_led_value == 0) {
            step = 1;
        }
    }
}

// timer state modifiers
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

// sets appropriate break state depending on number of pomodoros completed
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
    set_led_pwm(is_working ? pin::led_statusB : pin::led_statusG, 0);
    set_led_pwm(pin::led_statusR, 0);

    // pulse colour of previous state
    pause_led_pin = is_working ? pin::led_statusG : pin::led_statusB;

    timer.set_state(Timer::Pause);
}

// handles automatic switching of states
void to_next_state()
{
    switch(timer.state())
    {
        case Timer::Work:
            to_next_break();
            timer.pomodoro_complete();
            update_pomodoro_leds();
            break;

        case Timer::BreakLong:
            timer.reset();
            setup_indicators();
            break;

        case Timer::BreakShort:
            work();
            pause(); // start paused
            break;

        // theoretically will never occur due to check inside tick()
        case Timer::Pause:
            break;
    }

    update_display();
}

void toggle_pause()
{
    if (timer.state() == Timer::Pause) {
        timer.set_state(timer.state_previous());
    } else {
        pause();
    }
}

void toggle_break()
{
    if (timer.state() == Timer::Work) {
        break_short();
    } else if (timer.state() == Timer::BreakShort
            || timer.state() == Timer::BreakLong) {
        work();
    }

    // update the display immediately after switching states
    update_display();
    restart_timer();
}

void debounced_press(byte pin, int& pressed, unsigned long& pressed_time, unsigned long& last_debounced_time)
{
    const byte reading = digitalRead(pin);

    // only record last pressed time when the button state changes
    if (reading != pressed) {
        last_debounced_time = millis();

        if (reading == HIGH) {
            pressed_time = millis();
        }

        pressed = reading;
    }

    // filter out presses shorter than threshold
    if (millis() - last_debounced_time < debounce_threshold) {
        return;
    }

    if (pressed == LOW && pressed_time > 0) {
        const int pressed_duration = millis() - pressed_time;

        // NOTE: LONG PRESSES TEMPORARILY DISABLED
        if (pressed_duration > 0) {
            if (pin == pin::in_button1) {
                toggle_pause();
            } else {
                toggle_break();
            }
        }
        pressed_time = 0;
    }
}

// tips: do not mess with memory + keep as short as possible
void tick()
{
    if (skip_next_tick) {
        skip_next_tick = false;
        return;
    }

    if (timer.state() == Timer::Pause) {
        return;
    }

    if (timer.current_time() <= 0) {
        // flag so that hardware interrupt does not directly perform intensive tasks
        next_state_pending = true;
    } else {
        timer.tick(); // decrement time
    }

    if (timer.sound_tick()) {
        tone(pin::audio_ticker, 330, 5);
    }
    update_display();
}

void setup()
{
    setup_display();
    setup_pins();
    setup_timer();

    setup_indicators();
    // show the time immediately at startup
    update_display();
}

void loop()
{
    // button 1 handler
    debounced_press(pin::in_button1, button1_pressed, button1_pressed_time, button1_last_debounced_time);

    // button 2 handler
    debounced_press(pin::in_button2, button2_pressed, button2_pressed_time, button2_last_debounced_time);

    if (next_state_pending) {
        to_next_state();
        next_state_pending = false;
    }

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

    // must run repeatedly
    sevseg.refreshDisplay();
}
