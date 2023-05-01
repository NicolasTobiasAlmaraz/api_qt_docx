#include "dialogconstantes.h"
#include "ui_dialogconstantes.h"

DialogConstantes::DialogConstantes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConstantes)
{
    ui->setupUi(this);

    QRegExp regExpNombre("[A-Za-z_][A-Za-z0-9-_]+");
    QRegExp regExpValor("[a-zA-z0-9_+-/*]+");

    validatorNombre = new QRegExpValidator(regExpNombre);
    validatorValor = new QRegExpValidator(regExpValor);

    ui->lineEditNombre->setValidator(validatorNombre);
    ui->lineEditValor->setValidator(validatorValor);
}

DialogConstantes::~DialogConstantes()
{
    delete ui;
    delete validatorValor;
    delete validatorNombre;
}

QString DialogConstantes::getNombre()
{
    return ui->lineEditNombre->text();
}

QString DialogConstantes::getValor()
{
    return ui->lineEditValor->text();
}

void DialogConstantes::setDatos(QString nombre, QString valor)
{
    ui->lineEditNombre->setText(nombre);
    ui->lineEditNombre->setReadOnly(true);
    ui->lineEditValor->setText(valor);
    ui->lineEditValor->setFocus();
}

void DialogConstantes::on_pushButtonAceptar_clicked()
{
    this->accept();
}

void DialogConstantes::on_pushButtonCancelar_clicked()
{
   this->reject();
}
