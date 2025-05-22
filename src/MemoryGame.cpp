#include "MemoryGame.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>
#include <random>
#include <QSettings>
#include <QScreen>
#include <QGuiApplication>
#include <QPainter>

MemoryGame::MemoryGame(const QString& jsonFilePath, QWidget* parent)
    : QMainWindow(parent), jsonFilePath(jsonFilePath) {
    loadWords(jsonFilePath);
    initializeUI();
    setupGameBoard();
}

void MemoryGame::loadWords(const QString& level) {
    QString filePath = level;
    if (!filePath.startsWith("resources/")) {
        filePath = QString("resources/%1").arg(level);
    }
    qDebug() << "Trying to open dictionary file:" << filePath;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open words JSON file:" << filePath;
        QMessageBox::critical(this, "Error", QString("Failed to load dictionary file: %1").arg(filePath));
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        QJsonObject jsonObject = doc.object();
        wordsMap.clear();
        for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
            wordsMap.insert(it.key(), it.value().toString());
        }
        englishWords = wordsMap.keys();
    } else {
        QMessageBox::critical(this, "Error", "Invalid JSON format.");
    }
    file.close();
}

void MemoryGame::initializeUI() {
    setWindowTitle("Memory Game");
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
    painter.setOpacity(0.3);
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

    cardGrid = new QGridLayout();
    contentLayout->addLayout(cardGrid);

    QPushButton* resetBtn = new QPushButton("איפוס משחק", this);
    connect(resetBtn, &QPushButton::clicked, this, &MemoryGame::resetGame);

    QPushButton* backBtn = new QPushButton("חזור", this);
    connect(backBtn, &QPushButton::clicked, this, &MemoryGame::goBack);

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
        "   background-color: #f5d33f;" 
        "}";

    resetBtn->setStyleSheet(buttonStyle);
    backBtn->setStyleSheet(buttonStyle);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(backBtn);
    buttonLayout->addWidget(resetBtn);
    contentLayout->addLayout(buttonLayout);

    mainLayout->addWidget(contentWidget);

    resize(600, 400);

    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - this->width()) / 2;
        int y = (screenGeometry.height() - this->height()) / 2;
        this->move(x, y);
    }
}

void MemoryGame::setupGameBoard() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(englishWords.begin(), englishWords.end(), g);

    filterUsedWords(englishWords);

    QList<QString> selectedWords = englishWords.mid(0, 5);

    QList<QString> selectedHebrewWords;
    for (const QString& word : selectedWords) {
        selectedHebrewWords.append(wordsMap[word]);
    }

    std::shuffle(selectedHebrewWords.begin(), selectedHebrewWords.end(), g);

    englishButtons.clear();
    hebrewButtons.clear();

    for (int i = 0; i < selectedWords.size(); ++i) {
        QString englishWord = selectedWords[i];
        QString hebrewTranslation = selectedHebrewWords[i];

        QPushButton* englishBtn = new QPushButton(englishWord);
        QPushButton* hebrewBtn = new QPushButton(hebrewTranslation);

        setButtonStyle(englishBtn, "#f1c70c"); 
        setButtonStyle(hebrewBtn, "#f1c70c");

        englishBtn->setProperty("word", englishWord);
        hebrewBtn->setProperty("word", hebrewTranslation);

        connect(englishBtn, &QPushButton::clicked, this, &MemoryGame::handleCardClick);
        connect(hebrewBtn, &QPushButton::clicked, this, &MemoryGame::handleCardClick);

        englishButtons.append(englishBtn);
        hebrewButtons.append(hebrewBtn);
    }

    int row = 0;
    for (int i = 0; i < englishButtons.size(); ++i) {
        cardGrid->addWidget(englishButtons[i], row, 0);
        cardGrid->addWidget(hebrewButtons[i], row, 1);
        row++;
    }

    saveUsedWords(selectedWords);
}

