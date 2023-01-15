#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(long input_pid);
  int Pid() const;
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram() const;
  long int UpTime();
  bool operator<(Process const& process) const;

 private:
   long pid;
};

#endif