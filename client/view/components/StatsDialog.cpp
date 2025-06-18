#include "StatsDialog.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>

StatsDialog::StatsDialog(QWidget *parent,
                         std::shared_ptr<server::TextDatabase> db,
                         int playerId)
    : QDialog(parent), database(std::move(db)), playerId(playerId)
{
    setWindowTitle("Последние 10 игр");
    resize(800, 450);

    auto *layout = new QVBoxLayout(this);

    table = new QTableWidget(this);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"Дата", "Режим", "Скорость (WPM)", "Точность (%)"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    layout->addWidget(table);

    avgStatsLabel = new QLabel(this);
    avgStatsLabel->setAlignment(Qt::AlignCenter);
    avgStatsLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 10px;");
    layout->addWidget(avgStatsLabel);

    loadStats();
}

StatsDialog::~StatsDialog() = default;

void StatsDialog::loadStats()
{
    if (!database || playerId == -1)
        return;

    try
    {
        auto games = database->get_last_games(playerId);
        table->setRowCount(static_cast<int>(games.size()));

        for (int i = 0; i < static_cast<int>(games.size()); ++i)
        {
            const auto &g = games[i];
            table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(g.played_at)));
            table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(g.mode)));
            table->setItem(i, 2, new QTableWidgetItem(QString::number(g.speed_wpm, 'f', 2)));
            table->setItem(i, 3, new QTableWidgetItem(QString::number(g.accuracy, 'f', 2)));
        }

        auto avgStats = database->get_average_stats(playerId);

        QString text = QString("Средняя скорость: %1 WPM | Средняя точность: %2% | "
                               "Правильных символов: %3 | Ошибок: %4 | Пропущено: %5 | Лишних: %6")
                           .arg(avgStats.avg_speed_wpm, 0, 'f', 2)
                           .arg(avgStats.avg_accuracy, 0, 'f', 2)
                           .arg(avgStats.avg_correct_symbols, 0, 'f', 2)
                           .arg(avgStats.avg_wrong_symbols, 0, 'f', 2)
                           .arg(avgStats.avg_missed_symbols, 0, 'f', 2)
                           .arg(avgStats.avg_extra_symbols, 0, 'f', 2);

        avgStatsLabel->setText(text);
    }
    catch (const std::exception &ex)
    {
        qWarning() << "Ошибка загрузки статистики: " << ex.what();
        avgStatsLabel->setText("Не удалось загрузить средние статистики.");
    }
}
