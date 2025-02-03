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

    centralWidget->setStyleSheet("background-color: #F7F7F7;");

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
    titleLabel->setStyleSheet("font-size: 24px; color: #4A90E2; font-weight: bold;"); 
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QGridLayout* buttonGrid = new QGridLayout();
    buttonGrid->setSpacing(10); 

    QPushButton* btnHebrewMode = new QPushButton("Hebrew Mode", this);
    QPushButton* btnEnglishMode = new QPushButton("English Mode", this);
    QPushButton* btnPracticeMode = new QPushButton("Practice Mode", this);
    QPushButton* btnMemoryGame = new QPushButton("Memory Game", this);
    btnExit = new QPushButton("Exit", this);

    btnHebrewMode->setMinimumSize(200, 60); 
    btnEnglishMode->setMinimumSize(200, 60);
    btnPracticeMode->setMinimumSize(200, 60);
    btnMemoryGame->setMinimumSize(200, 60);
    btnExit->setMinimumSize(200, 60);

    QString buttonStyle =
        "QPushButton {"
        "   background-color: #7f5af0;" 
        "   color: white;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "   border: 4px solid white;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4A90E2;" 
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
    connect(btnExit, &QPushButton::clicked, this, &MainWindow::exit);

    resize(420, 420);
    setWindowTitle("Learn English");
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