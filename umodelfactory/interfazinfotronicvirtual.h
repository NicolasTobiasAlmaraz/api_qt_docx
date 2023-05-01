#ifndef INTERFAZINFOTRONICVIRTUAL_H
#define INTERFAZINFOTRONICVIRTUAL_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include "fuentes.h"
#include "converciondedatos.h"

namespace Ui {
class InterfazInfotronicVirtual;
}

class InterfazInfotronicVirtual : public QDialog
{
    Q_OBJECT

public:
    explicit InterfazInfotronicVirtual(QWidget *parent = nullptr);
    ~InterfazInfotronicVirtual( );
    void Infotronic( struct Elementos *  , ConvercionDeDatos * );
    void Infotronic2019( struct Elementos &  );
    void memoriaComboBox(int index);

private slots:
    void on_tabWidget_currentChanged(int index);

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_radioButton_Expansion1_clicked();

    void on_radioButton_Expansion2_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::InterfazInfotronicVirtual *ui;

    QLabel **m_labels_leds = new QLabel*[30];

    QComboBox ** m_SalidasDigitales ;
    QComboBox ** m_EntradasDigitales;
    QComboBox ** m_EntradasAnalogicas;
    QComboBox ** m_Pulsadores;
    QComboBox ** m_RGB;

    QComboBox ** m_Leds ;
    QLineEdit **m_Nombres ;

    QComboBox * m_Temporizadores;
    QComboBox * m_e1Temporizadores;
    QComboBox * m_e2Temporizadores;

    QList <QStringList> *m_Tabla_Temporizadores;
    QList <QStringList> *m_Tabla_eTemporizadores;

    // Convercion
    ConvercionDeDatos *m_Infotronic = new ConvercionDeDatos;

    struct Elementos *m_E;
};

#endif // INTERFAZINFOTRONICVIRTUAL_H
