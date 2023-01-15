#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

#define SAMESTRING 0

using std::string;
using std::to_string;
using std::vector;

int Process::GetPid() const { return Process::pid; }

float Process::GetCpuUtilization() const {
  long uptime, process_uptime, pid_total_time, clock;
  uptime = LinuxParser::UpTime();
  process_uptime = LinuxParser::UpTime(GetPid());
  pid_total_time = LinuxParser::ActiveJiffies(GetPid());
  clock = sysconf(_SC_CLK_TCK);

  return (pid_total_time*(clock*uptime - process_uptime))/(clock*clock);
}

string Process::GetCommand() const {
  string cmdline;

  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(GetPid()) + LinuxParser::kCmdlineFilename);

  if (stream.is_open()) {
    std::getline(stream, cmdline);
  }

  return cmdline.empty() ? LinuxParser::EMPTY : cmdline;
}

string Process::GetRam() const {
  string line, key_name;
  long virtual_memory_size = 0;
  std::istringstream currentline;

  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(GetPid()) + LinuxParser::kStatusFilename);

  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      currentline.str(line);
      currentline >> key_name;
      if (key_name.compare(LinuxParser::VMSIZE) == SAMESTRING) {
        currentline >> virtual_memory_size;
      } else {
        currentline.clear();
      }
    }
  }

  return std::to_string(virtual_memory_size);;
}

string Process::GetUid() const {
  string line, key_name, uid;
  std::istringstream currentline;

  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(GetPid()) + LinuxParser::kStatusFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      currentline.str(line);
      currentline >> key_name;
      if (key_name.compare(LinuxParser::UID) == SAMESTRING) {
        currentline >> uid;
        break;
      } else {
        currentline.clear();
      }
    }
  }
  return uid.empty() ? LinuxParser::EMPTY : uid;
}

string Process::GetUser() const {
  string line, username, passwd, uid, line_uid, user;
  std::istringstream currentline;
  std::ifstream stream(LinuxParser::kPasswordPath);

  uid = GetUid();

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      currentline.str(line);

      currentline >> username >> passwd >> line_uid;
      if (line_uid == uid) {
        user = username;
        break;
      } else {
        currentline.clear();
      }
    }
  }
  return user.empty() ? LinuxParser::EMPTY : user;
}

long int Process::GetUpTime() const {
  string line, value;
  vector<string> values;
  long start_time = 0;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  try {
    start_time = stol(values[21]) / sysconf(_SC_CLK_TCK);
    return start_time;
  } catch (...) {
    return start_time;
  }
}

bool Process::operator<(Process const& process) const { return GetRam() < process.GetRam(); }