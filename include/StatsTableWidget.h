#ifndef STATS_TABLE_WIDGET_H
#define STATS_TABLE_WIDGET_H

#include <QWidget>
#include <QString>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QMap>

class StatsTableWidget : public QWidget {
    Q_OBJECT

public:
    explicit StatsTableWidget(QWidget* parent = nullptr);

    void addScore(const QString& gameType, int score);

private:
    QTableWidget* table;
    QMap<QString, int> scores;
    void loadScores();
    void saveScores();
    void refreshTable();
    void deleteUserScores(QTableWidget* table);
};

#endif // STATS_TABLE_WIDGET_H