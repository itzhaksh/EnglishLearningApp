#ifndef USERNAMEDIALOG_H
#define USERNAMEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>

class UserNameDialog : public QDialog {
    Q_OBJECT

public:
    explicit UserNameDialog(QWidget* parent = nullptr);
    QString getUserName() const;
    QString getPassword() const;

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void validateUserName();

private:
    QLineEdit* userNameEdit;
    QLineEdit* passwordEdit;
    QLabel* imageLabel;
};

#endif // USERNAMEDIALOG_H