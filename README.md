# arduino-pomodoro
A Pomodoro timer built for Arduino.

# Background
For the final project of my Computer Science 3 High School course, we were given the freedom of developing any project related to the course. I decided to build a study timer for the Arduino microcontroller; specifically, for a spare Freaduino Arduino Mega 2560. This was my first experience with assembling a hardware project as well as writing embedded software.

## The Pomodoro Technique
The Pomodoro Technique is simple, yet effective. The idea is to break down work into intervals with short breaks in between. The work intervals are often referred to as *pomodoros*. Typically, each *pomodoro* is 25 minutes in length. You start the timer and work on a **single** task for the entire duration. Once the timer finishes, you stop working and take a short 3 to 5 minute break. This process is repeated up to 4 times, at which point you take a long break between 15 and 30 minutes and reset the count to zero.

In addition, *pomodoros* are indivisible. If a *pomodoro* is interrupted, it must be abandoned. The purpose of this is to reduce and eliminate internal and external distractions and to ultimately work with 100% focus.

The key principles that make this method so effective are as follows:
- Work on one specific task
- Completing a *pomodoro* should feel rewarding
- Eliminating distractions to achieve *focus* and *flow*

*Note: Lengths for the work and break periods for my project are completely customizable in `src/config.cpp`.*

# What did I get out of this project?
- An introduction to the world of electronics!
- Using issues on GitHub
- The importance of optimizing for limited hardware
- The fundamentals of circuits
  - Visualizing current
  - Cathodes and anodes—their significance
  - Resistors—why they're so important
  - Transistors
  - Capacitors—for switch debouncing, although I ended up doing it in software
  - Light-emitting diodes (LED)
- Techniques for interacting with hardware
  - The idea of *high* and *low* states
  - Pulse-width modulation (PWM)
  - Common-cathode and common-anode in the context of segmented LCD displays
  - Wrapping my mind around the idea of a constantly looping program
  - Managing state with enumerations
- An introduction to and application of the software development life cycle
  - Proof of concept
  - Hardware prototype
  - Software prototype
  - Testing
  - Debugging
  - Finalizing/polishing

# Libraries
I used several libraries which were installed using the PlatformIO IDE:
- The official Arduino library
- [SevSeg *by Dean Reading*](http://platformio.org/lib/show/1372/SevSeg)—for the 7 segment LCD dispay
- [TimerOne](http://platformio.org/lib/show/131/TimerOne)—to utilize the Arduino's hardware timer for precise "ticks"

# Struggles
The most challenging part of this project was (is) fixing strange bugs and making final changes. However, this is arguably the most important stage as it takes the project from feeling unstable to well-polished.

I learned a lot and look forward to building my next Arduino project! The next steps include either printing a PCB or manually wiring and soldering the components on a stripboard.

# Usage
## Text explanation
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

# Media
## Video
[![YouTube: Arduino Pomodoro Study Timer](https://i.imgur.com/uFG53Cp.jpg)](https://www.youtube.com/watch?v=Qk93Z4uxi0s)

[https://www.youtube.com/watch?v=Qk93Z4uxi0s](https://www.youtube.com/watch?v=Qk93Z4uxi0s)

## Images
### Electrical circuit diagram
![electrical circuit diagram](https://i.imgur.com/NIzQO4M.png)
*[full size image!](https://i.imgur.com/NIzQO4M.png)*

### Prototype
![photo of timer](https://i.imgur.com/5Wle0Sk.jpg)
