#ifndef DIALOGCONSTANTES_H
#define DIALOGCONSTANTES_H

#include <QDialog>
#include <QRegExpValidator>

namespace Ui {
class DialogConstantes;
}

class DialogConstantes : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConstantes(QWidget *parent = nullptr);
    ~DialogConstantes();

    QString getNombre();
    QString getValor();
    void setDatos(QString nombre, QString valor);

private slots:
    void on_pushButtonAceptar_clicked();
    void on_pushButtonCancelar_clicked();

private:
    Ui::DialogConstantes *ui;
    QRegExpValidator *validatorNombre, *validatorValor;
};

#endif // DIALOGCONSTANTES_H
