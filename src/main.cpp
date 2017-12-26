#include <Arduino.h>
#include "../include/constants.h"
#include "SevSeg.h"
#include "TimerOne.h"

SevSeg sevseg; // instantiate a seven segment object
static const int delayTime = 5;
static const int debounceDelay = 50;
int seconds = 60 * 15;

void tick();

void setup() {
    Serial.begin(115200);
    Serial.println("Console setup complete.");

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

    tick();
}

int count = 0;
int button1_pressed = LOW;
int button2_pressed = LOW;
unsigned long button1_pressed_time = 0;
unsigned long button2_pressed_time = 0;
unsigned long button1_last_debounced_time = 0;
unsigned long button2_last_debounced_time = 0;

// void checkIncrement(int pin, bool& pressed, int value) {
//
//     int reading = digitalRead(pin);
//
//     if (reading == HIGH) {
//         if (!pressed) {
//             Serial.print("Pin ");
//             Serial.print(pin);
//             Serial.print(" ");
//             Serial.println(value);
//
//             pressed = true;
//
//             count += value;
//             sevseg.setNumber(count);
//         }
//     } else {
//         pressed = false;
//     }
// }

void tick() {
    tone(pin::audio_ticker, 330, 5);
    seconds--;

    const int minutes = seconds / 60;
    const int seconds_difference = seconds - minutes*60;
    const float time = minutes + seconds_difference/100.0;

    sevseg.setNumber(time, 2);
}

void debounced_press(int pin, int& pressed, unsigned long& pressed_time, unsigned long& last_debounced_time, int value) {
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
        // Serial.print("pressed_time: ");
        // Serial.print(pressed_time);
        // Serial.print(" | pressed_duration: ");
        // Serial.println(pressed_duration);

        if (pressed_duration < 500) {
            if (value > 0) {
                Timer1.start();
            } else {
                Timer1.stop();
            }
            // sevseg.setNumber(count += value * 5);
        } else if (pressed_duration >= 500 && pressed_duration < 1000) {
            // sevseg.setNumber(count += value * 50
            Timer1.stop();
        } else {
            // sevseg.setNumber(count += value * 500);
            Timer1.stop();
        }

        // analogWrite(pin::led_statusR, (count%10)/10.0*255);
        // analogWrite(pin::led_statusG, (count%100)/10/10.0*255);
        // analogWrite(pin::led_statusB, (count%1000)/100/10.0*255);
        pressed_time = 0;
    }
}

void light_show() {
    static unsigned long timer = millis();
    static int deciSeconds = 0;

    if (millis() - timer >= delayTime) {
      timer += delayTime;

      // sevseg.setNumber(count--);

      deciSeconds++; // 100 milliSeconds is equal to 1 deciSecond

      if (deciSeconds == 10000) { // Reset to 0 after counting for 1000 seconds.
        deciSeconds=0;
      }
      analogWrite(pin::led_statusR, (deciSeconds%100)/10/10.0*255);
      analogWrite(pin::led_statusG, (deciSeconds%1000)/100/10.0*255);
      analogWrite(pin::led_statusB, (deciSeconds%10000)/1000/10.0*255);
      // sevseg.setNumber(deciSeconds, 1);
    }
}

void loop() {
    // checkIncrement(pin::in_button1, button1_pressed, 1);
    // checkIncrement(pin::in_button2, button2_pressed, -1);

    debounced_press(pin::in_button1, button1_pressed, button1_pressed_time, button1_last_debounced_time, 1);
    debounced_press(pin::in_button2, button2_pressed, button2_pressed_time, button2_last_debounced_time, -1);

    light_show();

    sevseg.refreshDisplay(); // Must run repeatedly
}
