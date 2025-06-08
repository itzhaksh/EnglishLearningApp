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
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSoundEffect>

MemoryGame::MemoryGame(const QString& jsonFilePath, QWidget* parent)
    : QMainWindow(parent), jsonFilePath(jsonFilePath) {
    loadWords(jsonFilePath);
    setupSoundEffects();
    initializeUI();
    setupGameBoard();
}

void MemoryGame::setupSoundEffects() {
    matchSound = new QSoundEffect(this);
    matchSound->setSource(QUrl::fromLocalFile(":/sounds/match.wav"));
    matchSound->setVolume(0.5f);

    mismatchSound = new QSoundEffect(this);
    mismatchSound->setSource(QUrl::fromLocalFile(":/sounds/mismatch.wav"));
    mismatchSound->setVolume(0.5f);

    flipSound = new QSoundEffect(this);
    flipSound->setSource(QUrl::fromLocalFile(":/sounds/flip.wav"));
    flipSound->setVolume(0.5f);
}

void MemoryGame::playCardFlipAnimation(QPushButton* button) {
    QPropertyAnimation* animation = new QPropertyAnimation(button, "geometry");
    animation->setDuration(300);
    animation->setStartValue(button->geometry());
    animation->setEndValue(QRect(button->x(), button->y() - 10, button->width(), button->height()));
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    flipSound->play();
}

void MemoryGame::playMatchAnimation(QPushButton* button1, QPushButton* button2) {
    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);

    QPropertyAnimation* anim1 = new QPropertyAnimation(button1, "geometry");
    anim1->setDuration(500);
    anim1->setStartValue(button1->geometry());
    anim1->setEndValue(QRect(button1->x(), button1->y() - 20, button1->width(), button1->height()));
    anim1->setEasingCurve(QEasingCurve::OutBounce);

    QPropertyAnimation* anim2 = new QPropertyAnimation(button2, "geometry");
    anim2->setDuration(500);
    anim2->setStartValue(button2->geometry());
    anim2->setEndValue(QRect(button2->x(), button2->y() - 20, button2->width(), button2->height()));
    anim2->setEasingCurve(QEasingCurve::OutBounce);

    group->addAnimation(anim1);
    group->addAnimation(anim2);
    group->start(QAbstractAnimation::DeleteWhenStopped);
    matchSound->play();
}

void MemoryGame::playMismatchAnimation(QPushButton* button1, QPushButton* button2) {
    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);

    QPropertyAnimation* anim1 = new QPropertyAnimation(button1, "geometry");
    anim1->setDuration(200);
    anim1->setStartValue(button1->geometry());
    anim1->setEndValue(QRect(button1->x() - 5, button1->y(), button1->width(), button1->height()));
    anim1->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation* anim2 = new QPropertyAnimation(button2, "geometry");
    anim2->setDuration(200);
    anim2->setStartValue(button2->geometry());
    anim2->setEndValue(QRect(button2->x() + 5, button2->y(), button2->width(), button2->height()));
    anim2->setEasingCurve(QEasingCurve::InOutQuad);

    group->addAnimation(anim1);
    group->addAnimation(anim2);
    group->start(QAbstractAnimation::DeleteWhenStopped);
    mismatchSound->play();
}

void MemoryGame::loadWords(const QString& level) {
    QString filePath = level;
    if (!filePath.startsWith("resources/")) {
        filePath = QString("resources/%1").arg(level);
    }

    filePath = filePath.replace(".json", "_enhanced.json");
    
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
        englishWords.clear();
        
        for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
            QJsonObject wordObj = it.value().toObject();
            QString englishWord = it.key();
            QString hebrewWord = wordObj["translation"].toString();
            wordsMap.insert(englishWord, hebrewWord);
            englishWords.append(englishWord);
        }
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

    playCardFlipAnimation(clickedButton);

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
        playMatchAnimation(firstButton, clickedButton);
        handleMatch(firstButton, clickedButton);
    } else {
        playMismatchAnimation(firstButton, clickedButton);
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