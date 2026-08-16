#include "cours.h"

Cours::Cours() { id = 0; dureeHeures = 0; prix = 0.0; idFormateur = 0; }

Cours::Cours(int id, QString titre, QString categorie, int dureeHeures, double prix, int idFormateur) {
    this->id = id;
    this->titre = titre;
    this->categorie = categorie;
    this->dureeHeures = dureeHeures;
    this->prix = prix;
    this->idFormateur = idFormateur;
}

bool Cours::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO COURS (ID_COURS, TITRE, CATEGORIE, DUREE_HEURES, PRIX, ID_FORMATEUR) "
                  "VALUES (:id, :titre, :cat, :duree, :prix, :id_f)");
    query.bindValue(":id", id);
    query.bindValue(":titre", titre);
    query.bindValue(":cat", categorie);
    query.bindValue(":duree", dureeHeures);
    query.bindValue(":prix", prix);
    query.bindValue(":id_f", idFormateur);
    return query.exec();
}

bool Cours::supprimer(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM COURS WHERE ID_COURS = :id");
    query.bindValue(":id", id);
    return query.exec();
}

bool Cours::modifier(int id) {
    QSqlQuery query;
    query.prepare("UPDATE COURS SET TITRE = :titre, CATEGORIE = :cat, DUREE_HEURES = :duree, "
                  "PRIX = :prix, ID_FORMATEUR = :id_f WHERE ID_COURS = :id");
    query.bindValue(":id", id);
    query.bindValue(":titre", titre);
    query.bindValue(":cat", categorie);
    query.bindValue(":duree", dureeHeures);
    query.bindValue(":prix", prix);
    query.bindValue(":id_f", idFormateur);
    return query.exec();
}

QSqlQueryModel* Cours::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT C.ID_COURS, C.TITRE, C.CATEGORIE, C.DUREE_HEURES, C.PRIX, F.NOM || ' ' || F.PRENOM AS FORMATEUR "
                    "FROM COURS C LEFT JOIN FORMATEUR F ON C.ID_FORMATEUR = F.ID_FORMATEUR");
    return model;
}

QSqlQueryModel* Cours::rechercherEtTrier(QString champRecherche, QString valeur, QString champTri, QString ordre) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    QString sql = "SELECT C.ID_COURS, C.TITRE, C.CATEGORIE, C.DUREE_HEURES, C.PRIX, F.NOM "
                  "FROM COURS C LEFT JOIN FORMATEUR F ON C.ID_FORMATEUR = F.ID_FORMATEUR WHERE 1=1";

    if (!valeur.isEmpty()) {
        if (champRecherche == "Titre") sql += " AND LOWER(C.TITRE) LIKE LOWER(:valeur)";
        else if (champRecherche == "Catégorie") sql += " AND LOWER(C.CATEGORIE) LIKE LOWER(:valeur)";
    }

    if (champTri == "Durée") sql += " ORDER BY C.DUREE_HEURES";
    else if (champTri == "Prix") sql += " ORDER BY C.PRIX";
    else if (champTri == "Titre") sql += " ORDER BY C.TITRE";

    sql += (ordre == "DESC") ? " DESC" : " ASC";

    query.prepare(sql);
    if (!valeur.isEmpty()) query.bindValue(":valeur", "%" + valeur + "%");
    query.exec();

    model->setQuery(query);
    return model;
}

QMap<QString, double> Cours::obtenirPrixMoyenParCategorie() {
    QMap<QString, double> stats;
    QSqlQuery query("SELECT CATEGORIE, AVG(PRIX) FROM COURS GROUP BY CATEGORIE");
    while (query.next()) {
        stats[query.value(0).toString()] = query.value(1).toDouble();
    }
    return stats;
}