#include <QMessageBox>

#include "interfazinfotronicvirtual.h"
#include "ui_interfazinfotronicvirtual.h"
#include "converciondedatos.h"
#define     SIN_SELECCION       "-------------------------"

InterfazInfotronicVirtual::InterfazInfotronicVirtual(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InterfazInfotronicVirtual)
{
    ui->setupUi(this);
    m_SalidasDigitales = nullptr ;
    m_EntradasDigitales = nullptr ;
    m_EntradasAnalogicas = nullptr ;
    m_Pulsadores = nullptr ;
    m_RGB = nullptr ;
    m_Leds = nullptr;
    m_Nombres = nullptr;
    m_Tabla_Temporizadores  = nullptr;
    m_Tabla_eTemporizadores = nullptr;

}

InterfazInfotronicVirtual::~InterfazInfotronicVirtual()
{
    delete ui;
}
void InterfazInfotronicVirtual::Infotronic( struct Elementos *E , ConvercionDeDatos *cruce)
{
    m_Infotronic = cruce;
    m_E = E;
    /////////////////////////////////////// sacar cuando se haga el soft de las otras solapas
    ui->tabWidget->setTabEnabled(3,false);
    ui->tabWidget->setTabEnabled(4,false);
    ui->tabWidget->setTabEnabled(0,false);
    ui->tabWidget->setTabEnabled(1,false);
    m_Infotronic->Setpagina(2);
    /////////////////////////////////////////
    ui->tabWidget->setCurrentIndex(  m_Infotronic->Get_pagina());

    if ( ui->tabWidget->currentIndex()== 2)
        memoriaComboBox(ui->tabWidget->currentIndex());
    else
        memoriaComboBox( 2 );

}

