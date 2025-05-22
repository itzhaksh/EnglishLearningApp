#include "PracticeWindow.h"
#include <QVBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include <QPainter>

PracticeWindow::PracticeWindow(const QString& jsonFile, QWidget* parent)
    : QMainWindow(parent), currentIndex(0), textToSpeech(new QTextToSpeech(this)) {
    setWindowTitle("תרגול אנגלית");

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
    contentLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter); 

    QString wordBoxStyle =
        "QLabel {"
        "   background-color: #ebbf00;" 
        "   color: black;"
        "   font-size: 24px;" 
        "   padding: 10px;"
        "   border-radius: 8px;" 
        "   font-weight: bold;"
        "}";

    englishWordLabel = new QLabel("מילה באנגלית", this);
    englishWordLabel->setStyleSheet(wordBoxStyle);
    englishWordLabel->setAlignment(Qt::AlignCenter);
    englishWordLabel->setFixedSize(300, 60); 
    contentLayout->addWidget(englishWordLabel, 0, Qt::AlignHCenter); 

    hebrewTranslationLabel = new QLabel("תרגום לעברית", this);
    hebrewTranslationLabel->setStyleSheet(wordBoxStyle);
    hebrewTranslationLabel->setAlignment(Qt::AlignCenter);
    hebrewTranslationLabel->setFixedSize(300, 60);
    contentLayout->addWidget(hebrewTranslationLabel, 0, Qt::AlignHCenter); 

    playSoundButton = new QPushButton("🔊 השמע שוב", this);
    nextButton = new QPushButton("הבא", this);
    prevButton = new QPushButton("קודם", this);
    backButton = new QPushButton("חזרה", this);

    QString buttonStyle =
        "QPushButton {"
        "   background-color: #ebbf00;"
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
        "   background-color: #ebbf00;" 
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

    connect(nextButton, &QPushButton::clicked, this, &PracticeWindow::nextWord);
    connect(prevButton, &QPushButton::clicked, this, &PracticeWindow::prevWord);
    connect(playSoundButton, &QPushButton::clicked, this, &PracticeWindow::playSound);
    connect(backButton, &QPushButton::clicked, this, &PracticeWindow::goBack);

    loadWords(jsonFile);
    updateDisplay();
    playSound();
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