#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

#define SAMESTRING 0
#define NO_UPTIME 0

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;

  std::ifstream filestream(kOSPath);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);

      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;

  std::ifstream stream(kProcDirectory + kVersionFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());

  struct dirent* file;

  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line, key_name, total, free;
  float total_memory, free_memory;
  std::istringstream currentline;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      currentline.str(line);
      currentline >> key_name;
      if (key_name.compare(TOTAL_MEMORY) == SAMESTRING){
        currentline >> total;
        total_memory = stof(total);
      } else if (key_name.compare(FREE_MEMORY) == SAMESTRING) {
        currentline >> free;
        free_memory = stof(free);
      } else {
        currentline.clear();
      }
    }
  }
  return (1 - free_memory/total_memory);
}

long LinuxParser::UpTime() {
  string line, uptime;
  std::istringstream currentline;

  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    currentline.str(line);
    currentline >> uptime;
  }

  return uptime.empty() ? NO_UPTIME : stol(uptime);
}

long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> values;
  long utime = 0, stime = 0;
  std::istringstream currentline;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    currentline.str(line);

    while (currentline >> value) {
      values.push_back(value);
    }
    currentline.clear();
  }

  if (std::all_of(values[13].begin(), values[13].end(), isdigit)) {
    utime = stol(values[13]);
  }
  if (std::all_of(values[14].begin(), values[14].end(), isdigit)) {
    stime = stol(values[14]);
  }

  return utime+stime;
}

long LinuxParser::ActiveJiffies() {
  auto jiffies = CpuUtilization();

  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) +
         stol(jiffies[CPUStates::kSteal_]);
}

long LinuxParser::IdleJiffies() {
  auto jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() {
  string line, cpu, value;
  vector<string> jiffies;
  std::istringstream currentline;

  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()){
    std::getline(stream, line);
    currentline.str(line);

    currentline >> cpu;

    while (currentline >> value) {
      jiffies.push_back(value);
    }
  }
  return jiffies;
}

int LinuxParser::TotalProcesses() {
  string line, key_name;
  int process = 0;
  std::istringstream currentline;

  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()){
    while (std::getline(stream, line)) {
      currentline.str(line);
      currentline >> key_name;
      if (key_name.compare(PROCESS) == SAMESTRING) {
        currentline >> process;
        break;
      } else {
        currentline.clear();
      }
    }
  }
  return process;
}

int LinuxParser::RunningProcesses() {
  string line, key_name;
  int process = 0;
  std::istringstream currentline;

  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      currentline.str(line);
      currentline >> key_name;
      if (key_name.compare(PROCESS_RUNNING) == SAMESTRING) {
        currentline >> process;
        break;
      } else {
        currentline.clear();
      }
    }
  }
  return process;
}

string LinuxParser::Command(int pid) {
  string cmdline;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

  if (stream.is_open()) {
    std::getline(stream, cmdline);
  }

  return cmdline.empty() ? EMPTY : cmdline;
}

string LinuxParser::Ram(int pid) {
  string line, key_name;
  long virtual_memory_size = 0;
  std::istringstream currentline;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      currentline.str(line);
      currentline >> key_name;
      if (key_name.compare(VMSIZE) == SAMESTRING) {
        currentline >> virtual_memory_size;
      } else {
        currentline.clear();
      }
    }
  }

  return std::to_string(virtual_memory_size);
}

string LinuxParser::Uid(int pid) {
  string line, key_name, uid;
  std::istringstream currentline;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      currentline.str(line);
      currentline >> key_name;
      if (key_name.compare(UID) == SAMESTRING) {
        currentline >> uid;
        break;
      } else {
        currentline.clear();
      }
    }
  }
  return uid.empty() ? EMPTY : uid;
}

string LinuxParser::User(int pid) {
  string line, username, passwd, uid, line_uid, user;
  std::istringstream currentline;
  uid = Uid(pid);

  std::ifstream stream(kPasswordPath);

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
  return user.empty() ? EMPTY : user;
}

long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> values;
  long start_time = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream currentline(line);
    while (currentline >> value) {
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
