/*
    Timer Resolution

        Raises the platform timer resolution and reports the achieved
        granularity, used by the frame pacer to decide when sleeping is
        accurate enough.
*/

#pragma once

#ifdef __linux__
/* Timer slack is per-thread on Linux, so the state must be too. */
#define TIMER_RESOLUTION_THREAD_LOCAL thread_local
#else
#define TIMER_RESOLUTION_THREAD_LOCAL
#endif

class TimerResolution
{
public:
    /**
     * \brief Raises the timer resolution to the finest value the platform
     * supports. Safe to call again; only the first successful call applies.
     * \return True when the platform supports raising the resolution.
     */
    static bool SetHighestResolution();

    /**
     * \brief Gets the timer granularity in milliseconds. Raises the
     * resolution on first call if SetHighestResolution was never called.
     * \return The timer granularity in milliseconds; 0.5 when the platform
     * does not support raising it.
     */
    static double GetGranularityMs();

    /**
     * \brief Releases the timer resolution request made by
     * SetHighestResolution. No-op when the resolution was never raised.
     *
     * Linux: Always a no-op; the timer slack is per-thread.
     */
    static void Release();

private:
    /* Achieved granularity in milliseconds; 0.5 until proven finer. */
    static TIMER_RESOLUTION_THREAD_LOCAL double s_granularityMs;

    /* Whether SetHighestResolution successfully raised the resolution. */
    static TIMER_RESOLUTION_THREAD_LOCAL bool s_isActive;

    /* Whether a raise was attempted, so failed attempts are not retried. */
    static TIMER_RESOLUTION_THREAD_LOCAL bool s_isResolved;
};
