#include "formateur.h"

Formateur::Formateur() {
    id = 0;
    tarifHoraire = 0.0;
}

Formateur::Formateur(int id, QString nom, QString prenom, QString specialite, QString email, QString telephone, double tarifHoraire) {
    this->id = id;
    this->nom = nom;
    this->prenom = prenom;
    this->specialite = specialite;
    this->email = email;
    this->telephone = telephone;
    this->tarifHoraire = tarifHoraire;
}

// ---------------- CRUD ----------------

bool Formateur::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO FORMATEUR (ID_FORMATEUR, NOM, PRENOM, SPECIALITE, EMAIL, TELEPHONE, TARIF_HORAIRE) "
                  "VALUES (:id, :nom, :prenom, :specialite, :email, :telephone, :tarif)");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":specialite", specialite);
    query.bindValue(":email", email);
    query.bindValue(":telephone", telephone);
    query.bindValue(":tarif", tarifHoraire);

    return query.exec();
}

bool Formateur::supprimer(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM FORMATEUR WHERE ID_FORMATEUR = :id");
    query.bindValue(":id", id);
    return query.exec();
}

bool Formateur::modifier(int id) {
    QSqlQuery query;
    query.prepare("UPDATE FORMATEUR SET NOM = :nom, PRENOM = :prenom, SPECIALITE = :specialite, "
                  "EMAIL = :email, TELEPHONE = :telephone, TARIF_HORAIRE = :tarif WHERE ID_FORMATEUR = :id");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":specialite", specialite);
    query.bindValue(":email", email);
    query.bindValue(":telephone", telephone);
    query.bindValue(":tarif", tarifHoraire);

    return query.exec();
}

QSqlQueryModel* Formateur::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM FORMATEUR ORDER BY ID_FORMATEUR ASC");
    return model;
}

// ---------------- FONCTIONNALITÉS AVANCÉES (MÉTIERS) ----------------

// Métier 1 : Recherche & Tri multi-critères
QSqlQueryModel* Formateur::rechercherEtTrier(QString champRecherche, QString valeur, QString champTri, QString ordre) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    QString sql = "SELECT * FROM FORMATEUR WHERE 1=1";

    if (!valeur.isEmpty()) {
        if (champRecherche == "Nom") sql += " AND LOWER(NOM) LIKE LOWER(:valeur)";
        else if (champRecherche == "Spécialité") sql += " AND LOWER(SPECIALITE) LIKE LOWER(:valeur)";
        else if (champRecherche == "Téléphone") sql += " AND TELEPHONE LIKE :valeur";
    }

    if (champTri == "ID") sql += " ORDER BY ID_FORMATEUR";
    else if (champTri == "Nom") sql += " ORDER BY NOM";
    else if (champTri == "Tarif") sql += " ORDER BY TARIF_HORAIRE";

    sql += (ordre == "DESC") ? " DESC" : " ASC";

    query.prepare(sql);
    if (!valeur.isEmpty()) {
        query.bindValue(":valeur", "%" + valeur + "%");
    }

    query.exec();
    model->setQuery(query);
    return model;
}

// Métier 2 : Statistiques par Spécialité
QMap<QString, int> Formateur::obtenirStatistiquesSpecialite() {
    QMap<QString, int> stats;
    QSqlQuery query("SELECT SPECIALITE, COUNT(*) FROM FORMATEUR GROUP BY SPECIALITE");
    while (query.next()) {
        stats[query.value(0).toString()] = query.value(1).toInt();
    }
    return stats;
}

// Métier 4 : Calcul Paie Estimée
double Formateur::calculerPaieEstimee(int idFormateur, int heuresTravaillees) {
    QSqlQuery query;
    query.prepare("SELECT TARIF_HORAIRE FROM FORMATEUR WHERE ID_FORMATEUR = :id");
    query.bindValue(":id", idFormateur);
    if (query.exec() && query.next()) {
        double tarif = query.value(0).toDouble();
        return tarif * heuresTravaillees;
    }
    return 0.0;
}

// Métier 5 : Vérification Unicité Email
bool Formateur::verifierEmailUnique(QString email) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM FORMATEUR WHERE EMAIL = :email");
    query.bindValue(":email", email);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() == 0;
    }
    return false;
}