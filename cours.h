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
    QMap<QString, int> obtenirStatistiquesCategorie();
    bool exporterPDF(QString filePath);

    // Calcule la marge estimée avec des coefficients d'ajustement
    double simulerMargeAjustee(double prixBase, int duree, double tarifFormateur, double coefInfrastructures);

    // Vérifie la compatibilité entre la spécialité et la catégorie
    bool verifierCompatibilite(QString specialiteFormateur, QString categorieCours);

    // Vérifie si l'affectation d'un cours entraîne la surcharge du formateur (quota maximum d'heures dépassé)
    bool verifierSurchargeFormateur(int idFormateur, int nouvelleDuree, int seuilMaxHeures = 100);
};

#endif // COURS_H