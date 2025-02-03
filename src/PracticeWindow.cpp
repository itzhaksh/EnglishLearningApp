#include "PracticeWindow.h"
#include <QVBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QMessageBox>


PracticeWindow::PracticeWindow(const QString& jsonFile, QWidget* parent)
    : QMainWindow(parent), currentIndex(0), textToSpeech(new QTextToSpeech(this)) {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    centralWidget->setStyleSheet("background-color: #F7F7F7;");
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    englishWordLabel = new QLabel("English Word", centralWidget);
    englishWordLabel->setStyleSheet("font-size: 38px; color: #7f5af0; font-weight: bold;");
    englishWordLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(englishWordLabel);

    hebrewTranslationLabel = new QLabel("תרגום לעברית", centralWidget);
    hebrewTranslationLabel->setStyleSheet("font-size: 38px; color: #7f5af0; font-weight: bold;");
    hebrewTranslationLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(hebrewTranslationLabel);

    playSoundButton = new QPushButton("🔊 Play Sound", centralWidget);
    nextButton = new QPushButton("Next", centralWidget);
    prevButton = new QPushButton("Prev", centralWidget);
    backButton = new QPushButton("Back", centralWidget);


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

    playSoundButton->setStyleSheet(buttonStyle);
    nextButton->setStyleSheet(buttonStyle);
    prevButton->setStyleSheet(buttonStyle);
    backButton->setStyleSheet(buttonStyle);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(prevButton);
    buttonLayout->addWidget(nextButton);
    buttonLayout->addWidget(playSoundButton);

    layout->addLayout(buttonLayout);

    connect(nextButton, &QPushButton::clicked, this, &PracticeWindow::nextWord);
    connect(prevButton, &QPushButton::clicked, this, &PracticeWindow::prevWord);
    connect(playSoundButton, &QPushButton::clicked, this, &PracticeWindow::playSound);
    connect(backButton, &QPushButton::clicked, this, &PracticeWindow::goBack);


    loadWords(jsonFile);
    updateDisplay();
    playSound();
    resize(400, 400);
    setWindowTitle("Practice English");
}

PracticeWindow::~PracticeWindow() {}

void PracticeWindow::loadWords(const QString& jsonFile) {
    QString filePath = QString("resources/%1").arg(jsonFile);  
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
        playSound();
    }
    else {
        qDebug() << "Already at the last word.";
    }
}

void PracticeWindow::prevWord() {
    if (currentIndex > 0) {
        currentIndex--;
        updateDisplay();
        playSound();
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


void PracticeWindow::goBack() {
    if (parentWidget()) {
        parentWidget()->show();
    }
    this->close();  
}