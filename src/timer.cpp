#include "../include/timer.h"
#include "../include/config.h"

// constructor and destructor
Timer::Timer()
{
    reset();
}

Timer::~Timer()
{

}

void Timer::reset()
{
    m_pomodoros_completed = 0;
    set_state(Work);
    set_state(Pause); // start paused
}

// getters
int Timer::pomodoro_goal() const
{
    return m_config.pomodoro_goal();
}

int Timer::pomodoros_completed() const
{
    return m_pomodoros_completed;
}

Timer::State Timer::state() const
{
    return m_state;
}

Timer::State Timer::state_previous() const
{
    return m_state_previous;
}

float Timer::current_time() const
{
    const int minutes = m_seconds / 60;
    const int seconds = m_seconds % 60;
    return minutes + seconds/100.0;
}

// modifiers
void Timer::pomodoro_complete()
{
    ++m_pomodoros_completed;

    // which break state to activate depending on pomodoros completed
    const int goal = m_config.pomodoro_goal();
    if (m_pomodoros_completed < goal) {
        set_state(BreakShort);
    } else {
        set_state(BreakLong);
    }
}

void Timer::set_state(Timer::State state)
{
    m_state_previous = m_state;
    m_state = state;

    // when exiting pause state, leave m_seconds alone
    if (m_state_previous == Pause) {
        return;
    } // otherwise, feel free to reset m_seconds to default values

    switch (m_state)
    {
        case Work:
            m_seconds = m_config.work_length();
            break;

        case BreakShort:
            m_seconds = m_config.break_short_length();
            break;

        case BreakLong:
            m_seconds = m_config.break_long_length();
            break;

        case Pause: // do not change the time
            break;
    }
}

void Timer::tick()
{
    --m_seconds;
}