void MemoryGame::handleCardClick() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    if (!clickedButton->isEnabled()) return;

    if (!firstSelectedButton) {
        firstSelectedButton = clickedButton;
        setButtonStyle(firstSelectedButton, "#ffe066"); 
        return;
    }

    setButtonStyle(clickedButton, "#ffe066"); 
    clickedButton->setEnabled(false);

    QString firstWord = firstSelectedButton->property("word").toString();
    QString secondWord = clickedButton->property("word").toString();

    bool isMatch = (wordsMap[firstWord] == secondWord) || (wordsMap[secondWord] == firstWord);

    clickedButton->setEnabled(false);

    QPushButton* firstButton = firstSelectedButton;

    if (isMatch) {
        handleMatch(firstButton, clickedButton);
    } else {
        QTimer::singleShot(1000, this, [this, firstButton, clickedButton]() {
            handleMismatch(firstButton, clickedButton);
        });
    }

    firstSelectedButton = nullptr;
}

void MemoryGame::handleMatch(QPushButton* firstButton, QPushButton* secondButton) {
    cardGrid->removeWidget(firstButton);
    cardGrid->removeWidget(secondButton);
    firstButton->setParent(nullptr);
    secondButton->setParent(nullptr);

    matchedPairs++;

    if (matchedPairs == 5) {
        int score = matchedPairs;
        saveScore("MemoryGame", score);
    }
}

void MemoryGame::saveScore(const QString& gameType, int score) {
    QSettings settings("MyCompany", "EnglishLearningApp");
    QString userName = settings.value("UserName", "").toString();
    QString key = QString("%1/%2/score").arg(userName).arg(gameType);

    int currentScore = settings.value(key, 0).toInt();
    settings.setValue(key, currentScore + score);
}

void MemoryGame::handleMismatch(QPushButton* firstButton, QPushButton* secondButton) {
    setButtonStyle(firstButton, "red");
    setButtonStyle(secondButton, "red");

    QTimer::singleShot(1000, this, [this, firstButton, secondButton]() {
        setButtonStyle(firstButton, "#f1c70c"); 
        setButtonStyle(secondButton, "#f1c70c");
        firstButton->setEnabled(true);
        secondButton->setEnabled(true);
    });
}

void MemoryGame::setButtonStyle(QPushButton* button, const QString& color) {
    QString styleSheet = QString(
        "QPushButton {"
        "   background-color: %1;"
        "   color: black;" 
        "   font-size: 16px;"
        "   min-width: 100px;"
        "   min-height: 50px;"
        "   border-radius: 8px;" 
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #f5d33f;" 
        "}"
        "QPushButton:pressed {"
        "   background-color: #f5d33f;" 
        "}"
    ).arg(color);

    button->setStyleSheet(styleSheet);
}

void MemoryGame::resetGame() {
    matchedPairs = 0;
    firstSelectedButton = nullptr;

    for (auto button : englishButtons) {
        cardGrid->removeWidget(button);
        delete button;
    }
    for (auto button : hebrewButtons) {
        cardGrid->removeWidget(button);
        delete button;
    }
    englishButtons.clear();
    hebrewButtons.clear();

    setupGameBoard();
}

void MemoryGame::goBack() {
    if (parentWidget()) {
        parentWidget()->show();
    }
    this->close();
}

void MemoryGame::filterUsedWords(QList<QString>& words) {
    QSettings settings("MyCompany", "EnglishLearningApp");
    QString userName = settings.value("UserName", "").toString();
    QString key = QString("%1/usedWords").arg(userName);

    QStringList usedWords = settings.value(key).toStringList();

    for (int i = words.size() - 1; i >= 0; --i) {
        if (usedWords.contains(words[i])) {
            words.removeAt(i);
        }
    }

    if (words.isEmpty()) {
        settings.remove(key);
    }
}

void MemoryGame::saveUsedWords(const QList<QString>& words) {
    QSettings settings("MyCompany", "EnglishLearningApp");
    QString userName = settings.value("UserName", "").toString();
    QString key = QString("%1/usedWords").arg(userName);

    QStringList usedWords = settings.value(key).toStringList();
    for (const QString& word : words) {
        if (!usedWords.contains(word)) {
            usedWords.append(word);
        }
    }
    settings.setValue(key, usedWords);
}