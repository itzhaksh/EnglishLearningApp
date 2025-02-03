#include "GameWindow.h"
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QFontDatabase>
#include <QRegularExpression>
#include <QGuiApplication>
#include <QInputMethod>

GameWindow::GameWindow(const QString& mode, const QString& level, QWidget* parent)
    : QMainWindow(parent),
    mode(mode),
    currentLevel(level),
    score(0),
    currentWordCount(0),
    tts(new QTextToSpeech(this)),
    voiceButtonGroup(new QButtonGroup(this)),
    showAnswerButton(new QPushButton("Show Answer", this))

{
    if (mode == "English") {
        voiceLayout = new QHBoxLayout();
    }
    else {
        voiceLayout = new QVBoxLayout();
    }

    setupUI();
    loadDictionary(level);
    setupQuestion();

    connect(btnCheck, &QPushButton::clicked, this, &GameWindow::checkAnswer);

    if (mode == "Hebrew") {
        tts->setLocale(QLocale(QLocale::Hebrew, QLocale::Israel));
    }
    else {
        tts->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    }

    QList<QVoice> availableVoices = tts->availableVoices();
    voices = availableVoices;

    updateVoiceButtons();
    playAudio();
}

GameWindow::~GameWindow() {}

void GameWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setStyleSheet("background-color: #F7F7F7;");

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QFont labelFont("Arial", 14, QFont::Bold);
    QFont buttonFont("Arial", 12);

    labelQuestion = new QLabel(this);
    labelQuestion->setFont(labelFont);
    labelQuestion->setStyleSheet("color: white;");

    labelFeedback = new QLabel(this);
    labelFeedback->setFont(labelFont);
    labelFeedback->setStyleSheet("color: white;");

    labelScore = new QLabel("Score: 0", this);
    labelScore->setFont(labelFont);
    labelScore->setStyleSheet("color: #4A90E2");

    lineEditAnswer = new QLineEdit(this);
    lineEditAnswer->setFont(labelFont);
    lineEditAnswer->setStyleSheet(
        "QLineEdit {"
        "   background-color: #7f5af0;"
        "   color: white;"
        "   border: 1px solid white;"
        "   padding: 10px;"
        "}"
    );

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

    QFrame* frame = new QFrame(this);
    frame->setFrameStyle(QFrame::Box | QFrame::Raised);
    frame->setLineWidth(2);
    QVBoxLayout* frameLayout = new QVBoxLayout(frame);
    frameLayout->addWidget(lineEditAnswer);
    frameLayout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(frame);

    lineEditAnswer->setPlaceholderText(mode == "Hebrew" ?
        "הקלד את התרגום..." :
        "Type your translation here...");

    layout->addWidget(btnCheck);
    layout->addWidget(btnAudio);
    voiceLayout->addWidget(showAnswerButton);
    showAnswerButton->setFont(buttonFont); 
    showAnswerButton->setStyleSheet(buttonStyle); 
    layout->addWidget(labelFeedback);
    layout->addWidget(labelScore);

    layout->addLayout(voiceLayout);
    layout->addWidget(btnClose);

    connect(btnAudio, &QPushButton::clicked, this, &GameWindow::playAudio);
    connect(btnClose, &QPushButton::clicked, this, &GameWindow::close);
    connect(lineEditAnswer, &QLineEdit::textChanged, this, &GameWindow::checkKeyboardLanguage);
    connect(showAnswerButton, &QPushButton::clicked, this, &GameWindow::revealAnswer);

    resize(400, 300);
    setWindowTitle("Translation Game");
    setStyleSheet("QMainWindow { background-color: #7f5af0; }");
}

