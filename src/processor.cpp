#include "linux_parser.h"
#include "processor.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
    long total = LinuxParser::Jiffies();
    long active = LinuxParser::ActiveJiffies();
    return active/total;
}