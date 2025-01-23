#include "DifficultyWindow.h"
#include <QVBoxLayout>
#include <QWidget>

DifficultyWindow::DifficultyWindow(const QString& mode, QWidget* parent)
    : QMainWindow(parent)
    , gameWindow(nullptr)
    , gameMode(mode)
{
    setupUI();
}

DifficultyWindow::~DifficultyWindow() {}

void DifficultyWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setStyleSheet("background-color: green;");

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setAlignment(Qt::AlignCenter);

    
    QLabel* titleLabel = new QLabel("Select Difficulty", this);
    titleLabel->setStyleSheet("font-size: 24px; color: white; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QString buttonStyle =
        "QPushButton {"
        "   background-color: green;"
        "   color: white;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "   border: 1px solid white;"
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

    btnEasy = new QPushButton("Easy (10 words)", this);
    btnMedium = new QPushButton("Medium (20 words)", this);
    btnHard = new QPushButton("Hard (30 words)", this);
    btnBack = new QPushButton("Back", this);

    btnEasy->setStyleSheet(buttonStyle);
    btnMedium->setStyleSheet(buttonStyle);
    btnHard->setStyleSheet(buttonStyle);
    btnBack->setStyleSheet(buttonStyle);

    layout->addWidget(titleLabel);
    layout->addWidget(btnEasy);
    layout->addWidget(btnMedium);
    layout->addWidget(btnHard);
    layout->addWidget(btnBack);

    connect(btnEasy, &QPushButton::clicked, this, &DifficultyWindow::openEasyMode);
    connect(btnMedium, &QPushButton::clicked, this, &DifficultyWindow::openMediumMode);
    connect(btnHard, &QPushButton::clicked, this, &DifficultyWindow::openHardMode);
    connect(btnBack, &QPushButton::clicked, this, &DifficultyWindow::close);

    resize(400, 400);
    setWindowTitle("Difficulty Selection");
}

void DifficultyWindow::openEasyMode()
{
    gameWindow = new GameWindow(gameMode, "easy", this);
    gameWindow->show();
    this->hide();
}

void DifficultyWindow::openMediumMode()
{
    gameWindow = new GameWindow(gameMode, "medium", this);
    gameWindow->show();
    this->hide();
}

void DifficultyWindow::openHardMode()
{
    gameWindow = new GameWindow(gameMode, "hard", this);
    gameWindow->show();
    this->hide();
}

void DifficultyWindow::close()
{
    if (parentWidget()) {
        parentWidget()->show();
    }
    QMainWindow::close();
    delete this;
}