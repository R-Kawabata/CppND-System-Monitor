#include <chrono>
#include <thread>

#include "linux_parser.h"
#include "processor.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  float initial_total = LinuxParser::Jiffies();
  float initial_idle = LinuxParser::IdleJiffies();
  float final_total = 0, final_idle = 0, delta_total;

  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  final_total = LinuxParser::Jiffies();
  final_idle = LinuxParser::IdleJiffies();
  delta_total = final_total - initial_total;

  return delta_total == 0 ? delta_total : (final_idle - initial_idle)/delta_total;
}