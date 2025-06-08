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
    explicit PracticeWindow(const QString& jsonFile, QWidget* parent = nullptr);

    ~PracticeWindow();

private slots:
    void nextWord();
    void prevWord();
    void playSound();
    void goBack();
private:
    void loadWords(QString& jsonFile);
    void updateDisplay();
    void loadWords(const QString& jsonFile);



    QLabel* englishWordLabel;
    QLabel* hebrewTranslationLabel;
    QPushButton* playSoundButton;
    QPushButton* nextButton;
    QPushButton* prevButton;
    QPushButton* closeButton;
    QPushButton* backButton;


    QMap<QString, QString> wordsMap;
    QStringList englishWords;
    QMap<QString, QPair<QString, QString>> examplesMap; // מאחסן את הדוגמאות
    QLabel* exampleEnLabel;
    QLabel* exampleHeLabel;
    int currentIndex;
    QTextToSpeech* textToSpeech;
    QList<QVoice> voices;

};

#endif // PRACTICEWINDOW_H
