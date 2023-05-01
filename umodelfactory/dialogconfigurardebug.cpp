#include "dialogconfigurardebug.h"
#include "ui_dialogconfigurardebug.h"

#include <QSerialPortInfo>
#include <QByteArray>
#include <QMessageBox>
#include <QFileDialog>

DialogConfigurarDebug::DialogConfigurarDebug(QWidget *parent, bool dbgTiempoReal, bool generarArchivoLog, QString fileName) :
    QDialog(parent),
    ui(new Ui::DialogConfigurarDebug)
{
    ui->setupUi(this);    

    m_replayDebugFileName = fileName;
    ui->lineEditArchivo->setText(fileName);

    enumerarPuertos();
    ui->checkBoxArchivoDebugLog->setChecked(generarArchivoLog);

    if(dbgTiempoReal == true)
        ui->tabWidget->setCurrentIndex(0);
    else
        ui->tabWidget->setCurrentIndex(1);
}

DialogConfigurarDebug::~DialogConfigurarDebug()
{
    delete ui;
}

void DialogConfigurarDebug::enumerarPuertos()
{
    ui->comboBoxPuertos->clear();

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    for (int i = 0; i < ports.size(); i++){
        ui->comboBoxPuertos->addItem(ports.at(i).portName(), ports.at(i).portName());
    }
}

QString DialogConfigurarDebug::getPortName()
{
    return this->m_portName;
}

QString DialogConfigurarDebug::getArchivoDebugLogName()
{
    return this->m_replayDebugFileName;
}

bool DialogConfigurarDebug::generarArchivoLogEnabled()
{
    return (ui->checkBoxArchivoDebugLog->isChecked());
}

bool DialogConfigurarDebug::debugTiempoRealEnabled()
{
    if (ui->tabWidget->currentIndex() == 0)
        return true;
    else
        return false;
}

void DialogConfigurarDebug::on_pushButtonAceptar_clicked()
{
    m_portName = ui->comboBoxPuertos->currentData().toString();
    accept();
}

void DialogConfigurarDebug::on_pushButtonCancelar_clicked()
{
    close();
}

void DialogConfigurarDebug::on_pushButtonRefreshPorts_clicked()
{
    enumerarPuertos();
}

void DialogConfigurarDebug::on_pushButtonAbrir_clicked()
{
    // Abro el Dialog para elegir un archivo de debug log (archivos .dbgumf)
    m_replayDebugFileName = QFileDialog::getOpenFileName(this, tr("Abrir Archivo Debug Log"),
                                                    QDir::homePath(), tr("Proyectos(*.umfdbg)"));

    // Si no se selecciona ningún archivo, salgo...
    if (m_replayDebugFileName.isEmpty()) {
        ui->lineEditArchivo->clear();
        return;
    }

    // Abro el archivo seleccionado
    QFile file(m_replayDebugFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error al Abrir Archivo"),
                             tr("No se pudo abrir el archivo \n%1").arg(m_replayDebugFileName));
        return;
    } else {
        ui->lineEditArchivo->setText(m_replayDebugFileName);
    }
}
