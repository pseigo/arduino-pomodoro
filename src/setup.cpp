#include <Arduino.h>
#include "../include/globals.h"
#include "../include/loop.h"
#include "../include/pomodoro.h"

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

void setup_timer()
{
    Timer1.initialize();
    Timer1.attachInterrupt(tick);
}

void setup()
{
    setup_display();
    setup_pins();
    setup_timer();

    reset_indicators();
    // show the time immediately at startup
    update_display();
}
