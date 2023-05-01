#ifndef NUEVOPROYECTO_H
#define NUEVOPROYECTO_H

#include <QDialog>

namespace Ui {
class DialogNuevoProyecto;
}

class DialogNuevoProyecto : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogNuevoProyecto(QWidget *parent = 0);
    ~DialogNuevoProyecto();
    
    QStringList selectedData() const;

private slots:

    void on_pushButtonBuscar_clicked();

    void on_pushButtonCrear_clicked();

    void on_pushButtonCancelar_clicked();

    void on_lineEditCarpeta_textChanged(const QString &path);

    void on_lineEditNombre_textChanged(const QString &nombre);

private:
    Ui::DialogNuevoProyecto *ui;
};

#endif // NEW_PROYECTO_H
