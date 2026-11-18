/*
    Sewer's Frame Pacer (2021)

    Allows frame pacing control of arbitrary code, such as a game logic loop (here)
    or animations.

    To use this class, simply place EndFrame at the end/exit points of a
    recurring logic loop. The FramePacer will do the rest.

    The Eggman Empire thanks you for your precise timing.
*/

#pragma once

#include <chrono>

class FramePacer
{
public:
    using Clock = std::chrono::steady_clock;

    /**
     * \brief How the remaining time to the next frame is waited out.
     */
    enum class WaitMode
    {
        /** \brief Sleeps most of it, then spins (yielding between
         * iterations) through the final slice. Most precise; negligible
         * CPU load. */
        SleepThenSpin,

        /** \brief Sleeps most of it, then yields through the final slice. */
        SleepThenYield,

        /** \brief Sleeps all of it; never spins or yields. Cheapest for
         * background threads; can wake up to one sleep quantum late. */
        SleepOnly
    };

    /**
     * \brief Sets the current framerate cap.
     * \param fpsLimit The framerate cap in frames per second. Values of zero
     * or below are ignored.
     */
    void SetFpsLimit(double fpsLimit);

    /**
     * \brief Marks the end of an individual frame/recurring piece of logic
     * to be performed/executed. You should put this at the end of a
     * reoccuring loop.
     * \param waitMode How the remaining wait time is spent. See WaitMode.
     */
    void EndFrame(WaitMode waitMode);

    /**
     * \brief Pauses execution until the next frame begins.
     * \param deadline The time the next frame begins at.
     * \param waitMode See EndFrame.
     */
    void SleepUntil(Clock::time_point deadline, WaitMode waitMode);

private:
    /* Waits until the deadline per WaitMode::SleepOnly. */
    void SleepOnly(Clock::time_point deadline);

    /* Waits until the deadline per WaitMode::SleepThenSpin. */
    void SleepThenSpin(Clock::time_point deadline);

    /* Waits until the deadline per WaitMode::SleepThenYield. */
    void SleepThenYield(Clock::time_point deadline);

    /* Spins until it is time to begin the next frame. */
    void Spin(Clock::time_point deadline);

    /* The current set maximum allowed time that a frame
       should be rendered in. 60 FPS until SetFpsLimit is called. */
    Clock::duration m_frameTimeTarget = std::chrono::nanoseconds(16666667);

    /* How late the last frame ended.

       Basically, sometimes we can oversleep a little; and we need to keep
       track of how much we overslept in the last frame.

       Likewise, if the game isn't fully running at max speed, then we may
       need to 'catch up', to the 100% rate. */
    Clock::duration m_catchUp {};

    /* Contains the stopwatch used for timing the frame time. */
    Clock::time_point m_frameStart = Clock::now();
};
