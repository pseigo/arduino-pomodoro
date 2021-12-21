# Arduino Study Timer

![YouTube: Arduino Pomodoro Study Timer](https://i.imgur.com/uFG53Cp.jpg)

for use as a pomodoro study timer.

## Libraries

- The official Arduino library
- [SevSeg *by Dean Reading*](https://platformio.org/lib/show/1372/SevSeg) (7 segment LCD dispay)
- [TimerOne](https://platformio.org/lib/show/131/TimerOne) (timed hardware interrupts)

(I installed these with the PlatformIO IDE)

## Usage

**Indicator LEDs**

RGB LED:

- *Green*: working
- *Blue*: having a break
- *Strobing*: paused

Four yellow LEDs indicate how many work periods are left to complete before a long break. An extinguished LED indicates a completed work period. When all work periods are complete, the LEDs turn back on.

**How to start**
Work periods begin paused. The timer may be (un)paused with the left push button.

If one of \[1, N\)th work periods completes, a short break begins.

If the Nth work period completes, a long break begins.

In this case, N is 4.

The right button manually changes between work and break states. The timer will be reset.

### Electrical circuit diagram

![electrical circuit diagram](https://i.imgur.com/NIzQO4M.png)
*[full size image!](https://i.imgur.com/NIzQO4M.png)*

### Prototype

![photo of timer](https://i.imgur.com/5Wle0Sk.jpg)
