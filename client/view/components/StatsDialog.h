#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTableWidget>
#include <memory>
#include "text_database.hpp"

class StatsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatsDialog(QWidget *parent = nullptr,
                         std::shared_ptr<server::TextDatabase> db = nullptr,
                         int playerId = -1);

    ~StatsDialog() override;

private:
    QTableWidget *table;
    QLabel *avgStatsLabel;
    void loadStats();

    std::shared_ptr<server::TextDatabase> database;
    int playerId;
};

#endif // STATSDIALOG_H
