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

    // getters
    int pomodoro_goal() const;
    int pomodoros_completed() const;
    State state() const;
    State state_previous() const;
    float current_time() const;

    // modifiers
    void pomodoro_complete();
    void set_state(State state);

    void tick();

private:
    void reset();

    State m_state;
    State m_state_previous; // to return from Pause state
    Config m_config;

    int m_pomodoros_completed;
    int m_seconds;
};
