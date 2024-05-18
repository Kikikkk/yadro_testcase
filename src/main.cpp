#include <iostream>
#include "processes.h"
#include "spent_time.h"
#include "event.h"
#include "table.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    Time startTime, endTime;
    int hourlyRate;
    std::vector<Event> events;
    try {
        std::vector<Table> tables = processInputFile(filename, startTime, endTime, hourlyRate, events);
        processEvents(events, tables, startTime, endTime, hourlyRate, std::cout);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
