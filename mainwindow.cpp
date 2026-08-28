#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formateur.h"
#include "cours.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QRegularExpression>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    rafraichirTables();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::rafraichirTables() {
    ui->tableView_formateur->setModel(formateurTmp.afficher());
    ui->tableView_cours->setModel(coursTmp.afficher());
}

// ---------------- CONTROLES DE SAISIE ----------------

bool MainWindow::validerControlesSaisieFormateur() {
    if (ui->lineEdit_id->text().isEmpty() || ui->lineEdit_nom->text().isEmpty() ||
        ui->lineEdit_email->text().isEmpty() || ui->lineEdit_phone->text().isEmpty()) {
        QMessageBox::warning(this, "Avertissement", "Veuillez remplir tous les champs obligatoires du formateur.");
        return false;
    }

    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!emailRegex.match(ui->lineEdit_email->text()).hasMatch()) {
        QMessageBox::warning(this, "Erreur de saisie", "Format d'adresse email invalide.");
        return false;
    }

    if (ui->lineEdit_phone->text().length() < 8) {
        QMessageBox::warning(this, "Erreur de saisie", "Numéro de téléphone invalide.");
        return false;
    }

    return true;
}

bool MainWindow::validerControlesSaisieCours() {
    if (ui->lineEdit_cours_id->text().isEmpty() || ui->lineEdit_cours_titre->text().isEmpty() ||
        ui->lineEdit_cours_duree->text().isEmpty() || ui->lineEdit_cours_prix->text().isEmpty() ||
        ui->lineEdit_cours_id_formateur->text().isEmpty()) {
        QMessageBox::warning(this, "Avertissement", "Veuillez remplir tous les champs du cours.");
        return false;
    }

    bool okDuree, okPrix;
    int duree = ui->lineEdit_cours_duree->text().toInt(&okDuree);
    double prix = ui->lineEdit_cours_prix->text().toDouble(&okPrix);

    if (!okDuree || duree <= 0) {
        QMessageBox::warning(this, "Erreur de saisie", "La durée doit être un nombre entier positif.");
        return false;
    }

    if (!okPrix || prix <= 0) {
        QMessageBox::warning(this, "Erreur de saisie", "Le prix doit être un nombre positif.");
        return false;
    }

    return true;
}

// ---------------- SLOTS FORMATEUR ----------------

