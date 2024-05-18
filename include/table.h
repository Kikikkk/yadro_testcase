#ifndef TABLE_H
#define TABLE_H

#include <string>
#include "spent_time.h"

struct Table {
    int number;
    int revenue = 0;
    int totalOccupiedTime = 0;
    std::string currentClient;
    Time startTime;
};

#endif