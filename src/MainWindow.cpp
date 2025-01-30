#include "MainWindow.h"
#include "PracticeWindow.h"
#include "MemoryGame.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QPalette>
#include <QPushButton>
#include <QGridLayout>
#include <QScreen>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , difficultyWindow(nullptr)
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    centralWidget->setStyleSheet("background-color: green;");

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel* imageLabel = new QLabel(this);

    QPixmap image("resources/learn_english_background.png");

    int dpi = logicalDpiX(); 
    int pixelsPerCm = dpi / 2.54; 

    QPixmap scaledImage = image.scaled(4 * pixelsPerCm, 4 * pixelsPerCm, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(scaledImage);
    mainLayout->addWidget(imageLabel);
    imageLabel->setAlignment(Qt::AlignCenter);


    QLabel* titleLabel = new QLabel("LearnEnglish", this);
    titleLabel->setStyleSheet("font-size: 24px; color: white; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QGridLayout* buttonGrid = new QGridLayout();
    buttonGrid->setSpacing(0);

    QPushButton* btnHebrewMode = new QPushButton("Hebrew Mode", this);
    QPushButton* btnEnglishMode = new QPushButton("English Mode", this);
    QPushButton* btnPracticeMode = new QPushButton("Practice Mode", this);
    QPushButton* btnMemoryGame = new QPushButton("Memory Game", this);


    btnExit = new QPushButton("Exit", this);

    QString buttonStyle =
        "QPushButton {"
        "   background-color: green;"
        "   color: white;"
        "   font-size: 16px;"
        "   padding: 15px;"
        "   border: 4px solid white;"
        "}"
        "QPushButton:hover {"
        "   border: 2px solid white;"    
        "   font-weight: bold;"          
        "}"
        "QPushButton:pressed {"
        "   background-color: green;"    
        "   border: 2px solid white;"    
        "   padding: 11px 9px 9px 11px;" 
        "}";

    btnHebrewMode->setStyleSheet(buttonStyle);
    btnEnglishMode->setStyleSheet(buttonStyle);
    btnPracticeMode->setStyleSheet(buttonStyle);
    btnMemoryGame->setStyleSheet(buttonStyle);

    btnExit->setStyleSheet(buttonStyle);

    buttonGrid->addWidget(btnHebrewMode, 0, 0);  
    buttonGrid->addWidget(btnEnglishMode, 0, 1); 
    buttonGrid->addWidget(btnPracticeMode, 1, 0); 
    buttonGrid->addWidget(btnMemoryGame, 1, 1);

    mainLayout->addLayout(buttonGrid);


    mainLayout->addWidget(btnExit);


connect(btnHebrewMode, &QPushButton::clicked, this, &MainWindow::openHebrewMode);
connect(btnEnglishMode, &QPushButton::clicked, this, &MainWindow::openEnglishMode);
connect(btnPracticeMode, &QPushButton::clicked, this, &MainWindow::openPracticeWindow);
connect(btnMemoryGame, &QPushButton::clicked, this, &MainWindow::openMemoryGame);


    resize(400, 400); 
    setWindowTitle("Learn English");

    connect(btnExit, &QPushButton::clicked, this, &MainWindow::exit);

}




MainWindow::~MainWindow()
{
}

void MainWindow::openHebrewMode()
{
    difficultyWindow = new DifficultyWindow("Hebrew", this);
    difficultyWindow->show();
    this->hide();
}

void MainWindow::openEnglishMode()
{
    difficultyWindow = new DifficultyWindow("English", this);
    difficultyWindow->show();
    this->hide();
}

void MainWindow::openPracticeWindow()
{
    difficultyWindow = new DifficultyWindow("Practice", this);
    difficultyWindow->show();
    this->hide();
}
void MainWindow::openMemoryGame() {
    difficultyWindow = new DifficultyWindow("Memory", this);
    difficultyWindow->show();
    this->hide();
}



void MainWindow::exit()
{
    QApplication::quit();
}
