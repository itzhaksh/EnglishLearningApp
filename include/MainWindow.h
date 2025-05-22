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

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void openHebrewMode();
    void openEnglishMode();
    void openPracticeWindow();
	void openMemoryGame();
    void openScoreTable();
    void exit();
    void updateStatsTable(int score); 
private:
    DifficultyWindow* difficultyWindow;
    QPushButton *btnExit;
  
};

#endif // MAINWINDOW_H
