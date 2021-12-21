# Arduino Study Timer

[![YouTube: Arduino Pomodoro Study Timer](https://i.imgur.com/uFG53Cp.jpg)](https://www.youtube.com/watch?v=Qk93Z4uxi0s)

A Pomodoro study timer built using an Arduino.

## Libraries

I used several libraries which were installed using the PlatformIO IDE:

- The official Arduino library
- [SevSeg *by Dean Reading*](http://platformio.org/lib/show/1372/SevSeg)—for the 7 segment LCD dispay
- [TimerOne](http://platformio.org/lib/show/131/TimerOne)—to utilize the Arduino's hardware timer for precise "ticks"

## Usage

**Indicator LED!**
The RGB LED indicates which state you are in.
- *Solid green*: working
- *Strobe green*: working, but paused
- *Solid blue*: on a break
- *Strobe blue*: on a break, but paused

**Keeping track of how many *pomodoros* have been completed!**
Four yellow LEDs indicate how many *pomodoros* are left to complete before a long break. After completing a *pomodoro*, an LED is turned off. Once all four *pomodoros* have been completed, the LEDs will reset.

**Pause and unpause states!**
Each work period begins in a pause state. The left push button pauses and unpauses the timer, as indicated by the indicator LED.

**Manually changing work/break states!**
The right button manually changes between work and break states. The timer will be reset to its configured value.

**Give me a break!**
After completing a *pomodoro*, the short break state will automatically begin. If four *pomodoros* have been completed, a long break state will begin.

## Media

📽 **Watch on YouTube**: [https://www.youtube.com/watch?v=Qk93Z4uxi0s](https://www.youtube.com/watch?v=Qk93Z4uxi0s)

### Electrical circuit diagram

![electrical circuit diagram](https://i.imgur.com/NIzQO4M.png)
*[full size image!](https://i.imgur.com/NIzQO4M.png)*

### Prototype

![photo of timer](https://i.imgur.com/5Wle0Sk.jpg)
