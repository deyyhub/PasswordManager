#include "mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSettings settings("DavidDev", "PasswordManager");
    bool primaVolta = !settings.contains("master_password");

    if (primaVolta) {
        bool ok;
        QString password = QInputDialog::getText(nullptr, "Configurazione", "Crea la tua Master Password:", QLineEdit::Password, "", &ok);

        if (ok && !password.isEmpty()) {
            settings.setValue("master_password", password);
            QMessageBox::information(nullptr, "Fatto!", "Password salvata! Riavvia l'app.");
        }
        return 0;
    }
    else {
        bool ok;
        QString passwordInserita = QInputDialog::getText(nullptr, "Accesso", "Inserisci la Master Password:", QLineEdit::Password, "", &ok);

        if (ok) {
            QString passwordSalvata = settings.value("master_password").toString();

            if (passwordInserita == passwordSalvata) {
                MainWindow w;
                w.show();
                return a.exec();
            } else {
                QMessageBox::critical(nullptr, "Errore", "Password Errata!");
                return 0;
            }
        }
        return 0;
    }

    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}