void MainWindow::on_btn_ajouter_formateur_clicked() {
    if (!validerControlesSaisieFormateur()) return;

    QString email = ui->lineEdit_email->text();

    // UTILISATION DU MÉTIER 5 : Vérification de l'unicité de l'email
    if (!formateurTmp.verifierEmailUnique(email)) {
        QMessageBox::warning(this, "Avertissement", "Cette adresse email est déjà utilisée par un autre formateur !");
        return;
    }

    int id = ui->lineEdit_id->text().toInt();
    QString nom = ui->lineEdit_nom->text();
    QString prenom = ui->lineEdit_prenom->text();
    QString spec = ui->combo_spec->currentText();
    QString phone = ui->lineEdit_phone->text();
    double tarif = ui->lineEdit_tarif->text().toDouble();

    Formateur f(id, nom, prenom, spec, email, phone, tarif);

    if (f.ajouter()) {
        QMessageBox::information(this, "Succès", "Formateur ajouté avec succès !");
        rafraichirTables();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout (Vérifiez l'ID unique).");
    }
}

void MainWindow::on_btn_modifier_formateur_clicked() {
    if (!validerControlesSaisieFormateur()) return;

    int id = ui->lineEdit_id->text().toInt();
    QString nom = ui->lineEdit_nom->text();
    QString prenom = ui->lineEdit_prenom->text();
    QString spec = ui->combo_spec->currentText();
    QString email = ui->lineEdit_email->text();
    QString phone = ui->lineEdit_phone->text();
    double tarif = ui->lineEdit_tarif->text().toDouble();

    Formateur f(id, nom, prenom, spec, email, phone, tarif);

    if (f.modifier(id)) {
        QMessageBox::information(this, "Succès", "Formateur modifié avec succès !");
        rafraichirTables();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification.");
    }
}

void MainWindow::on_btn_supprimer_formateur_clicked() {
    if (ui->lineEdit_id_suppr->text().isEmpty()) {
        QMessageBox::warning(this, "Avertissement", "Veuillez saisir l'ID du formateur à supprimer.");
        return;
    }

    int id = ui->lineEdit_id_suppr->text().toInt();
    if (formateurTmp.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Formateur supprimé avec succès !");
        rafraichirTables();
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de supprimer cet identifiant.");
    }
}

void MainWindow::on_btn_rechercher_formateur_clicked() {
    QString critereRec = ui->combo_recherche_critere->currentText();
    QString valeur = ui->lineEdit_recherche_valeur->text();
    QString critereTri = ui->combo_tri_critere->currentText();
    QString ordre = ui->combo_ordre->currentText();

    ui->tableView_formateur->setModel(formateurTmp.rechercherEtTrier(critereRec, valeur, critereTri, ordre));
}

// UTILISATION DU MÉTIER 4 : Calcul de la paie estimée
void MainWindow::on_btn_calculer_paie_clicked() {
    int id = -1;

    // 1. Si l'ID est saisi dans le lineEdit_id
    if (!ui->lineEdit_id->text().isEmpty()) {
        id = ui->lineEdit_id->text().toInt();
    }
    // 2. Sinon, on vérifie si une ligne est sélectionnée dans le tableView
    else if (ui->tableView_formateur->currentIndex().isValid()) {
        int row = ui->tableView_formateur->currentIndex().row();
        id = ui->tableView_formateur->model()->data(ui->tableView_formateur->model()->index(row, 0)).toInt();
    }
    // 3. Si aucun des deux n'est disponible
    else {
        QMessageBox::warning(this, "Sélection requise", "Veuillez d'abord saisir un ID ou sélectionner une ligne dans le tableau.");
        return;
    }

    bool ok;
    int heures = QInputDialog::getInt(this, "Calcul de Paie",
                                      "Entrez le nombre d'heures effectuées ce mois-ci :",
                                      160, 1, 300, 1, &ok);

    if (ok) {
        double paieTotale = formateurTmp.calculerPaieEstimee(id, heures);

        if (paieTotale > 0) {
            QMessageBox::information(this, "Fiche de Paie Estimée",
                                     QString("<b>Formateur ID :</b> %1<br>"
                                             "<b>Heures travaillées :</b> %2 h<br>"
                                             "<b>Montant Total à payer :</b> <font color='green'><b>%3 DT</b></font>")
                                         .arg(id).arg(heures).arg(paieTotale));
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de calculer la paie (Formateur introuvable ou tarif non défini).");
        }
    }
}

void MainWindow::on_btn_stat_formateur_clicked() {
    QMap<QString, int> stats = formateurTmp.obtenirStatistiquesSpecialite();

    QPieSeries *series = new QPieSeries();
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        series->append(it.key() + " (" + QString::number(it.value()) + ")", it.value());
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des Formateurs par Spécialité");
    chart->setAnimationOptions(QChart::AllAnimations);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(600, 400);
    chartView->show();
}

void MainWindow::on_btn_pdf_formateur_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QPainter painter(&printer);
    painter.setFont(QFont("Helvetica", 12));

    painter.drawText(100, 100, "===============================================");
    painter.drawText(100, 130, "         FICHE DES FORMATEURS REGISTRES        ");
    painter.drawText(100, 160, "===============================================");

    QSqlQueryModel *model = formateurTmp.afficher();
    int y = 220;

    for (int i = 0; i < model->rowCount(); ++i) {
        QString ligne = QString::number(model->data(model->index(i, 0)).toInt()) + " | " +
                        model->data(model->index(i, 1)).toString() + " " +
                        model->data(model->index(i, 2)).toString() + " | Spécialité : " +
                        model->data(model->index(i, 3)).toString() + " | " +
                        model->data(model->index(i, 6)).toString() + " DT/h";
        painter.drawText(100, y, ligne);
        y += 30;
    }

    painter.end();
    QMessageBox::information(this, "PDF", "Document PDF généré avec succès !");
}

