#ifndef COURS_H
#define COURS_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMap>

class Cours {
    int id;
    QString titre;
    QString categorie;
    int dureeHeures;
    double prix;
    int idFormateur;

public:
    Cours();
    Cours(int id, QString titre, QString categorie, int dureeHeures, double prix, int idFormateur);

    // CRUD
    bool ajouter();
    bool supprimer(int id);
    bool modifier(int id);
    QSqlQueryModel* afficher();

    // Métiers
    QSqlQueryModel* rechercherEtTrier(QString champRecherche, QString valeur, QString champTri, QString ordre);
    QMap<QString, double> obtenirPrixMoyenParCategorie();
    bool exporterPDF(QString filePath);
};

#endif // COURS_H