#include <dirent.h>
//#include <filesystem> // system G++ too old to compile c++ 17

#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <pwd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
  /*vector<int> pids;

  // Using C++17's filesystem library to read the directory
  for (const auto& entry : std::filesystem::directory_iterator(kProcDirectory)) {
    
    // Is this a directory?
    if (entry.is_directory()) {
      
      // Get the filename and check if it's an all-digit name
      string filename = entry.path().filename().string();
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        
        // Convert the filename to an integer (PID) and add it to the vector
        int pid = std::stoi(filename);
        pids.push_back(pid);
      }
    }
  }

  return pids;*/
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::ifstream fileStream(kProcDirectory + kMeminfoFilename);
  std::string line;
  long totalMemory = 0;
  long freeMemory = 0;

  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::istringstream stringStream(line);
      std::string key;
      long value;

      // Read key-value pairs from the file
      stringStream >> key >> value;

      if (key == "MemTotal:") {
        totalMemory = value;
      }

      if (key == "MemFree:") {
        freeMemory = value;
      }
      
      // Stop reading the file if both required values have been obtained
      if (totalMemory != 0 && freeMemory != 0) {
        break;
      }
    }
   
  }

  // Calculate the memory utilization
  if (totalMemory == 0) { // Handle division by zero
    return 0.0;
  }
  
  return static_cast<float>(totalMemory - freeMemory) / totalMemory;
  
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  long uptime = 0;
  
  if (stream.is_open()){
    // We get just the first value
    stream >> uptime;
  }
  // close stream
  stream.close();
  return uptime;

  
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::string line, cpu;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
  }
  
  // Total jiffies = Sum of all the times
  long total_jiffies = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;

  return total_jiffies;
 
  //return UpTime() * sysconf(_SC_CLK_TCK); 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::string line;
  long utime, stime, cutime, cstime;

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    std::string value;

    // skipping the first 13 values
    for (int i = 0; i < 13; ++i) {
      linestream >> value;
    }

    // these are the values we are interested in
    linestream >> utime >> stime >> cutime >> cstime;
  }

  // active jiffies for this PID
  long total = utime + stime + cutime + cstime;

  return total;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::string line;
  long totalActiveJiffies = 0;

  if (stream.is_open()) {
    std::getline(stream, line);  // Read the first line which corresponds to aggregate statistics
    std::istringstream linestream(line);
    std::string token;
    std::vector<long> jiffies;

    linestream >> token;  // Discard the first token ("cpu")

    // Loop to get all jiffies related to CPU utilization
    while (linestream >> token) {
      jiffies.push_back(std::stol(token));
    }

    // Calculating the active jiffies (all jiffies excluding the idle time)
    // user, nice, system, irq, softirq, steal, guest, guest_nice are considered active times
    for (int i = 0; i < 8; ++i) {
      totalActiveJiffies += jiffies[i];
    }
  }

  return totalActiveJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::string line;
  long idleJiffies = 0;

  if (stream.is_open()) {
    std::getline(stream, line);  // Read the first line which corresponds to aggregate statistics
    std::istringstream linestream(line);
    std::string token;
    std::vector<long> jiffies;

    linestream >> token;  // Discard the first token ("cpu")

    // Loop to get all jiffies related to CPU utilization
    while (linestream >> token) {
      jiffies.push_back(std::stol(token));
    }

    // The 4th and 5th elements in the vector represent idle and iowait jiffies
    idleJiffies = jiffies[3] + jiffies[4]; 
  }

  return idleJiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::string line;
  std::vector<std::string> cpuValues;

  if (stream.is_open()) {
    std::getline(stream, line);  // Read the first line which corresponds to aggregate statistics
    std::istringstream linestream(line);
    std::string value;

    // Skip the first token ("cpu")
    linestream >> value; 

    // Loop to get all jiffies related to CPU utilization
    while (linestream >> value) {
      cpuValues.push_back(value);
    }
  }

  return cpuValues;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
std::ifstream stream(kProcDirectory + kStatFilename);
  std::string line, key;
  int totalProcesses = 0;

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes") {
        linestream >> totalProcesses;
        break;
      }
    }
  }
  
  return totalProcesses;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::string line, key;
  int runningProcesses = 0;

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running") {
        linestream >> runningProcesses;
        break;
      }
    }
  }
  
  return runningProcesses;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  std::string line;
  
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
   std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  std::string line, key, value;

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;

      if (key == "VmSize:") {
        int sizeInKb = std::stoi(value);  // Convert to integer
        int sizeInMb = sizeInKb / 1024;   // Convert KB to MB
        return std::to_string(sizeInMb);  // Convert integer back to string and return
      }
    }
  }
  // Returns an empty string if not found
  return std::string("0");  
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  std::string line;
  std::string key;
  std::string value;
  
  // Construct path to the status file for this process
  std::string path = kProcDirectory + std::to_string(pid) + kStatusFilename;
  
  // Open the file at that path
  std::ifstream stream(path);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      
      // Look for the line that starts with "Uid:"
      if (key == "Uid:") {
        return value;  // Return the UID as a string
      }
    }
  }
  
  return std::string();  // Return an empty string if unsuccessful
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  // First, get the Uid associated with the pid
  std::string uid = Uid(pid);
  
  // Look up the username corresponding to the Uid
  struct passwd* pw = getpwuid(std::stoi(uid));
  
  if (pw != nullptr) {
    return std::string(pw->pw_name);  // Return the username
  }
  
  return std::string(); 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  // If we can't open the file, return 0
  if (!stream.is_open()) {
    return 0;
  }

  // Read the entire line from the stream
  std::getline(stream, line);
  std::istringstream buf(line);
  std::string token;

  // The starttime is the 22nd field in /proc/[pid]/stat
  for (int i = 0; i < 21; ++i) {
    buf >> token;
  }

  // Read the 22nd field
  long start_time;
  buf >> start_time;

  // Convert start_time to seconds by dividing with sysconf(_SC_CLK_TCK)
  start_time /= sysconf(_SC_CLK_TCK);

  // Get the system uptime in seconds
  long system_uptime = UpTime();  // Assuming you have implemented UpTime() for the system
  
  // Calculate the process uptime
  long process_uptime = system_uptime - start_time;

  return process_uptime;
}
