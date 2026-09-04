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

// En-têtes requis pour la génération du QR Code via Network API
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QUrl>

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

void MainWindow::on_tableView_formateur_clicked(const QModelIndex &index) {
    int row = index.row();
    QAbstractItemModel *model = ui->tableView_formateur->model();

    ui->lineEdit_id->setText(model->data(model->index(row, 0)).toString());
    ui->lineEdit_nom->setText(model->data(model->index(row, 1)).toString());
    ui->lineEdit_prenom->setText(model->data(model->index(row, 2)).toString());
    ui->combo_spec->setCurrentText(model->data(model->index(row, 3)).toString());
    ui->lineEdit_email->setText(model->data(model->index(row, 4)).toString());
    ui->lineEdit_phone->setText(model->data(model->index(row, 5)).toString());
    ui->lineEdit_tarif->setText(model->data(model->index(row, 6)).toString());

    ui->lineEdit_id_suppr->setText(model->data(model->index(row, 0)).toString());
}

void MainWindow::on_btn_ajouter_formateur_clicked() {
    if (!validerControlesSaisieFormateur()) return;

    QString email = ui->lineEdit_email->text();

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

void MainWindow::on_btn_calculer_paie_clicked() {
    int id = -1;

    if (!ui->lineEdit_id->text().isEmpty()) {
        id = ui->lineEdit_id->text().toInt();
    }
    else if (ui->tableView_formateur->currentIndex().isValid()) {
        int row = ui->tableView_formateur->currentIndex().row();
        id = ui->tableView_formateur->model()->data(ui->tableView_formateur->model()->index(row, 0)).toInt();
    }
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

void MainWindow::on_btn_qr_formateur_clicked() {
    // 1. Récupération des données du formulaire ou de la ligne sélectionnée
    QString id = ui->lineEdit_id->text();
    QString nom = ui->lineEdit_nom->text();
    QString prenom = ui->lineEdit_prenom->text();
    QString email = ui->lineEdit_email->text();
    QString phone = ui->lineEdit_phone->text();
    QString spec = ui->combo_spec->currentText();

    if (nom.isEmpty() || prenom.isEmpty()) {
        QMessageBox::warning(this, "Avertissement", "Veuillez sélectionner ou remplir les données d'un formateur.");
        return;
    }

    // 2. Formatage des données au format vCard 3.0 (Carte de visite standard)
    QString vcardData = QString(
                            "BEGIN:VCARD\n"
                            "VERSION:3.0\n"
                            "N:%1;%2;;;\n"
                            "FN:%2 %1\n"
                            "EMAIL:%3\n"
                            "TEL:%4\n"
                            "TITLE:Formateur %5\n"
                            "END:VCARD"
                            ).arg(nom, prenom, email, phone, spec);

    // 3. Construction de l'URL pour l'API QR Code
    QString apiUrl = "https://api.qrserver.com/v1/create-qr-code/?size=250x250&data="
                     + QUrl::toPercentEncoding(vcardData);

    // 4. Téléchargement de l'image QR Code via HTTP
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request((QUrl(apiUrl)));

    connect(manager, &QNetworkAccessManager::finished, this, [this, nom, prenom, manager](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(data);

            // 5. Affichage dans une fenêtre dédiée (Badge)
            QDialog *dialog = new QDialog(this);
            dialog->setWindowTitle("Badge QR Code - " + prenom + " " + nom);
            dialog->setFixedSize(300, 350);

            QVBoxLayout *layout = new QVBoxLayout(dialog);
            QLabel *imgLabel = new QLabel(dialog);
            imgLabel->setPixmap(pixmap);
            imgLabel->setAlignment(Qt::AlignCenter);

            QLabel *infoLabel = new QLabel(QString("<b>%1 %2</b><br>Scannez pour enregistrer le contact").arg(prenom, nom), dialog);
            infoLabel->setAlignment(Qt::AlignCenter);

            layout->addWidget(imgLabel);
            layout->addWidget(infoLabel);
            dialog->exec();
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de générer le QR Code (Vérifiez votre connexion internet).");
        }
        reply->deleteLater();
        manager->deleteLater();
    });

    manager->get(request);
}

// ---------------- SLOTS COURS ----------------

void MainWindow::on_tableView_cours_clicked(const QModelIndex &index) {
    int row = index.row();
    QAbstractItemModel *model = ui->tableView_cours->model();

    ui->lineEdit_cours_id->setText(model->data(model->index(row, 0)).toString());
    ui->lineEdit_cours_titre->setText(model->data(model->index(row, 1)).toString());
    ui->combo_cours_cat->setCurrentText(model->data(model->index(row, 2)).toString());
    ui->lineEdit_cours_duree->setText(model->data(model->index(row, 3)).toString());
    ui->lineEdit_cours_prix->setText(model->data(model->index(row, 4)).toString());
    ui->lineEdit_cours_id_formateur->setText(model->data(model->index(row, 5)).toString());

    ui->lineEdit_cours_id_suppr->setText(model->data(model->index(row, 0)).toString());
}

void MainWindow::on_lineEdit_cours_id_formateur_textChanged(const QString &arg1) {
    int idFormateur = arg1.toInt();
    if (idFormateur <= 0) {
        ui->label_alerte_surcharge->clear();
        return;
    }

    int dureeSaisie = ui->lineEdit_cours_duree->text().toInt();

    if (coursTmp.verifierSurchargeFormateur(idFormateur, dureeSaisie, 100)) {
        ui->label_alerte_surcharge->setStyleSheet("color: red; font-weight: bold;");
        ui->label_alerte_surcharge->setText("⚠️ Attention : Formateur proche de la surcharge (>100h) !");
    } else {
        ui->label_alerte_surcharge->setStyleSheet("color: green;");
        ui->label_alerte_surcharge->setText("✓ Charge de travail normale.");
    }
}

void MainWindow::on_lineEdit_cours_duree_textChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    on_lineEdit_cours_id_formateur_textChanged(ui->lineEdit_cours_id_formateur->text());
}

