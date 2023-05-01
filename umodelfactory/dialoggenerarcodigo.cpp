#include "dialoggenerarcodigo.h"
#include "ui_dialoggenerarcodigo.h"

DialogGenerarCodigo::DialogGenerarCodigo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGenerarCodigo)
{
    ui->setupUi(this);

    m_coment = m_depurar = false;

    QStringList items;
    items.append(QString::fromUtf8("Switch Case"));
    items.append(QString::fromUtf8("Puntero a Función"));
    ui->comboBoxCodigo->addItems(items);
}

DialogGenerarCodigo::~DialogGenerarCodigo()
{
    delete ui;
}

void DialogGenerarCodigo::on_pushButtonConfirmar_clicked()
{
    m_seleccionDeCodigo = ui->comboBoxCodigo->currentText();

    this->accept();
}

void DialogGenerarCodigo::on_checkBoxComentarios_toggled(bool checked)
{
    m_coment =  checked;
}

void DialogGenerarCodigo::on_checkBoxDebug_toggled(bool checked)
{
    m_depurar = checked;
}

bool DialogGenerarCodigo::getDepurar(void)
{
    return m_depurar;
}

void DialogGenerarCodigo::SetDepurar(bool d)
{
    m_depurar = d;
}

bool DialogGenerarCodigo::getComentarios(void)
{
    return m_coment;
}

void DialogGenerarCodigo::SetComentarios(bool d)
{
    m_coment = d;
}

QString DialogGenerarCodigo::getSeleccioDeCodigo(void)
{
    return m_seleccionDeCodigo;
}

void DialogGenerarCodigo::SetSeleccioDeCodigo(QString &sdc)
{
    m_seleccionDeCodigo = sdc;
}

