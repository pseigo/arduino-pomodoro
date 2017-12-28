#include "../include/config.h"

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
    m_work_length = 8; // 60 * 25;
    m_break_short_length = 5; // 60 * 5;
    m_break_long_length = 10; // 60 * 10;

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
