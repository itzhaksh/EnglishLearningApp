#include "StatsTableWidget.h"
#include <QSettings>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QHeaderView>

StatsTableWidget::StatsTableWidget(QWidget* parent)
    : QWidget(parent)
{
    QSettings settings("MyCompany", "EnglishLearningApp");

    QLabel* imageLabel = new QLabel(this);
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
    imageLabel->setPixmap(transparentPixmap.scaled(500, 400, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    imageLabel->setFixedSize(500, 400);
    imageLabel->lower();

    QTableWidget* table = new QTableWidget(this);
    table->setColumnCount(3); 
    table->setHorizontalHeaderLabels({"שם משתמש", "סוג משחק", "ניקוד"});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers); 
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setStyleSheet(
        "QTableWidget {"
        "   background-color: rgba(255, 255, 255, 0.7);"
        "   color: #333333;"
        "   font-size: 16px;"
        "   border: 3px solid #ebbf00;"
        "   border-radius: 10px;"
        "   padding: 5px;"
        "}"
        "QTableWidget::item {"
        "   padding: 10px;"
        "   background-color: transparent;" 
        "   color: #333333;" 
        "   border: none;" 
        "}"
        "QHeaderView::section {"
        "   background-color: #ebbf00;"
        "   color: black;"
        "   font-weight: bold;"
        "   font-size: 18px;"
        "   padding: 10px;"
        "   border: 1px solid #ebbf00;"
        "   border-radius: 5px;"
        "}"
        "QTableWidget::item:hover {"
        "   background-color: transparent;" 
        "   color: #333333;" 
        "}"
    );

    QStringList keys = settings.allKeys();
    for (const QString& key : keys) {
        QStringList parts = key.split('/');
        if (parts.size() == 3 && parts[2] == "score") {
            QString userName = parts[0];
            QString gameType = parts[1];
            int score = settings.value(key).toInt();

            int row = table->rowCount();
            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(userName));
            table->setItem(row, 1, new QTableWidgetItem(gameType));
            table->setItem(row, 2, new QTableWidgetItem(QString::number(score)));

            table->item(row, 0)->setFlags(table->item(row, 0)->flags() & ~Qt::ItemIsEditable);
            table->item(row, 1)->setFlags(table->item(row, 1)->flags() & ~Qt::ItemIsEditable);
            table->item(row, 2)->setFlags(table->item(row, 2)->flags() & ~Qt::ItemIsEditable);
        }
    }

    QPushButton* deleteScoresBtn = new QPushButton("מחק ניקוד", this);
    deleteScoresBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #ebbf00;"
        "   color: black;"
        "   font-size: 16px;"
        "   padding: 12px;"
        "   border-radius: 10px;"
        "   font-weight: bold;"
        "   box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);"
        "}"
        "QPushButton:hover {"
        "   background-color: #D4A017;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #ebbf00;"
        "}"
    );
    connect(deleteScoresBtn, &QPushButton::clicked, [this, table]() {
        deleteUserScores(table);
    });

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(table);
    layout->addWidget(deleteScoresBtn, 0, Qt::AlignCenter);
    setLayout(layout);

    resize(500, 400);

    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - this->width()) / 2;
        int y = (screenGeometry.height() - this->height()) / 2;
        this->move(x, y);
    }
}

void StatsTableWidget::addScore(const QString& gameType, int score)
{
    QSettings settings("MyCompany", "EnglishLearningApp");
    QString userName = settings.value("UserName", "").toString();

    QString key = QString("%1/%2/score").arg(userName).arg(gameType);
    settings.setValue(key, score);
}

void StatsTableWidget::deleteUserScores(QTableWidget* table)
{
    QSettings settings("MyCompany", "EnglishLearningApp");

    QString userName = QInputDialog::getText(this, "Authentication", "Enter your username:");
    QString password = QInputDialog::getText(this, "Authentication", "Enter your password:", QLineEdit::Password);

    QString storedUserName = settings.value("UserName", "").toString();
    QString storedPasswordKey = QString("%1/password").arg(userName); 
    QString storedPassword = settings.value(storedPasswordKey).toString();

    if (userName.isEmpty() || password.isEmpty() || userName != storedUserName || password != storedPassword) {
        QMessageBox::critical(this, "Error", "Invalid username or password!");
        return;
    }

    QStringList keys = settings.allKeys();
    for (const QString& key : keys) {
        if (key.startsWith(userName + "/")) {
            settings.remove(key);
        }
    }

    for (int i = table->rowCount() - 1; i >= 0; --i) {
        if (table->item(i, 0)->text() == userName) {
            table->removeRow(i);
        }
    }

    QMessageBox::information(this, "Success", "Your scores have been deleted.");
}

void printSettingsLocation()
{
    QSettings settings("MyCompany", "EnglishLearningApp");
    qDebug() << "Settings file location:" << settings.fileName();
}