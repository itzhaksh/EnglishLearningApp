#ifndef DIFFICULTYWINDOW_H
#define DIFFICULTYWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include "gamewindow.h"

class DifficultyWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DifficultyWindow(const QString& mode, QWidget* parent = nullptr);
    ~DifficultyWindow();

private slots:
    void openEasyMode();
    void openMediumMode();
    void openHardMode();
    void close();

private:
    void setupUI();
    GameWindow* gameWindow;
    QString gameMode;
    QPushButton* btnEasy;
    QPushButton* btnMedium;
    QPushButton* btnHard;
    QPushButton* btnBack;
};

#endif // DIFFICULTYWINDOW_H