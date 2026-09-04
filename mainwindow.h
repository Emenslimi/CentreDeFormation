#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QRegularExpression>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QTranslator>

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

#include "formateur.h"
#include "cours.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Theme & Multi-Langue
    void on_btn_toggle_theme_clicked();
    void on_combo_langue_currentIndexChanged(int index);

    // Slots Formateurs
    void on_btn_ajouter_formateur_clicked();
    void on_btn_supprimer_formateur_clicked();
    void on_btn_modifier_formateur_clicked();
    void on_btn_rechercher_formateur_clicked();
    void on_btn_stat_formateur_clicked();
    void on_btn_pdf_formateur_clicked();
    void on_btn_calculer_paie_clicked();
    void on_tableView_formateur_clicked(const QModelIndex &index);
    void on_btn_qr_formateur_clicked();

    // Slots Cours
    void on_btn_ajouter_cours_clicked();
    void on_btn_modifier_cours_clicked();
    void on_btn_supprimer_cours_clicked();
    void on_btn_rechercher_cours_clicked();
    void on_btn_stat_cours_clicked();
    void on_btn_pdf_cours_clicked();
    void on_btn_verifier_surcharge_clicked();
    void on_lineEdit_cours_id_formateur_textChanged(const QString &arg1);
    void on_lineEdit_cours_duree_textChanged(const QString &arg1);
    void on_tableView_cours_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    // Objets métier temporaires pour exécuter les requêtes
    Formateur formateurTmp;
    Cours coursTmp;

    // Traducteur pour le support multilingue
    QTranslator m_translator;

    // Méthodes utilitaires
    void rafraichirTables();
    bool validerControlesSaisieFormateur();
    bool validerControlesSaisieCours();
    void appliquerLangue(const QString &codeLangue);
};

#endif // MAINWINDOW_H