#include "spent_time.h"

bool Time::operator<=(const Time& other) const {
    return (hour < other.hour) || (hour == other.hour && minute <= other.minute);
}

bool Time::operator<(const Time& other) const {
    return (hour < other.hour) || (hour == other.hour && minute < other.minute);
}

bool Time::operator>(const Time& other) const {
    return (hour > other.hour) || (hour == other.hour && minute > other.minute);
}

Time Time::operator-(const Time& other) const {
    int totalMinutes = (hour * 60 + minute) - (other.hour * 60 + other.minute);
    return {totalMinutes / 60, totalMinutes % 60};
}

int Time::toMinutes() const {
    return hour * 60 + minute;
}

std::string Time::toString() const {
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hour << ':'
        << std::setw(2) << std::setfill('0') << minute;
    return oss.str();
}

Time parseTime(const std::string& timeStr) {
    Time time;
    std::stringstream ss(timeStr);
    char colon;

    if (!(ss >> time.hour >> colon >> time.minute) || colon != ':') {
        throw std::invalid_argument("Invalid time format");
    }

    return time;
}
