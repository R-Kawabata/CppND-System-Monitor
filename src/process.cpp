#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(long input_pid) {
  pid = input_pid;
}

// Return process's ID
int Process::Pid() const { return pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
  float act_time(LinuxParser::ActiveJiffies(Pid()));
  float uptime(LinuxParser::UpTime(Pid()));
  return float(act_time/uptime);
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// Return this process's memory utilization
string Process::Ram()const { return LinuxParser::Ram(Pid()); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& process) const { return Ram() < process.Ram(); }