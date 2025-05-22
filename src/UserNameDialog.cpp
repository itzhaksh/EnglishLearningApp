#include "UserNameDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QPixmap>
#include <QResizeEvent>
#include <QSettings>
#include <QFontDatabase>

UserNameDialog::UserNameDialog(QWidget* parent)
    : QDialog(parent), userNameEdit(new QLineEdit(this)), passwordEdit(new QLineEdit(this)), imageLabel(new QLabel(this)) {
    setWindowTitle("Login");

    QIcon windowIcon(":/Learn-English-Icon.png");
    setWindowIcon(windowIcon);

    imageLabel->setScaledContents(true); 
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    QPixmap pixmap(":/Learn-English.png");
    imageLabel->setPixmap(pixmap.scaled(600, 400, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    imageLabel->setFixedSize(600, 400); 

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(imageLabel);
    
    QWidget* contentWidget = new QWidget(this);
    contentWidget->setStyleSheet("background: transparent;"); 
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);

    QLabel* heading = new QLabel("Learn,practice,succeed!", this);
    QFont handwrittenFont("Comic Sans MS", 20);
    handwrittenFont.setBold(true);
    heading->setFont(handwrittenFont);
    heading->setStyleSheet("color: rgb(23, 22, 22); text-align: center;");
    heading->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(heading);

    userNameEdit->setPlaceholderText("שם משתמש");
    userNameEdit->setStyleSheet("background-color: rgba(255, 255, 255, 0.8); border: none; border-radius: 8px; padding: 10px; font-size: 16px;");
    contentLayout->addWidget(userNameEdit);

    passwordEdit->setPlaceholderText("סיסמה");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet("background-color: rgba(255, 255, 255, 0.8); border: none; border-radius: 8px; padding: 10px; font-size: 16px;");
    contentLayout->addWidget(passwordEdit);

    QPushButton* loginButton = new QPushButton("התחבר", this);
    loginButton->setStyleSheet("background-color: #ebbf00; color: white; border-radius: 8px; padding: 10px; font-size: 16px; font-weight: bold;");
    connect(loginButton, &QPushButton::clicked, this, &UserNameDialog::validateUserName);
    contentLayout->addWidget(loginButton);
    
    mainLayout->addWidget(contentWidget);
    setLayout(mainLayout);

    resize(600, 400);

    imageLabel->lower();
}

QString UserNameDialog::getUserName() const {
    return userNameEdit->text();
}

QString UserNameDialog::getPassword() const {
    return passwordEdit->text();
}

void UserNameDialog::validateUserName() {
    QSettings settings("MyCompany", "EnglishLearningApp");
    QString userName = userNameEdit->text();
    QString password = passwordEdit->text();

    if (userName.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "אנא מלא את כל השדות.");
        return;
    }

    QString key = QString("%1/password").arg(userName);
    if (settings.contains(key)) {
        QString storedPassword = settings.value(key).toString();
        if (storedPassword == password) {
            settings.setValue("UserName", userName);
            accept();
        } else {
            QMessageBox::critical(this, "Error", "סיסמה שגויה. נסה שוב.");
        }
    } else {
        settings.setValue(key, password);
        settings.setValue("UserName", userName);
        QMessageBox::information(this, "Success", "משתמש נרשם בהצלחה!");
        accept();
    }
}

void UserNameDialog::resizeEvent(QResizeEvent* event) {
    QDialog::resizeEvent(event);
}