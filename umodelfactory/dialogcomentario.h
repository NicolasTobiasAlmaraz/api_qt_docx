#ifndef DIALOGCOMENTARIO_H
#define DIALOGCOMENTARIO_H

#include <QDialog>
#include "xmlhandler.h"
#include <QMessageBox>

#define PLACEHOLDER_COMENTARIO "Escriba el comentario..."

namespace Ui {
class DialogComentario;
}

class DialogComentario : public QDialog
{
    Q_OBJECT

public:
    explicit DialogComentario(XmlHandler ** xHandler,const QString & id, QWidget *parent = nullptr);
    ~DialogComentario();

private slots:
    void on_pushButtonOk_clicked();

    void on_pushButtonCancel_clicked();

    void on_pushButtonDelete_clicked();

private:
    Ui::DialogComentario *ui;
    XmlHandler * xHandlerPtr;
    QString elementId;
    QString commentBuffer;
};

#endif // DIALOGCOMENTARIO_H
