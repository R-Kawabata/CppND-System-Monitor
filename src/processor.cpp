#include <chrono>
#include <thread>

#include "linux_parser.h"
#include "processor.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  float initial_total = LinuxParser::Jiffies();
  float initial_active = LinuxParser::ActiveJiffies();
  float final_total = 0, final_active = 0, delta_total;

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  final_total = LinuxParser::Jiffies();
  final_active = LinuxParser::ActiveJiffies();
  delta_total = final_total - initial_total;

  return delta_total == 0 ? delta_total : (final_active - initial_active)/delta_total;
}