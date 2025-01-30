#include "GameWindow.h"
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QFontDatabase>
#include <QRegularExpression>


GameWindow::GameWindow(const QString& mode, const QString& level, QWidget* parent)
    : QMainWindow(parent),
    mode(mode),
    currentLevel(level),
    score(0),
    currentWordCount(0),
    tts(new QTextToSpeech(this)),
    voiceButtonGroup(new QButtonGroup(this))
{

    setupUI();
    loadDictionary(level);
    setupQuestion();
    connect(btnCheck, &QPushButton::clicked, this, &GameWindow::checkAnswer);

    if (mode == "Hebrew") {
        tts->setLocale(QLocale(QLocale::Hebrew, QLocale::Israel));
    }
    else {  // English mode
        tts->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    }

    voices = tts->availableVoices();
    updateVoiceButtons();
    playAudio();
}

GameWindow::~GameWindow() {}

void GameWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setStyleSheet("background-color: green;");

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QFont labelFont = QFont("Arial", 14, QFont::Bold);
    QFont buttonFont = QFont("Arial", 12);

    labelQuestion = new QLabel(this);
    labelQuestion->setFont(labelFont);
    labelQuestion->setStyleSheet("color: white;");

    labelFeedback = new QLabel(this);
    labelFeedback->setFont(labelFont);
    labelFeedback->setStyleSheet("color: white;");

    labelScore = new QLabel("Score: 0", this);
    labelScore->setFont(labelFont);
    labelScore->setStyleSheet("color: white;");

    lineEditAnswer = new QLineEdit(this);
    lineEditAnswer->setFont(labelFont);
    lineEditAnswer->setStyleSheet(
        "QLineEdit {"
        "   background-color: green;"
        "   color: white;"
        "   border: 1px solid white;"
        "   padding: 10px;"
        "}"
    );

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
        "}"
        "QPushButton:pressed {"
        "   background-color: green;"
        "   border: 2px solid white;"
        "   padding: 11px 9px 9px 11px;"
        "}";

    btnAudio = new QPushButton("🔊 Play Word", this);
    btnAudio->setFont(buttonFont);
    btnAudio->setStyleSheet(buttonStyle);

    btnCheck = new QPushButton("Check Answer", this);
    btnCheck->setFont(buttonFont);
    btnCheck->setStyleSheet(buttonStyle);

    btnClose = new QPushButton("Close", this);
    btnClose->setFont(buttonFont);
    btnClose->setStyleSheet(buttonStyle);

    layout->addWidget(labelQuestion);
    layout->addWidget(lineEditAnswer);
    layout->addWidget(btnCheck);
    layout->addWidget(btnAudio);
    layout->addWidget(labelFeedback);
    layout->addWidget(labelScore);

    voiceLayout = new QVBoxLayout();
    layout->addLayout(voiceLayout);
    layout->addWidget(btnClose);

    // חיבור האירועים
    connect(btnAudio, &QPushButton::clicked, this, &GameWindow::playAudio);
    connect(btnClose, &QPushButton::clicked, this, &GameWindow::close);

    resize(400, 300);
    setWindowTitle("Translation Game");

    setStyleSheet("QMainWindow { background-color: green; }");
}

