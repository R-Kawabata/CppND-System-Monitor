#include <string>

#include "format.h"

#define HOUR 3600
#define MINUTE 60

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hour(seconds/HOUR);
  int minutes((seconds/MINUTE)%MINUTE);
  int second(seconds%MINUTE);
  string string_hour, string_minute, string_seconds;

  string_hour = hour < 10 ? "0" + std::to_string(hour) : std::to_string(hour);
  string_minute = minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes);
  string_seconds = second < 10 ? "0" + std::to_string(second) : std::to_string(second);

  return string_hour + ":" + string_minute + ":" + string_seconds;
}