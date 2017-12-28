#pragma once

class Config
{
public:
    Config();
    ~Config();

    // getters
    int work_length() const;
    int break_short_length() const;
    int break_long_length() const;
    int pomodoro_goal() const;
    bool sound_tick() const;

    // modifiers

private:
    void reset();

    // length of each state in seconds
    int m_work_length;
    int m_break_short_length;
    int m_break_long_length;

    int m_pomodoro_goal;

    bool m_sound_tick;
};
