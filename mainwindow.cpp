#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Charger les données dans les tables dès l'ouverture
    rafraichirTables();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::rafraichirTables() {
    ui->tableView_formateur->setModel(formateurTmp.afficher());
    ui->tableView_cours->setModel(coursTmp.afficher());
}

// ---------------- CONTROLE DE SAISIE ----------------
bool MainWindow::validerControlesSaisieFormateur() {
    if (ui->lineEdit_id->text().isEmpty() || ui->lineEdit_nom->text().isEmpty() ||
        ui->lineEdit_email->text().isEmpty() || ui->lineEdit_phone->text().isEmpty()) {
        QMessageBox::warning(this, "Avertissement", "Veuillez remplir tous les champs obligatoires.");
        return false;
    }

    // Contrôle format Email
    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!emailRegex.match(ui->lineEdit_email->text()).hasMatch()) {
        QMessageBox::warning(this, "Erreur de saisie", "Format d'adresse email invalide.");
        return false;
    }

    // Contrôle téléphone (8 chiffres min)
    if (ui->lineEdit_phone->text().length() < 8) {
        QMessageBox::warning(this, "Erreur de saisie", "Numéro de téléphone invalide.");
        return false;
    }

    return true;
}

// ---------------- SLOTS FORMATEUR ----------------
void MainWindow::on_btn_ajouter_formateur_clicked() {
    if (!validerControlesSaisieFormateur()) return;

    int id = ui->lineEdit_id->text().toInt();
    QString nom = ui->lineEdit_nom->text();
    QString prenom = ui->lineEdit_prenom->text();
    QString spec = ui->combo_spec->currentText();
    QString email = ui->lineEdit_email->text();
    QString phone = ui->lineEdit_phone->text();
    double tarif = ui->lineEdit_tarif->text().toDouble();

    Formateur f(id, nom, prenom, spec, email, phone, tarif);

    if (f.ajouter()) {
        QMessageBox::information(this, "Succès", "Formateur ajouté avec succès !");
        rafraichirTables(); // Mise à jour automatique
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout (Vérifiez les contraintes/ID unique).");
    }
}

void MainWindow::on_btn_supprimer_formateur_clicked() {
    int id = ui->lineEdit_id_suppr->text().toInt();
    if (formateurTmp.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Formateur supprimé.");
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

// Métier 2 : Génération de Graphique Statistique (QtCharts)
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

// Métier 3 : Génération de document PDF Personnalisé
void MainWindow::on_btn_pdf_formateur_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QPainter painter(&printer);
    painter.setFont(QFont("Helvetica", 12));

    painter.drawText(100, 100, "===============================================");
    painter.drawText(100, 130, "        FICHE DES FORMATEURS REGISTRES        ");
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

// ---------------- AUTRES SLOTS FORMATEUR ----------------
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

    if (f.modifier(id)) { // <-- Ajout du paramètre id ici
        QMessageBox::information(this, "Succès", "Formateur modifié avec succès !");
        rafraichirTables();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification.");
    }
}

// ---------------- SLOTS COURS ----------------
void MainWindow::on_btn_ajouter_cours_clicked() {
    // Insérez ici votre logique d'ajout de cours
    QMessageBox::information(this, "Cours", "Action Ajouter Cours déclenchée.");
}

void MainWindow::on_btn_supprimer_cours_clicked() {
    // Insérez ici votre logique de suppression de cours
    QMessageBox::information(this, "Cours", "Action Supprimer Cours déclenchée.");
}