void GameWindow::updateVoiceButtons()
{
    QLayoutItem* child;
    while ((child = voiceLayout->takeAt(0)) != nullptr) {
        if (child->widget() && child->widget() != showAnswerButton) { 
            delete child->widget();
        }
        delete child;
    }

    voiceLayout->addWidget(showAnswerButton);

    voiceButtonGroup->setExclusive(true);

    QList<QVoice> filteredVoices;
    for (const QVoice& voice : voices) {
        if (mode == "Hebrew") {
            if (voice.locale().language() == QLocale::Hebrew &&
                voice.name().contains("Asaf"))
            {
                filteredVoices.append(voice);
            }
        }
        else {
            if (voice.locale().language() == QLocale::English &&
                (voice.name().contains("David") ||
                    voice.name().contains("Mark") ||
                    voice.name().contains("Zira")))
            {
                filteredVoices.append(voice);
            }
        }
    }

    if (filteredVoices.isEmpty()) {
        qWarning() << "No voices available for the selected language.";
        QMessageBox::warning(this, "Voice Error", "No voices available for the selected language.");
        return;
    }

    voices = filteredVoices;

    QString voiceButtonStyle =
        "QPushButton {"
        "   background-color: #7f5af0;"
        "   color: white;"
        "   font-size: 14px;"
        "   padding: 8px;"
        "   border: 2px solid white;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4A90E2;"
        "   border: 1px solid white;"
        "   font-weight: bold;"
        "}"
        "QPushButton:pressed {"
        "   background-color: green;"
        "   border: 2px solid white;"
        "   padding: 9px 7px 7px 9px;"
        "}";

    for (int i = 0; i < voices.size(); ++i) {
        QString voiceName = voices[i].name();
        QString icon = (voiceName.contains("David") ||
            voiceName.contains("Mark") ||
            voiceName.contains("Asaf")) ? "👨" : "👩";

        voiceName.replace("Microsoft ", "");
        QPushButton* btnVoice = new QPushButton(icon + " " + voiceName, this);
        btnVoice->setStyleSheet(voiceButtonStyle);
        btnVoice->setCheckable(true);
        voiceButtonGroup->addButton(btnVoice, i);
        voiceLayout->addWidget(btnVoice);

        connect(btnVoice, &QPushButton::clicked, this, [=]() {
            selectVoice(i);
            });
    }

    if (!voiceButtonGroup->buttons().isEmpty()) {
        voiceButtonGroup->button(0)->setChecked(true);
        selectVoice(0);
    }
}

void GameWindow::revealAnswer()
{
    if (mode == "Hebrew") {
        labelFeedback->setText(QString("The answer is: %1").arg(currentQuestion));
    }
    else {
        labelFeedback->setText(QString("The answer is: %1").arg(correctAnswer));
    }
    labelFeedback->setStyleSheet(
        "color: #7f5af0;"
        "font-weight: bold;"
        "font-size: 18px;"
    );
    lineEditAnswer->setText((mode == "Hebrew") ? currentQuestion : correctAnswer);
    showAnswerButton->setEnabled(false);
}

void GameWindow::selectVoice(int voiceIndex)
{
    if (voiceIndex >= 0 && voiceIndex < voices.size()) {
        tts->setVoice(voices[voiceIndex]);
        qDebug() << "Using voice:" << voices[voiceIndex].name()
            << "Locale:" << voices[voiceIndex].locale().name();
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
        QMessageBox::critical(this, "Error",
            QString("Failed to load dictionary file for level %1").arg(level));
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
    showAnswerButton->setEnabled(true);
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
        "background-color: #7f5af0;"
        "border-radius: 10px;"
    );
    labelQuestion->setAlignment(Qt::AlignCenter);
    labelQuestion->setMinimumWidth(200);
    labelQuestion->setMinimumHeight(60);
}

QString GameWindow::removeHebrewDiacritics(const QString& text)
{
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
            "color: #7f5af0;"
            "font-size: 18px;"
            "font-weight: bold;"
            "text-shadow: 2px 2px #ff0000;"
        );
        return;
    }

    if (removeHebrewDiacritics(userAnswer.toLower()) ==
        removeHebrewDiacritics(correct.toLower()))
    {
        labelFeedback->setText("Correct!");
        labelFeedback->setStyleSheet(
            "color: #7f5af0;"
            "font-weight: bold;"
            "font-size: 18px;"
        );
        score++;
        labelScore->setText(QString("Score: %1").arg(score));
        labelScore->setStyleSheet(
            "font-size: 22px;"
            "color: #4A90E2;"
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

void GameWindow::checkKeyboardLanguage()
{
    QString text = lineEditAnswer->text();
    if (!text.isEmpty()) {
        QChar lastChar = text.back();
        bool isHebrew = (lastChar.unicode() >= 0x0590 && lastChar.unicode() <= 0x05FF);

        if ((mode == "Hebrew" && isHebrew) || (mode == "English" && !isHebrew)) {
            labelFeedback->setText(mode == "Hebrew" ?
                "Please switch to English keyboard" :
                "נא החלף למקלדת עברית");
            labelFeedback->setStyleSheet(
                "color: #4A90E2;"
                "font-size: 16px;"
                "font-weight: bold;"
            );
        }
        else {
            labelFeedback->clear();
        }
    }
}