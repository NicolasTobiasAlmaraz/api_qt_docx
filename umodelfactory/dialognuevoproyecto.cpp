#include "dialognuevoproyecto.h"
#include "ui_dialognuevoproyecto.h"
#include <QFileDialog>
#include <QRegExpValidator>

DialogNuevoProyecto::DialogNuevoProyecto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNuevoProyecto)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog);
    ui->lineEditCarpeta->setText(QDir::homePath()+"/uModelFactory");
    ui->pushButtonCrear->setDisabled(true);
    QRegExp rx1("[A-Za-z0-9-_().]+");
    QRegExpValidator  *valid1 = new QRegExpValidator(rx1, this);
    ui->lineEditNombre->setValidator(valid1);
    ui->lineEditAutor->setValidator(valid1);
    QRegExp rx2("[A-Za-z0-9-_().:/]+");
    QRegExpValidator  *valid2 = new QRegExpValidator(rx2, this);
    ui->lineEditCarpeta->setValidator(valid2);
}

DialogNuevoProyecto::~DialogNuevoProyecto()
{
    delete ui;
}

QStringList DialogNuevoProyecto::selectedData() const
{

    return ( QStringList() << (ui->lineEditNombre->text()) << (ui->lineEditAutor->text()) << (ui->lineEditCarpeta->text()) );
}

void DialogNuevoProyecto::on_pushButtonBuscar_clicked()
{
    // Abre una ventana para seleccionar la carpeta del proyecto
    QFileDialog dialogCarpeta(this, Qt::Window);

    dialogCarpeta.setFileMode(QFileDialog::Directory);
    dialogCarpeta.setOption(QFileDialog::ShowDirsOnly, true);
    dialogCarpeta.setDirectory(QDir::home());

    if( dialogCarpeta.exec() == QFileDialog::Accepted ) {
         ui->lineEditCarpeta->setText(dialogCarpeta.selectedFiles().first());
    }

}

void DialogNuevoProyecto::on_pushButtonCrear_clicked()
{
    if (!ui->labelAvisoCarpeta->text().isEmpty()) {
        if (QDir().mkpath(ui->lineEditCarpeta->text())) {
            ui->labelAvisoCarpeta->setText("");
        }
        else {
            ui->labelAvisoCarpeta->setText("No se pudo crear la carpeta. Verificar el nombre ingresado.");
            return;
        }
    }

    QFileInfo checkFile(ui->lineEditCarpeta->text()+"/"+ui->lineEditNombre->text()+".umf");
    // chequea si ya existe un archivo con el mismo nombre
    if (checkFile.exists() && checkFile.isFile()) {
        ui->labelAvisoCarpeta->setText("Ya existe un Proyecto con el mismo nombre.");
        return;
    }
    else {
        this->accept();
    }
}

void DialogNuevoProyecto::on_pushButtonCancelar_clicked()
{
    ui->labelAvisoCarpeta->text().clear();
    this->reject();
}

void DialogNuevoProyecto::on_lineEditCarpeta_textChanged(const QString &path)
{
    if ( QDir(path).exists() ) {
        // Verifica que exista la carpeta elegida
        ui->labelAvisoCarpeta->setText("");
        // Faltaria verificar que no exista un archivo con el mismo nombre dentro de la carpeta elegida
    }
    else
        ui->labelAvisoCarpeta->setText("El programa va a crear la carpeta especificada.");
}

void DialogNuevoProyecto::on_lineEditNombre_textChanged(const QString &nombre)
{
    // Falta verificar que no haya caracteres prohibidos en el nombre del proyecto ( ver validate() )
    // Además falta chequear que no exista de antes un archivo con el mismo nombre dentro de la carpeta actual

    if ( !nombre.isEmpty() )
        ui->pushButtonCrear->setEnabled(true);
    else
        ui->pushButtonCrear->setDisabled(true);

}
