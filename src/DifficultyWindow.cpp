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
    centralWidget->setStyleSheet("background-color: #F7F7F7;");

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* titleLabel = new QLabel("Select Level", this);
    titleLabel->setStyleSheet("font-size: 24px; color: #4A90E2; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

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

    btnLevel1 = new QPushButton("Level 1", this);
    btnLevel2 = new QPushButton("Level 2", this);
    btnLevel3 = new QPushButton("Level 3", this);
    btnLevel4 = new QPushButton("Level 4", this);
    btnLevel5 = new QPushButton("Level 5", this);
    btnBack = new QPushButton("Back", this);

    btnLevel1->setStyleSheet(buttonStyle);
    btnLevel2->setStyleSheet(buttonStyle);
    btnLevel3->setStyleSheet(buttonStyle);
    btnLevel4->setStyleSheet(buttonStyle);
    btnLevel5->setStyleSheet(buttonStyle);
    btnBack->setStyleSheet(buttonStyle);

    layout->addWidget(titleLabel);
    layout->addWidget(btnLevel1);
    layout->addWidget(btnLevel2);
    layout->addWidget(btnLevel3);
    layout->addWidget(btnLevel4);
    layout->addWidget(btnLevel5);
    layout->addWidget(btnBack);

    connect(btnLevel1, &QPushButton::clicked, this, [=]() { openLevel(1); });
    connect(btnLevel2, &QPushButton::clicked, this, [=]() { openLevel(2); });
    connect(btnLevel3, &QPushButton::clicked, this, [=]() { openLevel(3); });
    connect(btnLevel4, &QPushButton::clicked, this, [=]() { openLevel(4); });
    connect(btnLevel5, &QPushButton::clicked, this, [=]() { openLevel(5); });
    connect(btnBack, &QPushButton::clicked, this, &DifficultyWindow::close);

    resize(400, 500);
    setWindowTitle("Level Selection");
}

void DifficultyWindow::openLevel(int level)
{
    QString jsonFile = QString("dictionary_level%1.json").arg(level);
    if (gameMode == "Hebrew" || gameMode == "English") {
        gameWindow = new GameWindow(gameMode, jsonFile, this);
        gameWindow->show();
    }
    else if (gameMode == "Memory") {
        openMemoryGame(level);
    }
    else if (gameMode == "Practice") {
        openPracticeWindow(level);
    }

    this->hide();
}

void DifficultyWindow::openMemoryGame(int level)
{
    QString jsonFile = QString("dictionary_level%1.json").arg(level);
    MemoryGame* memoryGame = new MemoryGame(jsonFile, this);
    memoryGame->show();
}

void DifficultyWindow::openPracticeWindow(int level)
{
    QString jsonFile = QString("dictionary_level%1.json").arg(level);
    PracticeWindow* practiceWindow = new PracticeWindow(jsonFile, this);
    practiceWindow->show();
}

void DifficultyWindow::close()
{
    if (parentWidget()) {
        parentWidget()->show();
    }
    QMainWindow::close();
    delete this;
}
