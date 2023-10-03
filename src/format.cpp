#include <string>
#include <iomanip>
#include <sstream>
#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  // let's get the hours, minutes and seconds from the seconds input
  long hours = seconds / 3600;
  long leftOverSeconds = seconds % 3600;
  long minutes = leftOverSeconds / 60;
  long secs = leftOverSeconds % 60;
  
  // Let's use create an output string stream 
  // Easy for getting each section value HH:MM:SS
  std::ostringstream stream;
  // Set Stream and manipulate it to print our time string in format HH:MM:SS
  stream << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << secs;
  
  // Convert and return a string from stream
  return stream.str();
  
}