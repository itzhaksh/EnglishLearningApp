#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "gamewindow.h"
#include "difficultywindow.h"




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openHebrewMode();
    void openEnglishMode();
    void openPracticeWindow();
    void exit();
private:
    DifficultyWindow* difficultyWindow;
    QPushButton *btnExit;
  
};

#endif // MAINWINDOW_H
