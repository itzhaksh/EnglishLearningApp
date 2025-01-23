#include "MemoryGame.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>
#include <random>

MemoryGame::MemoryGame(const QString& jsonFilePath, QWidget* parent)
    : QMainWindow(parent), jsonFilePath(jsonFilePath) {
    loadWords();
    initializeUI();
    setupGameBoard();
}

void MemoryGame::loadWords() {
    QFile file("resources/dictionary.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Could not open words JSON file.");
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject jsonObject = doc.object();
    file.close();

    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        wordsMap.insert(it.key(), it.value().toString());
    }

    englishWords = wordsMap.keys();
}

void MemoryGame::initializeUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    cardGrid = new QGridLayout();
    mainLayout->addLayout(cardGrid);

    QPushButton* resetBtn = new QPushButton("Reset Game");
    connect(resetBtn, &QPushButton::clicked, this, &MemoryGame::resetGame);
    mainLayout->addWidget(resetBtn);

    centralWidget->setStyleSheet("background-color: green;");
    resize(600, 600);
    setWindowTitle("Memory Game");
}

void MemoryGame::setupGameBoard() {
    // Shuffle English words
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(englishWords.begin(), englishWords.end(), g);

    // Take first 5 words or less
    QList<QString> selectedWords = englishWords.mid(0, 5);

    // Prepare the Hebrew words list
    QList<QString> selectedHebrewWords;
    for (const QString& word : selectedWords) {
        selectedHebrewWords.append(wordsMap[word]);
    }

    // Shuffle Hebrew words as well
    std::shuffle(selectedHebrewWords.begin(), selectedHebrewWords.end(), g);

    englishButtons.clear();
    hebrewButtons.clear();

    // Create the buttons
    for (int i = 0; i < selectedWords.size(); ++i) {
        QString englishWord = selectedWords[i];
        QString hebrewTranslation = selectedHebrewWords[i];

        // Create English button
        QPushButton* englishBtn = new QPushButton(englishWord);
        englishBtn->setStyleSheet("QPushButton { background-color: blue; color: white; font-size: 16px; min-width: 100px; min-height: 50px; }");

        // Create Hebrew button
        QPushButton* hebrewBtn = new QPushButton(hebrewTranslation);
        hebrewBtn->setStyleSheet("QPushButton { background-color: blue; color: white; font-size: 16px; min-width: 100px; min-height: 50px; }");

        // Set properties for matching
        englishBtn->setProperty("word", englishWord);
        hebrewBtn->setProperty("word", hebrewTranslation);

        connect(englishBtn, &QPushButton::clicked, this, &MemoryGame::handleCardClick);
        connect(hebrewBtn, &QPushButton::clicked, this, &MemoryGame::handleCardClick);

        // Add the buttons to their respective lists
        englishButtons.append(englishBtn);
        hebrewButtons.append(hebrewBtn);
    }

    // Add the English and Hebrew buttons to the grid
    int row = 0;
    for (int i = 0; i < englishButtons.size(); ++i) {
        // Add English button to the left side (column 0)
        cardGrid->addWidget(englishButtons[i], row, 0);

        // Add Hebrew button to the right side (column 1)
        cardGrid->addWidget(hebrewButtons[i], row, 1);

        row++;
    }
}

void MemoryGame::handleCardClick() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    // לא ניתן לבחור קלף אם הוא כבר לא פעיל
    if (!clickedButton->isEnabled()) return;

    // First selection - אם זהו הקלף הראשון שנבחר
    if (!firstSelectedButton) {
        firstSelectedButton = clickedButton;
        firstSelectedButton->setEnabled(false);
        return;
    }

    // Second selection - בחירת הקלף השני
    QString firstWord = firstSelectedButton->property("word").toString();
    QString secondWord = clickedButton->property("word").toString();

    // אם מדובר במילים תואמות
    bool isMatch = (wordsMap[firstWord] == secondWord) || (wordsMap[secondWord] == firstWord);

    if (isMatch) {
        handleMatch(firstSelectedButton, clickedButton);
    }
    else {
        handleMismatch(firstSelectedButton, clickedButton);
    }
}

void MemoryGame::handleMatch(QPushButton* firstButton, QPushButton* secondButton) {
    // Disable both buttons and simulate removal from the board
    firstButton->setEnabled(false);
    secondButton->setEnabled(false);
    matchedPairs++;

    // Change the color to green when a match is found
    setButtonStyle(firstButton, "green");
    setButtonStyle(secondButton, "green");

    // Optionally, remove the buttons from the layout
    cardGrid->removeWidget(firstButton);
    cardGrid->removeWidget(secondButton);
    firstButton->setParent(nullptr);
    secondButton->setParent(nullptr);

    // Check if game is complete
    if (matchedPairs == englishButtons.size()) {
        QMessageBox::information(this, "Congratulations", "You matched all words!");
    }

    // Reset first selected button
    firstSelectedButton = nullptr;
}

void MemoryGame::handleMismatch(QPushButton* firstButton, QPushButton* secondButton) {
    // Change the color to red for both buttons and reset after 1 second
    setButtonStyle(firstButton, "red");
    setButtonStyle(secondButton, "red");

    // Use QTimer to reset the buttons after 1 second
    QTimer::singleShot(1000, this, [this, firstButton, secondButton]() {
        // Reset colors back to blue
        setButtonStyle(firstButton, "blue");
        setButtonStyle(secondButton, "blue");

        // Re-enable the buttons
        firstButton->setEnabled(true);
        secondButton->setEnabled(true);

        // Reset first selected button
        firstSelectedButton = nullptr;
        });
}

void MemoryGame::setButtonStyle(QPushButton* button, const QString& color) {
    button->setStyleSheet(QString("QPushButton { background-color: %1; color: white; font-size: 16px; min-width: 100px; min-height: 50px; }").arg(color));
}

void MemoryGame::resetGame() {
    // Clear existing buttons
    for (auto btn : englishButtons + hebrewButtons) {
        cardGrid->removeWidget(btn);
        delete btn;
    }

    englishButtons.clear();
    hebrewButtons.clear();
    matchedPairs = 0;

    // Reset first selected button to nullptr
    firstSelectedButton = nullptr;

    // Setup new game
    setupGameBoard();
}
