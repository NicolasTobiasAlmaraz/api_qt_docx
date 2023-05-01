#ifndef DIALOGEDITORESTADO_H
#define DIALOGEDITORESTADO_H

#include <QDialog>

namespace Ui {
class DialogEditorEstado;
}

class XmlHandler;
class DiagramScene;
class DiagramItem;
class DiagramTransition;

class DialogEditorEstado : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditorEstado(QWidget *parent = 0);
    ~DialogEditorEstado();

    void setDatos(int indexEstado, XmlHandler* &datos, DiagramScene* &scene);
    bool datosModificados(void) { return m_modificado; }

private slots:
    void on_pushButtonEliminar_clicked();  
    void on_pushButtonAceptar_clicked();
    void on_listWidgetTransiciones_itemSelectionChanged();
    void updateColorEstado(int idColor);
    void updateNombreEstado();

private:
    Ui::DialogEditorEstado *ui;
    DiagramScene *m_scene;
    XmlHandler *m_datos;
    int m_index;
    bool m_modificado;
};

#endif // DIALOGEDITORESTADO_H
