#ifndef PRACTICEWINDOW_H
#define PRACTICEWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include <QTextToSpeech>

class PracticeWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit PracticeWindow(QWidget* parent = nullptr);
    ~PracticeWindow();

private slots:
    void nextWord();
    void prevWord();
    void playSound();
    void closeWindow();

private:
    void loadWords();
    void updateDisplay();

    QLabel* englishWordLabel;
    QLabel* hebrewTranslationLabel;
    QPushButton* playSoundButton;
    QPushButton* nextButton;
    QPushButton* prevButton;
    QPushButton* closeButton;

    QMap<QString, QString> wordsMap;
    QList<QString> englishWords;
    int currentIndex;
    QTextToSpeech* textToSpeech;
    QList<QVoice> voices;

};

#endif // PRACTICEWINDOW_H
