#pragma once

#include <functional>

namespace xanthus {
/**
 * @brief A callback fired when initialization completes.
 *
 * This represents your program's initialization.
 * Here, you will initialize whatever your project needs to run.
 * We choose to wrap initialization so we can serve as a unified front for your
 * ports: initialize us, initialize you, then run your project.
 *
 * Pass through argc and argv. Raise errors if something fails.
 */
using InitializationCallback = std::function<void(int, char *[])>;

/**
 * @brief Xanthus project initialization.
 *
 * This is where Xanthus boots up core subsystems
 * and sets defaults chosen by your program (see ).
 * Once initialization is complete, it calls into your program's.
 */
void initialize();

/**
 * @brief Xanthus event processing.
 *
 * This is where Xanthus runs per-frame code, such as video
 * audio, and processing SDL3 events. Call it in a while loop
 * and simulate a tick / frame of your game.
 */
void processEvents();
}