void MainWindow::on_btn_verifier_surcharge_clicked() {
    QString idStr = ui->lineEdit_cours_id_formateur->text().trimmed();
    QString dureeStr = ui->lineEdit_cours_duree->text().trimmed();

    if (idStr.isEmpty() || dureeStr.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants", "Veuillez saisir l'ID Formateur et la Durée pour effectuer la vérification.");
        return;
    }

    int idFormateur = idStr.toInt();
    int duree = dureeStr.toInt();

    bool surcharge = coursTmp.verifierSurchargeFormateur(idFormateur, duree, 100);

    if (surcharge) {
        ui->label_alerte_surcharge->setStyleSheet("color: red; font-weight: bold;");
        ui->label_alerte_surcharge->setText("⚠️ Attention : Seuil d'heures dépassé (>100h) !");
        QMessageBox::warning(this, "Alerte Seuil Dépassé",
                             QString("Le formateur %1 dépasse le seuil autorisé (100h) avec ce cours.").arg(idFormateur));
    } else {
        ui->label_alerte_surcharge->setStyleSheet("color: green; font-weight: bold;");
        ui->label_alerte_surcharge->setText("✓ Charge d'heures sous le seuil autorise.");
        QMessageBox::information(this, "Vérification Réussie",
                                 QString("Le formateur %1 respecte la limite de charge horaire.").arg(idFormateur));
    }
}

void MainWindow::on_btn_ajouter_cours_clicked() {
    if (!validerControlesSaisieCours()) return;

    int id = ui->lineEdit_cours_id->text().toInt();
    QString titre = ui->lineEdit_cours_titre->text();
    QString categorie = ui->combo_cours_cat->currentText();
    int duree = ui->lineEdit_cours_duree->text().toInt();
    double prix = ui->lineEdit_cours_prix->text().toDouble();
    int idFormateur = ui->lineEdit_cours_id_formateur->text().toInt();

    if (coursTmp.verifierSurchargeFormateur(idFormateur, duree, 100)) {
        QMessageBox::StandardButton reponse = QMessageBox::warning(
            this,
            "Alerte Surcharge Formateur",
            "Attention : Ce formateur va dépasser le seuil conseillé (100h de cours).\n\n"
            "Voulez-vous quand même lui assigner ce cours ?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
            );

        if (reponse == QMessageBox::No) {
            return;
        }
    }

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
    painter.drawText(100, 130, "              CATALOGUE DES COURS              ");
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
void MainWindow::on_btn_toggle_theme_clicked()
{
    static bool isDark = false;
    isDark = !isDark;

    if (isDark) {
        // Déclaration du style Anthracite & Émeraude
        QString darkStyle =
            "QMainWindow { background-color: #121212; }"
            "QWidget { background-color: #1e1e1e; color: #e0e0e0; font-family: 'Segoe UI', sans-serif; font-size: 13px; }"
            "QTabWidget::pane { border: 1px solid #2d2d2d; background-color: #1e1e1e; border-radius: 8px; }"
            "QTabBar::tab { background: #2d2d2d; color: #a0a0a0; padding: 8px 16px; border-top-left-radius: 6px; border-top-right-radius: 6px; font-weight: bold; }"
            "QTabBar::tab:selected { background: #00b894; color: #ffffff; }"
            "QGroupBox { font-weight: bold; border: 1px solid #333333; border-radius: 8px; margin-top: 12px; padding-top: 12px; color: #00b894; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }"
            "QLineEdit, QComboBox { background-color: #2b2b2b; border: 1px solid #3d3d3d; border-radius: 5px; padding: 6px; color: #ffffff; selection-background-color: #00b894; }"
            "QLineEdit:focus, QComboBox:focus { border: 1px solid #00b894; }"
            "QPushButton { background-color: #00b894; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 8px 14px; border: none; }"
            "QPushButton:hover { background-color: #55efc4; color: #000000; }"
            "QPushButton:pressed { background-color: #00876c; }"
            "QTableView { background-color: #232323; gridline-color: #333333; border-radius: 6px; selection-background-color: #00b894; selection-color: #ffffff; }"
            "QHeaderView::section { background-color: #2d2d2d; color: #00b894; padding: 6px; font-weight: bold; border: none; }";

        this->setStyleSheet(darkStyle);
        ui->btn_toggle_theme->setText("☀️ Mode Clair");
    } else {
        this->setStyleSheet("");
        ui->btn_toggle_theme->setText("🌙 Mode Sombre");
    }
}