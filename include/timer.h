#pragma once
#include "config.h"

class Timer {
public:
    enum State
    {
        Work,
        BreakShort,
        BreakLong,
        Pause
    };

    Timer();
    ~Timer();
    void reset();

    // getters
    float current_time() const;
    int pomodoro_goal() const;
    int pomodoros_completed() const;
    bool sound_tick() const;
    State state() const;
    State state_previous() const;

    // modifiers
    void pomodoro_complete();
    void set_state(State state);

    void tick();

private:
    State m_state;
    State m_state_previous; // to return from Pause state
    Config m_config;

    int m_pomodoros_completed;
    int m_seconds;
};
