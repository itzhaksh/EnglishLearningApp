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
#include <QRandomGenerator>

GameWindow::GameWindow(const QString& mode, const QString& level, QWidget* parent)
    : QMainWindow(parent),
      mode(mode),
      currentLevel(level),
      score(0),
      currentWordCount(0),
      tts(new QTextToSpeech(this)),
      voiceButtonGroup(new QButtonGroup(this)),
      showAnswerButton(new QPushButton("הראה תשובה", this)),
      showExampleButton(new QPushButton("הצג דוגמא", this))
{
    if (mode == "English") {
        voiceLayout = new QHBoxLayout();
    } else {
        voiceLayout = new QVBoxLayout();
    }

    setupUI();
    loadDictionary(level);
    setupQuestion();

    connect(btnCheck, &QPushButton::clicked, this, &GameWindow::checkAnswer);
    connect(showExampleButton, &QPushButton::clicked, this, &GameWindow::showExample);
    connect(showAnswerButton, &QPushButton::clicked, this, &GameWindow::revealAnswer);

    if (mode == "Hebrew") {
        tts->setLocale(QLocale(QLocale::Hebrew, QLocale::Israel));
    } else {
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
    labelFeedback->setStyleSheet("color: black;");

    lineEditAnswer = new QLineEdit(this);
    lineEditAnswer->setFont(labelFont);
    lineEditAnswer->setStyleSheet(
        "QLineEdit {"
        "   background-color: rgba(255, 255, 255, 0.8);"
        "   color: black;"
        "   border: 1px solid white;"
        "   padding: 10px;"
        "}");

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

    showAnswerButton->setFont(buttonFont);
    showAnswerButton->setStyleSheet(buttonStyle);

    showExampleButton->setFont(buttonFont);
    showExampleButton->setStyleSheet(buttonStyle);

    contentLayout->addWidget(labelQuestion);

    QFrame* frame = new QFrame(this);
    frame->setFrameStyle(QFrame::Box | QFrame::Raised);
    frame->setLineWidth(2);
    QVBoxLayout* frameLayout = new QVBoxLayout(frame);
    frameLayout->addWidget(lineEditAnswer);
    frameLayout->setContentsMargins(5, 5, 5, 5);
    contentLayout->addWidget(frame);

    lineEditAnswer->setPlaceholderText(mode == "Hebrew" ?
        "...Type the English translation" : 
        "...הקלד את התרגום לעברית");       

    contentLayout->addWidget(btnCheck);
    contentLayout->addWidget(btnAudio);
    voiceLayout->addWidget(showAnswerButton);
    voiceLayout->addWidget(showExampleButton);
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
        if (child->widget() && child->widget() != showAnswerButton && child->widget() != showExampleButton) {
            delete child->widget();
        }
        delete child;
    }

    voiceLayout->addWidget(showAnswerButton);
    voiceLayout->addWidget(showExampleButton);

    voiceButtonGroup->setExclusive(true);

    QList<QVoice> filteredVoices;
    for (const QVoice& voice : voices) {
        if (mode == "Hebrew") {
            if (voice.locale().language() == QLocale::Hebrew && voice.name().contains("Asaf")) {
                filteredVoices.append(voice);
            }
        } else {
            if (voice.locale().language() == QLocale::English &&
                (voice.name().contains("David") || voice.name().contains("Mark") || voice.name().contains("Zira"))) {
                filteredVoices.append(voice);
            }
        }
    }

    if (filteredVoices.isEmpty()) {
        qWarning() << "No voices available for the selected language.";
        QMessageBox::warning(this, "Voice Error", "אין קולות זמינים עבור השפה שנבחרה.");
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

    if (mode != "Hebrew") {
        for (int i = 0; i < voices.size(); ++i) {
            QString voiceName = voices[i].name();
            QString icon = (voiceName.contains("David") || voiceName.contains("Mark")) ? "👨" : "👩";
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
    }

    if (!voiceButtonGroup->buttons().isEmpty()) {
        voiceButtonGroup->button(0)->setChecked(true);
        selectVoice(0);
    } else if (!voices.isEmpty()) {
        selectVoice(0); 
    }
}

void GameWindow::revealAnswer()
{
    QString answerText = (mode == "Hebrew") ? 
        QString("התשובה היא: %1").arg(correctAnswer) :
        QString("התשובה היא: %1").arg(correctAnswer);

    labelFeedback->setText(answerText);
    labelFeedback->setStyleSheet(
        "color: black;"
        "font-weight: bold;"
        "font-size: 18px;"
    );
    lineEditAnswer->setText(correctAnswer);
    showAnswerButton->setEnabled(false);
}

void GameWindow::showExample()
{
   if (examplesMap.contains(currentKey)) {
        QPair<QString, QString> examples = examplesMap[currentKey];
        QString exampleText;

        if (examples.first.isEmpty() && examples.second.isEmpty()) {
            exampleText = "אין דוגמא זמינה.";
        } else {
            QString hebrewText = examples.second.isEmpty() ? "משפט בעברית חסר" : examples.second;
            QString englishText = examples.first.isEmpty() ? "משפט באנגלית חסר" : examples.first;
            exampleText = QString("משפט: %1\nתרגום: %2").arg(hebrewText).arg(englishText);
        }

        labelFeedback->setText(exampleText);
        labelFeedback->setStyleSheet(
            "color: black;"
            "font-weight: bold;"
            "font-size: 16px;"
        );
    } else {
        labelFeedback->setText("אין דוגמא זמינה.");
        labelFeedback->setStyleSheet(
            "color: black;"
            "font-weight: bold;"
            "font-size: 16px;"
        );
    }
}

void GameWindow::selectVoice(int voiceIndex)
{
    if (voiceIndex >= 0 && voiceIndex < voices.size()) {
        tts->setVoice(voices[voiceIndex]);
        qDebug() << "בחירת קול:" << voices[voiceIndex].name()
                 << "שפה:" << voices[voiceIndex].locale().name();
    }
}

void GameWindow::playAudio()
{
    if (voices.isEmpty()) {
        qWarning() << "אין קולות זמינים.";
        return;
    }

    QString textToSpeak = currentQuestion;
    tts->say(textToSpeak);
}

void GameWindow::loadDictionary(const QString& level) {
    QString filePath = QString("resources/%1").arg(level);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "לא ניתן לפתוח קובץ מילון:" << filePath;
        QMessageBox::critical(this, "שגיאה", QString("נכשל בטעינת קובץ מילון: %1").arg(filePath));
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    dictionary.clear();
    examplesMap.clear();

    if (doc.isObject()) {
        QJsonObject jsonObject = doc.object();
        for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
            QString englishWord = it.key();
            QJsonObject wordObj = it.value().toObject();
            QString hebrewWord = removeHebrewDiacritics(wordObj["translation"].toString());
            QString exampleEn = wordObj["ex_en"].toString();
            QString exampleHe = wordObj["ex_he"].toString();

            if (mode == "Hebrew") {
                dictionary.insert(hebrewWord, englishWord);
                examplesMap.insert(hebrewWord, qMakePair(exampleEn, exampleHe));
            } else {
                dictionary.insert(englishWord, hebrewWord);
                examplesMap.insert(englishWord, qMakePair(exampleEn, exampleHe));
            }
        }
    }

    file.close();
    qDebug() << "נטענו" << dictionary.size() << "מילים במצב" << mode;
}

void GameWindow::setupQuestion()
{
    showAnswerButton->setEnabled(true);
    showExampleButton->setEnabled(true);
    QStringList keys = dictionary.keys();

    if (keys.isEmpty()) {
        labelQuestion->setText("אין שאלות זמינות");
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(keys.size());
    currentKey = keys.at(randomIndex);
    
    if (mode == "Hebrew") {
       
        currentQuestion = currentKey;     
        correctAnswer = dictionary[currentKey]; 
    } else {
        currentQuestion = currentKey;  
        correctAnswer = dictionary[currentKey]; 
    }

    labelQuestion->setText(currentQuestion);
    labelQuestion->setStyleSheet(
        "color: black;"
        "border: 2px solid white;"
        "padding: 10px;"
        "font-size: 26px;"
        "background-color: rgba(255, 255, 255, 0.3);"
        "border-radius: 8px;"
    );
    labelQuestion->setAlignment(Qt::AlignCenter);
    labelQuestion->setMinimumWidth(200);
    labelQuestion->setMinimumHeight(60);

    lineEditAnswer->clear();
    labelFeedback->clear();
}

QString GameWindow::removeHebrewDiacritics(const QString& text) {
    static QRegularExpression diacriticsRegex(QString::fromUtf8("[\\u0591-\\u05C7\\u05B0-\\u05BC\\u05C1-\\u05C2\\u05C4-\\u05C5\\u05C7]"));
    return text.normalized(QString::NormalizationForm_D).remove(diacriticsRegex);
}

void GameWindow::checkAnswer() {
    QString userAnswer = lineEditAnswer->text().trimmed();
    
    if (userAnswer.isEmpty()) {
        labelFeedback->setText("אנא הכנס תרגום");
        labelFeedback->setStyleSheet(
            "color: black;"
            "font-size: 18px;"
            "font-weight: bold;"
        );
        return;
    }

    QString cleanUserAnswer = removeHebrewDiacritics(userAnswer.toLower());
    QString cleanCorrectAnswer = removeHebrewDiacritics(correctAnswer.toLower());

    if (cleanUserAnswer == cleanCorrectAnswer) {
        labelFeedback->setText("נכון!");
        labelFeedback->setStyleSheet(
            "color: black;"
            "font-weight: bold;"
            "font-size: 18px;"
        );
        score++;
        labelScore->setText(QString("ניקוד: %1").arg(score));
        labelScore->setStyleSheet(
            "font-size: 22px;"
            "color: black;"
        );
        currentWordCount++;
        if (currentWordCount >= dictionary.count()) {
            QString levelText = QString("שלב %1").arg(currentLevel);
            QMessageBox::information(this, "השלב הושלם",
                QString("כל הכבוד! השלמת את %1!\nניקוד סופי: %2/%3")
                .arg(levelText)
                .arg(score)
                .arg(dictionary.count()));
            close();
            return;
        }
        setupQuestion();
        playAudio();
    } else {
        labelFeedback->setText("לא נכון, נסה שוב!");
        labelFeedback->setStyleSheet(
            "color: black;"
            "font-size: 18px;"
            "font-weight: bold;"
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
                "נא החלף למקלדת אנגלית" :
                "נא החלף למקלדת עברית");
            labelFeedback->setStyleSheet(
                "color: red;"
                "font-size: 16px;"
                "font-weight: bold;"
            );
        } else {
            labelFeedback->clear();
        }
    }
}