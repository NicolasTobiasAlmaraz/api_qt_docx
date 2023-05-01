#ifndef DIALOGTRANSICION_H
#define DIALOGTRANSICION_H

#include <QDialog>

// Para borrado de temporizadores
#define    M_BORRAR_FASE_0      0
#define    M_BORRAR_FASE_1      1
#define    M_BORRAR_FASE_2      2

namespace Ui {
class DialogTransicion;
}

class QListWidgetItem;
class QStringListModel;
class QCompleter;
class XmlHandler;
class DiagramScene;
class DiagramTransition;

class DialogTransicion : public QDialog
{
    Q_OBJECT

public:
    enum ModoTransicion {NuevaTransicion, EditarTransicion};

    explicit DialogTransicion(QWidget *parent = nullptr);
    ~DialogTransicion();

    QString selectedEvent() const { return m_evento; }
    QString selectedActions() const { return m_acciones; }
    void setDatos(DiagramTransition* &arrow, XmlHandler* &datos, QList<DiagramScene *> &sceneList, DiagramScene* &scene, ModoTransicion modo);

private slots:
    void on_listWidgetEventos_itemDoubleClicked(QListWidgetItem *item);
    void on_pushButtonNuevoEvento_clicked();
    void on_pushButtonBorrarEvento_clicked();
    void on_pushButtonElegirEvento_clicked();

    void on_pushButtonAnd_clicked();
    void on_pushButtonOr_clicked();
    void on_pushButtonNot_clicked();
    void on_pushButtonParentesis_clicked();
    void on_pushButtonParentesisCerrar_clicked();
    void on_pushButtonBorrarCondicion_clicked();

    void on_listWidgetAcciones_itemDoubleClicked(QListWidgetItem *item);
    void on_pushButtonElegirAccion_clicked();
    void on_pushButtonBorrarItemAccion_clicked();
    void on_pushButtonSubirItemAccion_clicked();
    void on_pushButtonBajarItemAccion_clicked();
    void on_pushButtonNuevaAccion_clicked();
    void on_pushButtonBorrarAccion_clicked();

    void on_listWidgetVariables_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidgetVariables_2_itemDoubleClicked(QListWidgetItem *item);
    void on_pushButtonElegirExpresion_clicked();
    void on_pushButtonElegirExpresion_2_clicked();

    void on_pushButtonNuevaVariable_clicked();
    void on_pushButtonBorrarVariable_clicked();
    void on_pushButtonNuevaVariable_2_clicked();
    void on_pushButtonBorrarVariable_2_clicked();

    void on_pushButtonAceptar_clicked();
    void on_pushButtonCancelar_clicked();

    void on_pushButtonNuevoTemporizador_clicked();

    void on_pushButtonBorrarTemporizador_clicked();

    void on_pushButtonElegirTemporizador_clicked();

    void on_pushButtonNuevaMacro_clicked();

    void on_pushButtonBorrarMacro_clicked();

    void on_pushButtonElegirMacro_clicked();

    void on_listWidgetMacros_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidgetTemporizadores_itemDoubleClicked(QListWidgetItem *item);

private:
    void borrarVariable(QString variableActual, int rowActual);
    Ui::DialogTransicion *ui;
    XmlHandler *m_datos;
    QList<DiagramScene *>m_sceneList;
    DiagramScene *m_scene;
    DiagramTransition *m_arrow;
    QCompleter *m_completer;
    QStringListModel *m_completerModel;
    QString m_evento;
    QString m_acciones;
    int m_difParentesis;
    QStringList m_eventosEliminados;
    QStringList m_accionesEliminadas;
    QStringList m_variablesEliminadas;
    int m_Borrar_Evento_Y_Timer;

};

#endif // DIALOGTRANSICION_H
