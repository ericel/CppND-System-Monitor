#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
  
  std::vector<int> pids = LinuxParser::Pids();
  std::set<int> ordered_pids;

  for (Process const& process : processes_) {
    ordered_pids.insert(process.Pid());
  }

  for (int pid : pids) {
    if (ordered_pids.find(pid) == ordered_pids.end()) {
      // pid is new; emplace it back into processes_
      processes_.emplace_back(pid);
      // Also update the map
      pid_map_[pid] = processes_.back();
    }
  }

  // Remove stale processes
  auto it = processes_.begin();
  while (it != processes_.end()) {
    if (std::find(pids.begin(), pids.end(), it->Pid()) == pids.end()) {
      // The process is no longer running; remove it
      it = processes_.erase(it);
    } else {
      ++it;
    }
  }

  // Update CPU utilization for each process
  for (auto& process : processes_) {
    process.CpuUtilization(LinuxParser::ActiveJiffies(process.Pid()), LinuxParser::Jiffies());
  }

  // Sort processes by CPU utilization
  std::sort(processes_.begin(), processes_.end(), std::greater<Process>());

  return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { 
  return LinuxParser::OperatingSystem();
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { 
  return LinuxParser::UpTime();
}
