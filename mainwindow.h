#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QRegularExpression>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>

// Inclusion des modules QtCharts
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
    // Slots Formateurs
    void on_btn_ajouter_formateur_clicked();
    void on_btn_supprimer_formateur_clicked();
    void on_btn_modifier_formateur_clicked();
    void on_btn_rechercher_formateur_clicked();
    void on_btn_stat_formateur_clicked();
    void on_btn_pdf_formateur_clicked();

    // Slots Cours
    void on_btn_ajouter_cours_clicked();
    void on_btn_modifier_cours_clicked();
    void on_btn_supprimer_cours_clicked();
    void on_btn_rechercher_cours_clicked();

private:
    Ui::MainWindow *ui;
    Formateur formateurTmp;
    Cours coursTmp;

    // Méthodes utilitaires
    void rafraichirTables();
    bool validerControlesSaisieFormateur();
    bool validerControlesSaisieCours();
};

#endif // MAINWINDOW_H