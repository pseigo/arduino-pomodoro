#include "../include/config.h"

#define DEVEL

// constructor and destructor
Config::Config()
{
    reset();
}

Config::~Config()
{

}

void Config::reset()
{
#ifdef DEVEL
    m_work_length = 8;
    m_break_short_length = 5;
    m_break_long_length = 10;
#else
    m_work_length = 60 * 25;
    m_break_short_length = 60 * 5;
    m_break_long_length = 60 * 10;
#endif
    m_pomodoro_goal = 4;
    m_sound_tick = true;
}

// getters
int Config::work_length() const
{
    return m_work_length;
}

int Config::break_short_length() const
{
    return m_break_short_length;
}

int Config::break_long_length() const
{
    return m_break_long_length;
}

int Config::pomodoro_goal() const
{
    return m_pomodoro_goal;
}

bool Config::sound_tick() const
{
    return m_sound_tick;
}

// modifiers
