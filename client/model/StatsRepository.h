#pragma once
#include <QDateTime>
#include <vector>
#include "SessionStats.h"

class StatsRepository {
public:
    struct TestRecord {
        SessionStats stats;
        QDateTime timestamp;
    };

    void addTest(const SessionStats& stats);
    double averageWPM() const;
    double averageWPM_LastN(int n = 10) const;
    double highestWPM() const;
    QDateTime timeOfHighestWPM() const;

private:
    std::vector<TestRecord> allTests;
};
