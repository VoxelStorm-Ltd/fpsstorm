# FPSStorm

FPSStorm is a small C++ framerate reporting and limiting library used in VoxelStorm projects such as **AdvertCity** and **sphereFACE**. A single `fpsstorm` object tracks a loop's performance, optionally limits its frame rate, and calls application callbacks to adjust workload when performance changes.

## Features

- **Frame pacing:** sleep until the next frame is due, or poll a non-blocking deadline to decide whether to render.
- **Periodic FPS readings:** count frames over a configurable reporting interval; defaults are a 60 FPS cap, a 40 FPS performance target, and a four-second reporting interval.
- **Performance feedback:** callbacks let the application reduce or increase rendering detail according to measured FPS. The target is separate from the cap.
- **Session statistics:** report minimum, maximum, average, and the proportion of reporting intervals meeting the target. The destructor prints statistics to standard output if any measurements were collected.

Compile [fpsstorm/fpsstorm.cpp](fpsstorm/fpsstorm.cpp) into your application and include [fpsstorm/fpsstorm.h](fpsstorm/fpsstorm.h). There are no third-party dependencies: the implementation uses the C++ standard library, including `<chrono>` and `<thread>`. The source uses C++11 features and GNU-style attributes; GCC and Clang are suitable compilers. No standalone build configuration is included.

## Typical game loop

Both games create a local monitor for each gameplay, menu, or pause loop, then call `monitor()` and `wait_fps_cap()` after rendering. The application functions below are placeholders for your own event handling and rendering:

```cpp
#include "fpsstorm/fpsstorm.h"

bool running();
void update();
void render();

void run_game() {
  /// Measure completed frames and pace the game loop
  fpsstorm fps_monitor;
  fps_monitor.set_fps_cap(60.0);
  fps_monitor.set_fps_target(40.0f);
  fps_monitor.set_fps_report_interval(2.0);
  fps_monitor.reset();

  while(running()) {
    update();
    render();
    fps_monitor.monitor();
    fps_monitor.wait_fps_cap();
  }
}
```

Call `monitor()` once per rendered frame. `get_fps()` returns the last completed interval's reading, initially zero; `get_fps_on_target()` returns its target status, initially true. `get_stats()` returns a formatted statistics string. Call `reset()` just before entering or resuming a loop to restart the measurement window and pacing deadline; it preserves accumulated lifetime statistics and callbacks.

## Redraw throttling during loading

AdvertCity uses `time_for_next_frame()` in `citymap.cpp` to limit progress-screen rendering during road and building generation. The generator keeps working between redraws, avoiding a sleep on every generation step. Here `advance_generation()` performs one chunk of work and reports whether more remains, while `render_progress()` also handles any required UI/event updates:

```cpp
fpsstorm fps_monitor;
fps_monitor.set_fps_cap(20.0);
fps_monitor.reset();

while(advance_generation()) {
  if(fps_monitor.time_for_next_frame()) {
    render_progress();
    fps_monitor.monitor();
  }
}
```

A successful `time_for_next_frame()` call advances the deadline. Use it as an alternative to `wait_fps_cap()` for a given loop, rather than calling both for the same frame. Monitoring is optional when only redraw throttling is needed.

## Adapting rendering detail

sphereFACE's `universe::set_performance_callbacks()` reduces its particle limit to 80% when below target and increases it by 5% when above target, bounded by the game's configured minimum and maximum. The same pattern can be connected to your own detail controls:

```cpp
fps_monitor.callback_below_target = [&particles]{
  particles.adjust_max_safe(0.8f, 100, 10'000);
};
fps_monitor.callback_above_target = [&particles]{
  particles.adjust_max_safe(1.05f, 100, 10'000);
};
```

Here `particles` is an application-owned manager exposing `adjust_max_safe(scale, minimum, maximum)`, as in sphereFACE. Keep it alive while the callbacks can run. Callbacks execute on every completed reporting interval in the applicable category, not only when the category changes.

## Timing and execution model

All work happens on the calling thread. FPSStorm creates no worker threads: `monitor()` updates counters and invokes callbacks synchronously, `wait_fps_cap()` sleeps the caller, and `time_for_next_frame()` returns immediately. Use each instance from one thread, or provide external synchronization. Timing uses `std::chrono::high_resolution_clock`, whose monotonicity depends on the standard-library implementation.

A few details of the current implementation matter when integrating it:

- The cap is approximate: its timestep is `unsigned(1000 / fps_cap) - 1` milliseconds, so a nominal 60 FPS cap uses 15 ms. Sleep accuracy also depends on the OS. This is not a precise fixed-step simulation clock or a replacement for vsync. Use finite positive settings; cap values above 1,000 FPS underflow the timestep calculation, and zero does not mean unlimited.
- FPS is counted over the configured interval, rather than divided by actual elapsed time. Long stalls can distort a reading. Statistics describe these interval readings; the average is their arithmetic mean, and the minimum starts at the default cap of 60 FPS.
- `callback_on_target` exists, but the current comparisons send every ordinary reading at or above the target to `callback_above_target`. “Above target” therefore means meeting the target, not necessarily reaching the cap. Below-target readings invoke `callback_below_target`.
- Setters update durations without restarting existing deadlines; call `reset()` after initial configuration. Lowering the cap below the target also lowers the target. Reporting intervals should be at least one millisecond.

Configuration changes print to standard output. Non-`NDEBUG` builds also print below-target warnings; `DEBUG_FPS` enables additional per-interval diagnostics and an unconditional destructor report.
