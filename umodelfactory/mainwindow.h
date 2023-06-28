#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTcpSocket>
#include <QTcpServer>
#include "ayudadinamica.h"

#include "dialoggenerarcodigo.h"
#include "fuentes.h"
#include "converciondedatos.h"
#include "timers.h"
#include "variablesdeproceso.h"
#include "expresiones.h"
#include "dialogcomentario.h"
#include "documentacion.h"
#include <QFileDialog>
#include <QDir>
#include <QStringList>
#include <list>
#include <dialogconfigdocumentacion.h>
#include <dialogdocumentaciongenerada.h>
#include "ayudadinamica.h"

#define DEFAULT_NAME_NUEVA_MAQUINA  "MdE"
#define DEFAULT_NAME_NUEVO_ESTADO   "ESTADO"
#define SCENE_INITIAL_WIDTH         2500
#define SCENE_INITIAL_HEIGHT        1500
#define T_SIM_RESET_INIT            1000
#define T_SIM_ESTADO_ACTIVO         900
#define T_SIM_ESTADO_INACTIVO       500

#define T_DBG_PROGRESO          100
#define DBG_ID_MAQUINA          0
#define DBG_ID_ESTADO           1
#define DBG_ID_EVENTO           2
#define DBG_ID_TIMESTAMP        3

#define ESPERO_MENSAJE          0
#define RECIBO_DATOS_MAQ        1
#define RECIBO_DATOS_ESTADO     2
#define RECIBO_DATOS_EVENTO     3
#define FIN_DE_TRAMA            4

//comentarios
#define ACTION_ADD_COMENTARIO   "Agregar comentario"
#define ACTION_EDIT_COMENTARIO  "Editar comentario"

#define NOMBRE_ARCHIVO_DOCUMENTACION "documentacion.docx"

class XmlHandler;
class QMdiSubWindow;
class SubWindow;
class QTreeWidgetItem;
class QLabel;
class QComboBox;
class QCheckBox;
class QActionGroup;
class QTimer;
class QElapsedTimer;
class QGraphicsItem;
class DiagramScene;
class DiagramItem;
class DiagramTransition;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum UiMode { Editar, Simular, Verificar, Depurar };
    enum DebugMode { NoDebug, DebugEnPausa, DebugRunning, DebugFinalizado };
    enum TreeItemType { Proyecto, Sistema, Eventos, Acciones, Variables, Constantes,
                        Maquina, ItemEstado, ItemEvento, ItemAccion, ItemVariable, ItemConstante };

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    // Acciones
    void on_actionNuevoProyecto_triggered();
    void on_actionAbrirProyecto_triggered();
    void on_actionGuardar_triggered();
    void on_actionGuardarComo_triggered();
    void on_actionCerrarProyecto_triggered();
    void on_actionAcercaDe_triggered();
    // Herramientas
    void on_actionNuevaMaquina_triggered();
    void on_actionEliminarMaquina_triggered();
    void on_actionRenombrarMaquina_triggered();
    void on_actionSeleccionar_triggered();
    void on_actionNuevoEstado_triggered();
    void on_actionEstadoInicial_triggered();
    void on_actionNuevaTransicion_triggered();
    void on_actionEliminarElemento_triggered();
    void on_actionNuevoEvento_triggered();
    void on_actionNuevaAccion_triggered();
    void on_actionNuevaVariable_triggered();
    void on_actionNuevaConstante_triggered();
    void on_actionBorrarEvento_triggered();
    void on_actionBorrarAccion_triggered();
    void on_actionBorrarVariable_triggered();
    void on_actionBorrarConstante_triggered();
    void on_actionEditarConstante_triggered();

    // Simulación
    void on_actionSimular_triggered();
    void on_updateSimulacionTimeout();
    void on_tableWidgetSimEventos_cellDoubleClicked(int row, int column);
    void on_actionTerminarSimulacion_triggered();
    // Verificación
    void on_actionVerificar_triggered();
    // Generación
    void on_actionGenerarCodigo_triggered();
    void on_actionGenerarDocumentacion_triggered();
    // SubWindows
    void on_mdiArea_subWindowActivated(QMdiSubWindow *activeSubWindow);
    // DiagramScene
    void on_itemInserted(DiagramItem *item);
    void on_arrowInserted(DiagramTransition* arrow);
    void on_itemSelected(QGraphicsItem *item);

    void on_sceneSelectionChanged();

    void on_itemDoubleClicked(int index);
    void on_arrowDoubleClicked(DiagramTransition *arrow);
    void on_sceneScaleChanged(const QString &scale);
    // Explorador
    void on_treeWidgetProyecto_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeWidgetProyecto_customContextMenuRequested(const QPoint &pos);
    // Importar Recursos
    void on_actionImportarRecursosDesdeArchivo_triggered();
    void on_actionImportarRecursosDesdeOtroProyecto_triggered();
    // Depuración
    void on_actionConfigurarDepuracion_triggered();
    void on_actionDepurar_triggered();
    void on_actionTerminarDepuracion_triggered();
    void on_pushButtonDebugStop_clicked();
    void on_pushButtonDebugPlay_clicked();
    void on_pushButtonDebugPausa_clicked();
    void on_pushButtonDebugStepFw_clicked();
    void on_doubleSpinBoxEscalaTiempoDebug_valueChanged(double arg1);
    void on_checkBoxMaquinasDebug_clicked();
    void on_replayDebugTimeout();
    void on_progresoDebugTimeout();
    void on_datosRecibidosSerial();
    void on_updateDebugTimeout();
    // Simulación TCP
    void on_nuevaConexionSimTCP();
    void on_cambioEnClientSocketSim(QAbstractSocket::SocketState socketState);
    void on_datosRecibidosSimTCP();

    void on_actionInterfaz_Virtual_triggered();
    void on_timerevent( void );
    void on_actionAgregar_comentario_triggered();

    void on_actionEliminar_comentario_triggered();

    // Ayuda
    void on_actionAyudaDesactivada_triggered(bool checked);
    void on_actionAyudaUsuarioInicial_triggered(bool checked);
    void on_actionAyudaUsuarioAvanzado_triggered(bool checked);

