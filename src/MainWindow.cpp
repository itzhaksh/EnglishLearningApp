#include "MainWindow.h"
#include "PracticeWindow.h"
#include "MemoryGame.h"
#include "StatsTableWidget.h"
#include "UserNameDialog.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QPalette>
#include <QPushButton>
#include <QGridLayout>
#include <QScreen>
#include <QSettings>
#include <QTimer>
#include <QMessageBox>
#include <QShowEvent>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , difficultyWindow(nullptr)
{
    QSettings settings("MyCompany", "EnglishLearningApp");
    QString userName = settings.value("UserName", "").toString();
    if (userName.isEmpty()) {
        UserNameDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            userName = dialog.getUserName();
            QString password = dialog.getPassword();
            if (userName == password) {
                settings.setValue("UserName", userName);
            } else {
                QMessageBox::critical(this, "Error", "Username and password do not match!");
                QTimer::singleShot(0, this, SLOT(close()));
                return;
            }
        } else {
            QTimer::singleShot(0, this, SLOT(close()));
            return;
        }
    }

    setWindowTitle("Game Selection");

    QIcon windowIcon(":/Learn-English-Icon.png");
    setWindowIcon(windowIcon);

    QLabel* imageLabel = new QLabel(this);
    imageLabel->setScaledContents(true);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    QPixmap pixmap(":/Learn-English.png");
    imageLabel->setPixmap(pixmap.scaled(600, 400, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    imageLabel->setFixedSize(600, 400);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(imageLabel);
    
    QWidget* contentWidget = new QWidget(this);
    contentWidget->setStyleSheet("background: transparent;");
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);

    QGridLayout* buttonGrid = new QGridLayout();
    buttonGrid->setSpacing(10);

    QPushButton* btnHebrewMode = new QPushButton("תרגול עברית", this);
    QPushButton* btnEnglishMode = new QPushButton("תרגול אנגלית", this);
    QPushButton* btnPracticeMode = new QPushButton("לימוד", this);
    QPushButton* btnMemoryGame = new QPushButton("משחק התאמה", this);
    QPushButton* btnScoreTable = new QPushButton("צפה בטבלת ניקוד", this); 
    QPushButton* btnExit = new QPushButton("יציאה", this);

    QString buttonStyle =
        "QPushButton {"
        "   background-color: #ebbf00;" 
        "   color: black;"
        "   font-size: 18px;"
        "   padding: 15px;"
        "   border-radius: 10px;"
        "   font-weight: bold;"
        "   box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);"
        "}"
        "QPushButton:hover {"
        "   background-color: #D4A017;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #D4A017;"
        "}";

    btnHebrewMode->setStyleSheet(buttonStyle);
    btnEnglishMode->setStyleSheet(buttonStyle);
    btnPracticeMode->setStyleSheet(buttonStyle);
    btnMemoryGame->setStyleSheet(buttonStyle);
    btnScoreTable->setStyleSheet(buttonStyle); 
    btnExit->setStyleSheet(buttonStyle);

    buttonGrid->addWidget(btnHebrewMode, 0, 0);
    buttonGrid->addWidget(btnEnglishMode, 0, 1);
    buttonGrid->addWidget(btnPracticeMode, 1, 0);
    buttonGrid->addWidget(btnMemoryGame, 1, 1);
    buttonGrid->addWidget(btnScoreTable, 2, 0, 1, 2); 
    buttonGrid->addWidget(btnExit, 3, 0, 1, 2); 

    contentLayout->addLayout(buttonGrid);

    mainLayout->addWidget(contentWidget);

    centralWidget->setLayout(mainLayout);

    resize(600, 400);

    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - this->width()) / 2;
        int y = (screenGeometry.height() - this->height()) / 2;
        this->move(x, y);
    }

    imageLabel->lower();

    connect(btnHebrewMode, &QPushButton::clicked, this, &MainWindow::openHebrewMode);
    connect(btnEnglishMode, &QPushButton::clicked, this, &MainWindow::openEnglishMode);
    connect(btnPracticeMode, &QPushButton::clicked, this, &MainWindow::openPracticeWindow);
    connect(btnMemoryGame, &QPushButton::clicked, this, &MainWindow::openMemoryGame);
    connect(btnScoreTable, &QPushButton::clicked, this, &MainWindow::openScoreTable); 
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

void MainWindow::openMemoryGame()
{
    difficultyWindow = new DifficultyWindow("Memory", this);
    difficultyWindow->show();
    this->hide();
}

void MainWindow::updateStatsTable(int score)
{
    StatsTableWidget* statsTable = new StatsTableWidget();
    statsTable->addScore("MemoryGame", score);
    statsTable->setWindowTitle("Score Table");
    statsTable->resize(500, 400);
    statsTable->show();
}

void MainWindow::openScoreTable()
{
    StatsTableWidget* statsTable = new StatsTableWidget();
    statsTable->setWindowTitle("Score Table");
    statsTable->resize(500, 400);
    statsTable->show();
}

void MainWindow::exit()
{
    QApplication::quit();
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);

    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - this->width()) / 2;
        int y = (screenGeometry.height() - this->height()) / 2;
        this->move(x, y);
    }
}