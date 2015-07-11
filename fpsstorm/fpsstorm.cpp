#include "fpsstorm.h"
#include <iostream>
#include <thread>
#include <sstream>

fpsstorm::fpsstorm() {
  /// Default constructor
  reset();
}

fpsstorm::~fpsstorm() {
  /// Default destructor
  std::cout << "FPSStorm: Stats: " << get_stats() << std::endl;
}

void fpsstorm::reset() {
  /// Reset the FPS monitor's statistics - call this just before entering a loop so the first count isn't distorted by pauses etc
  set_fps_cap(fps_cap);
  set_fps_report_interval(fps_report_interval);
  fps = 0;
  fps_on_target = true;
  frames_last_interval = 0;
  fps_cap_next_tick_start = std::chrono::high_resolution_clock::now() + fps_cap_timestep;
}

double fpsstorm::get_fps_cap() const {
  return fps_cap;
}

void fpsstorm::set_fps_cap(double new_max) {
  /// Update the new max FPS
  fps_cap = new_max;
  fps_cap_timestep = std::chrono::milliseconds(static_cast<unsigned int>(1000 / fps_cap) - 1); // -1 to go a bit over;
  std::cout << "FPSStorm: Framerate now capped to " << fps_cap << "FPS (" << 1.0 / fps_cap << " seconds)." << std::endl;
}

void fpsstorm::wait_fps_cap() {
  /// Cap the framerate to the max even if vsync is off
  std::this_thread::sleep_until(fps_cap_next_tick_start);
  fps_cap_next_tick_start = std::chrono::high_resolution_clock::now() + fps_cap_timestep;
}

float fpsstorm::get_fps() const {
  /// Last measured frames per second reading
  return fps;
}
bool fpsstorm::get_fps_on_target() const {
  /// Are we currently on target for monitoring FPS?
  return fps_on_target;
}

void fpsstorm::set_fps_report_interval(double new_interval) {
  /// Update the new FPS report interval (in seconds) - how often the FPS counter and watcher refreshes, longer intervals are less prone to blips
  fps_report_interval = new_interval;
  fps_report_timestep = std::chrono::milliseconds(static_cast<unsigned int>(1000 * fps_report_interval));
}

void fpsstorm::monitor() {
  /// Monitor the framerate and report if we drop below
  ++frames_last_interval;
  if(std::chrono::high_resolution_clock::now() > fps_update_next_tick_start) {
    fps = frames_last_interval / fps_report_interval;
    fps_on_target = fps >= fps_target;

    // update lifetime scores
    fps_max = std::max(fps_max, fps);
    fps_min = std::min(fps_min, fps);
    fps_average_total += fps;
    ++fps_average_count;
    fps_average = fps_average_total / static_cast<double>(fps_average_count);
    if(fps_on_target) {
      ++fps_on_target_count;
      #ifndef NDEBUG
        std::cout << "FPSStorm: DEBUG: " << fps << "FPS (on target)" << std::endl;
      #endif // NDEBUG
    } else {
      #ifndef NDEBUG
        std::cout << "FPSStorm: Warning: " << fps << "FPS, " <<
                     (1.0f - (fps / fps_cap)) * 100 << "% below cap, " <<
                     (1.0f - (fps / fps_target)) * 100 << "% below target!" << std::endl;
      #endif
    }

    frames_last_interval = 0;
    fps_update_next_tick_start = std::chrono::high_resolution_clock::now() + fps_report_timestep;
  }
}

std::string const fpsstorm::get_stats() const {
  /// Output a string with the lifetime stats of this run so far
  std::stringstream ss;
  if(fps_average_count == 0) {
    ss << "No stats collected this run.";
  } else {
    ss << fps_max << " max, " << fps_min << " min, " << fps_average << " avg, " << fps_on_target_count << "/" << fps_average_count << " on target (" << (fps_on_target_count * 100) / fps_average_count << "%)";
  }
  return ss.str();

}
