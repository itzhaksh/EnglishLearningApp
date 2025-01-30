#ifndef MEMORY_GAME_H
#define MEMORY_GAME_H

#include <QMainWindow>
#include <QMap>
#include <QList>
#include <QGridLayout>
#include <QPushButton>

class MemoryGame : public QMainWindow {
    Q_OBJECT
public:
    explicit MemoryGame(const QString& jsonFilePath, QWidget* parent = nullptr);


private slots:
    void handleCardClick();
    void resetGame();
    void goBack();
    void setButtonStyle(QPushButton* button, const QString& color);
    void handleMismatch(QPushButton* firstButton, QPushButton* secondButton);
    void handleMatch(QPushButton* firstButton, QPushButton* secondButton);

private:
    void loadWords(const QString& level);
    void initializeUI();
    void setupGameBoard();
    QMap<QString, QString> wordsMap;
    QList<QString> englishWords;

    QVBoxLayout* mainLayout;
    QGridLayout* cardGrid;
    QList<QPushButton*> englishButtons;
    QList<QPushButton*> hebrewButtons;
    QString jsonFilePath;
    QPushButton* firstSelectedButton = nullptr;
    int matchedPairs = 0;
};
#endif