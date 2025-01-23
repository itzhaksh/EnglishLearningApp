#include "PracticeWindow.h"
#include <QVBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

PracticeWindow::PracticeWindow(QWidget* parent)
    : QMainWindow(parent), currentIndex(0), textToSpeech(new QTextToSpeech(this)) {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    centralWidget->setStyleSheet("background-color: green;");
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    englishWordLabel = new QLabel("English Word", centralWidget);
    englishWordLabel->setStyleSheet("font-size: 24px; color: white;");
    englishWordLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(englishWordLabel);

    hebrewTranslationLabel = new QLabel("תרגום לעברית", centralWidget);
    hebrewTranslationLabel->setStyleSheet("font-size: 20px; color: white;");
    hebrewTranslationLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(hebrewTranslationLabel);

    playSoundButton = new QPushButton("🔊 Play Sound", centralWidget);
    playSoundButton->setStyleSheet("font-size: 16px; color: white; background-color: green; border: 1px solid white;QPushButton:hover {"
        "   border: 2px solid;"
        "   font-weight: bold;"
        "}");
    layout->addWidget(playSoundButton);

    nextButton = new QPushButton("Next", centralWidget);
    prevButton = new QPushButton("Prev", centralWidget);
    closeButton = new QPushButton("Close", centralWidget);

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
        "}";

    nextButton->setStyleSheet(buttonStyle);
    prevButton->setStyleSheet(buttonStyle);
    closeButton->setStyleSheet(buttonStyle);

    layout->addWidget(prevButton);
    layout->addWidget(nextButton);
    layout->addWidget(closeButton);

    connect(nextButton, &QPushButton::clicked, this, &PracticeWindow::nextWord);
    connect(prevButton, &QPushButton::clicked, this, &PracticeWindow::prevWord);
    connect(playSoundButton, &QPushButton::clicked, this, &PracticeWindow::playSound);
    connect(closeButton, &QPushButton::clicked, this, &PracticeWindow::closeWindow);

    loadWords();
    updateDisplay();

    resize(400, 400);
    setWindowTitle("Practice English");
}

PracticeWindow::~PracticeWindow() {}

void PracticeWindow::loadWords() {
    QFile file("dictionary.json");
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

void PracticeWindow::updateDisplay() {
    if (englishWords.isEmpty() || currentIndex < 0 || currentIndex >= englishWords.size())
        return;

    QString englishWord = englishWords[currentIndex];
    QString hebrewTranslation = wordsMap.value(englishWord);

    englishWordLabel->setText(englishWord);
    hebrewTranslationLabel->setText(hebrewTranslation);
}

void PracticeWindow::nextWord() {
    if (currentIndex < englishWords.size() - 1) {
        currentIndex++;
        updateDisplay();
    }
    else {
        qDebug() << "Already at the last word.";
    }
}

void PracticeWindow::prevWord() {
    if (currentIndex > 0) {
        currentIndex--;
        updateDisplay();
    }
    else {
        qDebug() << "Already at the first word.";
    }
}

void PracticeWindow::playSound() {
    QString wordToSpeak = englishWordLabel->text();
    if (!wordToSpeak.isEmpty()) {
        textToSpeech->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        textToSpeech->say(wordToSpeak);
    }
    else {
        qDebug() << "No word to speak.";
    }
}

void PracticeWindow::closeWindow() {
    close();
}
