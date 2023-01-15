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
    return string(std::to_string(hour) + ":" + std::to_string(minutes) + ":" + std::to_string(second));
    }