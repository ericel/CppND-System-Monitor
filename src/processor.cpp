#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  // Initialize a variable to hold the CPU utilization
  float cpuUtilization = 0.0;

  // Fetch the current number of "active" jiffies (CPU time spent on actual work)
  long active_ticks = LinuxParser::ActiveJiffies();

  // Fetch the current number of "idle" jiffies (CPU time spent idling)
  long idle_ticks = LinuxParser::IdleJiffies();

  // Calculate the change in "active" jiffies since the last time this function was called
  long duration_active = active_ticks - already_active_ticks;

  // Calculate the change in "idle" jiffies since the last time this function was called
  long duration_idle = idle_ticks - already_idle_ticks;

  // Calculate the total change in jiffies (both active and idle)
  long total_duration = duration_active + duration_idle;

  // Calculate CPU Utilization as the ratio of change in "active" jiffies to total change in jiffies
  cpuUtilization = static_cast<float>(duration_active) / total_duration;

  // Cache the current "active" and "idle" jiffies for use in subsequent calls to this function
  already_active_ticks = active_ticks;
  already_idle_ticks = idle_ticks;

  // Return the calculated CPU utilization
  return cpuUtilization;
}