private:
    int maybeSave();
    void uiDisable();
    void uiEnable();
    void uiReset();
    void readSettings();
    void saveSettings();
    void initSubWindow(const QString &name);
    void initExploradorProyecto();
    void initTablaTransiciones();
    void initTablaSimulacion();
    void updateUi();
    void updateTablaTransiciones(DiagramScene *scene);
    void updateExploradorProyecto(const TreeItemType &type = TreeItemType::Proyecto);
    void addItemProyecto(QTreeWidgetItem *parent, const QStringList &list,
                         const TreeItemType &type, const QString &icon);
    void addItemMaquina(QTreeWidgetItem *parent, const QString &name,
                        const QStringList &list, const QString &icon);
    QString getNameNuevaMaquina();
    QString getNameNuevoEstado();
    int indexOfMaquina(const QString &nameMaquina) const;
    int indexOfEstado(const QString &nameMaquina, const QString &nameEstado) const;
    QString uniqueNameMaquina(const QString &name) const;
    QString uniqueNameEstado(const QString &name) const;
    QString accionesDivididas(QStringList acciones);

    // Generación de Código
    void leerUmf(QList<struct MdeE> &Maquinas, struct Elementos &Elm);

    // Documentación
    std::list<QString> exportarImagenesDiagramas();
    std::list<QString> obtenerNombresArchivos();
    Documentacion m_docManager;
    void cargarContenidosInfoTeorica();

    // Depuracion
    void procesarDatosRecibidosSerial();
    void updateMonitorDebug();

    // Simulación TCP
    void procesarDatosRecibidosSimTCP(QString eventoRecibido);

    // Proyecto
    void guardarDatos();

    Ui::MainWindow *ui;
    XmlHandler *m_datosXml;
    QList<DiagramScene *>m_sceneList;

    QMenu *m_itemMenu;
    QActionGroup *m_pointerTypeGroup;
    QActionGroup *m_nivelAyudaGroup;
    QComboBox *m_sceneScaleCombo;
    QLabel *m_zoomLabel;
    QLabel *m_zoomLabelEnd;

    QMenu *m_contextMenuExplorador;
    QTreeWidgetItem *m_selectedItemExplorador;

    QList<int> m_indexEstadosActuales;

    bool m_modificado;
    bool m_resetDockVerificar;
    UiMode m_uiMode;

    DiagramScene *m_scene;

    // Depuración
    QSerialPort *m_serialPortDebug;
    QByteArray m_datosRecibidos;
    QString m_portName;
    QString m_replayDebugFileName;
    int m_idMaquinaRx;
    int m_idEstadoRx;
    int m_idEventoRx;
    bool m_debugTiempoReal;
    bool m_generarArchivoDebugLog;
    bool m_resetActionDepurar;
    QElapsedTimer m_timerCounter;
    QTimer m_timerUpdateDebug;
    QTimer m_timerReplayDebug;
    QTimer m_timerProgresoDebug;
    QFile m_archivoDebugLog;
    int m_timeStampActual;
    QByteArray debugData;
    DebugMode m_debugMode;
    QList <QStringList> m_debugFileData;
    int m_indexDebugData;
    QList<QCheckBox*> m_checkBoxMaquinasDebug;

    // Simulación
    QTimer m_timerUpdateSim;
    QTcpSocket m_socketSim;
    QTcpServer m_serverSim;
    QList<QTcpSocket*> m_clientSocketsSim;

    // Generación de Código - MT
    DialogGenerarCodigo m_permisosCodigo;
    Fuentes m_funcionesCodigo;

    // Conversion
    ConvercionDeDatos *m_Infotronic = new ConvercionDeDatos ;//, *Infotronic_2020 = new ConvercionDeDatos;
    bool m_simular_con_Infotronic_virtual;
    // falta 2020 chica y experto,

    // Maquinaria de timers
    QTimer m_timers;
    timers TimerRun;

    QList <struct MdeE> m_Maquinas;
    struct Elementos m_Elmementos;

    variablesdeproceso m_variablesdeproceso;

    expresiones m_expresiones;

    void MostrarTablaVariables();
    void OcultarTablaVariables();
    void BorrarTablaVariables();
    void InicializarTablaVariables();

	//comentarios
    QString commentElementId;
    QAction * m_actionAgregarComentario;
    QString convertStringComentario(const QString & comment);

    // Ayuda Dinámica
    AyudaDinamica ayudaDinamica;

};

#endif // MAINWINDOW_H
