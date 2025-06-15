#pragma once

#include <QString>

struct ModeParams {
    QString mode;
    int timeLimit = 0;
    int wordCount = 0;
    QString customText;
    QString quoteLength;
    QString language;
};
