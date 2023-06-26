#include "dialogdocumentaciongenerada.h"
#include "ui_dialogdocumentaciongenerada.h"

DialogDocumentacionGenerada::DialogDocumentacionGenerada(QWidget *parent, QString ruta) :
    QDialog(parent),
    ui(new Ui::DialogDocumentacionGenerada)
{
    mRuta = ruta;
    ui->setupUi(this);
    ui->TB_ruta->setText(ruta);
    this->setWindowTitle("Documentación Generada");
    this->setFixedSize(470,150);
}

DialogDocumentacionGenerada::~DialogDocumentacionGenerada()
{
    delete ui;
}

void DialogDocumentacionGenerada::on_PB_ver_clicked()
{
    QUrl directoryUrl = QUrl::fromLocalFile(mRuta);
    QDesktopServices::openUrl(directoryUrl);
}


void DialogDocumentacionGenerada::on_PB_ok_clicked()
{
    close();
}

