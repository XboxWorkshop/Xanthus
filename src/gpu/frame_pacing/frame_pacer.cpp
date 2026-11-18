#include "frame_pacer.h"
#include "timer_resolution.h"
#include <algorithm>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include "precision_sleep_win10.h"
#endif

using Clock = FramePacer::Clock;

// The sleep timer's granularity, rounded up to whole clock ticks.
static Clock::duration GetTimerGranularity()
{
    return std::chrono::ceil<Clock::duration>(
        std::chrono::duration<double, std::milli>(TimerResolution::GetGranularityMs()));
}

void FramePacer::SetFpsLimit(double fpsLimit)
{
    if (fpsLimit > 0.0)
        m_frameTimeTarget = std::chrono::round<Clock::duration>(std::chrono::duration<double>(1.0 / fpsLimit));
}

void FramePacer::EndFrame(WaitMode waitMode)
{
    // Summarize stats for the current frame.
    const auto sleepStart = Clock::now();
    const auto renderTime = sleepStart - m_frameStart;

    auto sleepTime = m_frameTimeTarget - m_catchUp - renderTime;
    Clock::duration overrun {};
    if (sleepTime < Clock::duration::zero())
    {
        // Late frames (FPS drop) can't sleep.
        // Instead, carry this delay to the next frame.
        // That way, next frame can compensate by sleeping less..
        overrun = std::min(-sleepTime, m_frameTimeTarget);
        sleepTime = Clock::duration::zero();
    }

    // Sleep
    const auto deadline = sleepStart + sleepTime;
    SleepUntil(deadline, waitMode);

    // Restart calculation for new frame, minus however late we woke up.
    m_frameStart = Clock::now();
    m_catchUp = m_frameStart - deadline + overrun;
}

void FramePacer::SleepUntil(Clock::time_point deadline, WaitMode waitMode)
{
    // First pacer use on a thread raises its timer resolution; finer timer
    // ticks mean every mode wakes closer to its deadline.
    TimerResolution::GetGranularityMs();

    switch (waitMode)
    {
    case WaitMode::SleepOnly:
        SleepOnly(deadline);
        break;
    case WaitMode::SleepThenSpin:
        SleepThenSpin(deadline);
        break;
    case WaitMode::SleepThenYield:
        SleepThenYield(deadline);
        break;
    }
}

void FramePacer::SleepOnly(Clock::time_point deadline)
{
    // Hands the OS the full remaining time instead of spinning/yielding
    // through the final slice; it can wake up to one sleep quantum late,
    // never early.
    Clock::time_point now;
    while ((now = Clock::now()) < deadline)
    {
#ifdef _WIN32
        // High-resolution waitable timer; see precision_sleep_win10.h.
        if (PrecisionSleep::IsAvailable())
        {
            PrecisionSleep::Sleep(deadline - now);
            continue;
        }
#endif
        std::this_thread::sleep_for(deadline - now);
    }
}

void FramePacer::SleepThenSpin(Clock::time_point deadline)
{
    const auto timerGranularity = GetTimerGranularity();

    Clock::time_point now;
    while ((now = Clock::now()) < deadline)
    {
        const auto timeLeft = deadline - now;

        // Spin if below allocation granularity.
        if (timeLeft < timerGranularity)
        {
            Spin(deadline);
            break;
        }

        // One granularity is subtracted because a sleep can wake up to one
        // granularity tick late (per Windows docs); the spin finishes the
        // rest. The same logic applies to Linux.
        const auto sleepTimeLeft = timeLeft - timerGranularity;

#ifdef _WIN32
        // High-resolution waitable timer; see precision_sleep_win10.h.
        if (PrecisionSleep::IsAvailable())
        {
            PrecisionSleep::Sleep(sleepTimeLeft);
            continue;
        }
#else
        if (sleepTimeLeft > Clock::duration::zero())
            std::this_thread::sleep_for(sleepTimeLeft);
#endif
    }
}

void FramePacer::SleepThenYield(Clock::time_point deadline)
{
    const auto timerGranularity = GetTimerGranularity();

    Clock::time_point now;
    while ((now = Clock::now()) < deadline)
    {
        // One granularity is subtracted because a sleep can wake up to one
        // granularity tick late (per Windows docs); the yield finishes the
        // rest. The same logic applies to Linux.
        const auto sleepTimeLeft = deadline - now - timerGranularity;

#ifdef _WIN32
        // High-resolution waitable timer; see precision_sleep_win10.h.
        if (PrecisionSleep::IsAvailable() && sleepTimeLeft > Clock::duration::zero())
        {
            PrecisionSleep::Sleep(sleepTimeLeft);
            continue;
        }
#endif
        if (sleepTimeLeft > Clock::duration::zero())
            std::this_thread::sleep_for(sleepTimeLeft);
        else
            std::this_thread::yield();
    }
}

void FramePacer::Spin(Clock::time_point deadline)
{
    Clock::duration lastYieldTime {};
    Clock::time_point now;

    // Spin!!
    auto runSpinIteration = [&]()
    {
        const auto yieldStart = Clock::now();
        std::this_thread::yield();
        lastYieldTime = Clock::now() - yieldStart;
    };

    while ((now = Clock::now()) < deadline)
    {
        if (lastYieldTime > Clock::duration::zero())
        {
            if (deadline - now > lastYieldTime)
                runSpinIteration();
        }
        else
        {
            runSpinIteration();
        }
    }
}
