#pragma once
#include <QtGlobal>

class SessionStats {
public:
    int correctChars = 0;
    int incorrectChars = 0;
    int extraChars = 0;
    int missedChars = 0;

    qint64 typingTimeMs = 0;   // ms
    qint64 rawTimeMs = 0;      // ms

    double getCPS() const;
    double getWPS() const;
    double getWPM() const;
    double getCPM() const;
    double getAccuracy() const;
    double getRawWPM() const;
};
