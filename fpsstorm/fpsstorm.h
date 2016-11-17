#ifndef FPSSTORM_H_INCLUDED
#define FPSSTORM_H_INCLUDED

#include <chrono>
#include <string>
#include <functional>

class fpsstorm {
public:
  std::function<void()> callback_on_target    = []{};                           // callback called when the monitor reports we're on target
  std::function<void()> callback_below_target = []{};                           // callback for when the monitor says we're under target
  std::function<void()> callback_above_target = []{};                           // callback for when the monitor is on the framerate limiter

private:
  double fps_cap             = 60.0;                                            // default cap on frames per second
  double fps_report_interval = 4.0;                                             // how many seconds between reporting frames per second performance
  std::chrono::duration<double> fps_cap_timestep;
  std::chrono::duration<double> fps_report_timestep;
  std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> fps_cap_next_tick_start    = std::chrono::high_resolution_clock::now();
  std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> fps_update_next_tick_start = std::chrono::high_resolution_clock::now();
  //std::chrono::high_resolution_clock::time_point fps_cap_next_tick_start    = std::chrono::high_resolution_clock::now();
  //std::chrono::high_resolution_clock::time_point fps_update_next_tick_start = std::chrono::high_resolution_clock::now();

  double       fps_average          = 0.0;                                      // cached fps lifetime average
  double       fps_average_total    = 0.0;                                      // running total of all fps measurements

  float        fps_target           = 40.0;                                     // if we're below this target, notify
  unsigned int frames_last_interval = 0;
  float        fps = 0;                                                         // cached frames per second value
  float        fps_max              = 0.0f;                                     // lowest observed fps
  float        fps_min              = static_cast<float>(fps_cap);              // highest observed fps

  unsigned int fps_average_count    = 0;                                        // running count of fps measurements
  unsigned int fps_on_target_count  = 0;                                        // running count of entries on target

  bool fps_on_target = true;                                                    // are we currently on or above our target FPS?

public:
  fpsstorm();
  ~fpsstorm();

  void reset();

  double get_fps_cap() const __attribute__((__const__));
  void set_fps_cap(double new_max);
  void set_fps_target(float new_target);
  void wait_fps_cap();
  bool time_for_next_frame();

  float get_fps() const __attribute__((__const__));
  std::chrono::duration<double> get_fps_cap_timestep() const __attribute__((__pure__));
  bool get_fps_on_target() const __attribute__((__const__));
  void set_fps_report_interval(double new_interval);
  void monitor();

  std::string const get_stats() const;
};

#endif // FPSSTORM_H_INCLUDED
