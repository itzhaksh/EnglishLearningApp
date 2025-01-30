#ifndef DIFFICULTYWINDOW_H
#define DIFFICULTYWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include "GameWindow.h"
#include "MemoryGame.h"
#include "PracticeWindow.h"


class DifficultyWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DifficultyWindow(const QString& mode, QWidget* parent = nullptr);
    ~DifficultyWindow();

private slots:
    void openLevel(int level);
    void close();

private:
    void setupUI();
    void openMemoryGame(int level);
    void openPracticeWindow(int level);
    GameWindow* gameWindow;
    QString gameMode;
    QPushButton* btnLevel1;
    QPushButton* btnLevel2;
    QPushButton* btnLevel3;
    QPushButton* btnLevel4;
    QPushButton* btnLevel5;
    QPushButton* btnBack;
};

#endif // DIFFICULTYWINDOW_H
