#pragma once
#include <QWidget>
#include <QSpinBox>
#include "../common_types.h"

class QPushButton;
class QTextEdit;
class QComboBox;
class QLabel;
class QVBoxLayout;

namespace view {

class ModeSelectionScreen : public QWidget {
    Q_OBJECT
public:
    explicit ModeSelectionScreen(QWidget *parent = nullptr);
    QString getSelectedMode() const;
    int getTimeLimit() const;
    int getWordCount() const;
    QString getCustomText() const;
    QString getQuoteLength() const;
    QString getLanguage() const;

signals:
    void modeSelected(const QString &mode, const ModeParams &params);

private:
    void setupTimeModeOptions(QVBoxLayout *layout);
    void setupWordCountModeOptions(QVBoxLayout *layout);
    void setupQuoteModeOptions(QVBoxLayout *layout);
    void setupCustomTextMode(QVBoxLayout *layout);
    void setupCodeModeOptions(QVBoxLayout *layout);
    
    QPushButton *createModeButton(const QString &label, const QString &mode);

    QString currentMode;
    QTextEdit *customTextEdit;
    QWidget *currentOptionsPanel;
    QComboBox *timeComboBox;
    QComboBox *wordsComboBox;
    QComboBox *quoteLengthComboBox;
    QComboBox *languageComboBox;
    QLabel *optionsLabel;
    QPushButton *startButton;
};

} // namespace view
