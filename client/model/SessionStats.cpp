#include "SessionStats.h"

double SessionStats::getCPS() const {
    return typingTimeMs ? (double)correctChars / (typingTimeMs / 1000.0) : 0.0;
}

double SessionStats::getWPS() const {
    return getCPS() / 5.0;
}

double SessionStats::getWPM() const {
    return getWPS() * 60.0;
}

double SessionStats::getCPM() const {
    return getCPS() * 60.0;
}

double SessionStats::getAccuracy() const {
    int total = correctChars + incorrectChars + extraChars;
    return total > 0 ? 100.0 * correctChars / total : 0.0;
}

double SessionStats::getRawWPM() const {
    return rawTimeMs ? ((double)correctChars / 5.0) / (rawTimeMs / 60000.0) : 0.0;
}