// ---------------- SLOTS COURS ----------------

void MainWindow::on_btn_ajouter_cours_clicked() {
    if (!validerControlesSaisieCours()) return;

    int id = ui->lineEdit_cours_id->text().toInt();
    QString titre = ui->lineEdit_cours_titre->text();
    QString categorie = ui->combo_cours_cat->currentText();
    int duree = ui->lineEdit_cours_duree->text().toInt();
    double prix = ui->lineEdit_cours_prix->text().toDouble();
    int idFormateur = ui->lineEdit_cours_id_formateur->text().toInt();

    Cours c(id, titre, categorie, duree, prix, idFormateur);

    if (c.ajouter()) {
        QMessageBox::information(this, "Succès", "Cours ajouté avec succès !");
        rafraichirTables();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout du cours.");
    }
}

void MainWindow::on_btn_modifier_cours_clicked() {
    if (!validerControlesSaisieCours()) return;

    int id = ui->lineEdit_cours_id->text().toInt();
    QString titre = ui->lineEdit_cours_titre->text();
    QString categorie = ui->combo_cours_cat->currentText();
    int duree = ui->lineEdit_cours_duree->text().toInt();
    double prix = ui->lineEdit_cours_prix->text().toDouble();
    int idFormateur = ui->lineEdit_cours_id_formateur->text().toInt();

    Cours c(id, titre, categorie, duree, prix, idFormateur);

    if (c.modifier(id)) {
        QMessageBox::information(this, "Succès", "Cours modifié avec succès !");
        rafraichirTables();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification du cours.");
    }
}

void MainWindow::on_btn_supprimer_cours_clicked() {
    if (ui->lineEdit_cours_id_suppr->text().isEmpty()) {
        QMessageBox::warning(this, "Avertissement", "Veuillez saisir l'ID du cours à supprimer.");
        return;
    }

    int id = ui->lineEdit_cours_id_suppr->text().toInt();
    if (coursTmp.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Cours supprimé avec succès !");
        rafraichirTables();
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de supprimer ce cours.");
    }
}

void MainWindow::on_btn_rechercher_cours_clicked() {
    QString critereRec = ui->combo_cours_recherche_critere->currentText();
    QString valeur = ui->lineEdit_cours_recherche_valeur->text();
    QString critereTri = ui->combo_cours_tri_critere->currentText();
    QString ordre = ui->combo_cours_ordre->currentText();

    ui->tableView_cours->setModel(coursTmp.rechercherEtTrier(critereRec, valeur, critereTri, ordre));
}

void MainWindow::on_btn_stat_cours_clicked() {
    QMap<QString, int> stats = coursTmp.obtenirStatistiquesCategorie();

    QPieSeries *series = new QPieSeries();
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        series->append(it.key() + " (" + QString::number(it.value()) + ")", it.value());
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des Cours par Catégorie");
    chart->setAnimationOptions(QChart::AllAnimations);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(600, 400);
    chartView->show();
}

void MainWindow::on_btn_pdf_cours_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter Cours en PDF", "", "*.pdf");
    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QPainter painter(&printer);
    painter.setFont(QFont("Helvetica", 12));

    painter.drawText(100, 100, "===============================================");
    painter.drawText(100, 130, "            CATALOGUE DES COURS                ");
    painter.drawText(100, 160, "===============================================");

    QSqlQueryModel *model = coursTmp.afficher();
    int y = 220;

    for (int i = 0; i < model->rowCount(); ++i) {
        QString ligne = QString::number(model->data(model->index(i, 0)).toInt()) + " | " +
                        model->data(model->index(i, 1)).toString() + " | Cat : " +
                        model->data(model->index(i, 2)).toString() + " | Durée : " +
                        model->data(model->index(i, 3)).toString() + "h | Prix : " +
                        model->data(model->index(i, 4)).toString() + " DT";
        painter.drawText(100, y, ligne);
        y += 30;
    }

    painter.end();
    QMessageBox::information(this, "PDF", "Catalogue des cours exporté en PDF avec succès !");
}