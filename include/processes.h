#ifndef PROCESSES_H
#define PROCESSES_H

#include <vector>
#include <fstream>
#include <map>
#include <queue>
#include <algorithm>
#include "spent_time.h"
#include "table.h"
#include "event.h"
#include <iostream>

int roundUpToHour(int minutes) {
    return (minutes + 59) / 60;
}

std::vector<Table> processInputFile(const std::string& filename, Time& startTime, Time& endTime, int& hourlyRate, std::vector<Event>& events) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open input file");
    }

    std::vector<Table> tables;

    std::string line;
    if (!std::getline(file, line)) {
        throw std::invalid_argument("Invalid input file format");
    }
    int numTables = std::stoi(line);

    tables.reserve(numTables);
    for (int i = 1; i <= numTables; ++i) {
      tables.push_back({i, 0, 0, "", {}});
    }

    if (!std::getline(file, line)) {
        throw std::invalid_argument("Invalid input file format");
    }
    startTime = parseTime(line.substr(0, 5));
    endTime = parseTime(line.substr(6, 5));

    if (!std::getline(file, line)) {
        throw std::invalid_argument("Invalid input file format");
    }
    hourlyRate = std::stoi(line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        Event event;
        std::string timeStr;
        if (!(ss >> timeStr)) {
            throw std::invalid_argument("Invalid input file format");
        }
        event.time = parseTime(timeStr);
        if (!(ss >> event.id)) {
            throw std::invalid_argument("Invalid input file format");
        }
        if (event.id == 1 || event.id == 3 || event.id == 4) {
            ss >> event.clientName;
            if (!Event::isValidClientName(event.clientName)) {
                throw std::invalid_argument("Invalid client name");
            }
        }
        if (event.id == 2) {
            ss >> event.clientName >> event.tableNumber;
            if (!Event::isValidClientName(event.clientName)) {
                throw std::invalid_argument("Invalid client name");
            }
        }
        events.push_back(event);
    }

    return tables;
}

void processEvents(const std::vector<Event>& events, std::vector<Table>& tables, const Time& startTime, const Time& endTime, int hourlyRate, std::ostream& output) {
    std::map<std::string, Time> clientsInClub;
    std::queue<std::string> waitingQueue;
    std::vector<Event> outputEvents;

    auto addErrorEvent = [&](const Event& event, const std::string& error) {
        Event errorEvent = event;
        errorEvent.id = 13;
        errorEvent.clientName = error;
        outputEvents.push_back(errorEvent);
    };

    for (const auto& event : events) {
        if (event.time < startTime) {
            outputEvents.push_back(event);
            addErrorEvent(event, "NotOpenYet");
            continue;
        }

        if (event.time > endTime) {
            outputEvents.push_back(event);
            addErrorEvent(event, "NotOpenYet");
            continue;
        }

        switch (event.id) {
            case 1: { // Клиент пришел
                if (clientsInClub.count(event.clientName) > 0) {
                    outputEvents.push_back(event);
                    addErrorEvent(event, "YouShallNotPass");
                } else {
                    clientsInClub[event.clientName] = event.time;
                    outputEvents.push_back(event);
                }
                break;
            }
            case 2: { // Клиент сел за стол
                auto it = std::find_if(tables.begin(), tables.end(), [&](const Table& table) {
                    return table.number == event.tableNumber;
                });

                if (it == tables.end()) {
                    outputEvents.push_back(event);
                    addErrorEvent(event, "InvalidTable");
                    break;
                }

                if (clientsInClub.count(event.clientName) == 0) {
                    outputEvents.push_back(event);
                    addErrorEvent(event, "ClientUnknown");
                    break;
                }

                if (!it->currentClient.empty() && it->currentClient != event.clientName) {
                    outputEvents.push_back(event);
                    addErrorEvent(event, "PlaceIsBusy");
                    break;
                }

                if (!it->currentClient.empty() && it->currentClient == event.clientName) {
                    outputEvents.push_back(event);
                    break;
                }

                it->currentClient = event.clientName;
                it->startTime = event.time;
                outputEvents.push_back(event);
                break;
            }
            case 3: { // Клиент ожидает
                if (clientsInClub.count(event.clientName) == 0) {
                    outputEvents.push_back(event);
                    addErrorEvent(event, "ClientUnknown");
                } else if (waitingQueue.size() >= tables.size()) {
                    Event leaveEvent = event;
                    leaveEvent.id = 11;
                    outputEvents.push_back(leaveEvent);
                } else if (std::any_of(tables.begin(), tables.end(), [](const Table& table) { return table.currentClient.empty(); })) {
                    outputEvents.push_back(event);
                    addErrorEvent(event, "ICanWaitNoLonger!");
                } else {
                    waitingQueue.push(event.clientName);
                    outputEvents.push_back(event);
                }
                break;
            }
            case 4: { // Клиент ушел
                if (clientsInClub.count(event.clientName) == 0) {
                    outputEvents.push_back(event);
                    addErrorEvent(event, "ClientUnknown");
                    break;
                }

                auto it = std::find_if(tables.begin(), tables.end(), [&](const Table& table) {
                    return table.currentClient == event.clientName;
                });

                if (it != tables.end()) {
                    Time duration = event.time - it->startTime;
                    int minutesOccupied = duration.toMinutes();
                    int hoursOccupied = roundUpToHour(minutesOccupied);
                    it->revenue += hoursOccupied * hourlyRate;
                    it->totalOccupiedTime += minutesOccupied;
                    it->currentClient.clear();
                    outputEvents.push_back(event);
                    if (!waitingQueue.empty()) {
                        it->currentClient = waitingQueue.front();
                        waitingQueue.pop();
                        Event sitEvent = event;
                        sitEvent.id = 12;
                        sitEvent.clientName = it->currentClient;
                        sitEvent.tableNumber = it->number;
                        sitEvent.time = event.time;
                        outputEvents.push_back(sitEvent);
                        it->startTime = event.time;
                    }
                }
                clientsInClub.erase(event.clientName);
                break;
            }
            default:
                addErrorEvent(event, "UnknownEvent");
                break;
        }
    }

    for (const auto& [clientName, arrivalTime] : clientsInClub) {
        Event leaveEvent;
        leaveEvent.time = endTime;
        leaveEvent.id = 11;
        leaveEvent.clientName = clientName;
        outputEvents.push_back(leaveEvent);

        auto it = std::find_if(tables.begin(), tables.end(), [&](const Table& table) {
            return table.currentClient == clientName;
        });

        if (it != tables.end()) {
            Time duration = endTime - it->startTime;
            int minutesOccupied = duration.toMinutes();
            int hoursOccupied = roundUpToHour(minutesOccupied);
            it->revenue += hoursOccupied * hourlyRate;
            it->totalOccupiedTime += minutesOccupied;
            it->currentClient.clear();
        }
    }

    output << startTime.toString() << '\n';
    for (const auto& event : outputEvents) {
        output << event.toString() << '\n';
    }
    output << endTime.toString() << '\n';

    for (const auto& table : tables) {
        output << table.number << ' ' << table.revenue << ' '
               << std::setw(2) << std::setfill('0') << table.totalOccupiedTime / 60 << ':'
               << std::setw(2) << std::setfill('0') << table.totalOccupiedTime % 60 << '\n';
    }
}
#endif