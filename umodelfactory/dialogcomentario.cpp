#include "dialogcomentario.h"
#include "ui_dialogcomentario.h"

DialogComentario::DialogComentario(XmlHandler ** xPtr, const QString & id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogComentario)
{
    ui->setupUi(this);
    xHandlerPtr = *xPtr;
    //xHandlerPtr = new XmlHandler();
    //QFile file("D:/Felipe/Universidad/Informatica_2/umodelSimulacion/projects/aasd/aasd.umf");
    //file.open(QFile::ReadOnly);
    //xHandlerPtr->load(file);
    elementId = id;
    commentBuffer = xHandlerPtr->getComentario(id);
    if (commentBuffer == "") {
        ui->plainTextEditComment->setPlaceholderText(PLACEHOLDER_COMENTARIO);
    } else {
        ui->plainTextEditComment->setPlainText(commentBuffer);
    }

}

DialogComentario::~DialogComentario()
{
    delete ui;
}

void DialogComentario::on_pushButtonOk_clicked()
{
    QRegularExpression nonAllowedChars("[<>]+|\\/\\/|\\/\\*|\\*\\/");
    //agregar el comentario al elemento que llamo
    if (commentBuffer == "") {
        commentBuffer = ui->plainTextEditComment->document()->toRawText().replace(nonAllowedChars, "");
        xHandlerPtr->setComentario(elementId, commentBuffer);
    } else {
        commentBuffer = ui->plainTextEditComment->document()->toRawText().replace(nonAllowedChars, "");
        xHandlerPtr->clearComentario(elementId, true);
        xHandlerPtr->setComentario(elementId, commentBuffer);
    }
    this->accept();
}

void DialogComentario::on_pushButtonCancel_clicked()
{
    //cancelar la modificacion del comentario y guardar lo que habia antes
    this->reject();
}

void DialogComentario::on_pushButtonDelete_clicked()
{
    //borrar el comentario de este elemento
    int ret = QMessageBox::warning(this, "Advertencia", "Esta seguro? Esta operacion no se puede deshacer", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        xHandlerPtr->clearComentario(elementId, true);
        this->reject();
    }
}
