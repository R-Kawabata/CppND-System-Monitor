#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(long input_pid) : pid(input_pid){};
  int GetPid() const;
  std::string GetUid() const; 
  std::string GetUser() const;
  std::string GetCommand() const;
  float GetCpuUtilization() const;
  std::string GetRam() const;
  long int GetUpTime() const;
  bool operator<(Process const& process) const;

 private:
   long pid;
};

#endif