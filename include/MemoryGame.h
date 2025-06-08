#ifndef MEMORY_GAME_H
#define MEMORY_GAME_H

#include <QMainWindow>
#include <QMap>
#include <QList>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSoundEffect>

class MemoryGame : public QMainWindow {
    Q_OBJECT
public:
    explicit MemoryGame(const QString& jsonFilePath, QWidget* parent = nullptr);

signals:
    void memoryGameFinished(int score);

private slots:
    void handleCardClick();
    void resetGame();
    void goBack();
    void setButtonStyle(QPushButton* button, const QString& color);
    void handleMismatch(QPushButton* firstButton, QPushButton* secondButton);
    void handleMatch(QPushButton* firstButton, QPushButton* secondButton);
    void saveScore(const QString& gameType, int score);
    void playCardFlipAnimation(QPushButton* button);
    void playMatchAnimation(QPushButton* button1, QPushButton* button2);
    void playMismatchAnimation(QPushButton* button1, QPushButton* button2);

private:
    void loadWords(const QString& level);
    void initializeUI();
    void setupGameBoard();
    void filterUsedWords(QList<QString>& words);
    void saveUsedWords(const QList<QString>& words);
    void setupSoundEffects();

    QMap<QString, QString> wordsMap;
    QList<QString> englishWords;

    QVBoxLayout* mainLayout;
    QGridLayout* cardGrid;
    QList<QPushButton*> englishButtons;
    QList<QPushButton*> hebrewButtons;
    QString jsonFilePath;
    QPushButton* firstSelectedButton = nullptr;
    QLabel* imageLabel;
    int matchedPairs = 0;

    // Sound effects
    QSoundEffect* matchSound;
    QSoundEffect* mismatchSound;
    QSoundEffect* flipSound;
};

#endif // MEMORY_GAME_H