void GameWindow::updateVoiceButtons()
{
    QLayoutItem* child;
    while ((child = voiceLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    voiceButtonGroup->setExclusive(true);

    QList<QVoice> filteredVoices;
    for (const QVoice& voice : voices) {
        if (mode == "Hebrew") {
            if (voice.locale().language() == QLocale::Hebrew &&
                voice.name().contains("Asaf")) {
                filteredVoices.append(voice);
            }
        }
        else {  // English mode
            if (voice.locale().language() == QLocale::English &&
                (voice.name().contains("David") ||
                    voice.name().contains("Mark") ||
                    voice.name().contains("Zira"))) {
                filteredVoices.append(voice);
            }
        }
    }

    if (filteredVoices.isEmpty()) {
        qWarning() << "No voices available for the selected language.";
        QMessageBox::warning(this, "Voice Error", "No voices available for the selected language.");
        return;
    }

    QString voiceButtonStyle =
        "QPushButton {"
        "   background-color: green;"
        "   color: white;"
        "   font-size: 14px;"
        "   padding: 8px;"
        "   border: 1px solid white;"
        "}"
        "QPushButton:hover {"
        "   border: 2px solid white;"
        "   font-weight: bold;"
        "}"
        "QPushButton:pressed {"
        "   background-color: green;"
        "   border: 2px solid white;"
        "   padding: 9px 7px 7px 9px;"
        "}"
        "QPushButton:checked {"
        "   border: 2px solid white;"
        "}";

    for (int i = 0; i < filteredVoices.size(); ++i) {
        QString voiceName = filteredVoices[i].name();
        QString icon = (voiceName.contains("David") || voiceName.contains("Mark") || voiceName.contains("Asaf")) ? "👨" : "👩";

        QPushButton* btnVoice = new QPushButton(icon + " " + voiceName, this);
        btnVoice->setStyleSheet(voiceButtonStyle);
        btnVoice->setCheckable(true);
        voiceButtonGroup->addButton(btnVoice, i);
        voiceLayout->addWidget(btnVoice);

        connect(btnVoice, &QPushButton::clicked, this, [=]() {
            selectVoice(i);
            });
    }

    voiceButtonGroup->button(0)->setChecked(true);
    selectVoice(0);
}

void GameWindow::selectVoice(int voiceIndex)
{
    if (voiceIndex >= 0 && voiceIndex < voices.size()) {
        tts->setVoice(voices[voiceIndex]);
        qDebug() << "Using voice:" << voices[voiceIndex].name() << "Locale:" << voices[voiceIndex].locale().name();
    }
}

void GameWindow::playAudio()
{
    if (voices.isEmpty()) {
        qWarning() << "No voices available.";
        return;
    }

    QString textToSpeak = labelQuestion->text();
    tts->say(textToSpeak);
}


void GameWindow::loadDictionary(const QString& level)
{

    QString filePath = QString("resources/%1").arg(level);
    qDebug() << "Trying to open dictionary file:" << filePath;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", QString("Failed to load dictionary file for level %1").arg(level));
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    if (document.isObject()) {
        dictionary = document.object();
    }
    else {
        QMessageBox::critical(this, "Error", "Invalid JSON format.");
    }
    file.close();
}

void GameWindow::setupQuestion()
{
    QStringList keys = dictionary.keys();
    if (keys.isEmpty()) {
        labelQuestion->setText("No questions available");
        return;
    }

    currentQuestion = keys.at(rand() % keys.size());
    correctAnswer = dictionary[currentQuestion].toString();

    if (mode == "Hebrew") {
        labelQuestion->setText(QString("%1").arg(correctAnswer));
    }
    else {
        labelQuestion->setText(QString("%1").arg(currentQuestion));
    }

    labelQuestion->setStyleSheet(
        "color: white;"
        "border: 3px solid white;"
        "padding: 15px;"
        "font-size: 26px;"
        "background-color: green;"
        "border-radius: 10px;"  
    );
    labelQuestion->setAlignment(Qt::AlignCenter);

    labelQuestion->setMinimumWidth(200);
    labelQuestion->setMinimumHeight(60);
}

QString GameWindow::removeHebrewDiacritics(const QString& text) {
    QRegularExpression diacriticsRegex("[\u0591-\u05C7]");
    QString result = text;
    return result.remove(diacriticsRegex);
}

void GameWindow::checkAnswer()
{
    QString userAnswer = lineEditAnswer->text();
    QString correct = (mode == "Hebrew") ? currentQuestion : correctAnswer;

    if (userAnswer.isEmpty()) {
        labelFeedback->setText("Please enter the translation");
        labelFeedback->setStyleSheet(
            "font: Yu Mincho Light;"
            "color: #FF0000;"      
            "font-size: 18px;"      
            "font-style: italic;"
           "text-shadow: 2px 2px #ff0000;"


        );
        return;  
    }

    if (removeHebrewDiacritics(userAnswer.toLower()) == removeHebrewDiacritics(correct.toLower())) {
        labelFeedback->setText("Correct!");
        labelFeedback->setStyleSheet(
            "color: #00FF00;"      
            "font-weight: bold;"    
            "font-size: 18px;"    
        );
        score++;
        labelScore->setText(QString("Score: %1").arg(score));
        labelScore->setStyleSheet(
            "font-size: 22px;"      
        );
        currentWordCount++;
        if (currentWordCount >= dictionary.count()) {
            QString levelText = QString("Level %1").arg(currentLevel);
            QMessageBox::information(this, "Level Complete",
                QString("Congratulations! You've completed %1!\nFinal Score: %2/%3")
                .arg(levelText)
                .arg(score)
                .arg(dictionary.count()));
            close();
            return;
        }
        setupQuestion();
        playAudio();
    }
    else {
        labelFeedback->setText("Incorrect. Try again!");
        labelFeedback->setStyleSheet(
            "font: Arial;"
            "color: #FF0000;"      
            "font-size: 18px;"      
        );
    }

    lineEditAnswer->clear();
}

void GameWindow::close()
{
    if (parentWidget()) {
        parentWidget()->show();
    }
    QMainWindow::close();
    delete this;
}
