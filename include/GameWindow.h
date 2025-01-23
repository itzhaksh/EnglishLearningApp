#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextToSpeech>
#include <QVBoxLayout>
#include <QButtonGroup>

class GameWindow : public QMainWindow
{
    Q_OBJECT
public:
    GameWindow(const QString& mode, const QString& difficulty, QWidget* parent = nullptr);
    ~GameWindow();

private slots:
    void checkAnswer();
    void close();
    void selectVoice(int voiceIndex);
    void playAudio();

private:

    void loadDictionary();
    void setupQuestion();
    void setupUI();
    void updateVoiceButtons();

    QLabel* labelQuestion;
    QLabel* labelFeedback;
    QLabel* labelScore;
    QLineEdit* lineEditAnswer;
    QPushButton* btnCheck;
    QPushButton* btnClose;
    QPushButton* btnAudio;

    QJsonObject dictionary;
    QString currentQuestion;
    QString correctAnswer;
    QString mode;
    int score;

    QString difficulty;
    int wordsLimit;
    int currentWordCount;

    QTextToSpeech* tts;
    QList<QVoice> voices;
    QVBoxLayout* voiceLayout; // Layout for voice buttons
    QButtonGroup* voiceButtonGroup;
};

#endif // GAMEWINDOW_H
