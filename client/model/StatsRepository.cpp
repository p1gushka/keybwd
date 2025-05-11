#include "StatsRepository.h"

void StatsRepository::addTest(const SessionStats& stats) {
    allTests.push_back({stats, QDateTime::currentDateTime()});
}

double StatsRepository::averageWPM() const {
    if (allTests.empty()) return 0.0;
    double sum = 0;
    for (const auto& record : allTests)
        sum += record.stats.getWPM();
    return sum / allTests.size();
}

double StatsRepository::averageWPM_LastN(int n) const {
    int count = std::min((int)allTests.size(), n);
    if (count == 0) return 0.0;
    double sum = 0;
    for (int i = allTests.size() - count; i < allTests.size(); ++i)
        sum += allTests[i].stats.getWPM();
    return sum / count;
}

double StatsRepository::highestWPM() const {
    double max = 0.0;
    for (const auto& record : allTests)
        max = std::max(max, record.stats.getWPM());
    return max;
}

QDateTime StatsRepository::timeOfHighestWPM() const {
    double max = 0.0;
    QDateTime result;
    for (const auto& record : allTests) {
        double wpm = record.stats.getWPM();
        if (wpm > max) {
            max = wpm;
            result = record.timestamp;
        }
    }
    return result;
}
