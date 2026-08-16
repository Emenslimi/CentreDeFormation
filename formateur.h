#ifndef FORMATEUR_H
#define FORMATEUR_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMap>

class Formateur {
    int id;
    QString nom;
    QString prenom;
    QString specialite;
    QString email;
    QString telephone;
    double tarifHoraire;

public:
    Formateur();
    Formateur(int id, QString nom, QString prenom, QString specialite, QString email, QString telephone, double tarifHoraire);

    // Getters & Setters
    int getId() const { return id; }
    QString getNom() const { return nom; }
    QString getPrenom() const { return prenom; }
    QString getSpecialite() const { return specialite; }
    QString getEmail() const { return email; }
    QString getTelephone() const { return telephone; }
    double getTarifHoraire() const { return tarifHoraire; }

    // CRUD (Fonctionnalités de base)
    bool ajouter();
    bool supprimer(int id);
    bool modifier(int id);
    QSqlQueryModel* afficher();

    // Métiers
    QSqlQueryModel* rechercherEtTrier(QString champRecherche, QString valeur, QString champTri, QString ordre);
    QMap<QString, int> obtenirStatistiquesSpecialite();
    bool exporterPDF(QString filePath, int idFormateur);
    double calculerPaieEstimee(int idFormateur, int heuresTravaillees);
    bool verifierEmailUnique(QString email);
};

#endif // FORMATEUR_H