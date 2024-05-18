#ifndef EVENT_H
#define EVENT_H

#include "spent_time.h"

struct Event {
    Time time;
    int id;
    std::string clientName;
    int tableNumber;

    std::string toString() const {
        std::ostringstream oss;
        oss << time.toString() << ' ' << id << ' ' << clientName;
        if (id == 2 || id == 12) {
            oss << ' ' << tableNumber;
        }
        return oss.str();
    }

    static bool isValidClientName(const std::string& name) {
        for (char ch : name) {
            if (!((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '-')) {
                return false;
            }
        }
        return true;
    }
};

#endif