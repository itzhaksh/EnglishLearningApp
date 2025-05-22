#include "DifficultyWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QScreen>
#include <QGuiApplication>
#include <QPainter>

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
    setWindowTitle("בחירת רמה");

    QIcon windowIcon(":/Learn-English-Icon.png");
    setWindowIcon(windowIcon);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* imageLabel = new QLabel(centralWidget);
    imageLabel->setScaledContents(true);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QPixmap pixmap(":/Learn-English.png");
    QImage image = pixmap.toImage();
    QImage transparentImage(image.size(), QImage::Format_ARGB32);
    transparentImage.fill(Qt::transparent);
    QPainter painter(&transparentImage);
    painter.setOpacity(0.6);
    painter.drawImage(0, 0, image);
    painter.end();
    QPixmap transparentPixmap = QPixmap::fromImage(transparentImage);
    imageLabel->setPixmap(transparentPixmap.scaled(600, 400, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    imageLabel->setFixedSize(600, 400);
    imageLabel->setGeometry(0, 0, 600, 400);
    imageLabel->lower();

    QWidget* contentWidget = new QWidget(centralWidget);
    contentWidget->setStyleSheet("background: transparent;");
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setAlignment(Qt::AlignCenter);

    QLabel* titleLabel = new QLabel("בחר רמה", this);
    titleLabel->setStyleSheet("font-size: 24px; color:rgb(16, 16, 13); font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QString buttonStyle =
        "QPushButton {"
        "   background-color: #f1c70c;" 
        "   color: black;" 
        "   font-size: 16px;"
        "   padding: 10px;"
        "   border-radius: 8px;" 
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #f5d33f;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #f1c70c;"
        "}";

    btnLevel1 = new QPushButton("רמה 1", this);
    btnLevel2 = new QPushButton("רמה 2", this);
    btnLevel3 = new QPushButton("רמה 3", this);
    btnLevel4 = new QPushButton("רמה 4", this);
    btnLevel5 = new QPushButton("רמה 5", this);
    btnBack = new QPushButton("חזור", this);

    btnLevel1->setStyleSheet(buttonStyle);
    btnLevel2->setStyleSheet(buttonStyle);
    btnLevel3->setStyleSheet(buttonStyle);
    btnLevel4->setStyleSheet(buttonStyle);
    btnLevel5->setStyleSheet(buttonStyle);
    btnBack->setStyleSheet(buttonStyle);

    contentLayout->addWidget(titleLabel);
    contentLayout->addWidget(btnLevel1);
    contentLayout->addWidget(btnLevel2);
    contentLayout->addWidget(btnLevel3);
    contentLayout->addWidget(btnLevel4);
    contentLayout->addWidget(btnLevel5);
    contentLayout->addWidget(btnBack);

    mainLayout->addWidget(contentWidget);

    resize(600, 400);

    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - this->width()) / 2;
        int y = (screenGeometry.height() - this->height()) / 2;
        this->move(x, y);
    }

    connect(btnLevel1, &QPushButton::clicked, this, [=]() { openLevel(1); });
    connect(btnLevel2, &QPushButton::clicked, this, [=]() { openLevel(2); });
    connect(btnLevel3, &QPushButton::clicked, this, [=]() { openLevel(3); });
    connect(btnLevel4, &QPushButton::clicked, this, [=]() { openLevel(4); });
    connect(btnLevel5, &QPushButton::clicked, this, [=]() { openLevel(5); });
    connect(btnBack, &QPushButton::clicked, this, &DifficultyWindow::close);
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
        this->hide();
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