void InterfazInfotronicVirtual::Infotronic2019( struct Elementos &E )
{
    m_Leds[0] = ui->comboBox_Led0_OFF;
    m_Leds[1] = ui->comboBox_Led0_ON;
    m_Leds[2] = ui->comboBox_Led1_OFF;
    m_Leds[3] = ui->comboBox_Led1_ON;
    m_Leds[4] = ui->comboBox_Led2_OFF;
    m_Leds[5] = ui->comboBox_Led2_ON;
    m_Leds[6] = ui->comboBox_Led3_OFF;
    m_Leds[7] = ui->comboBox_Led3_ON;
    m_Leds[8] = ui->comboBox_Led4_OFF;
    m_Leds[9] = ui->comboBox_Led4_ON;
    m_Leds[10] = ui->comboBox_Led5_OFF;
    m_Leds[11] = ui->comboBox_Led5_ON;
    m_Leds[12] = ui->comboBox_Led6_OFF;
    m_Leds[13] = ui->comboBox_Led6_ON;
    m_Leds[14] = ui->comboBox_Led7_OFF;
    m_Leds[15] = ui->comboBox_Led7_ON;

    m_Nombres[ 0 ] = ui->lineEdit_nombre1;
    m_Nombres[ 1 ] = ui->lineEdit_nombre2;
    m_Nombres[ 2 ] = ui->lineEdit_nombre3;
    m_Nombres[ 3 ] = ui->lineEdit_nombre4;
    m_Nombres[ 4 ] = ui->lineEdit_nombre5;
    m_Nombres[ 5 ] = ui->lineEdit_nombre6;
    m_Nombres[ 6 ] = ui->lineEdit_nombre7;
    m_Nombres[ 7 ] = ui->lineEdit_nombre8;
    m_Nombres[ 8 ] = ui->lineEdit_nombre9;
    m_Nombres[ 9 ] = ui->lineEdit_nombre10;
    m_Nombres[ 10 ] = ui->lineEdit_nombre11;
    m_Nombres[ 11 ] = ui->lineEdit_nombre12;
    m_Nombres[ 12 ] = ui->lineEdit_nombre13;
    m_Nombres[ 13 ] = ui->lineEdit_nombre14;
    m_Nombres[ 14 ] = ui->lineEdit_nombre15;
    m_Nombres[ 15 ] = ui->lineEdit_nombre16;
    m_Nombres[ 16 ] = ui->lineEdit_nombre17;
    m_Nombres[ 17 ] = ui->lineEdit_nombre18;
    m_Nombres[ 18 ] = ui->lineEdit_nombre19;
    m_Nombres[ 19 ] = ui->lineEdit_nombre20;
    m_Nombres[ 20 ] = ui->lineEdit_nombre21;
    m_Nombres[ 21 ] = ui->lineEdit_nombre22;
    m_Nombres[ 22 ] = ui->lineEdit_nombre23;
    m_Nombres[ 23 ] = ui->lineEdit_nombre24;
    m_Nombres[ 24 ] = ui->lineEdit_nombre25;

    m_labels_leds[ 0 ] = ui->label_Led0;
    m_labels_leds[ 1 ] = ui->label_Led1;
    m_labels_leds[ 2 ] = ui->label_Led2;
    m_labels_leds[ 3 ] = ui->label_Led3;
    m_labels_leds[ 4 ] = ui->label_Led4;
    m_labels_leds[ 5 ] = ui->label_Led5;
    m_labels_leds[ 6 ] = ui->label_Led6;
    m_labels_leds[ 7 ] = ui->label_Led7;
    m_labels_leds[ 8 ] = ui->label_Led0_OFF;
    m_labels_leds[ 9 ] = ui->label_Led2_OFF;
    m_labels_leds[ 10 ] = ui->label_Led5_OFF;
    m_labels_leds[ 11 ] = ui->label_Led0_ON;
    m_labels_leds[ 12 ] = ui->label_Led2_ON;
    m_labels_leds[ 13 ] = ui->label_Led5_ON;

    // Visualizacion de la expancion habilitada
    if ( m_Infotronic->GetCheckExpansion(  ) == 0)
          ui->radioButton_Expansion1->setChecked(true);
    else
         ui->radioButton_Expansion2->setChecked(true);

    if ( ui->radioButton_Expansion1->isChecked())
    {
        for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 / 2 ; i++ )
            m_Nombres[ i + 17 ]->setVisible(false);
        for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 ; i++ )
            m_Leds[ i ]->setVisible(false);
        ui->label_Led0->setVisible(false);

        for ( int i = 0 ; i < MAX_CARTELES_EXP2_INFOTRONIC2019 ; i++ )
            m_labels_leds[ i ]->setVisible(false);
        ui->groupBox_EXP2->setVisible(false);
    }
    else
    {
        for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 / 2 ; i++ )
            m_Nombres[ i + 18 ]->setVisible(true);
        for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 ; i++ )
            m_Leds[ i ]->setVisible(true);
        ui->label_Led0->setVisible(true);

        for ( int i = 0 ; i < MAX_CARTELES_EXP2_INFOTRONIC2019 ; i++ )
            m_labels_leds[ i ]->setVisible(true);
        ui->groupBox_EXP2->setVisible(true);
    }

    m_SalidasDigitales[ 0 ] = ui->comboBox;
    m_SalidasDigitales[ 1 ] = ui->comboBox_2;
    m_SalidasDigitales[ 2 ] = ui->comboBox_6;
    m_SalidasDigitales[ 3 ] = ui->comboBox_5;
    m_SalidasDigitales[ 4 ] = ui->comboBox_13;
    m_SalidasDigitales[ 5 ] = ui->comboBox_11;
    m_SalidasDigitales[ 6 ] = ui->comboBox_12;
    m_SalidasDigitales[ 7 ] = ui->comboBox_14;

    m_EntradasDigitales[ 0 ] = ui->comboBox_17;
    m_EntradasDigitales[ 1 ] = ui->comboBox_15;
    m_EntradasDigitales[ 2 ] = ui->comboBox_16;
    m_EntradasDigitales[ 3 ] = ui->comboBox_30;
    m_EntradasDigitales[ 4 ] = ui->comboBox_31;
    m_EntradasDigitales[ 5 ] = ui->comboBox_32;

    m_Pulsadores[ 0 ] = ui->comboBox_18;
    m_Pulsadores[ 1 ] = ui->comboBox_19;
    m_Pulsadores[ 2 ] = ui->comboBox_20;
    m_Pulsadores[ 3 ] = ui->comboBox_21;
    m_Pulsadores[ 4 ] = ui->comboBox_22;

    m_EntradasAnalogicas[ 0 ] = ui->comboBox_23;
    m_EntradasAnalogicas[ 1 ] = ui->comboBox_24;
    m_EntradasAnalogicas[ 2 ] = ui->comboBox_25;

    m_RGB[ 0 ] = ui->comboBox_26;
    m_RGB[ 1 ] = ui->comboBox_27;
    m_RGB[ 2 ] = ui->comboBox_28;
    m_RGB[ 3 ] = ui->comboBox_79;
    m_RGB[ 4 ] = ui->comboBox_80;
    m_RGB[ 5 ] = ui->comboBox_81;

    // Limpiamos lo previo
    for ( int i = 0 ; i < MAX_SALIDAS_DIGITALES_INFOTRONIC2019 ; i++)
            m_SalidasDigitales[i]->clear();
    for ( int i = 0 ; i < MAX_ENTRADAS_DIGITALES_INFOTRONIC2019 ; i++)
            m_EntradasDigitales[i]->clear();
    for ( int i = 0 ; i < MAX_ENTRADAS_ANALOGICAS_INFOTRONIC2019 ; i++)
            m_EntradasAnalogicas[i]->clear();
    for ( int i = 0 ; i < MAX_PULSADORES_INFOTRONIC2019 ; i++)
            m_Pulsadores[i]->clear();
    for ( int i = 0 ; i < MAX_RGB_INFOTRONIC2019 ; i++)
            m_RGB[i]->clear();
    for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 ; i++)
            m_Leds[i]->clear();

    m_Tabla_Temporizadores->clear();
    m_Tabla_eTemporizadores->clear();

    // Cargamos el combo con el primer elemento SIN_SELECCION
    for ( int i = 0 ; i < MAX_SALIDAS_DIGITALES_INFOTRONIC2019 ; i++)
        m_SalidasDigitales[i]->addItem(SIN_SELECCION);
    for ( int i = 0 ; i < MAX_ENTRADAS_DIGITALES_INFOTRONIC2019 ; i++)
        m_EntradasDigitales[i]->addItem(SIN_SELECCION);
    for ( int i = 0 ; i < MAX_PULSADORES_INFOTRONIC2019 ; i++)
        m_Pulsadores[i]->addItem(SIN_SELECCION);
    for ( int i = 0 ; i < MAX_ENTRADAS_ANALOGICAS_INFOTRONIC2019 ; i++)
        m_EntradasAnalogicas[i]->addItem(SIN_SELECCION);
    for ( int i = 0 ; i < MAX_RGB_INFOTRONIC2019 ; i++)
        m_RGB[i]->addItem(SIN_SELECCION);

    for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 ; i++)
        m_Leds[i]->addItem(SIN_SELECCION);

    //!< Cargamos los combos
    for ( int i = 0 ; i < MAX_SALIDAS_DIGITALES_INFOTRONIC2019 ; i++)
    {
        for ( int j = 0 ; E.nAcciones[j] !="" ; j++ )
            m_SalidasDigitales[i]->addItem(E.nAcciones[j].replace("()",""));
        for ( int j = 0 ; E.nMacros[j] !="" ; j++ )
            m_SalidasDigitales[i]->addItem(E.nMacros[j].replace("()",""));
    }

    for ( int i = 0 ; i < MAX_RGB_INFOTRONIC2019 ; i++)
    {
        for ( int j = 0 ; E.nAcciones[j] !="" ; j++ )
            m_RGB[i]->addItem(E.nAcciones[j].replace("()",""));
        for ( int j = 0 ; E.nMacros[j] !="" ; j++ )
            m_RGB[i]->addItem(E.nMacros[j].replace("()",""));
    }
    for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 ; i++)
    {
        for ( int j = 0 ; E.nAcciones[j] !="" ; j++ )
            m_Leds[i]->addItem(E.nAcciones[j].replace("()",""));
        for ( int j = 0 ; E.nMacros[j] !="" ; j++ )
            m_Leds[i]->addItem(E.nMacros[j].replace("()",""));
    }

    for ( int i = 0 ; i < MAX_ENTRADAS_DIGITALES_INFOTRONIC2019 ; i++)
    {
        for ( int j = 0 ; E.nEventos[j] !="" ; j++ )
        {
            QStringList aux;
            aux = E.nEventos[j].split("_");
            if (aux[0] != "e" )
                m_EntradasDigitales[i]->addItem(E.nEventos[j]);
        }
    }

    for ( int i = 0 ; i < MAX_PULSADORES_INFOTRONIC2019 ; i++)
    {
        for ( int j = 0 ; E.nEventos[j] !="" ; j++ )
        {
            QStringList aux;
            aux = E.nEventos[j].split("_");
            if (aux[0] != "e" )
                m_Pulsadores[i]->addItem(E.nEventos[j]);
        }
    }

    /***************************************
     * FALTA DEFINIR FUNCIONES ANALOGICAS
     * *************************************/

    //colocamos la seleccion previa, si no coincide con  ninguna del combo,
    //pone la primera

    for ( int i = 0 ; i < MAX_SALIDAS_DIGITALES_INFOTRONIC2019 ; i++)
        m_SalidasDigitales[i]->setCurrentText( m_Infotronic->GetSalidasDigitales(i) );
    for ( int i = 0 ; i < MAX_ENTRADAS_DIGITALES_INFOTRONIC2019 ; i++)
        m_EntradasDigitales[i]->setCurrentText( m_Infotronic->GetEntradasDigitales(i) );
    for ( int i = 0 ; i < MAX_ENTRADAS_ANALOGICAS_INFOTRONIC2019 ; i++)
        m_EntradasAnalogicas[i]->setCurrentText( m_Infotronic->GetEntradasAnalogicas(i) );
    for ( int i = 0 ; i < MAX_PULSADORES_INFOTRONIC2019 ; i++)
        m_Pulsadores[i]->setCurrentText( m_Infotronic->GetPulsadores(i) );
    for ( int i = 0 ; i < MAX_RGB_INFOTRONIC2019; i++)
        m_RGB[i]->setCurrentText( m_Infotronic->GetRGB(i) );   
    for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 ; i++)
        m_Leds[i]->setCurrentText( m_Infotronic->GetLED(i) );

    m_Tabla_Temporizadores = m_Infotronic->GetTemporizadores( m_E);

    for (int i = 0 ; i <  m_Tabla_Temporizadores->count(); i++ )
    {
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(m_Tabla_Temporizadores->at(i)[0]));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(m_Tabla_Temporizadores->at(i)[1]));   
        ui->tableWidget->insertRow( ui->tableWidget->rowCount( ) );
    }

    m_Tabla_eTemporizadores = m_Infotronic->GeteTemporizadores();

    for (int i = 0 ; i <  m_Tabla_eTemporizadores->count(); i++ )
    {
        ui->tableWidget_2->insertRow( ui->tableWidget_2->rowCount( ) );
        ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(m_Tabla_eTemporizadores->at(i)[0]));
        ui->tableWidget_2->setItem(i,1,new QTableWidgetItem(m_Tabla_eTemporizadores->at(i)[1]));
    }
}

