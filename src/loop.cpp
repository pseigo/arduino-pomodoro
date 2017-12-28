#include "../include/globals.h"
#include "../include/loop.h" // not necessary in this file
#include "../include/pomodoro.h"

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
