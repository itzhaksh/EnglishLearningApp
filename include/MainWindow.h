#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "GameWindow.h"
#include "DifficultyWindow.h"




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
	void openMemoryGame();
    void exit();
private:
    DifficultyWindow* difficultyWindow;
    QPushButton *btnExit;
  
};

#endif // MAINWINDOW_H