void InterfazInfotronicVirtual::on_tabWidget_currentChanged(int index)
{
    static int tag_acual = 0;

    if ( tag_acual != index  )
    {
        tag_acual = index;

        if ( m_SalidasDigitales != nullptr )
        {
            delete [] m_SalidasDigitales ;
            delete [] m_EntradasDigitales ;
            delete [] m_EntradasAnalogicas ;
            delete [] m_Pulsadores ;
            delete [] m_RGB ;
            delete [] m_Leds ;
            delete [] m_Nombres;
        }
        memoriaComboBox(index) ;
    }
}

void InterfazInfotronicVirtual::memoriaComboBox( int index )
{
    switch ( index )
    {
        case 0:
            m_SalidasDigitales = nullptr ;
            m_EntradasDigitales = nullptr ;
            m_EntradasAnalogicas = nullptr ;
            m_Pulsadores = nullptr ;
            m_RGB = nullptr ;
            m_Leds = nullptr;
            m_Nombres = nullptr;
            m_Tabla_Temporizadores  = nullptr;
            m_Tabla_eTemporizadores = nullptr;

            break;
        case 1:
            m_SalidasDigitales = nullptr ;
            m_EntradasDigitales = nullptr ;
            m_EntradasAnalogicas = nullptr ;
            m_Pulsadores = nullptr ;
            m_RGB = nullptr ;
            m_Leds = nullptr;
            m_Nombres = nullptr;
            m_Tabla_Temporizadores  = nullptr;
            m_Tabla_eTemporizadores = nullptr;
            break;
        case 2:
            //!< Pido memoria solo la primera vez que entra al caso
            if ( !m_Infotronic->Get_max_salidas_digitales() )
                m_Infotronic->NewInfotronic(MAX_SALIDAS_DIGITALES_INFOTRONIC2019,MAX_ENTRADAS_DIGITALES_INFOTRONIC2019,MAX_ENTRADAS_ANALOGICAS_INFOTRONIC2019,MAX_PULSADORES_INFOTRONIC2019,MAX_RGB_INFOTRONIC2019,MAX_LEDS_INFOTRONIC2019,MAX_NOMBRES_INFOTRONIC2019);

            m_SalidasDigitales = new QComboBox*[MAX_SALIDAS_DIGITALES_INFOTRONIC2019] ;
            m_EntradasDigitales = new QComboBox*[ MAX_ENTRADAS_DIGITALES_INFOTRONIC2019 ];
            m_EntradasAnalogicas = new QComboBox*[ MAX_ENTRADAS_ANALOGICAS_INFOTRONIC2019 ];
            m_Pulsadores = new QComboBox*[ MAX_PULSADORES_INFOTRONIC2019 ];
            m_RGB = new QComboBox*[ MAX_RGB_INFOTRONIC2019 ];
            m_Tabla_Temporizadores = new QList<QStringList>;
            m_Tabla_eTemporizadores = new QList<QStringList>;
            m_Leds = new QComboBox*[MAX_LEDS_INFOTRONIC2019];
            m_Nombres = new QLineEdit*[MAX_NOMBRES_INFOTRONIC2019];
            Infotronic2019( *m_E );

            break;
        case 3:
            m_SalidasDigitales = nullptr ;
            m_EntradasDigitales = nullptr ;
            m_EntradasAnalogicas = nullptr ;
            m_Pulsadores = nullptr ;
            m_RGB = nullptr ;
            m_Leds = nullptr;
            m_Nombres = nullptr;
            m_Tabla_Temporizadores  = nullptr;
            m_Tabla_eTemporizadores = nullptr;

            break;
        case 4:
            m_SalidasDigitales = nullptr ;
            m_EntradasDigitales = nullptr ;
            m_EntradasAnalogicas = nullptr ;
            m_Pulsadores = nullptr ;
            m_RGB = nullptr ;
            m_Leds = nullptr;
            m_Nombres = nullptr;
            m_Tabla_Temporizadores  = nullptr;
            m_Tabla_eTemporizadores = nullptr;

            break;
    }
}

