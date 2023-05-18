#include "dialogconfigdocumentacion.h"
#include "ui_dialogconfigdocumentacion.h"

DialogConfigDocumentacion::DialogConfigDocumentacion(QWidget *parent, const QString &descripcion) :
QDialog(parent), ui(new Ui::DialogConfigDocumentacion) {
    ui->setupUi(this);
    ui->CB_codigo->setEnabled(false);
    ui->TE_descripcion->setPlaceholderText("Escriba la descripcion aquí ...");
    ui->LE_path->setDisabled(true);
    fDescripcion = ui->CB_descipcion->isChecked();
    fIntroTeorica = ui->CB_intro->isChecked();
    fTabla = ui->CB_tabla->isChecked();
    fCodigo = ui->CB_codigo->isChecked();
    fOdt = ui->CB_odt->isChecked();
    fHtml = ui->CB_html->isChecked();
    ui->TE_descripcion->setText(descripcion);
}

DialogConfigDocumentacion::~DialogConfigDocumentacion() {
    delete ui;
}

void DialogConfigDocumentacion::on_CB_tabla_toggled(bool checked) {
    fTabla = checked;
}

void DialogConfigDocumentacion::on_CB_intro_toggled(bool checked) {
    fIntroTeorica = checked;
}

void DialogConfigDocumentacion::on_CB_descipcion_toggled(bool checked) {
    fDescripcion = checked;
    if(fDescripcion)
        ui->TE_descripcion->setEnabled(true);
    else
        ui->TE_descripcion->setEnabled(false);
}

void DialogConfigDocumentacion::on_CB_codigo_toggled(bool checked) {
    fCodigo = checked;
}

void DialogConfigDocumentacion::on_CB_odt_toggled(bool checked) {
    fOdt = checked;
}

void DialogConfigDocumentacion::on_CB_html_toggled(bool checked) {
    fHtml = checked;
}

void DialogConfigDocumentacion::on_PB_explorar_clicked() {
    QString dirAnterior = ui->LE_path->text();
    if(dirAnterior.isEmpty())
        dirAnterior = "/";

    // Abrir un diálogo para que el usuario seleccione un directorio
    QString dir = QFileDialog::getExistingDirectory(nullptr, "Seleccionar directorio", dirAnterior);

    //Si eligio un directrio
    if(!dir.isEmpty()) {

        //Actualizo ui
        ui->LE_path->setText(dir);

        // Crear un objeto QDir para acceder a los archivos dentro del directorio seleccionado
        QDir directorio(dir);

        // Obtener una lista de nombres de archivos dentro del directorio
        QStringList archivosQList = directorio.entryList(QDir::Files);

        // Convertir la lista de QString a una lista de std::string
        std::list<QString> listaArchivosStd;
        for (QString archivo : archivosQList) {
            listaArchivos.push_back(dir+"/"+archivo);
        }

        //Si no hay ruta seleccionada no habilito a que puedan incluir codigo
        if(listaArchivos.size()==0)
            ui->CB_codigo->setEnabled(false);
        else
            ui->CB_codigo->setEnabled(true);
    }
}

#include <QMessageBox>
void DialogConfigDocumentacion::on_PB_ok_accepted() {
    descripcion = ui->TE_descripcion->toPlainText();

    //Valido que el directorio que puso el usuario este bien
    QDir dir(ui->LE_path->text());
    if(fCodigo && !dir.exists()) {
        QMessageBox::critical(nullptr,"Directorio no encontrado", "El directorio de las implementaciones en C no fue encontrado");
        return;
    }
    this->accept();
}


void DialogConfigDocumentacion::on_LE_path_textEdited(const QString &arg1) {
    if(arg1.isEmpty()) {
        ui->CB_codigo->setEnabled(false);
        ui->CB_codigo->setChecked(false);
    } else
        ui->CB_codigo->setEnabled(true);
}

