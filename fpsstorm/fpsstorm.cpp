  fps_report_timestep = std::chrono::milliseconds(static_cast<unsigned int>(1000 * fps_report_interval));
}

void fpsstorm::monitor() {
  /// Monitor the framerate and report if we drop below
  ++frames_last_interval;
  if(std::chrono::high_resolution_clock::now() > fps_update_next_tick_start) {
#include "fpsstorm.h"
    fps = static_cast<float>(frames_last_interval / fps_report_interval);
#include <iostream>
    fps_on_target = fps >= fps_target;
#include <thread>

#include <sstream>
    // update lifetime scores

    fps_max = std::max(fps_max, fps);
fpsstorm::fpsstorm() {
    fps_min = std::min(fps_min, fps);
  /// Default constructor
    fps_average_total += static_cast<double>(fps);
  reset();
    ++fps_average_count;
}
    fps_average = fps_average_total / static_cast<double>(fps_average_count);

    if(fps_on_target) {
fpsstorm::~fpsstorm() {
      ++fps_on_target_count;
  /// Default destructor
      if(fps > fps_target - 1.0f) {
  #ifndef DEBUG_FPS
        #ifdef DEBUG_FPS
    if(fps_average_count != 0) {
          std::cout << "FPSStorm: DEBUG: " << fps << "FPS (above target)" << std::endl;
  #endif // DEBUG_FPS
        #endif // DEBUG_FPS
      std::cout << "FPSStorm: Stats: " << get_stats() << std::endl;
        callback_above_target();
  #ifndef DEBUG_FPS
      } else {
    }
        #ifdef DEBUG_FPS
  #endif // DEBUG_FPS
}
          std::cout << "FPSStorm: DEBUG: " << fps << "FPS (on target)" << std::endl;

        #endif // DEBUG_FPS
void fpsstorm::reset() {
        callback_on_target();
  /// Reset the FPS monitor's statistics - call this just before entering a loop so the first count isn't distorted by pauses etc
      }
  set_fps_cap(fps_cap);
    } else {
  set_fps_report_interval(fps_report_interval);
      #ifndef NDEBUG
  fps = 0;
        std::cout << "FPSStorm: Warning: " << fps << "FPS, " <<
  fps_on_target = true;
                     static_cast<int>((1.0 - (static_cast<double>(fps) / fps_cap)) * 100) << "% below cap, " <<
  frames_last_interval = 0;
                     static_cast<int>((1.0 - (static_cast<double>(fps) / static_cast<double>(fps_target))) * 100) << "% below target!" << std::endl;
  fps_cap_next_tick_start    = std::chrono::high_resolution_clock::now() + fps_cap_timestep;
      #endif // NDEBUG
  fps_update_next_tick_start = std::chrono::high_resolution_clock::now() + fps_report_timestep;
      callback_below_target();
}

    }
double fpsstorm::get_fps_cap() const {

  return fps_cap;
    frames_last_interval = 0;
}
    fps_update_next_tick_start = std::chrono::high_resolution_clock::now() + fps_report_timestep;

  }
void fpsstorm::set_fps_cap(double new_max) {
}
  /// Update the new max FPS

  fps_cap = new_max;
std::string const fpsstorm::get_stats() const {
  fps_cap_timestep = std::chrono::milliseconds(static_cast<unsigned int>(1000 / fps_cap) - 1); // -1 to go a bit over;
  /// Output a string with the lifetime stats of this run so far
  std::cout << "FPSStorm: Framerate now capped to " << fps_cap << "FPS (" << 1.0 / fps_cap << " seconds)." << std::endl;
  std::stringstream ss;
  if(fps_cap < static_cast<double>(fps_target)) {
  if(fps_average_count == 0) {
    set_fps_target(static_cast<float>(fps_cap));
    ss << "No stats collected this run.";
  }
  } else {
}
    ss << fps_max << " max, " << fps_min << " min, " << fps_average << " avg, " << fps_on_target_count << "/" << fps_average_count << " on target (" << (fps_on_target_count * 100) / fps_average_count << "%)";
void fpsstorm::set_fps_target(float new_target) {
  }
  /// Update the new FPS target
  return ss.str();
  fps_target = new_target;

  std::cout << "FPSStorm: Framerate target now " << new_target << "FPS (" << 1.0f / new_target << " seconds)." << std::endl;
}
}

void fpsstorm::wait_fps_cap() {
  /// Cap the framerate to the max even if vsync is off
  std::this_thread::sleep_until(fps_cap_next_tick_start);
  fps_cap_next_tick_start = std::chrono::high_resolution_clock::now() + fps_cap_timestep;
}

bool fpsstorm::time_for_next_frame() {
  /// Returns whether it's time to draw the next frame yet, without blocking like wait_fps_cap does
  if(std::chrono::high_resolution_clock::now() >= fps_cap_next_tick_start) {
    fps_cap_next_tick_start = std::chrono::high_resolution_clock::now() + fps_cap_timestep;
    return true;
  } else {
    return false;
  }
}

float fpsstorm::get_fps() const {
  /// Last measured frames per second reading
  return fps;
}
std::chrono::duration<double> fpsstorm::get_fps_cap_timestep() const {
  /// The currently set timestep duration
  return fps_cap_timestep;
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
    fps = static_cast<float>(frames_last_interval / fps_report_interval);
    fps_on_target = fps >= fps_target;

    // update lifetime scores
    fps_max = std::max(fps_max, fps);
    fps_min = std::min(fps_min, fps);
    fps_average_total += static_cast<double>(fps);
    ++fps_average_count;
    fps_average = fps_average_total / static_cast<double>(fps_average_count);
    if(fps_on_target) {
      ++fps_on_target_count;
      if(fps > fps_target - 1.0f) {
        #ifdef DEBUG_FPS
          std::cout << "FPSStorm: DEBUG: " << fps << "FPS (above target)" << std::endl;
        #endif // DEBUG_FPS
        callback_above_target();
      } else {
        #ifdef DEBUG_FPS
          std::cout << "FPSStorm: DEBUG: " << fps << "FPS (on target)" << std::endl;
        #endif // DEBUG_FPS
        callback_on_target();
      }
    } else {
      #ifndef NDEBUG
        std::cout << "FPSStorm: Warning: " << fps << "FPS, " <<
                     static_cast<int>((1.0 - (static_cast<double>(fps) / fps_cap)) * 100) << "% below cap, " <<
                     static_cast<int>((1.0 - (static_cast<double>(fps) / static_cast<double>(fps_target))) * 100) << "% below target!" << std::endl;
      #endif // NDEBUG
      callback_below_target();
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
