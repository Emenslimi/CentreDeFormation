#include "mainwindow.h"
#include "connection.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Connection c;
    bool test = c.createconnect();

    MainWindow w;
    if (test) {
        w.show();
        QMessageBox::information(nullptr, "Base de Données", "Connexion à Oracle établie avec succès.");
    } else {
        QMessageBox::critical(nullptr, "Base de Données", "Échec de la connexion à Oracle. L'application va se fermer.");
        return -1;
    }

    return a.exec();
}