#ifndef SPENT_TIME_H
#define SPENT_TIME_H

#include <string>
#include <iomanip>

struct Time {
    int hour;
    int minute;

    Time() : hour(0), minute(0) {}
    Time(int h, int m) : hour(h), minute(m) {}

    bool operator<=(const Time& other) const;
    bool operator<(const Time& other) const;
    bool operator>(const Time& other) const;
    Time operator-(const Time& other) const;

    int toMinutes() const;

    std::string toString() const;
};

Time parseTime(const std::string& timeStr);

#endif