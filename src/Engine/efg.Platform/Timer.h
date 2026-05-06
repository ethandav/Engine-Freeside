#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

class Timer
{
public:
    Timer()
    {
        QueryPerformanceFrequency(&m_frequency);
        QueryPerformanceCounter(&m_lastCounter);
    }

    float Tick()
    {
        LARGE_INTEGER currentCounter;
        QueryPerformanceCounter(&currentCounter);
        const LONGLONG elapsedCounts = currentCounter.QuadPart - m_lastCounter.QuadPart;
        m_lastCounter = currentCounter;

        return static_cast<float>(elapsedCounts) / static_cast<float>(m_frequency.QuadPart);
    }

private:
    LARGE_INTEGER m_frequency = {};
    LARGE_INTEGER m_lastCounter = {};
};