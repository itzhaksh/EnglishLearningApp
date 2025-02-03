#include "MemoryGame.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>
#include <random>

MemoryGame::MemoryGame(const QString& jsonFilePath, QWidget* parent)
    : QMainWindow(parent), jsonFilePath(jsonFilePath) {
    loadWords(jsonFilePath);
    initializeUI();
    setupGameBoard();
}

void MemoryGame::loadWords(const QString& level) {
    QString filePath = QString("resources/%1").arg(jsonFilePath);
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
    }
    else {
        QMessageBox::critical(this, "Error", "Invalid JSON format.");
    }
    file.close();
}

void MemoryGame::initializeUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    cardGrid = new QGridLayout();
    mainLayout->addLayout(cardGrid);

    QPushButton* resetBtn = new QPushButton("Reset Game", this);
    connect(resetBtn, &QPushButton::clicked, this, &MemoryGame::resetGame);

    QPushButton* backBtn = new QPushButton("Back", this);
    connect(backBtn, &QPushButton::clicked, this, &MemoryGame::goBack);

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

    resetBtn->setStyleSheet(buttonStyle);
    backBtn->setStyleSheet(buttonStyle);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(backBtn);
    buttonLayout->addWidget(resetBtn);
    mainLayout->addLayout(buttonLayout);


    centralWidget->setStyleSheet("background-color: #F7F7F7;");
    resize(600, 600);
    setWindowTitle("Memory Game");
}

void MemoryGame::setupGameBoard() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(englishWords.begin(), englishWords.end(), g);

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

        setButtonStyle(englishBtn, "#7f5af0");
        setButtonStyle(hebrewBtn, "#7f5af0");

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
}

void MemoryGame::handleCardClick() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    if (!clickedButton->isEnabled()) return;

    if (!firstSelectedButton) {
        firstSelectedButton = clickedButton;
        setButtonStyle(firstSelectedButton, "#4A90E2");
        return;
    }

    setButtonStyle(clickedButton,"#4A90E2");
    clickedButton->setEnabled(false);

    QString firstWord = firstSelectedButton->property("word").toString();
    QString secondWord = clickedButton->property("word").toString();

    bool isMatch = (wordsMap[firstWord] == secondWord) || (wordsMap[secondWord] == firstWord);

    clickedButton->setEnabled(false);

    QPushButton* firstButton = firstSelectedButton;

    if (isMatch) {
        handleMatch(firstButton, clickedButton);
    }
    else {
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


    if (matchedPairs == englishButtons.size()) {
        QMessageBox::information(this, "Congratulations", "You matched all words!");
    }
}

void MemoryGame::handleMismatch(QPushButton* firstButton, QPushButton* secondButton) {
    setButtonStyle(firstButton, "red");
    setButtonStyle(secondButton, "red");

    QTimer::singleShot(300, this, [this, firstButton, secondButton]() {
        setButtonStyle(firstButton, "#7f5af0");
        setButtonStyle(secondButton, "#7f5af0");

        firstButton->setEnabled(true);
        secondButton->setEnabled(true);

        });
}

void MemoryGame::setButtonStyle(QPushButton* button, const QString& color) {
    QString styleSheet = QString(
        "QPushButton {"
        "   background-color: %1;"
        "   color: white;"
        "   font-size: 16px;"
        "   min-width: 100px;"
        "   min-height: 50px;"
        "   border: 2px solid white;"  
        "}"
        "QPushButton:hover {"
        "   background-color: %1;"  
        "   border: 3px solid white;"   
        "}"
    ).arg(color);

    button->setStyleSheet(styleSheet);
}
void MemoryGame::resetGame() {
    for (auto btn : englishButtons + hebrewButtons) {
        cardGrid->removeWidget(btn);
        delete btn;
    }

    englishButtons.clear();
    hebrewButtons.clear();
    matchedPairs = 0;

    firstSelectedButton = nullptr;

    setupGameBoard();
}

void MemoryGame::goBack() {
    if (parentWidget()) {
        parentWidget()->show();  
    }
    this->close();  
}