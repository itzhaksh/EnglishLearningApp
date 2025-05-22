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
#include <QPainter>
#include <QScreen>

GameWindow::GameWindow(const QString& mode, const QString& level, QWidget* parent)
    : QMainWindow(parent),
    mode(mode),
    currentLevel(level),
    score(0),
    currentWordCount(0),
    tts(new QTextToSpeech(this)),
    voiceButtonGroup(new QButtonGroup(this)),
    showAnswerButton(new QPushButton("הראה תשובה", this)) 
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
    setWindowTitle("Translation Game");

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

    QFont labelFont("Arial", 14, QFont::Bold);
    QFont buttonFont("Arial", 12);

    labelQuestion = new QLabel(this);
    labelQuestion->setFont(labelFont);
    labelQuestion->setStyleSheet("color: black;");
    labelQuestion->setAlignment(Qt::AlignCenter);

    labelFeedback = new QLabel(this);
    labelFeedback->setFont(labelFont);
    labelFeedback->setStyleSheet("color: black;");

    labelScore = new QLabel("Score: 0", this);
    labelScore->setFont(labelFont);
    labelScore->setStyleSheet("color: black;");

    lineEditAnswer = new QLineEdit(this);
    lineEditAnswer->setFont(labelFont);
    lineEditAnswer->setStyleSheet(
        "QLineEdit {"
        "   background-color: rgba(255, 255, 255, 0.8);"
        "   color: black;"
        "   border: 1px solid white;"
        "   padding: 10px;"
        "}"
    );

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

    btnAudio = new QPushButton("🔊 השמע שוב", this); 
    btnAudio->setFont(buttonFont);
    btnAudio->setStyleSheet(buttonStyle);

    btnCheck = new QPushButton("בדוק תשובה", this); 
    btnCheck->setFont(buttonFont);
    btnCheck->setStyleSheet(buttonStyle);

    btnClose = new QPushButton("סגור", this); 
    btnClose->setFont(buttonFont);
    btnClose->setStyleSheet(buttonStyle);

    contentLayout->addWidget(labelQuestion);

    QFrame* frame = new QFrame(this);
    frame->setFrameStyle(QFrame::Box | QFrame::Raised);
    frame->setLineWidth(2);
    QVBoxLayout* frameLayout = new QVBoxLayout(frame);
    frameLayout->addWidget(lineEditAnswer);
    frameLayout->setContentsMargins(5, 5, 5, 5);
    contentLayout->addWidget(frame);

    lineEditAnswer->setPlaceholderText(mode == "Hebrew" ?
        "הקלד את התרגום..." :
        "Type your translation here...");

    contentLayout->addWidget(btnCheck);
    contentLayout->addWidget(btnAudio);
    voiceLayout->addWidget(showAnswerButton);
    showAnswerButton->setFont(buttonFont);
    showAnswerButton->setStyleSheet(buttonStyle);
    contentLayout->addWidget(labelFeedback);
    contentLayout->addWidget(labelScore);

    contentLayout->addLayout(voiceLayout);
    contentLayout->addWidget(btnClose);

    mainLayout->addWidget(contentWidget);

    resize(600, 400);

    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - this->width()) / 2;
        int y = (screenGeometry.height() - this->height()) / 2;
        this->move(x, y);
    }

    connect(btnAudio, &QPushButton::clicked, this, &GameWindow::playAudio);
    connect(btnClose, &QPushButton::clicked, this, &GameWindow::close);
    connect(lineEditAnswer, &QLineEdit::textChanged, this, &GameWindow::checkKeyboardLanguage);
    connect(showAnswerButton, &QPushButton::clicked, this, &GameWindow::revealAnswer);
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
        "   background-color: #f1c70c;"
        "   color: black;"
        "   font-size: 14px;"
        "   padding: 8px;"
        "   border-radius: 8px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #f5d33f;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #f5d33f;"
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
        "color: black;"
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
        "color: black;"
        "border: 3px solid white;"
        "padding: 15px;"
        "font-size: 26px;"
        "background-color: rgba(255, 255, 255, 0.5);"
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
            "color: black;"
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
            "color: black;"
            "font-weight: bold;"
            "font-size: 18px;"
        );
        score++;
        labelScore->setText(QString("Score: %1").arg(score));
        labelScore->setStyleSheet(
            "font-size: 22px;"
            "color: black;"
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
            "color: black;"
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
                "color: black;"
                "font-size: 16px;"
                "font-weight: bold;"
            );
        }
        else {
            labelFeedback->clear();
        }
    }
}