void InterfazInfotronicVirtual::on_pushButton_clicked()
{

}

void InterfazInfotronicVirtual::on_pushButton_3_clicked()
{
}

void InterfazInfotronicVirtual::on_pushButton_5_clicked()
{
}

void InterfazInfotronicVirtual::on_pushButton_4_clicked()
{
    if ( ! (ui->tableWidget_2->rowCount( ) ) )
    {
        QMessageBox msgBox;
        msgBox.setText("La tabla no esta vacia!");
        msgBox.exec();

    }
    else
        ui->tableWidget_2->removeRow( ui->tableWidget_2->rowCount( ) - 1 );
}

void InterfazInfotronicVirtual::on_radioButton_Expansion1_clicked()
{
     for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 / 2 ; i++ )
         m_Nombres[ i + 17 ]->setVisible(false);
     for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 ; i++ )
         m_Leds[ i ]->setVisible(false);
     for ( int i = 0 ; i < MAX_CARTELES_EXP2_INFOTRONIC2019 ; i++ )
         m_labels_leds[ i ]->setVisible(false);
     ui->groupBox_EXP2->setVisible(false);
}

void InterfazInfotronicVirtual::on_radioButton_Expansion2_clicked()
{
    for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 / 2 ; i++ )
        m_Nombres[ i + 17 ]->setVisible(true);
    for ( int i = 0 ; i < MAX_LEDS_INFOTRONIC2019 ; i++ )
        m_Leds[ i ]->setVisible(true);
    for ( int i = 0 ; i < MAX_CARTELES_EXP2_INFOTRONIC2019 ; i++ )
        m_labels_leds[ i ]->setVisible(true);
    ui->groupBox_EXP2->setVisible(true);

}

