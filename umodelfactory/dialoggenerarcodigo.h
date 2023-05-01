#ifndef DIALOGGENERARCODIGO_H
#define DIALOGGENERARCODIGO_H

#include <QDialog>

namespace Ui {
class DialogGenerarCodigo;
}

class DialogGenerarCodigo : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGenerarCodigo(QWidget *parent = nullptr);
    ~DialogGenerarCodigo();

    bool getDepurar(void);
    void SetDepurar(bool d);
    bool getComentarios(void);
    void SetComentarios(bool d);
    QString getSeleccioDeCodigo(void);
    void SetSeleccioDeCodigo(QString &sdc);

private slots:
    void on_pushButtonConfirmar_clicked();

    void on_checkBoxComentarios_toggled(bool checked);

    void on_checkBoxDebug_toggled(bool checked);

private:
    Ui::DialogGenerarCodigo *ui;
    bool m_coment, m_depurar;
    QString m_seleccionDeCodigo;
};

#endif // DIALOGGENERARCODIGO_H
