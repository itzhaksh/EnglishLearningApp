#include "MainWindow.h"
#include "UserNameDialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    UserNameDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QString userName = dialog.getUserName();
        if (userName.isEmpty()) {
            return 0; 
        }

        MainWindow w;
        w.setWindowTitle("Hey " + userName);
        w.show();
        return a.exec();
    }
    return 0;
}