void InterfazInfotronicVirtual::on_pushButton_6_clicked()
{
    int maxAcciones = m_E->nAcciones.size() - 1; // con el -1 descarto el ""
    int maxMacros = m_E->nMacros.size() - 1 ; // con el -1 descarto el ""
    int marcasAcciones[ maxAcciones + maxMacros ];

    int maxEventos = m_E->nEventos.size() - 1; // con el -1 descarto el ""
    int marcasEventos[maxEventos];

    for( int i = 0 ; i < maxAcciones + maxMacros ; i++ )
        marcasAcciones [i] = 0;
    for( int i = 0 ; i < maxEventos ; i++ )
        marcasEventos [i] = 0;

    for( int i = 0 ; i < maxAcciones ; i++ )
    {
        for ( int j = 0 ; j < m_Infotronic->Get_max_salidas_digitales() ; j++)
        {            
            if (  m_E->nAcciones.at(i) == m_SalidasDigitales[j]->currentText())
                marcasAcciones [i] ++;
        }
    }

    for( int i = 0 ; i < maxMacros ; i++ )
    {
        for ( int j = 0 ; j < m_Infotronic->Get_max_salidas_digitales() ; j++)
        {
            if (  m_E->nMacros.at(i) == m_SalidasDigitales[j]->currentText())
                marcasAcciones [i + maxAcciones ] ++;
        }
    }
    for( int i = 0 ; i < maxAcciones ; i++ )
    {
        for ( int j = 0 ; j < m_Infotronic->Get_max_rgb() ; j++)
        {
            if (  m_E->nAcciones.at(i) == m_RGB[j]->currentText())
                marcasAcciones [i] ++;
        }
    }

    for( int i = 0 ; i < maxMacros ; i++ )
    {
        for ( int j = 0 ; j < m_Infotronic->Get_max_rgb() ; j++)
        {
            if (  m_E->nMacros.at(i) == m_RGB[j]->currentText())
                marcasAcciones [i + maxAcciones ] ++;
        }
    }
    for( int i = 0 ; i < maxAcciones ; i++ )
    {
        for ( int j = 0 ; j < m_Infotronic->Get_max_leds() ; j++)
        {
            if (  m_E->nAcciones.at(i) == m_Leds[j]->currentText())
                marcasAcciones [i] ++;
        }
    }
    for( int i = 0 ; i < maxMacros ; i++ )
    {
        for ( int j = 0 ; j < m_Infotronic->Get_max_leds() ; j++)
        {
            if (  m_E->nMacros.at(i) == m_Leds[j]->currentText())
                marcasAcciones [i + maxAcciones ] ++;
        }
    }
    QString q = "--> LAS ACCIONES\n\n" ;
    QString AccionesRepetidas = "" , AccionesNoAsignadas = "";
    QString EventosRepetidos = "" , EventosNoAsignados = "";
    int faltanAcciones = 0;

    for( int i = 0 ; i < maxAcciones; i++ )
    {
        if ( !marcasAcciones [i])
        {
            AccionesNoAsignadas += "       ";
            AccionesNoAsignadas += m_E->nAcciones.at(i);
            AccionesNoAsignadas += "\n";
            faltanAcciones = 1;
        }

        if ( marcasAcciones [i] >= 2 )
        {
            QString aux;
            aux = aux.setNum(marcasAcciones [i]);
            AccionesRepetidas += "       ";
            AccionesRepetidas += m_E->nAcciones.at(i);
            AccionesRepetidas +=": esta siendo utilizada ";
            AccionesRepetidas += aux;
            AccionesRepetidas +=" veces\n";
        }
    }
    for( int i = 0 ; i < maxMacros; i++ )
    {
        if ( !marcasAcciones [i + maxAcciones ])
        {
            AccionesNoAsignadas += "       ";
            AccionesNoAsignadas += m_E->nMacros.at(i);
            AccionesNoAsignadas += "\n";
            faltanAcciones = 1;
        }

        if ( marcasAcciones [ i + maxAcciones] >= 2 )
        {
            QString aux;
            aux = aux.setNum(marcasAcciones [ i + maxAcciones ]);
            AccionesRepetidas += "       ";
            AccionesRepetidas += m_E->nMacros.at(i);
            AccionesRepetidas +=": esta siendo utilizada ";
            AccionesRepetidas += aux;
            AccionesRepetidas +=" veces\n";
        }
    }

    if (AccionesNoAsignadas != "")
    {
        q += AccionesNoAsignadas;
        q += "\nNo fueron asignadas !!";
    }
    else
        q = "";

    for( int i = 0 ; i < maxEventos ; i++ )
    {
        for ( int j = 0 ; j < m_Infotronic->Get_max_entradas_digitales() ; j++)
        {
            if (  m_E->nEventos.at(i) == m_EntradasDigitales[j]->currentText())
                marcasEventos [i] ++;
        }
    }

    for( int i = 0 ; i < maxEventos ; i++ )
    {
        for ( int j = 0 ; j < m_Infotronic->Get_max_pulsadores() ; j++)
        {
            if (  m_E->nEventos.at(i) == m_Pulsadores[j]->currentText())
                marcasEventos [i] ++;
        }
    }

    int faltanEventos = 0;

    for( int i = 0 ; i < maxEventos; i++ )
    {
        if ( !marcasEventos [i])
        {
            QStringList aux = m_E->nEventos.at(i).split("_");
            if ( aux[0] != "e")
            {
                EventosNoAsignados += "       ";
                EventosNoAsignados += m_E->nEventos.at(i);
                EventosNoAsignados += "\n";
                faltanEventos = 1;
            }
        }
        if ( marcasEventos [i] >= 2 )
        {
            QString aux;
            aux = aux.setNum(marcasEventos [i]);
            EventosRepetidos +="       ";
            EventosRepetidos += m_E->nEventos.at(i);
            EventosRepetidos +=": esta siendo utilizado ";
            EventosRepetidos += aux;
            EventosRepetidos +=" veces\n";
        }
    }
    if ( AccionesRepetidas != "" )
        AccionesRepetidas = "--> ACCIONES\n\n" + AccionesRepetidas;

    if ( EventosRepetidos != "" )
    {
        if (AccionesRepetidas == "" )
            EventosRepetidos = "--> EVENTOS\n\n" + EventosRepetidos;
        else
            EventosRepetidos = "\n--> EVENTOS\n\n" + EventosRepetidos;
    }

    if ( EventosNoAsignados != "")
    {
        if ( q != "")
            q += "\n\n--> LOS EVENTOS\n\n";
        else
            q = "--> LOS EVENTOS\n\n";

            q += EventosNoAsignados;
            q += "\nNo fueron asignados !!";
    }
    else
    {
        if ( AccionesNoAsignadas == "")
            q = "";
    }

    if ( !faltanAcciones && !faltanEventos )
    {
        if ( AccionesRepetidas != "" || EventosRepetidos != "")
        {
            QMessageBox msg_box(QMessageBox::Critical, "Verificacion de acciones y Eventos repetidos", AccionesRepetidas + EventosRepetidos,
                                QMessageBox::Yes );
            msg_box.setButtonText(QMessageBox::Yes, "OK");
            msg_box.exec();
            return ;
        }

        for ( int i = 0 ; i < m_Infotronic->Get_max_salidas_digitales() ; i++)
            m_Infotronic->SetSalidasDigitales( i , m_SalidasDigitales[i]->currentText());

        for ( int i = 0 ; i < m_Infotronic->Get_max_leds() ; i++)
            m_Infotronic->SetLED( i , m_Leds[i]->currentText());

        for ( int i = 0 ; i < m_Infotronic->Get_max_rgb() ; i++)
            m_Infotronic->SetRGB( i , m_RGB[ i ]->currentText() );

        for ( int i = 0 ; i < m_Infotronic->Get_max_entradas_digitales() ; i++)
            m_Infotronic->SetEntradasDigitales( i ,m_EntradasDigitales[ i ]->currentText());

        for ( int i = 0 ; i < m_Infotronic->Get_max_pulsadores() ; i++)
            m_Infotronic->SetPulsadores( i ,m_Pulsadores[ i ]->currentText() );

        for ( int i = 0 ; i < m_Infotronic->Get_max_entradas_analogicas() ; i++)
            m_Infotronic->SetEntradasAnalogicas( i ,m_EntradasAnalogicas[ i ]->currentText());


        m_Infotronic->ClearTemporizadores();
        for (int i = 0 ; i <  ui->tableWidget->rowCount() - 1 ; i++ )
            m_Infotronic->SetTemporizadores( ui->tableWidget->item( i , 0 )->text( ) , ui->tableWidget->item( i , 1 )->text( ));

        m_Infotronic->CleareTemporizadores();
        for (int i = 0 ; i <  ui->tableWidget_2->rowCount() - 1; i++ )
            m_Infotronic->SeteTemporizadores( ui->tableWidget_2->item( i , 0 )->text( ) , ui->tableWidget_2->item( i , 1 )->text( ));

        m_Infotronic->Setpagina( ui->tabWidget->currentIndex());
        bool a = ui->radioButton_Expansion1->isChecked();
        if ( a )
            m_Infotronic->SetCheckExpansion( 0 );
        else
            m_Infotronic->SetCheckExpansion( 1 );

        this->done( true );
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Critical, "Verificacion de acciones y Eventos repetidos", AccionesRepetidas + EventosRepetidos,
                            QMessageBox::Yes );

        msgBox.setText(q);
        msgBox.setWindowTitle("Verificacion de Compatibilidad");
        msgBox.setButtonText(QMessageBox::Yes, "OK");
        msgBox.exec();
    }
}

void InterfazInfotronicVirtual::on_pushButton_7_clicked()
{

     this->done( false );
}
