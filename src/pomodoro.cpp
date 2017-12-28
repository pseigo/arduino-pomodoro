#include "../include/globals.h"
#include "../include/pomodoro.h"

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
void reset_indicators()
{
    update_pomodoro_leds();
    set_all_leds_pwm(0);
}

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

// sets appropriate break state depending on number of pomodoros completed
void to_next_break()
{
    if (timer.pomodoros_completed() < timer.pomodoro_goal()) {
        break_short();
    } else {
        break_long();
    }
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
            reset_indicators();
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
