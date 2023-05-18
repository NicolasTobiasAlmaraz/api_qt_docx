#ifndef DIALOGCONFIGDOCUMENTACION_H
#define DIALOGCONFIGDOCUMENTACION_H

#include <QDialog>
#include <QDir>
#include <QFileDialog>


namespace Ui {
class DialogConfigDocumentacion;
}

class DialogConfigDocumentacion : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfigDocumentacion(QWidget *parent = nullptr, const QString &descripcion="");
    ~DialogConfigDocumentacion();
    inline bool getFlagDescripcion() {return fDescripcion;}
    inline bool getFlagIntroTeorica() {return fIntroTeorica;}
    inline bool getFlagTabla() {return fTabla;}
    inline bool getFlagCodigo() {return fCodigo;}
    inline bool getFlagOdt() {return fOdt;}
    inline bool getFlagHtml() {return fHtml;}
    inline QString getTextoDescripcion() {return descripcion;}
    inline std::list <QString> getListaArchivos() {return listaArchivos;}

private slots:
    void on_CB_tabla_toggled(bool checked);
    void on_CB_intro_toggled(bool checked);
    void on_CB_descipcion_toggled(bool checked);
    void on_CB_codigo_toggled(bool checked);
    void on_CB_odt_toggled(bool checked);
    void on_CB_html_toggled(bool checked);
    void on_PB_explorar_clicked();

    void on_PB_ok_accepted();

    void on_LE_path_textEdited(const QString &arg1);

private:
    Ui::DialogConfigDocumentacion *ui;
    std::list<QString> listaArchivos;
    QString descripcion;
    bool fDescripcion;
    bool fIntroTeorica;
    bool fTabla;
    bool fCodigo;
    bool fOdt;
    bool fHtml;
};

#endif // DIALOGCONFIGDOCUMENTACION_H
