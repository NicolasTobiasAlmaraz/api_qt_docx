#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xmlhandler.h"
#include "dialognuevoproyecto.h"
#include "dialogtransicion.h"
#include "dialogeditorestado.h"
#include "dialogconfigurardebug.h"
#include "dialogconstantes.h"
#include "regexpinputdialog.h"
#include "subwindow.h"
#include "diagramscene.h"
#include "diagramitem.h"
#include "diagramtransition.h"
#include "fuentes.h"
#include "interfazinfotronicvirtual.h"
#include "expresiones.h"

#include <QSettings>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QTreeWidgetItem>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QImage>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QDebug>
#include <QTimer>
#include <QTextTable>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QTcpSocket>
#include <QTcpServer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_datosXml = nullptr;
    m_modificado = false;

    // ------- Toolbars -------
    m_sceneScaleCombo = new QComboBox();
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%") << tr("200%");
    m_sceneScaleCombo->addItems(scales);
    m_sceneScaleCombo->setCurrentIndex(2);
    connect(m_sceneScaleCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(on_sceneScaleChanged(QString)));
    m_zoomLabel = new QLabel(" Zoom: ");
    m_zoomLabelEnd = new QLabel("  ");
    ui->mainToolBar->addWidget(m_zoomLabel);
    ui->mainToolBar->addWidget(m_sceneScaleCombo);
    ui->mainToolBar->addWidget(m_zoomLabelEnd);
    ui->mainToolBar->addSeparator();

    // ------- Actions -------
    m_pointerTypeGroup = new QActionGroup(this);
    m_pointerTypeGroup->addAction(ui->actionSeleccionar);
    m_pointerTypeGroup->addAction(ui->actionNuevoEstado);
    m_pointerTypeGroup->addAction(ui->actionNuevaTransicion);
    m_pointerTypeGroup->addAction(ui->actionEstadoInicial);

    connect(ui->actionSalir, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionMosaico, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
    connect(ui->actionCascada, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));

    m_nivelAyudaGroup = new QActionGroup(this);
    m_nivelAyudaGroup->addAction(ui->actionAyudaDesactivada);
    m_nivelAyudaGroup->addAction(ui->actionAyudaUsuarioInicial);
    m_nivelAyudaGroup->addAction(ui->actionAyudaUsuarioAvanzado);
    m_nivelAyudaGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);

    // ------- Menus -------
    m_itemMenu = new QMenu(this);
    m_itemMenu->addAction(ui->actionEliminarElemento);
    m_itemMenu->addSeparator();
        
    ui->menuVer->addSeparator();
    ui->menuVer->addAction(ui->dockWidgetProyecto->toggleViewAction());
    ui->menuVer->addAction(ui->dockWidgetTransiciones->toggleViewAction());

    m_contextMenuExplorador = new QMenu(this);

    //Workspace settings
    readSettings();
    m_uiMode = MainWindow::Editar;
    ui->dockWidgetProyecto->show();

    // ------ UI -------
    uiDisable();
    initTablaTransiciones();
    initTablaSimulacion();
    ui->treeWidgetProyecto->setContextMenuPolicy(Qt::CustomContextMenu);

    // ---- Simulación -----
    connect(&m_timerUpdateSim, SIGNAL(timeout()), this, SLOT(on_updateSimulacionTimeout()));
    m_serverSim.listen(QHostAddress::Any, 9000);
    connect(&m_serverSim, SIGNAL(newConnection()), this, SLOT(on_nuevaConexionSimTCP()));

    // ----- Debug -----
    m_serialPortDebug = nullptr;
    m_debugTiempoReal = true;
    m_generarArchivoDebugLog = true;
    m_resetActionDepurar = false;
    connect(&m_timerUpdateDebug, SIGNAL(timeout()), this, SLOT(on_updateDebugTimeout()));
    connect(&m_timerReplayDebug, SIGNAL(timeout()), this, SLOT(on_replayDebugTimeout()));
    connect(&m_timerProgresoDebug, SIGNAL(timeout()), this, SLOT(on_progresoDebugTimeout()));    
    // Por ahora quedan deshabilitados hasta no implementar Escala de tiempo variable
    ui->doubleSpinBoxEscalaTiempoDebug->setDisabled(true);
    ui->labelSpinBoxEscalaTiempo->setDisabled(true);
    ui->labelEscalaTiempo->setDisabled(true);

    statusBar()->showMessage(tr("Listo."));

    ui->actionInterfaz_Virtual->setEnabled(false);
    m_simular_con_Infotronic_virtual = false ;

    connect(& m_timers , SIGNAL(timeout()) , this , SLOT( on_timerevent()) );
    TimerRun.start_timer( m_timers );
    TimerRun.Set_basedetiempo(1);
//    struct timer t;
//    t.evento = "eventoTimer_B";
//    t.tiempo = 8000;
//    t.deltaT = 8000;
//    t.Evento = nullptr;
//    t.n = 0;
//    TimerRun.timerstart(t);

    //tabla de variables
    BorrarTablaVariables();
    OcultarTablaVariables();
    
    //comentarios
    commentElementId = "";
    m_actionAgregarComentario = new QAction();
    connect(m_actionAgregarComentario, SIGNAL(triggered()), this, SLOT(on_actionAgregar_comentario_triggered()));

    //ayuda
    ayudaDinamica.cargarContenidos();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_actionAgregarComentario;
}

void MainWindow::on_actionNuevoProyecto_triggered()
{
    // Chequeo si hay otro proyecto abierto
    if (m_datosXml != nullptr) {
        int rv = maybeSave();
        if(rv == QMessageBox::Cancel)
            return;
        else if (rv == QMessageBox::Save)
            guardarDatos();

        delete m_datosXml;
        m_datosXml = nullptr;
        uiReset();
    }

    // Crea y muestra el QDialog para crear un nuevo proyecto
    DialogNuevoProyecto dialogProyecto(this);
    if (dialogProyecto.exec() == QDialog::Accepted) {
        // Recupera los datos del QDialog
        QStringList datosNuevoProyecto = dialogProyecto.selectedData();
        // Nuevo handler con el nombre del proyecto
        m_datosXml = new XmlHandler(datosNuevoProyecto.first(), NUEVO_XML_DATOS);
        // Estos métodos cargan los datos a la estructura XML
        m_datosXml->setContenidoGeneral(NOMBRE, datosNuevoProyecto.at(0));
        m_datosXml->setContenidoGeneral(AUTORES, datosNuevoProyecto.at(1));
        m_datosXml->setContenidoGeneral(CARPETA, datosNuevoProyecto.at(2));
        m_datosXml->setContenidoGeneral(CREACION, QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"));
        m_datosXml->setContenidoGeneral(MODIFICACION, QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"));

        // NUEVO! pruebo generar ctes por default
        m_datosXml->addNuevaConstante("TRUE,1");
        m_datosXml->addNuevaConstante("FALSE,0");
        m_datosXml->addNuevaConstante("ON,1");
        m_datosXml->addNuevaConstante("OFF,0");

        m_datosXml->save();

        // habilito ui parcialmente
        ui->actionNuevaMaquina->setEnabled(true);
        ui->actionGuardar->setEnabled(true);
        ui->actionGuardarComo->setEnabled(true);
        ui->actionCerrarProyecto->setEnabled(true);
        ui->menuImportarRecursos->setEnabled(true);
        ui->actionImportarRecursosDesdeArchivo->setEnabled(true);
        ui->actionImportarRecursosDesdeOtroProyecto->setEnabled(true);
        ui->dockWidgetProyecto->setEnabled(true);
        ui->mdiArea->setEnabled(true);
        ui->actionInterfaz_Virtual->setEnabled(true);

        initExploradorProyecto();
        updateExploradorProyecto(Proyecto);
        setWindowTitle("uModel Factory | UTN-FRBA - " + datosNuevoProyecto.at(0) + ".umf");
        m_modificado = false;

        //automaticamente abro el cuadro de dialogo para una nueva MdE...
        //on_actionNuevaMaquina_triggered();
    }
}

void MainWindow::on_actionAbrirProyecto_triggered()
{
    // Chequeo si hay que guardar cambios...
    if(m_datosXml != nullptr) {
        int rv = maybeSave();
        if(rv == QMessageBox::Cancel)
            return;
        else if (rv == QMessageBox::Save)
            guardarDatos();
    }

    // Abro el Dialog para elegir un proyecto (archivos .umf)
    QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir Proyecto"),
                                                    QDir::homePath(), tr("Proyectos(*.umf)"));

    // Si no se selecciona ningún archivo, salgo...
    if (fileName.isEmpty())
        return;

    // Abro el archivo seleccionado
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error al Abrir Proyecto"),
                             tr("No se pudo abrir el archivo \n%1").arg(fileName));
        return;
    }

    // Libero la memoria del handler xml y reseteo la ui
    if(m_datosXml != nullptr) {
        delete m_datosXml;
        m_datosXml = nullptr;
        uiReset();
    }

    // Genero un nuevo handler xml y cargo los datos del archivo .umf
    m_datosXml = new XmlHandler();
    if (!m_datosXml->load(file)) {
        QMessageBox::warning(this, tr("Error al Abrir Proyecto"),
                            tr("No se pudo procesar el archivo \n%1").arg(fileName));
        return;
    }
    file.close();

    // Actualizo la carpeta del proyecto
    QString pathProy(QFileInfo(fileName).absolutePath());
    m_datosXml->setContenidoGeneral(CARPETA, pathProy);

    // Habilito ui parcialmente
    ui->actionNuevaMaquina->setEnabled(true);
    ui->actionGuardar->setEnabled(true);
    ui->actionGuardarComo->setEnabled(true);
    ui->actionCerrarProyecto->setEnabled(true);
    ui->menuImportarRecursos->setEnabled(true);
    ui->actionImportarRecursosDesdeArchivo->setEnabled(true);
    ui->actionImportarRecursosDesdeOtroProyecto->setEnabled(true);
    ui->dockWidgetProyecto->setEnabled(true);
    ui->mdiArea->setEnabled(true);
     ui->actionInterfaz_Virtual->setEnabled(true);
    initExploradorProyecto();

    // Cargo los datos del proyecto
    int cantMaquinas, cantEstados, cantTransiciones;
    QString name;
    int width, height;
    double pos_x, pos_y;
    QColor color;

    cantMaquinas = m_datosXml->cantidadMaquinas();
    for (int k = 0; k < cantMaquinas; ++k) {
        name = m_datosXml->getContenidoMaquina(MAQUINA+QString::number(k), NOMBRE_MAQUINA);
        initSubWindow(name);
        SubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->subWindowList().at(k));
        QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
        DiagramScene *scene = static_cast<DiagramScene *>(view->scene());



        cantEstados = m_datosXml->cantidadEstados(MAQUINA+QString::number(k));
        for (int a = 0 ; a < cantEstados ; ++a) {
            name = m_datosXml->getContenidoDiagrama(ESTADO+QString::number(a),
                                                    NOMBRE_ESTADO, MAQUINA+QString::number(k));
            pos_x = (m_datosXml->getContenidoDiagrama(ESTADO+QString::number(a),
                                                      POS_X_ESTADO, MAQUINA+QString::number(k))).toDouble();
            pos_y = (m_datosXml->getContenidoDiagrama(ESTADO+QString::number(a),
                                                      POS_Y_ESTADO, MAQUINA+QString::number(k))).toDouble();
            width = (m_datosXml->getContenidoDiagrama(ESTADO+QString::number(a),
                                                      ANCHO_ESTADO, MAQUINA+QString::number(k))).toInt();
            height = (m_datosXml->getContenidoDiagrama(ESTADO+QString::number(a),
                                                       ALTO_ESTADO, MAQUINA+QString::number(k))).toInt();
            color = QColor(m_datosXml->getContenidoDiagrama(ESTADO+QString::number(a),
                                                      COLOR_ESTADO, MAQUINA+QString::number(k)));
            DiagramItem *item = new DiagramItem(DiagramItem::EstadoSimple, m_itemMenu);
            scene->addItem(item);
            scene->addToList(item);
            item->setPos(QPointF(pos_x, pos_y));
            item->setSize(width, height);
            item->setName(name);
            item->setColor(color);
        }
        for (int i = 0 ; i < cantEstados ; ++i) {
            cantTransiciones = m_datosXml->cantidadTransiciones(ESTADO+QString::number(i), MAQUINA+QString::number(k));
            for (int j = 0; j < cantTransiciones ; ++j) {
                int idProx = m_datosXml->getContenidoTransicion(ESTADO+QString::number(i),
                                                                ID_TRANS+QString::number(j), ID_PROX_ESTADO,
                                                                MAQUINA+QString::number(k)).right(1).toInt();
                QString evento = m_datosXml->getContenidoTransicion(ESTADO+QString::number(i),
                                                                    ID_TRANS+QString::number(j), ID_EVENTO,
                                                                    MAQUINA+QString::number(k));
                QStringList acciones = (m_datosXml->getContenidoTransicion(ESTADO+QString::number(i),
                                                                           ID_TRANS+QString::number(j), ID_ACCIONES,
                                                                           MAQUINA+QString::number(k))).split(",");
                QStringList mid_pos_xy = (m_datosXml->getContenidoTransicion(ESTADO+QString::number(i),
                                                                             ID_TRANS+QString::number(j), MID_POS,
                                                                             MAQUINA+QString::number(k))).split(",");
                QStringList text_pos_xy = (m_datosXml->getContenidoTransicion(ESTADO+QString::number(i),
                                                                              ID_TRANS+QString::number(j), TEXT_POS,
                                                                              MAQUINA+QString::number(k))).split(",");
                DiagramItem *startItem = scene->itemList().at(i);
                DiagramItem *endItem = scene->itemList().at(idProx);
                DiagramTransition *arrow = new DiagramTransition(startItem, endItem, m_itemMenu);
                arrow->setColor(QColor(Qt::black));
                arrow->setText(evento, acciones);
                arrow->midItem()->setPos(QPointF(mid_pos_xy.at(0).toDouble(), mid_pos_xy.at(1).toDouble()));
                startItem->addTransition(arrow);
                // para no agregar dos veces las auto-transiciones
                if(endItem != startItem) {
                    endItem->addTransition(arrow);
                }
                scene->addItem(arrow);
                arrow->setTextRelativePos(QPointF(text_pos_xy.at(0).toDouble(), text_pos_xy.at(1).toDouble()));
                arrow->updatePosition();
            }
        }
        int idxInicial = indexOfEstado(m_datosXml->getContenidoMaquina(MAQUINA+QString::number(k), NOMBRE_MAQUINA),
                                       m_datosXml->getEstadoInicial(MAQUINA+QString::number(k)));
        if(idxInicial != -1) {
            scene->itemList().at(idxInicial)->setAsEstadoInicial(true);
            double pos_x = (m_datosXml->getContenidoInicial(POS_X_ESTADO, MAQUINA+QString::number(k))).toDouble();
            double pos_y = (m_datosXml->getContenidoInicial(POS_Y_ESTADO, MAQUINA+QString::number(k))).toDouble();
            QStringList acciones = (m_datosXml->getContenidoInicial(ID_ACCIONES, MAQUINA+QString::number(k))).split(",");
            QStringList mid_pos_xy = (m_datosXml->getContenidoInicial(MID_POS, MAQUINA+QString::number(k))).split(",");
            QStringList text_pos_xy = (m_datosXml->getContenidoInicial(TEXT_POS, MAQUINA+QString::number(k))).split(",");
            DiagramItem *resetItem = new DiagramItem(DiagramItem::EstadoReset, m_itemMenu);
            scene->addItem(resetItem);
            scene->setResetItem(resetItem);
            // chequeo para compatibilidad con versiones anteriores            
            if(qFuzzyCompare(pos_x, -1.0) && qFuzzyCompare(pos_y, -1.0) && mid_pos_xy.at(0) == "-1" && text_pos_xy.at(0) == "-1") {
                DiagramItem *endItem = scene->itemList().at(idxInicial);
                scene->resetItem()->setPos(endItem->pos()-QPointF((endItem->rect().width()/2)+40,(endItem->rect().height()/2)+40));
                DiagramTransition *arrow = new DiagramTransition(resetItem, endItem, m_itemMenu);
                acciones.clear();
                acciones << "";
                arrow->setText("-", acciones);
                scene->resetItem()->addTransition(arrow);
                endItem->addTransition(arrow);
                scene->addItem(arrow);
            } else {
                scene->resetItem()->setPos(QPointF(pos_x, pos_y));
                DiagramItem *endItem = scene->itemList().at(idxInicial);
                DiagramTransition *arrow = new DiagramTransition(resetItem, endItem, m_itemMenu);
                arrow->setText("-", acciones);
                arrow->midItem()->setPos(QPointF(mid_pos_xy.at(0).toDouble(), mid_pos_xy.at(1).toDouble()));
                scene->resetItem()->addTransition(arrow);
                endItem->addTransition(arrow);
                scene->addItem(arrow);
                arrow->setTextRelativePos(QPointF(text_pos_xy.at(0).toDouble(), text_pos_xy.at(1).toDouble()));
                arrow->updatePosition();
            }
        } else {
            scene->setResetItem(nullptr);
        }
        updateTablaTransiciones(scene);
    }

    // Verifico que las constantes tengan definido un valor en el .umf (compatibilidad con versiones viejas)
    int cantConstantes = m_datosXml->cantidadConstantes();
    QString contenido;
    for (int i = 0 ; i < cantConstantes ; ++i) {
        contenido = m_datosXml->getContenidoConstante(CONSTANTE+QString::number(i));
        if(!contenido.contains(",")) {
            m_datosXml->editarConstante(i, contenido+","+"X");
        }
    }

    // Si ya existen máquinas definidas, habilito la ui por completo
    if(cantMaquinas > 0)
        uiEnable();

    // Actualizo el explorador de proyecto y la mainwindow
    updateExploradorProyecto();
    this->setWindowTitle("uModel Factory | UTN-FRBA - " + QFileInfo(fileName).baseName());
}

void MainWindow::on_actionGuardar_triggered()
{
    guardarDatos();
}

void MainWindow::on_actionGuardarComo_triggered()
{
    // Crea y muestra el QDialog para Guardar Como...
    QFileDialog dialogGuardar(this);

    dialogGuardar.setFileMode(QFileDialog::AnyFile);
    dialogGuardar.setAcceptMode(QFileDialog::AcceptSave);
    dialogGuardar.setDirectory(QDir(m_datosXml->getContenidoGeneral(CARPETA)));
    dialogGuardar.setNameFilter("proyecto uMF (*.umf)");

    if (dialogGuardar.exec() == QFileDialog::Accepted) {
        // Recupera los datos del QDialog
        QFileInfo infoGuardarComo(dialogGuardar.selectedFiles().first());
        // Actualiza los datos de la estructura XML
        m_datosXml->setContenidoGeneral(NOMBRE, infoGuardarComo.baseName());
        m_datosXml->setContenidoGeneral(CARPETA, infoGuardarComo.absolutePath());
        m_datosXml->setContenidoGeneral(CREACION,
                                         QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"));
        m_datosXml->setContenidoGeneral(MODIFICACION,
                                         QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"));
        guardarDatos();
        m_modificado = false;
        this->setWindowTitle("uModel Factory | UTN-FRBA - " + infoGuardarComo.baseName() + ".umf");
        updateExploradorProyecto(Proyecto);
    }
}

void MainWindow::on_actionCerrarProyecto_triggered()
{
    int rv = maybeSave();   // Chequeo si hay que guardar cambios

    if(rv == QMessageBox::Cancel)
        return;
    else if (rv == QMessageBox::Save)
        guardarDatos();

    delete m_datosXml;
    m_datosXml = nullptr;
    uiReset();
}

void MainWindow::on_actionAcercaDe_triggered()
{
    QMessageBox::about(this, "Acerca de uModel Factory | "
                             "UTN FRBA", "Grupo de Investigación: "
                             "Marcelo Giura, Marcelo Trujillo, "
                             "Juan Manuel Cruz, Mariana Prieto, "
                             "Nahuel Gonzalez, Pablo D'Angelo, "
                             "Yao Ming Kuo, Lisandro Sugezky. \n\n"
                             "Contacto: umodelfactory@frba.utn.edu.ar\n"
                             "YouTube (próximamente más videos)\n"
                             "https://youtu.be/rb5pkW8ZEAs");
}

void MainWindow::on_actionNuevaMaquina_triggered()
{
    // genero una nueva DiagramScene, en una nueva SubWindow
    QString name = getNameNuevaMaquina();
    if (!name.isEmpty()) {
        initSubWindow(name);
        // habilito actions por completo
        uiEnable();
        m_modificado = true;
        updateExploradorProyecto(Proyecto);
    }
}

void MainWindow::on_actionEliminarMaquina_triggered()
{
    // Obtengo la máquina activa y genero un Dialog para confirmar antes de eliminar
    SubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->activeSubWindow());
    if(subWindow != nullptr) {
        QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
        DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
        QMessageBox msgBox;
        QPushButton *si = msgBox.addButton(tr("Si"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("No"), QMessageBox::RejectRole);
        msgBox.setText("¿Está seguro que desea eliminar la Máquina de Estados completa?");
        msgBox.setWindowTitle(tr("Eliminar Máquina: ")+scene->name());
        msgBox.setWindowIcon(QIcon(":/img/img/application_delete.png"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        if (msgBox.clickedButton() == si) {
            subWindow->close();
            scene->clear();
            m_sceneList.removeOne(scene);
            delete scene;
            ui->mdiArea->removeSubWindow(subWindow);
            delete subWindow;

            // Actualizo el explorador de proyecto
            updateExploradorProyecto(TreeItemType::Sistema);
            // Si se eliminó la última máquina, deshabilito controles ui
            if(m_sceneList.isEmpty()) {
                ui->actionEliminarMaquina->setDisabled(true);
                ui->actionRenombrarMaquina->setDisabled(true);
                ui->actionSeleccionar->setDisabled(true);
                ui->actionNuevaTransicion->setDisabled(true);
                ui->actionNuevoEstado->setDisabled(true);
                ui->actionEstadoInicial->setDisabled(true);
                ui->actionEliminarElemento->setDisabled(true);
                ui->actionGenerarCodigo->setDisabled(true);
                ui->actionGenerarDocumentacion->setDisabled(true);
                ui->actionSimular->setDisabled(true);
                ui->actionTerminarSimulacion->setDisabled(true);
                ui->actionVerificar->setDisabled(true);
                ui->tableWidgetTransiciones->clearContents();
                ui->tableWidgetTransiciones->setRowCount(0);
                ui->dockWidgetTransiciones->setWindowTitle("Tabla de Estados y Transiciones");
            }
            m_modificado = true;
            updateUi();
        }
    }
}

void MainWindow::on_actionRenombrarMaquina_triggered()
{
    bool ok = false;
    bool valido = false;
    QRegExp regExp("[A-Za-z0-9-_ñÑ ]+");
    QIcon icon(QIcon(":/img/img/application edit.png"));
    QString name;

    // Obtengo la máquina activa y genero un Dialog para renombrar
    SubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->activeSubWindow());
    DiagramScene *scene = static_cast<DiagramScene *>(static_cast<QGraphicsView *>(subWindow->widget())->scene());
    QString prev_name = scene->name();
    while (!valido) {
        name = RegExpInputDialog::getText(this, tr("Cambiar Nombre de Máquina"),
                                                  tr("Nombre:"),
                                                  prev_name,
                                                  regExp, &ok, icon, Qt::Dialog);
        name.replace(" ", "_");
        if (indexOfMaquina(name) != -1) {
            QMessageBox(QMessageBox::Icon::Warning, tr("Error!"),
                        tr("Ya existe otra máquina con el mismo nombre"),
                        QMessageBox::StandardButton::Ok, this).exec();
            valido = false;
        } else {
            valido = true;
        }
    }
    if (ok) {
        // Actualizo el nombre de la máquina + ui
        m_sceneList.at(indexOfMaquina(prev_name))->setName(name);
        foreach (QMdiSubWindow *subWindow, ui->mdiArea->subWindowList()) {
            QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
            DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
            if (scene->name() == name) {
                subWindow->setWindowTitle(name);
            }
        }
        updateExploradorProyecto(TreeItemType::Sistema);
        updateTablaTransiciones(m_sceneList.at(indexOfMaquina(name)));
        updateUi();
        m_modificado = true;
    }
}

void MainWindow::on_actionSeleccionar_triggered()
{
    // Paso al modo SELECCIONAR en todas las subwindows
    foreach (QMdiSubWindow *subWindow, ui->mdiArea->subWindowList()) {
        QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
        DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
        scene->setMode(DiagramScene::MoveItem);
        view->viewport()->setCursor(Qt::ArrowCursor);
        view->viewport()->repaint();
        view->setDragMode(QGraphicsView::RubberBandDrag);
    }
    if(ui->actionSeleccionar->isChecked() == false)
        ui->actionSeleccionar->setChecked(true);
}

void MainWindow::on_actionNuevoEstado_triggered()
{
    // Paso al modo NUEVO ESTADO en todas las subwindows
    foreach (QMdiSubWindow *subWindow, ui->mdiArea->subWindowList()) {
        QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
        DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
        scene->setItemType(DiagramItem::EstadoSimple);
        scene->setMode(DiagramScene::InsertItem);
        scene->clearSelection();
        view->viewport()->setCursor(Qt::CrossCursor);
        view->setDragMode(QGraphicsView::NoDrag);
    }
}

void MainWindow::on_actionEstadoInicial_triggered()
{
    QMdiSubWindow *subWindow = (ui->mdiArea->activeSubWindow());
    if (subWindow != nullptr) {
        QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
        DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
        int cant = scene->itemList().count();
        QStringList items;

        for (int k=0; k < cant; ++k)
            items.append(scene->itemList().at(k)->name().toUpper());

        if(!items.isEmpty()) {
            QString item;
            QInputDialog *dialogEstadoInicial = new QInputDialog(this, Qt::Dialog);

            dialogEstadoInicial->setOkButtonText("Aceptar");
            dialogEstadoInicial->setCancelButtonText("Cancelar");
            dialogEstadoInicial->setFont(QFont("MS Shell Dlg 2",9));
            dialogEstadoInicial->setOption(QInputDialog::UseListViewForComboBoxItems);
            dialogEstadoInicial->setWindowTitle(scene->name());
            dialogEstadoInicial->setLabelText("Seleccionar el Estado Inicial:");
            dialogEstadoInicial->setComboBoxItems(items);

            if (dialogEstadoInicial->exec() == QDialog::Accepted) {
                item = dialogEstadoInicial->textValue();
                for (int j=0; j < cant; j++) {
                    if (item == scene->itemList().at(j)->name().toUpper()) {
                        for (int k=0; k < cant; ++k)
                            scene->itemList().at(k)->setAsEstadoInicial(false);
                        scene->itemList().at(j)->setAsEstadoInicial(true);
                        scene->insertInitialStateItem(j);
                        break;
                    }
                }
                updateTablaTransiciones(scene);
                updateExploradorProyecto(TreeItemType::Sistema);
                m_modificado = true;
                updateUi();
           }
        } else {
            QMessageBox::warning(this, QLatin1String("Advertencia: ")+scene->name(),
                                 QString::fromUtf8("No hay ningún estado definido para seleccionar como inicial."));
        }
    }
    // Vuelvo al modo Seleccionar
    ui->actionSeleccionar->trigger();
}

void MainWindow::on_actionNuevaTransicion_triggered()
{
    // Paso al modo NUEVA TRANSICIÓN solo en la subwindow activa
    QMdiSubWindow *subWindow = (ui->mdiArea->activeSubWindow());
    if (subWindow != nullptr) {
        QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
        DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
        scene->setMode(DiagramScene::InsertLineStart);
        view->viewport()->setCursor(Qt::CrossCursor);
        view->setDragMode(QGraphicsView::NoDrag);
    }
}

void MainWindow::on_actionEliminarElemento_triggered()
{
    // Obtengo la subwindow actual y genero un Dialog para confirmar antes de eliminar
    SubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->activeSubWindow());
    if(subWindow != nullptr) {
        QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
        DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
        if(!scene->selectedItems().isEmpty()) {
            QMessageBox msgBox;
            QPushButton *si = msgBox.addButton(tr("Si"), QMessageBox::AcceptRole);
            msgBox.addButton(tr("No"), QMessageBox::RejectRole);
            msgBox.setText("¿Está seguro que desea eliminar el Elemento del Diagrama?");
            msgBox.setWindowIcon(QIcon(":/img/img/cross.png"));
            msgBox.setWindowTitle(tr("Borrar Elemento"));
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
            if (msgBox.clickedButton() == si) {
                scene->removeSelectedItems();
                updateTablaTransiciones(scene);
                updateExploradorProyecto();
                m_modificado = true;
                updateUi();
            }
        }
    }
}

void MainWindow::on_actionNuevoEvento_triggered()
{
    QString name;
    QRegExp regExp("[A-Za-z_][A-Za-z0-9-_]+");
    QIcon icon(QIcon(":/img/img/events.pn"));
    bool ok = false;
    bool valido = false;

    while (!valido) {
        name = RegExpInputDialog::getText(this, tr("Nuevo Evento"),
                                          tr("Definir el nombre:"),"",
                                          regExp, &ok, icon, Qt::Dialog);
        if (m_datosXml->getNombresEventosDefinidos().contains(name, Qt::CaseSensitive)) {
            QMessageBox::information(this, tr("Definir Evento"),
                                     tr("Ya existe un evento definido con el mismo nombre."));
            valido=false;
        } else {
            valido = true;
        }
    }
    if (ok && !name.isEmpty()) {
        m_datosXml->addNuevoEvento(name);
        m_modificado = true;
        updateUi();
        updateExploradorProyecto(TreeItemType::Eventos);
    }
}

void MainWindow::on_actionNuevaAccion_triggered()
{
    QString name;
    QRegExp regExp("[A-Za-z_][A-Za-z0-9-_]+");
    QIcon icon(QIcon(":/img/img/external.png"));
    bool ok = false;
    bool valido = false;

    while (!valido) {
        name = RegExpInputDialog::getText(this, tr("Nueva Acción"),
                                          tr("Definir el nombre:"), "",
                                          regExp, &ok, icon, Qt::Dialog).append("()");
        if (m_datosXml->getNombresAccionesDefinidas().contains(name, Qt::CaseSensitive)) {
            QMessageBox::information(this, tr("Definir Acción"),
                                     tr("Ya existe una acción definida con el mismo nombre."));
            valido=false;
        } else {
            valido = true;
        }
    }
    if (ok && name != "()") {
        m_datosXml->addNuevaAccion(name);
        m_modificado = true;
        updateUi();
        updateExploradorProyecto(TreeItemType::Acciones);
    }
}

void MainWindow::on_actionNuevaVariable_triggered()
{
    QString name;
    QRegExp regExp("[A-Za-z_][A-Za-z0-9-_]+");
    QIcon icon(QIcon(":/img/img/brick.png"));
    bool ok = false;
    bool valido = false;

    while (!valido) {
        name = RegExpInputDialog::getText(this, tr("Nueva Variable"),
                                          tr("Definir el nombre:"), "",
                                          regExp, &ok, icon, Qt::Dialog);

        if (m_datosXml->getNombresVariablesDefinidas().contains(name, Qt::CaseSensitive)) {
            QMessageBox::information(this, tr("Definir Variable"),
                                     tr("Ya existe una variable definida con el mismo nombre."));
            valido=false;
        } else if (m_datosXml->getNombresConstantesDefinidas().contains(name, Qt::CaseSensitive)) {
            QMessageBox::information(this, tr("Definir Variable"),
                                     tr("Ya existe una constante definida con el mismo nombre."));
            valido=false;
        } else {
            valido = true;
        }
    }
    if (ok && !name.isEmpty()) {
        m_datosXml->addNuevaVariable(name);
        m_modificado = true;
        updateUi();
        updateExploradorProyecto(TreeItemType::Variables);
    }
}

void MainWindow::on_actionNuevaConstante_triggered()
{  
    QString nombreConstante;
    QString valorConstante;
    bool ok = false;

    DialogConstantes m_dialogConstantes;
    m_dialogConstantes.setWindowTitle("Nueva Constante");
    while (!ok) {
        if (m_dialogConstantes.exec() == QDialog::Accepted) {
            nombreConstante = m_dialogConstantes.getNombre();
            if (m_datosXml->getNombresConstantesDefinidas().contains(nombreConstante, Qt::CaseSensitive)) {
                QMessageBox::information(this, tr("Definir Constante"),
                                         tr("Ya existe una constante definida con el mismo nombre."));
            } else if (m_datosXml->getNombresVariablesDefinidas().contains(nombreConstante, Qt::CaseSensitive)) {
                QMessageBox::information(this, tr("Definir Constante"),
                                         tr("Ya existe una variable definida con el mismo nombre."));;
            } else {
                ok = true;
                valorConstante = m_dialogConstantes.getValor();
                m_datosXml->addNuevaConstante(nombreConstante+","+valorConstante);
                updateUi();
                updateExploradorProyecto(TreeItemType::Constantes);
            }
        } else {
            break;
        }
    }
}

void MainWindow::on_actionBorrarEvento_triggered()
{
    QString eventoActual;
    QStringList triggers;

    if(m_selectedItemExplorador != nullptr)
        if(m_selectedItemExplorador->type() == ItemEvento) {
            eventoActual = m_selectedItemExplorador->text(0);
            // Chequear si el evento a borrar está siendo utilizado (ADVERTENCIA!)
            QRegularExpression re("\\b"+eventoActual+"\\b");
            int cantMaquinas = m_sceneList.count();
            for (int k = 0; k < cantMaquinas; ++k) {
                int cantItems = m_sceneList.at(k)->itemList().count();
                for (int i = 0; i < cantItems; ++i) {
                    DiagramItem *item = m_sceneList.at(k)->itemList().at(i);
                    foreach (DiagramTransition *arrow, item->transitions()) {
                        triggers = arrow->event().split(QRegExp("\\&\\&|\\|\\|"), Qt::SkipEmptyParts);
                        foreach (QString str, triggers) {
                            if (str.contains(re) && !str.contains(QRegExp("\\=|\\!\\=|\\<|\\>"))) {
                                // si está siendo utilizado -> ADVERTENCIA!
                                QMessageBox::warning(this, "Borrar Evento",
                                                     "El evento forma parte de otra transición ya definida. Primero debe remover la transición.");
                                return;
                            }
                        }
                    }
                }
            }
            // Si llegué acá, elimino el evento seleccionado
            m_datosXml->borrarEvento(m_selectedItemExplorador->parent()->indexOfChild(m_selectedItemExplorador));
            updateUi();
            m_modificado = true;
            updateExploradorProyecto(Eventos);
        }
}

void MainWindow::on_actionBorrarAccion_triggered()
{
    QString accionActual;

    if(m_selectedItemExplorador != nullptr)
        if(m_selectedItemExplorador->type() == ItemAccion) {
            accionActual = m_selectedItemExplorador->text(0);
            // Chequear si la acción a borrar está siendo utilizada (ADVERTENCIA)
            int cantMaquinas = m_sceneList.count();
            for (int k = 0; k < cantMaquinas; ++k) {
                int cantItems = m_sceneList.at(k)->itemList().count();
                for (int i = 0; i < cantItems; ++i) {
                    DiagramItem *item = m_sceneList.at(k)->itemList().at(i);
                    foreach (DiagramTransition *arrow, item->transitions()) {
                        if(arrow->actions().contains(accionActual)) {
                            // si está siendo utilizada -> ADVERTENCIA!
                            QMessageBox::warning(this, tr("Borrar Acción"),
                                                     tr("La acción forma parte de una transición ya definida.\nPrimero debe remover la transición."));
                            return;
                        }
                    }
                }
            }
            // Si llegué acá, elimino la acción seleccionada
            m_datosXml->borrarAccion(m_selectedItemExplorador->parent()->indexOfChild(m_selectedItemExplorador));
            updateUi();
            m_modificado = true;
            updateExploradorProyecto(Acciones);
        }
}

void MainWindow::on_actionBorrarVariable_triggered()
{
    bool flagBorrar = true;
    QString variableActual;
    QStringList triggers;

    if(m_selectedItemExplorador != nullptr)
        if(m_selectedItemExplorador->type() == ItemVariable) {
            variableActual = m_selectedItemExplorador->text(0);
            // Chequear si la variable a borrar está siendo utilizada (ADVERTENCIA)
            QRegularExpression re("\\b"+variableActual+"\\b");
            int cantMaquinas = m_sceneList.count();
            for (int k = 0; k < cantMaquinas; ++k) {
                int cantItems = m_sceneList.at(k)->itemList().count();
                for (int i = 0; i < cantItems; ++i) {
                    DiagramItem *item = m_sceneList.at(k)->itemList().at(i);
                    foreach (DiagramTransition *arrow, item->transitions()) {
                        // Disparador de la transición
                        triggers = arrow->event().split(QRegExp("\\&\\&|\\|\\|"), Qt::SkipEmptyParts);
                        foreach (QString str, triggers) {
                            if (str.contains(re) && str.contains(QRegExp("\\=|\\!\\=|\\<|\\>"))) {
                                flagBorrar = false;
                            }
                        }
                        // Acciones de la transición
                        foreach (QString str, arrow->actions()) {
                            if (str.contains(re) && str.contains(QRegExp("\\="))) {
                                flagBorrar = false;
                            }
                        }
                    }
                }
            }
            // si está siendo utilizada -> ADVERTENCIA!
            if (flagBorrar == false) {
                QMessageBox::warning(this, "Borrar Variable",
                                     "La variable forma parte de una transición ya definida. "
                                     "Primero debe remover la transición.");
                return;
            }
            // Si llegué acá, elimino la variable seleccionada
            m_datosXml->borrarVariable(m_selectedItemExplorador->parent()->indexOfChild(m_selectedItemExplorador));
            updateUi();
            m_modificado = true;
            updateExploradorProyecto(Variables);
        }
}

void MainWindow::on_actionBorrarConstante_triggered()
{
    bool flagBorrar = true;
    QString constActual;
    QStringList triggers;

    if(m_selectedItemExplorador != nullptr)
        if(m_selectedItemExplorador->type() == ItemConstante) {
            // Obtengo el nombre de la constante seleccionada
            constActual = m_selectedItemExplorador->text(0).split(" ").first();
            // Chequear si la constante a borrar está siendo utilizada (ADVERTENCIA)
            QRegularExpression re("\\b"+constActual+"\\b");
            int cantMaquinas = m_sceneList.count();
            for (int k = 0; k < cantMaquinas; ++k) {
                int cantItems = m_sceneList.at(k)->itemList().count();
                for (int i = 0; i < cantItems; ++i) {
                    DiagramItem *item = m_sceneList.at(k)->itemList().at(i);
                    foreach (DiagramTransition *arrow, item->transitions()) {
                        // Disparador de la transición
                        triggers = arrow->event().split(QRegExp("\\&\\&|\\|\\|"), Qt::SkipEmptyParts);
                        foreach (QString str, triggers) {
                            if (str.contains(re) && str.contains(QRegExp("\\=|\\!\\=|\\<|\\>"))) {
                                flagBorrar = false;
                            }
                        }
                        // Acciones de la transición
                        foreach (QString str, arrow->actions()) {
                            if (str.contains(re) && str.contains(QRegExp("\\="))) {
                                flagBorrar = false;
                            }
                        }
                    }
                }
            }
            // si está siendo utilizada -> ADVERTENCIA!
            if (flagBorrar == false) {
                QMessageBox::warning(this, "Borrar Constante",
                                     "La constante forma parte de una transición ya definida. "
                                     "Primero debe remover la transición.");
                return;
            }
            // Si llegué acá, elimino la constante seleccionada
            m_datosXml->borrarConstante(m_selectedItemExplorador->parent()->indexOfChild(m_selectedItemExplorador));
            updateUi();
            m_modificado = true;
            updateExploradorProyecto(Constantes);
        }
}

void MainWindow::on_actionEditarConstante_triggered()
{
    QStringList auxDatosConstante;
    QString valorConstante;
    int indexConstante;

    if(m_selectedItemExplorador != nullptr)
        if(m_selectedItemExplorador->type() == ItemConstante) {
            // Obtengo el index de la constante seleccionada
            indexConstante = m_selectedItemExplorador->parent()->indexOfChild(m_selectedItemExplorador);
            auxDatosConstante = m_datosXml->getContenidoConstante(CONSTANTE+QString::number(indexConstante)).split(",");
            // Cargo los datos actuales en el Dialog para editar (por ahora sólo puedo modificar el valor)
            DialogConstantes m_dialogConstantes;
            m_dialogConstantes.setWindowTitle("Editar Constante");
            m_dialogConstantes.setDatos(auxDatosConstante.first(), auxDatosConstante.last());
            if (m_dialogConstantes.exec() == QDialog::Accepted) {
                valorConstante = m_dialogConstantes.getValor();
                m_datosXml->editarConstante(indexConstante, auxDatosConstante.first()+","+valorConstante);
                updateUi();
                updateExploradorProyecto(TreeItemType::Constantes);
            }
    }
}

void MainWindow::on_mdiArea_subWindowActivated(QMdiSubWindow *activeSubWindow)
{
    SubWindow *subWindow = qobject_cast<SubWindow *>(activeSubWindow);
    if (subWindow != nullptr) {
        // Actualizo el combobox del zoom, según la subwindow actual
        int currentScale = static_cast<int> (subWindow->currentScale() * 100.0);
        QString scale = QString::number(currentScale) + "%";
        m_sceneScaleCombo->setCurrentIndex(m_sceneScaleCombo->findText(scale));

        // Actualizo el item actual en el Explorador de Proyecto
        QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
        if(view != nullptr) {
            DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
            if(scene != nullptr)
                updateTablaTransiciones(scene);
        }

        // Actualizo las herramientas
        if(ui->actionNuevaTransicion->isChecked()) {
            ui->actionSeleccionar->trigger();
        }
    }
}

void MainWindow::on_treeWidgetProyecto_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    QString name;
    QTreeWidgetItem *parentItem = item->parent();

    if(parentItem != nullptr) {
        if(parentItem->text(0) == "SISTEMA") {
            name = item->text(0);
            // Activo/muestro las subwindows segun el item seleccionado
            if(!name.isEmpty()) {
                foreach (QMdiSubWindow *subWindow, ui->mdiArea->subWindowList()) {
                    QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
                    DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
                    if (scene->name() == name) {
                        if (subWindow->isVisible()) {
                            ui->mdiArea->setActiveSubWindow(subWindow);
                        } else {
                            subWindow->show();
                            ui->mdiArea->tileSubWindows();
                            ui->mdiArea->setActiveSubWindow(subWindow);
                        }
                        break;
                    }
                }
            }
        }
    }
}


void MainWindow::on_treeWidgetProyecto_customContextMenuRequested(const QPoint &pos)
{
    // obtengo el item seleccionado
    m_selectedItemExplorador = ui->treeWidgetProyecto->itemAt(pos);
    // lo uso para determinar el indice del comentario que corresponde
    QTreeWidgetItem *parentItem = m_selectedItemExplorador->parent();
    // reseteo el menú contextual
    m_contextMenuExplorador->clear();
    if(m_selectedItemExplorador != nullptr) {
        // Muestro las distintas opciones en función del elemento
        switch (m_selectedItemExplorador->type()) {
        case Proyecto:
            break;
        case Sistema:
            m_contextMenuExplorador->addAction(ui->actionNuevaMaquina);
            m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        case Eventos:
            m_contextMenuExplorador->addAction(ui->actionNuevoEvento);
            m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        case Acciones:
            m_contextMenuExplorador->addAction(ui->actionNuevaAccion);
            m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        case Variables:
            m_contextMenuExplorador->addAction(ui->actionNuevaVariable);
            m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        case Constantes:
            m_contextMenuExplorador->addAction(ui->actionNuevaConstante);
            m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        case Maquina:
            // Primero activo la subwindow de la máquina seleccionada
            foreach (QMdiSubWindow *subWindow, ui->mdiArea->subWindowList()) {
                QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
                DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
                if (scene->name() == m_selectedItemExplorador->text(0)) {
                    ui->mdiArea->setActiveSubWindow(subWindow);
                    }
                    break;
            }
            // Genero y muestro el menú
            m_contextMenuExplorador->addAction(ui->actionNuevaMaquina);
            if(m_datosXml->cantidadMaquinas() > 0) {
                m_contextMenuExplorador->addAction(ui->actionRenombrarMaquina);
                m_contextMenuExplorador->addAction(ui->actionEliminarMaquina);
            }
            m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        case ItemEstado:
            if (m_datosXml->cantidadEstados() > 0) {
                commentElementId = ESTADO + QString::number(parentItem->indexOfChild(m_selectedItemExplorador));
	            if (m_datosXml->getComentario(commentElementId) == "") {
	                m_actionAgregarComentario->setText(ACTION_ADD_COMENTARIO);
	                m_actionAgregarComentario->setIcon(QIcon(":/img/img/add.png"));
	                m_contextMenuExplorador->addAction(m_actionAgregarComentario);
	            } else {
	                m_actionAgregarComentario->setText(ACTION_EDIT_COMENTARIO);
	                m_actionAgregarComentario->setIcon(QIcon(":/img/img/cog.png"));
	                m_contextMenuExplorador->addAction(m_actionAgregarComentario);
	                m_contextMenuExplorador->addAction(ui->actionEliminar_comentario);
	         	}
	        }
	        m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        case ItemEvento:
            m_contextMenuExplorador->addAction(ui->actionNuevoEvento);
            if(m_datosXml->cantidadEventos() > 0) {
                m_contextMenuExplorador->addAction(ui->actionBorrarEvento);
                commentElementId = EVENTO + QString::number(parentItem->indexOfChild(m_selectedItemExplorador));
	            if (m_datosXml->getComentario(commentElementId) == "") {
	                m_actionAgregarComentario->setText(ACTION_ADD_COMENTARIO);
	                m_actionAgregarComentario->setIcon(QIcon(":/img/img/add.png"));
	                m_contextMenuExplorador->addAction(m_actionAgregarComentario);
	            } else {
	                m_actionAgregarComentario->setText(ACTION_EDIT_COMENTARIO);
	                m_actionAgregarComentario->setIcon(QIcon(":/img/img/cog.png"));
	                m_contextMenuExplorador->addAction(m_actionAgregarComentario);
	                m_contextMenuExplorador->addAction(ui->actionEliminar_comentario);
	            }
	        }
            m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        case ItemAccion:
            m_contextMenuExplorador->addAction(ui->actionNuevaAccion);
            if(m_datosXml->cantidadAcciones() > 0) {
                m_contextMenuExplorador->addAction(ui->actionBorrarAccion);
                commentElementId = ACCION + QString::number(parentItem->indexOfChild(m_selectedItemExplorador));
	            if (m_datosXml->getComentario(commentElementId) == "") {
	                m_actionAgregarComentario->setText(ACTION_ADD_COMENTARIO);
	                m_actionAgregarComentario->setIcon(QIcon(":/img/img/add.png"));
	                m_contextMenuExplorador->addAction(m_actionAgregarComentario);
	            } else {
	                m_actionAgregarComentario->setText(ACTION_EDIT_COMENTARIO);
	                m_actionAgregarComentario->setIcon(QIcon(":/img/img/cog.png"));
	                m_contextMenuExplorador->addAction(m_actionAgregarComentario);
	                m_contextMenuExplorador->addAction(ui->actionEliminar_comentario);
	            }
	        }
            m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        case ItemVariable:
            m_contextMenuExplorador->addAction(ui->actionNuevaVariable);
            if(m_datosXml->cantidadVariables() > 0) {
                m_contextMenuExplorador->addAction(ui->actionBorrarVariable);
                commentElementId = VARIABLE + QString::number(parentItem->indexOfChild(m_selectedItemExplorador));
	            if (m_datosXml->getComentario(commentElementId) == "") {
	                m_actionAgregarComentario->setText(ACTION_ADD_COMENTARIO);
	                m_actionAgregarComentario->setIcon(QIcon(":/img/img/add.png"));
	                m_contextMenuExplorador->addAction(m_actionAgregarComentario);
	            } else {
	                m_actionAgregarComentario->setText(ACTION_EDIT_COMENTARIO);
	                m_actionAgregarComentario->setIcon(QIcon(":/img/img/cog.png"));
	                m_contextMenuExplorador->addAction(m_actionAgregarComentario);
	                m_contextMenuExplorador->addAction(ui->actionEliminar_comentario);
	            }
	        }
            m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        case ItemConstante:
            m_contextMenuExplorador->addAction(ui->actionNuevaConstante);
            if(m_datosXml->cantidadConstantes() > 0) {
                m_contextMenuExplorador->addAction(ui->actionEditarConstante);
                m_contextMenuExplorador->addAction(ui->actionBorrarConstante);
                commentElementId = CONSTANTE + QString::number(parentItem->indexOfChild(m_selectedItemExplorador));
	            if (m_datosXml->getComentario(commentElementId) == "") {
	                m_actionAgregarComentario->setText(ACTION_ADD_COMENTARIO);
	                m_actionAgregarComentario->setIcon(QIcon(":/img/img/add.png"));
	                m_contextMenuExplorador->addAction(m_actionAgregarComentario);
	            } else {
	                m_actionAgregarComentario->setText(ACTION_EDIT_COMENTARIO);
	                m_actionAgregarComentario->setIcon(QIcon(":/img/img/cog.png"));
	                m_contextMenuExplorador->addAction(m_actionAgregarComentario);
	                m_contextMenuExplorador->addAction(ui->actionEliminar_comentario);
	            }
	        }
            m_contextMenuExplorador->popup(ui->treeWidgetProyecto->mapToGlobal(pos));
            break;
        default:
            break;
        }
    }
}

void MainWindow::on_itemInserted(DiagramItem *item)
{
    // Genero el Dialog para definir el nombre del estado
    QString name = getNameNuevoEstado();
     if (!name.isEmpty()) {
         item->setName(name);
         m_modificado = true;
         updateExploradorProyecto(TreeItemType::Sistema);
         updateTablaTransiciones(static_cast<DiagramScene *>(sender()));
         updateUi();
     } else {
         static_cast<DiagramScene *>(sender())->removeSelectedItems();
     }
     // Vuelvo al modo Seleccionar (en todas las subwindows)
     ui->actionSeleccionar->trigger();
}
void MainWindow::on_arrowInserted(DiagramTransition *arrow)
{
    // Crea y muestra el QDialog para editar la transición
    SubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->activeSubWindow());
    QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
    DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
    DialogTransicion editorTransicion(this);
    editorTransicion.setDatos(arrow, m_datosXml, m_sceneList, scene, DialogTransicion::NuevaTransicion);
    if (editorTransicion.exec() == QFileDialog::Accepted) {
        arrow->setText(editorTransicion.selectedEvent(),
                       editorTransicion.selectedActions().split(","));
        arrow->alignText();
        m_modificado = true;
        updateUi();
    } else {
        if (arrow->startItem()->itemType() == DiagramItem::EstadoReset) {
            DiagramItem *init = arrow->startItem();
            delete init;
            scene->setResetItem(nullptr);
        } else {
            arrow->startItem()->removeTransition(arrow);
        }
        arrow->endItem()->removeTransition(arrow);
        delete arrow;
    }
    updateExploradorProyecto();
    updateTablaTransiciones(scene);
    // Fix cursor
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setDragMode(QGraphicsView::NoDrag);
    // Vuelvo al modo Seleccionar
    ui->actionSeleccionar->trigger();
}

void MainWindow::on_itemSelected(QGraphicsItem *item)
{
//    switch (item->type()) {
//        case DiagramItem::Type:
//            if(qgraphicsitem_cast<DiagramItem *>(item)->itemType() == DiagramItem::EstadoReset) {
//                ui->textEditAyuda->setHtml(ayudaDinamica.getTextoEstadoInicial());
//            } else {
//                ui->textEditAyuda->setHtml(ayudaDinamica.getTextoEstadoSimple());
//            }
//            break;

//    case DiagramTransition::Type:
//        ui->textEditAyuda->setHtml(ayudaDinamica.getTextoTransicion());
//        break;

//    default:
//            break;
//    }
}

void MainWindow::on_sceneSelectionChanged()
{

    bool EstadoSimpleSelected = false;
    bool EstadoResetSelected = false;
    bool TransicionSelected = false;

    SubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->activeSubWindow());
    QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
    DiagramScene *scene = static_cast<DiagramScene *>(view->scene());

    if(ayudaDinamica.nivelAyuda() != AyudaDinamica::DESACTIVADA) {
        ui->textEditAyuda->clear();
        foreach (QGraphicsItem* item, scene->selectedItems()) {
            switch (item->type()) {
                case DiagramItem::Type:
                    if(qgraphicsitem_cast<DiagramItem *>(item)->itemType() == DiagramItem::EstadoReset) {
                        if(!EstadoResetSelected) {
                            ui->textEditAyuda->insertHtml(ayudaDinamica.getTextoEstadoInicial());
                            EstadoResetSelected = true;
                        }
                    } else {
                        if(!EstadoSimpleSelected) {
                            ui->textEditAyuda->insertHtml(ayudaDinamica.getTextoEstadoSimple());
                            EstadoSimpleSelected = true;
                        }
                    }
                    break;

            case DiagramTransition::Type:
                if(!TransicionSelected) {
                    ui->textEditAyuda->insertHtml(ayudaDinamica.getTextoTransicion());
                    TransicionSelected = true;
                }
                break;

            default:
                    break;
            }
        }
    }
}

void MainWindow::on_itemDoubleClicked(int index)
{
    // Crea y muestra el QDialog para editar el estado
    SubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->activeSubWindow());
    QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
    DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
    DialogEditorEstado dialogEstado(this);
    dialogEstado.setDatos(index, m_datosXml, scene);
    dialogEstado.exec();

    if(dialogEstado.datosModificados() == true) {
        m_modificado = true;
        updateExploradorProyecto(TreeItemType::Sistema);
        updateTablaTransiciones(scene);
        updateUi();
    }
}

void MainWindow::on_arrowDoubleClicked(DiagramTransition *arrow)
{
    // Crea y muestra el QDialog para editar la transición
    SubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->activeSubWindow());
    QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
    DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
    DialogTransicion editorTransicion(this);
    editorTransicion.setDatos(arrow, m_datosXml, m_sceneList, scene, DialogTransicion::EditarTransicion);
    if (editorTransicion.exec() == QFileDialog::Accepted) {
        arrow->setText(editorTransicion.selectedEvent(),
                       editorTransicion.selectedActions().split(","));
        m_modificado = true;
        updateUi();
    }
    updateExploradorProyecto();
    updateTablaTransiciones(scene);
    // Fix cursor
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setDragMode(QGraphicsView::NoDrag);
    // Vuelvo al modo Seleccionar
    ui->actionSeleccionar->trigger();
}

void MainWindow::on_sceneScaleChanged(const QString &scale)
{
    SubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->activeSubWindow());
    if(subWindow != nullptr) {
        QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
        double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
        if (!qFuzzyCompare(newScale, subWindow->currentScale())) {
            subWindow->setScale(newScale);
            QTransform oldMatrix = view->transform();
            view->resetTransform();
            view->translate(oldMatrix.dx(), oldMatrix.dy());
            view->scale(newScale, newScale);
            view->update();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int rv = maybeSave();   // Chequeo si hay que guardar cambios

    if(rv == QMessageBox::Cancel) {
        event->ignore();
    } else {
        if (rv == QMessageBox::Save) {
            guardarDatos();
        }
        saveSettings();
        event->accept();
    }
}

void MainWindow::on_actionSimular_triggered()
{
    if(m_uiMode != MainWindow::Simular) {
         m_simular_con_Infotronic_virtual = false ;
        guardarDatos();
        // Chequeo si están definidos todos los estados iniciales
        int cantMaq = m_sceneList.count();
        for(int j=0; j < cantMaq; ++j) {
            if(m_datosXml->getEstadoInicial(MAQUINA+QString::number(j)) == "-1") {
                QMessageBox::warning(this, QLatin1String("Advertencia"), m_sceneList.at(j)->name()
                                     +QLatin1String(": No se encuentra definido el Estado Inicial"));
                ui->actionSimular->setChecked(false);
                return;
            }
        }

        //// Carga De Variables
        //// Para armar la trazabilidad de las variables
        m_Maquinas.clear();
        m_Elmementos.clear_elementos();
        leerUmf(m_Maquinas,m_Elmementos);

        m_expresiones.clear_variables();
        //entiendo que aca estas cargando las variables pero no veo donde está el valor!!!
        //inicio columnas
        InicializarTablaVariables();

        for ( int i = 0 ; m_Elmementos.nVariables.at(i) != "" ; i++)
        {
            struct variables_de_proceso vdp;
            vdp.texto_variable =  m_Elmementos.nVariables.value(i);
            vdp.valor_variable =  0;
            m_expresiones.setVariables(vdp);

            //completo con contenidos
            ui->tableWidgetVariables->insertRow( ui->tableWidgetVariables->rowCount( ) );
            ui->tableWidgetVariables->setItem(i,0,new QTableWidgetItem(vdp.texto_variable));
            ui->tableWidgetVariables->setItem(i,1,new QTableWidgetItem(QString::number(vdp.valor_variable)));
        }

        for ( int i = 0 ; m_Elmementos.nEventos.at(i) != "" ; i++)
        {
            struct pulsadores_de_proceso pulsador;
            if ( m_Elmementos.nEventos[i].contains("s_"))
            {
                    pulsador.texto_pulsadores =  m_Elmementos.nEventos.value(i);
                    pulsador.valor_pulsadores  =  0;
                    m_expresiones.setPulsadores(pulsador);
            }
        }

        //lo muestro
        MostrarTablaVariables();

        // Paso al modo Simulación...
        m_uiMode = MainWindow::Simular;
        // Arranco timer inicial para el reset
        m_timerUpdateSim.start(T_SIM_RESET_INIT);
        // Actualizo los controles de la ui para Simular
        if (ui->dockWidgetVerificar->isVisible()) {
            ui->dockWidgetVerificar->hide();
            m_resetDockVerificar = true;
        }
        ui->dockWidgetProyecto->hide();
        ui->dockWidgetSimulacion->show();
        ui->menuVer->removeAction(ui->dockWidgetProyecto->toggleViewAction());
        ui->menuVer->addAction(ui->dockWidgetSimulacion->toggleViewAction());
        ui->actionTerminarSimulacion->setEnabled(true);
        ui->actionNuevaMaquina->setDisabled(true);
        ui->actionEliminarMaquina->setDisabled(true);
        ui->actionRenombrarMaquina->setDisabled(true);
        ui->menuDefinir->setDisabled(true);
        ui->actionEstadoInicial->setDisabled(true);
        ui->actionSeleccionar->setDisabled(true);
        ui->actionNuevoEstado->setDisabled(true);
        ui->actionNuevaTransicion->setDisabled(true);
        ui->actionGenerarCodigo->setDisabled(true);
        ui->actionGenerarDocumentacion->setDisabled(true);
        ui->actionVerificar->setDisabled(true);
        ui->actionAbrirProyecto->setDisabled(true);
        ui->actionNuevoProyecto->setDisabled(true);
        ui->actionCerrarProyecto->setDisabled(true);
        ui->actionGuardar->setDisabled(true);
        ui->actionGuardarComo->setDisabled(true);
        ui->actionEliminarElemento->setDisabled(true);
        ui->actionConfigurarDepuracion->setDisabled(true);
        if (ui->actionDepurar->isEnabled()) {
            m_resetActionDepurar = true;
            ui->actionDepurar->setDisabled(true);
        }

        ui->tableWidgetSimAcciones->clearContents();
        ui->tableWidgetSimEventos->clearContents();

        m_indexEstadosActuales.clear();
        // Inicializo todas las máquinas y organizo las subWindows
        for(int j=0; j < cantMaq; ++j) {
            QMdiSubWindow *subWindow = ui->mdiArea->subWindowList().at(j);
            if (!subWindow->isVisible()) {
                subWindow->show();
                ui->mdiArea->tileSubWindows();
            }
            m_sceneList.at(j)->resetItem()->setActive(true);   // activo el estado de reset
            m_sceneList.at(j)->setMode(DiagramScene::Simulation);
            m_sceneList.at(j)->views().first()->setDragMode(QGraphicsView::NoDrag);
            m_indexEstadosActuales << -1;
        }
        // Cargo los estados de cada máquina y los eventos
        ui->treeWidgetSimEstados->setHeaderLabel(m_datosXml->getContenidoGeneral(NOMBRE));
        ui->treeWidgetSimEstados->clear();   
        ui->tableWidgetSimEventos->setRowCount(0);
        int row = 0;
        QStringList eventosSimulacion;
        // Cargo el evento reset
        QTableWidgetItem *newItem = new QTableWidgetItem("(reset)");
        newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        newItem->setIcon(QIcon(":/img/img/events.png"));
        ui->tableWidgetSimEventos->insertRow(row);
        ui->tableWidgetSimEventos->setItem(row, 0, newItem);
        row++;
        QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList("SISTEMA"));
        topItem->setFlags(Qt::ItemIsEnabled);
        topItem->setIcon(0,QIcon(":/img/img/plugin.png"));
        ui->treeWidgetSimEstados->addTopLevelItem(topItem);
        QList<QTreeWidgetItem *> treeItems;
        QList<QTreeWidgetItem *> subTreeItems;
        foreach (DiagramScene *scene, m_sceneList) {
            QTreeWidgetItem *newTreeItem = new QTreeWidgetItem(QStringList(scene->name()));
            newTreeItem->setFlags(Qt::ItemIsEnabled);
            newTreeItem->setIcon(0,QIcon(":/img/img/application.png"));
            treeItems.append(newTreeItem);
            subTreeItems.clear();
            QTreeWidgetItem *resetItem = new QTreeWidgetItem(QStringList("(RESET)"));
            resetItem->setFlags(Qt::ItemIsEnabled);
            resetItem->setIcon(0,QIcon(":/img/img/bullet_red.png"));
            subTreeItems.append(resetItem);
            foreach (DiagramItem *item, scene->itemList()) {
                QTreeWidgetItem *newSubTreeItem = new QTreeWidgetItem(QStringList(item->name().toUpper()));
                newSubTreeItem->setFlags(Qt::ItemIsEnabled);
                newSubTreeItem->setIcon(0,QIcon(":/img/img/button.png"));
                subTreeItems.append(newSubTreeItem);
                // Eventos de cada estado...
                foreach (DiagramTransition * arrow,  item->transitions()) {
                    if(arrow->startItem() == item) {
                        QString evento = arrow->event();
                        if(!eventosSimulacion.contains(evento)) {
                                eventosSimulacion.append(evento);
                                QTableWidgetItem *newItem = new QTableWidgetItem(evento);
                                newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                                ui->tableWidgetSimEventos->insertRow(row);
                                ui->tableWidgetSimEventos->setItem(row, 0, newItem);
                                row++;
                        }
                    }
                }
            }
            newTreeItem->addChildren(subTreeItems);
        }
        topItem->addChildren(treeItems);
        ui->treeWidgetSimEstados->expandAll();
        ui->tableWidgetSimEventos->resizeRowsToContents();
    } else {
        ui->actionSimular->setChecked(true);
    }
}

void MainWindow::on_updateSimulacionTimeout()
{
    int row;
    int cantMaq = m_sceneList.count();
    for(int j = 0; j < cantMaq; ++j) {
        // Si existe una transición previa, ya no estoy en el estado inicial...
        if(m_sceneList.at(j)->previousArrow() != nullptr) {
            // Actualizo el estado actual para hacerlo titilar
            if (m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->isActive()) {
                m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->setActive(false);
                m_sceneList.at(j)->update();
                m_timerUpdateSim.start(T_SIM_ESTADO_INACTIVO);
            } else {
                m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->setActive(true);
                m_sceneList.at(j)->update();
                m_timerUpdateSim.start(T_SIM_ESTADO_ACTIVO);
            }
        } else {
            // Si estoy en el estado inicial, disparo la transicion inicial
            if (m_sceneList.at(j)->resetItem()->isActive()) {
                DiagramTransition *arrow = m_sceneList.at(j)->resetItem()->transitions().at(0);
                int idProx = m_sceneList.at(j)->itemList().indexOf(arrow->endItem());
                QTreeWidgetItem *itemMaquina = ui->treeWidgetSimEstados->topLevelItem(0)->child(j);
                if(m_indexEstadosActuales.at(j) == -1) {
                    // nuevo
                    m_sceneList.at(j)->resetItem()->setActive(false);
                    itemMaquina->child(0)->setIcon(0, QIcon(":/img/img/bullet black.png"));
                } else {
                    //REVISAR!
                    itemMaquina->child(m_indexEstadosActuales.at(j)+1)->setIcon(0,QIcon(":/img/img/button.png"));
                }
                itemMaquina->child(idProx+1)->setIcon(0,QIcon(":/img/img/button2.png"));
                // actualizo la escena
                if (m_indexEstadosActuales.at(j) != -1)
                    m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->setActive(false);
                m_indexEstadosActuales[j] = idProx;
                m_sceneList.at(j)->itemList().at(idProx)->setActive(true);
                if(m_sceneList.at(j)->previousArrow() != nullptr)
                   m_sceneList.at(j)->previousArrow()->setActive(false);
                arrow->setActive(true);
                m_sceneList.at(j)->setPreviousArrow(arrow);
                // Muestro las acciones asociadas
                if(j == 0) {
                    ui->tableWidgetSimAcciones->clearContents();
                    ui->tableWidgetSimAcciones->setRowCount(0);
                }
                row = ui->tableWidgetSimAcciones->rowCount();
                foreach (QString action, arrow->actions()) {
                   if(!action.isEmpty()) {
                       QString actionText = action+" ["+m_sceneList.at(j)->name()+"]";
                       QTableWidgetItem *newItem = new QTableWidgetItem(actionText);
                       newItem->setFlags(Qt::ItemIsEnabled);
                       newItem->setIcon(QIcon(":/img/img/arrow_right.png"));
                       ui->tableWidgetSimAcciones->insertRow(row);
                       ui->tableWidgetSimAcciones->setItem(row, 0, newItem);
                       row++;
                   }
                }
                ui->tableWidgetSimAcciones->resizeRowsToContents();
                // Actualizo los eventos disponibles
                int count = ui->tableWidgetSimEventos->rowCount();
                if(j==0) {
                    for(int k=0; k < count; k++) {
                        if(ui->tableWidgetSimEventos->item(k,0)->text() != "(reset)") {
                            ui->tableWidgetSimEventos->item(k,0)->setIcon(QIcon());
                        }
                    }
                }
                foreach (DiagramTransition * arrow,  m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->transitions()) {
                    if(arrow->startItem() == m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))) {
                        for(int k=0; k < count; k++) {
                            if((ui->tableWidgetSimEventos->item(k,0)->text() == arrow->event())) {
                                ui->tableWidgetSimEventos->item(k,0)->setIcon(QIcon(":/img/img/events.png"));
                            }
                        }
                    }
                }
                // Termino la transición
            }
        }
    }
}

void MainWindow::on_tableWidgetSimEventos_cellDoubleClicked(int row, int column)
{
    bool clearSimAcciones = false ;
    int rowAcciones = 0;
   
    QString auxEvento, auxMaquina, auxEstado;
    QStringList separoAcciones;

    QTableWidgetItem *itemEvento = ui->tableWidgetSimEventos->item(row, column);

    // Sim-TCP
    auxEvento = itemEvento->text() ;
    auxMaquina= "";
    auxEstado = "";
    QString auxEstadoFut="";
    QString auxAccion="";
    bool evEncontrado = false;

    if(itemEvento->text() == "(reset)")
    {
        m_expresiones.clear_variables();
        for ( int i = 0 ; m_Elmementos.nVariables.at(i) != "" ; i++)
        {
            struct variables_de_proceso vdp;
            vdp.texto_variable =  m_Elmementos.nVariables.value(i);
            vdp.valor_variable =  0;
            m_expresiones.setVariables(vdp);

            //completo con contenidos
            ui->tableWidgetVariables->setItem(i,0,new QTableWidgetItem(vdp.texto_variable));
            ui->tableWidgetVariables->setItem(i,1,new QTableWidgetItem(QString::number(vdp.valor_variable)));
        }
        for ( int i = 0 ; m_Elmementos.nEventos.at(i) != "" ; i++)
        {
            struct pulsadores_de_proceso pulsador;
            if ( m_Elmementos.nEventos[i].contains("s_"))
            {
                    pulsador.texto_pulsadores =  m_Elmementos.nEventos.value(i);
                    pulsador.valor_pulsadores  =  0;
                    m_expresiones.setPulsadores(pulsador);
            }
        }
        // SimTCP: Genero conexión
        QString auxIP = "127.0.0.1";
        QString auxPuerto = "8000";
        if(m_socketSim.state() !=  QAbstractSocket::ConnectedState)
            m_socketSim.connectToHost(QHostAddress(auxIP), quint16(auxPuerto.toUInt()));
        qDebug() << "Se produjo el reset" ;
        QString auxDatosReset = auxMaquina + "," + auxEstado + "," + itemEvento->text() + "," + auxEstadoFut + "," + auxAccion;
        QString datosReset =   QString::number(auxDatosReset.length()) + ":" + auxDatosReset + "@";
        qDebug() << datosReset;
        m_socketSim.write(datosReset.toLatin1());

        // Actualizo los estados de cada máquina
        int cantMaq = m_sceneList.count();
        for(int j = 0; j < cantMaq; ++j) {
            QTreeWidgetItem *itemMaquina = ui->treeWidgetSimEstados->topLevelItem(0)->child(j);
            itemMaquina->child(0)->setIcon(0,QIcon(":/img/img/bullet_red.png"));
            itemMaquina->child(m_indexEstadosActuales.at(j)+1)->setIcon(0,QIcon(":/img/img/button.png"));
            // Actualizo las escenas
            if (m_indexEstadosActuales.at(j) != -1) {
                m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->setActive(false);
            }
            if(m_sceneList.at(j)->previousArrow() != nullptr) {
                m_sceneList.at(j)->previousArrow()->setActive(false);
            }
            m_sceneList.at(j)->setPreviousArrow(nullptr);
            m_timerUpdateSim.start(T_SIM_RESET_INIT);               // arranco timer inicial para el reset
            m_sceneList.at(j)->resetItem()->setActive(true); // activo el estado de reset
            m_indexEstadosActuales[j] = -1;       // idx reset = -1
            m_sceneList.at(j)->update();
        }
        // Limpio las acciones asociadas
        ui->tableWidgetSimAcciones->clearContents();
        ui->tableWidgetSimAcciones->setRowCount(0);
        // actualizo los eventos disponibles
        int count = ui->tableWidgetSimEventos->rowCount();
        for(int k=0; k < count; k++) {
            if(ui->tableWidgetSimEventos->item(k,0)->text() != "(reset)") {
                ui->tableWidgetSimEventos->item(k,0)->setIcon(QIcon());
            }
        }
    }
    else
    {
        int cantMaq = m_sceneList.count();
        for(int j = 0; j < cantMaq; ++j)
        {
            if (m_indexEstadosActuales.at(j) != -1) {
                DiagramItem *estadoActual = m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j));

                QStringList listaeventos;

//-*-*-*-*-*-*-*-*-*-
                struct pulsadores_de_proceso pulsador;
                //si encuentra al sensor lo tiene que poner su estado en 1
                m_expresiones.ActivaPulsador(auxEvento);

                foreach (DiagramTransition *arrow, estadoActual->transitions()) {
                    // Sólo chequeo las transiciones salientes del estado actual
                    if(arrow->startItem() == estadoActual) {
                        // Si encuentro el evento genero la transición

                        QString arrow_event =arrow->event();
                        bool exito ;
                        exito = arrow_event.contains(auxEvento , Qt::CaseSensitive);

                        if (arrow->event() == itemEvento->text() || exito == true )
                        {
                            int idProx = m_sceneList.at(j)->itemList().indexOf(arrow->endItem());
                            // Actualizo el estado
                            QTreeWidgetItem *itemMaquina = ui->treeWidgetSimEstados->topLevelItem(0)->child(j);
                            itemMaquina->child(m_indexEstadosActuales.at(j)+1)->setIcon(0,QIcon(":/img/img/button.png"));
                            itemMaquina->child(idProx+1)->setIcon(0,QIcon(":/img/img/button2.png"));                            

                            // SimTCP
                            evEncontrado = true;
                            auxMaquina = itemMaquina->text(0);
                            qDebug() << "máquina: " << auxMaquina;
                            auxEstado = estadoActual->name();
                            qDebug() << "estado actual: " << auxEstado;
                            auxEstadoFut = m_sceneList.at(j)->itemList().at(idProx)->name();
                            qDebug() << "estado futuro: " << auxEstadoFut;

                            // Actualizo la escena
                            estadoActual->setActive(false);
                            m_indexEstadosActuales[j] = idProx;
                            m_sceneList.at(j)->itemList().at(idProx)->setActive(true);
                            if(m_sceneList.at(j)->previousArrow() != nullptr)
                                m_sceneList.at(j)->previousArrow()->setActive(false);
                            arrow->setActive(true);
                            m_sceneList.at(j)->setPreviousArrow(arrow);
                            // Muestro las acciones asociadas
                            if (!clearSimAcciones) {
                                ui->tableWidgetSimAcciones->clearContents();
                                ui->tableWidgetSimAcciones->setRowCount(0);
                                rowAcciones = 0;
                                clearSimAcciones = true;
                            }
                            foreach (QString action, arrow->actions()) {
                                if(action != "") {
                                    QString actionText = action+" ["+m_sceneList.at(j)->name()+"]";                                  
                                    QTableWidgetItem *newItem = new QTableWidgetItem(actionText);
                                    newItem->setFlags(Qt::ItemIsEnabled);
                                    newItem->setIcon(QIcon(":/img/img/arrow_right.png"));
                                    ui->tableWidgetSimAcciones->insertRow(rowAcciones);
                                    ui->tableWidgetSimAcciones->setItem(rowAcciones, 0, newItem);
                                    rowAcciones++;

                                    // Sim-TCP
                                    qDebug() << "acción: " << actionText;
                                    QStringList listaAccion = actionText.split("(") ;
                                    auxAccion = listaAccion[0];
                                    separoAcciones.append(auxAccion);
                                    auxAccion = "";
                                }
                            }
                            // < -- > Aca tengo que disparar los timers si se esta en simulacion sin remotos                 
                            if ( m_simular_con_Infotronic_virtual == false )
                            {
                                for ( int i = 0 ;i < separoAcciones.count() ; i++)
                                {
                                    QString a;
                                    QStringList partes;
                                    a = separoAcciones.value(i);
                                    if( a.contains("t_"))
                                    {
                                        int n , base;
                                        struct timer t;

                                        partes = a.split("_");
                                        n = partes.count();

                                        if (partes.value(n-1) == "S")
                                            base = 1000;
                                        if (partes.value(n-1) == "D")
                                            base = 100;

                                        if ( partes.value(n-1) == "D" || partes.value(n-1) == "S")
                                        {
                                            t.tiempo = t.deltaT = partes.value(n-2).toInt() * base;
                                            t.evento = "e_" + partes.value(1);
                                            t.Evento = nullptr;
                                            t.n = 0;
                                            TimerRun.timerstart(t);
                                        }
                                        else if ( partes.value(n-1) == "stop")
                                            TimerRun.timerstop("e_" + partes.value(1));
                                        else
                                            TimerRun.timerclose( );

                                    }
                                }
                                // < --- >
                                for ( int i = 0 ;i < separoAcciones.count() ; i++)
                                {
                                    for (int j = 0 ; m_Elmementos.nVariables.at(j) != "" ; j ++)
                                    {
                                        if ( separoAcciones.value(i).contains(m_Elmementos.nVariables.value(j)) )
                                        {
                                           // m_variablesdeproceso.set_valor(separoAcciones.value(i) ); // que hacer con la variable
                                            m_expresiones.setNuevoValorDeVariable(separoAcciones.value(i));
                                        }

                                    }
                                }
                                //////////////////////////////////////////////////////////////
                                //// <<<< aca va el analisis de las expreciones >>>>

                                QStringList eventosposibles;
                                int r;
                                for ( int j = 0 ; j <   m_Elmementos.nExpreciones.count() ; j ++)
                                {
                                    QStringList expreciones , operadores;
                                    // Descompongo en expresiones simples
                                    if(  m_Elmementos.nExpreciones[j].contains( "(" ) )
                                        m_expresiones.AnalisisDeExpresiones (  m_Elmementos.nExpreciones[j] , expreciones , operadores );
                                    else
                                        expreciones << m_Elmementos.nExpreciones[j];

                                    r = m_expresiones.ResultadoExpresiones ( expreciones , operadores );
                                    if ( r == true )
                                        eventosposibles << m_Elmementos.nExpreciones[j];
                                }
                                ///////////////////////////////////////////////////////////////////////
                                m_expresiones.DesActivaPulsador();
                                struct timer t;
                                for ( int r = 0 ; r < eventosposibles.count( ) ; r++)
                                {
                                    t.tiempo = 0;
                                    t.evento = eventosposibles.value(r);
                                    t.Evento = nullptr;
                                    t.n = 0;
                                    TimerRun.timerstart(t);
                                }

                            }
                                                               
                            ui->tableWidgetSimAcciones->resizeRowsToContents();
                            // Termino la transición
                            for ( int i = 0 ; i < ui->tableWidgetVariables->rowCount( ) ; i++)
                            {
                                struct variables_de_proceso vdp;
                                m_expresiones.getVariables(vdp,i);

                                //completo con contenidos
                                ui->tableWidgetVariables->setItem(i,0,new QTableWidgetItem(vdp.texto_variable));
                                ui->tableWidgetVariables->setItem(i,1,new QTableWidgetItem(QString::number(vdp.valor_variable)));
                            }



                            break;
                        }
                    }
                }
            }
        }

        // Sim-TCP
        if(evEncontrado == true) {
            evEncontrado = false  ;
            // Parseao las acciones y luego lo sumo a los campos a enviar
            // --> Intercambio
            QStringList AuxseparoAcciones;

            if ( m_simular_con_Infotronic_virtual == true )
            {
                if(separoAcciones.length() > 0)
                {
                    for ( int i = 0 ; i < separoAcciones.length() ; i++ )
                    {
                        QString AccionSeparada ;
                        AccionSeparada = m_Infotronic->GetRelayConvertido(separoAcciones[i]);
                        if(AccionSeparada != "" )
                            AuxseparoAcciones << AccionSeparada;
                    }

                    for ( int i = 0 ; i < separoAcciones.length() ; i++ )
                    {
                        QString AccionSeparada;
                        AccionSeparada =m_Infotronic->GetRGBConvertido(separoAcciones[i]);
                        if( AccionSeparada != "" )
                            AuxseparoAcciones << AccionSeparada;
                    }

                    for ( int i = 0 ; i < separoAcciones.length() ; i++ )
                    {
                        QString AccionSeparada;
                        AccionSeparada = m_Infotronic->GetTemporizadoresConvertido(separoAcciones[i]);
                        if( AccionSeparada != "" )
                            AuxseparoAcciones << AccionSeparada;
                    }

                    for ( int i = 0 ; i < separoAcciones.length() ; i++ )
                    {
                        QString AccionSeparada;
                        AccionSeparada = m_Infotronic->GetLEDS_EXP2Convertido(separoAcciones[i]);
                        if( AccionSeparada != "" )
                            AuxseparoAcciones << AccionSeparada;
                    }

                    auxAccion = accionesDivididas(AuxseparoAcciones);
                }
                else
                    auxAccion = "";
            }
            else
            {
                if(separoAcciones.length() > 0)
                {
                    auxAccion = accionesDivididas( separoAcciones );
                }
                else
                    auxAccion = "";
            }

            // Genero conexion TCP
            QString auxIP = "127.0.0.1";
            QString auxPuerto = "8000";
            if(m_socketSim.state() !=  QAbstractSocket::ConnectedState)
                m_socketSim.connectToHost(QHostAddress(auxIP), quint16(auxPuerto.toUInt()));
            QString auxDatos = auxMaquina + "," + auxEstado + "," + auxEvento + "," + auxEstadoFut + "," + auxAccion;
            auxAccion = "";
            separoAcciones.clear();
            QString datos =   QString::number(auxDatos.length()) + ":" + auxDatos + "@";
            qDebug() << datos;
            m_socketSim.write(datos.toLatin1());
        }

        // Actualizo los eventos disponibles
        int count = ui->tableWidgetSimEventos->rowCount();
        for(int k=0; k < count; k++)
            if(ui->tableWidgetSimEventos->item(k,0)->text() != "(reset)")
                ui->tableWidgetSimEventos->item(k,0)->setIcon(QIcon());
        for(int j = 0; j < cantMaq; ++j) {
            if (m_indexEstadosActuales.at(j) != -1) {
                foreach (DiagramTransition * arrow,  m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->transitions()) {
                    if(arrow->startItem() == m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at((j))))
                        for(int k = 0; k < count; k++)
                            if((ui->tableWidgetSimEventos->item(k,0)->text() == arrow->event()))
                                ui->tableWidgetSimEventos->item(k,0)->setIcon(QIcon(":/img/img/events.png"));
                }
            }
        }
    }
}

QString MainWindow::accionesDivididas(QStringList acciones)
{
   QString resultado = "";

   if(acciones.length() == 1) {
       resultado = acciones.at(0);
   } else {
       resultado = acciones.at(0);
       for(int a = 1 ; a < acciones.length() ; a++) {
         resultado = resultado + ";" + acciones.at(a);
       }
   }
   return resultado;
}

void MainWindow::on_actionTerminarSimulacion_triggered()
{
    if(m_uiMode == MainWindow::Simular) {
         TimerRun.timerclose();

         m_simular_con_Infotronic_virtual = false;

         ui->actionInterfaz_Virtual->setChecked(false);
         ui->actionSimular->setDisabled(false);
        int cantMaq = m_sceneList.count();
        for(int j=0; j < cantMaq; ++j) {
            m_sceneList.at(j)->setMode(DiagramScene::MoveItem);
            m_sceneList.at(j)->views().first()->setDragMode(QGraphicsView::RubberBandDrag);
            if (m_indexEstadosActuales.at(j) != -1) {
                m_sceneList.at(j)->previousArrow()->setActive(false);
                m_sceneList.at(j)->setPreviousArrow(nullptr);
                m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->setActive(false);
            } else {
                 m_sceneList.at(j)->resetItem()->setActive(false);
            }
        }
        m_uiMode = MainWindow::Editar;
        m_timerUpdateSim.stop();
        ui->actionTerminarSimulacion->setDisabled(true);
        ui->actionNuevaMaquina->setEnabled(true);
        ui->actionEliminarMaquina->setEnabled(true);
        ui->actionRenombrarMaquina->setEnabled(true);
        ui->menuDefinir->setEnabled(true);
        ui->actionEstadoInicial->setEnabled(true);
        ui->actionSeleccionar->setEnabled(true);
        ui->actionNuevoEstado->setEnabled(true);
        ui->actionNuevaTransicion->setEnabled(true);
        ui->actionGenerarCodigo->setEnabled(true);
        ui->actionGenerarDocumentacion->setEnabled(true);
        ui->actionEliminarElemento->setEnabled(true);
        ui->actionVerificar->setEnabled(true);
        ui->actionAbrirProyecto->setEnabled(true);
        ui->actionNuevoProyecto->setEnabled(true);
        ui->actionCerrarProyecto->setEnabled(true);
        ui->actionGuardar->setEnabled(true);
        ui->actionGuardarComo->setEnabled(true);       
        ui->actionConfigurarDepuracion->setEnabled(true);

        OcultarTablaVariables();
        BorrarTablaVariables();

        if (m_resetActionDepurar) {
            ui->actionDepurar->setEnabled(true);
            m_resetActionDepurar = false;
        }

        ui->menuVer->removeAction(ui->dockWidgetSimulacion->toggleViewAction());
        ui->menuVer->addAction(ui->dockWidgetProyecto->toggleViewAction());

        if(m_resetDockVerificar == true) {
            m_resetDockVerificar = false;
            ui->dockWidgetVerificar->show();
            tabifyDockWidget(ui->dockWidgetProyecto, ui->dockWidgetVerificar);
        }
        ui->tableWidgetSimAcciones->setRowCount(0);
        ui->tableWidgetSimEventos->setRowCount(0);
        ui->dockWidgetSimulacion->hide();
        ui->dockWidgetProyecto->show();
        ui->actionSimular->setChecked(false);
    }
}

void MainWindow::on_actionVerificar_triggered()
{
    guardarDatos();
    // Chequeo si están definidos todos los estados iniciales
    int cantMaq = m_sceneList.count();
    for(int j=0; j < cantMaq; ++j) {
        if(m_datosXml->getEstadoInicial(MAQUINA+QString::number(j)) == "-1") {
            QMessageBox::warning(this, QLatin1String("Advertencia"), m_sceneList.at(j)->name()
                                 +QLatin1String(": No se encuentra definido el Estado Inicial"));
            return;
        }
    }
    // Actualizo ui
    ui->dockWidgetVerificar->setEnabled(true);
    ui->dockWidgetVerificar->show();
    tabifyDockWidget(ui->dockWidgetProyecto, ui->dockWidgetVerificar);
    // Preparo el treeWidget
    ui->treeWidgetVerificar->setHeaderLabel("Verificando: "+m_datosXml->getContenidoGeneral(NOMBRE));
    ui->treeWidgetVerificar->clear();
    QTreeWidgetItem *itemEstSinSalida = new QTreeWidgetItem(QStringList("Chequeo de estados sin salida"));
    QTreeWidgetItem *itemEstSinUsar = new QTreeWidgetItem(QStringList("Chequeo de estados sin activar"));
    QTreeWidgetItem *itemElemSinUsar = new QTreeWidgetItem(QStringList("Chequeo de elementos sin utilizar"));
    QTreeWidgetItem *itemEvSinUsar = new QTreeWidgetItem(QStringList("Eventos sin utilizar"));
    QTreeWidgetItem *itemActSinUsar = new QTreeWidgetItem(QStringList("Acciones sin utilizar"));
    QTreeWidgetItem *itemVarSinUsar = new QTreeWidgetItem(QStringList("Variables sin utilizar"));
    QTreeWidgetItem *itemConstSinUsar = new QTreeWidgetItem(QStringList("Constantes sin utilizar"));
    itemEstSinSalida->setFlags(Qt::ItemIsEnabled);
    itemEstSinUsar->setFlags(Qt::ItemIsEnabled);
    itemElemSinUsar->setFlags(Qt::ItemIsEnabled);
    itemEvSinUsar->setFlags(Qt::ItemIsEnabled);
    itemActSinUsar->setFlags(Qt::ItemIsEnabled);
    itemVarSinUsar->setFlags(Qt::ItemIsEnabled);
    itemConstSinUsar->setFlags(Qt::ItemIsEnabled);
    QList<QTreeWidgetItem *> childItemsElementos;
    childItemsElementos << itemEvSinUsar << itemActSinUsar << itemVarSinUsar << itemConstSinUsar;
    ui->treeWidgetVerificar->addTopLevelItem(itemEstSinSalida);
    ui->treeWidgetVerificar->addTopLevelItem(itemEstSinUsar);
    ui->treeWidgetVerificar->addTopLevelItem(itemElemSinUsar);
    itemElemSinUsar->addChildren(childItemsElementos);
    itemEstSinSalida->setIcon(0,QIcon(":/img/img/tick.png"));
    itemElemSinUsar->setIcon(0,QIcon(":/img/img/tick.png"));
    itemEvSinUsar->setIcon(0,QIcon(":/img/img/tick.png"));
    itemActSinUsar->setIcon(0,QIcon(":/img/img/tick.png"));
    itemVarSinUsar->setIcon(0,QIcon(":/img/img/tick.png"));
    itemConstSinUsar->setIcon(0,QIcon(":/img/img/tick.png"));
    ui->treeWidgetVerificar->expandAll();
    QVector<QVector<bool>> estadosUtilizados;
    QVector<QVector<bool>> estadosConSalida;
    QVector<bool> eventosUtilizados;
    QVector<bool> accionesUtilizadas;
    QVector<bool> variablesUtilizadas;
    QVector<bool> constantesUtilizadas;
    QStringList acciones = m_datosXml->getNombresAccionesDefinidas();
    QStringList eventos = m_datosXml->getNombresEventosDefinidos();
    QStringList variables = m_datosXml->getNombresVariablesDefinidas();
    QStringList constantes = m_datosXml->getNombresConstantesDefinidas();
    QStringList triggers;
    QRegularExpression re;
    int k;
    // Estados
    QVector<int> cantEstados;
    cantEstados.resize(cantMaq);
    estadosUtilizados.resize(cantMaq);
    estadosConSalida.resize(cantMaq);
    for(int j=0; j < cantMaq; ++j) {
        cantEstados[j] = m_sceneList.at(j)->itemList().count();
        estadosUtilizados[j].resize(cantEstados.at(j));
        estadosConSalida[j].resize(cantEstados.at(j));
    }
    // Eventos
    int cantEventos = m_datosXml->cantidadEventos();
    if (cantEventos != -1)
        eventosUtilizados.resize(cantEventos);
    else
        cantEventos = 0;
    // Acciones
    int cantAcciones = m_datosXml->cantidadAcciones();
    if (cantAcciones != -1)
        accionesUtilizadas.resize(cantAcciones);
    else
        cantAcciones = 0;
    // Variables
    int cantVar = m_datosXml->cantidadVariables();
    if (cantVar != -1)
        variablesUtilizadas.resize(cantVar);
    else
        cantVar = 0;
    // Ctes
    int cantConst = m_datosXml->cantidadConstantes();
    if (cantConst != -1)
        constantesUtilizadas.resize(cantConst);
    else
        cantConst = 0;

    // Chequeo estados sin utilizar/activar y estados sin salida
    for (int j=0; j < cantMaq; ++j) {
        for (k = 0; k < cantEstados[j]; ++k) {
            DiagramItem *item = m_sceneList.at(j)->itemList().at(k);
            int idProx;
            foreach (DiagramTransition *arrow, item->transitions()) {
                idProx = m_sceneList.at(j)->itemList().indexOf(arrow->endItem());
                // verifico que no sean autotransiciones (llega por lo menos una transición)
                if(arrow->endItem() != arrow->startItem())
                    estadosUtilizados[j][idProx] = true;
                // verifico que exista por lo menos una transicion a otro estado
                if ((arrow->startItem() == item) && (arrow->endItem() != item))
                   estadosConSalida[j][k] = true;

                triggers = arrow->event().split(QRegExp("\\&\\&|\\|\\|"), QString::SkipEmptyParts);
                foreach (QString str_ev, triggers) {
                    // verifico eventos utilizados
                    for (int i=0; i < cantEventos; ++i) {
                        re.setPattern("\\b"+eventos.at(i)+"\\b");
                        if(str_ev.contains(re) && !str_ev.contains(QRegExp("\\=|\\!\\=|\\<|\\>"))) {
                            eventosUtilizados[i] = true;
                        }
                    }
                    // verifico variables en el disparador de la transición
                    for (int i=0; i < cantVar; ++i) {
                        re.setPattern("\\b"+variables.at(i)+"\\b");
                        if (str_ev.contains(re) && str_ev.contains(QRegExp("\\=|\\!\\=|\\<|\\>"))) {
                            variablesUtilizadas[i] = true;
                        }
                    }
                }
                // verifico acciones
                for (int i=0; i < cantAcciones; ++i) {
                    foreach (QString str_act, arrow->actions()) {
                        if(str_act.contains(acciones.at(i))) {
                            accionesUtilizadas[i] = true;
                        }
                    }
                }
                // verifico variables en las acciones de la transición
                for (int i=0; i < cantVar; ++i) {
                    foreach (QString str_act, arrow->actions()) {
                        if (str_act.contains(variables.at(i), Qt::CaseSensitive)) {
                            variablesUtilizadas[i] = true;
                        }
                    }
                }
                // constantes
                //                for (int i=0; i < cantEventos; ++i) {
                //                    if(arrow->event() == eventos.at(i))
                //                        eventosUtilizados.data()[i] = true;
                //                }
            }
        }
    }

    // Analizo y muestro los resultados ----------------------------------------
    for (int j=0; j < cantMaq; ++j) {
        // Estados sin utilizar
        if (estadosUtilizados[j].contains(false)) {
            // chequeo cuales son los estados sin activar
            QList<QTreeWidgetItem *> childItemsEstados;
            for(k = 0; k < cantEstados[j]; ++k) {
                if (estadosUtilizados[j][k] == false)  {
                    QString name = m_sceneList.at(j)->itemList().at(k)->name().toUpper()+
                            " ["+m_sceneList.at(j)->name()+"]";
                    QTreeWidgetItem *itemEstado =
                            new QTreeWidgetItem(QStringList(name));
                    itemEstado->setFlags(Qt::ItemIsEnabled);
                    itemEstado->setIcon(0,QIcon(":/img/img/error.png"));
                    childItemsEstados << itemEstado;
                }
            }
            itemEstSinUsar->addChildren(childItemsEstados);
            itemEstSinUsar->setIcon(0,QIcon(":/img/img/error.png"));
            ui->treeWidgetVerificar->expandAll();
        } else {
            itemEstSinUsar->setIcon(0,QIcon(":/img/img/tick.png"));
        }
        // Estados sin salida
        if (estadosConSalida[j].contains(false)) {
            // chequeo cuales son los estados sin salida
            QList<QTreeWidgetItem *> childItemsEstados;
            for(k = 0; k < cantEstados[j]; ++k) {
                if (estadosConSalida[j][k] == false)  {
                    QString name = m_sceneList.at(j)->itemList().at(k)->name().toUpper()+
                            " ["+m_sceneList.at(j)->name()+"]";
                    QTreeWidgetItem *itemEstado =
                            new QTreeWidgetItem(QStringList(name));
                    itemEstado->setFlags(Qt::ItemIsEnabled);
                    itemEstado->setIcon(0,QIcon(":/img/img/error.png"));
                    childItemsEstados << itemEstado;
                }
            }
            itemEstSinSalida->addChildren(childItemsEstados);
            itemEstSinSalida->setIcon(0,QIcon(":/img/img/error.png"));
            ui->treeWidgetVerificar->expandAll();
        } else {
            itemEstSinSalida->setIcon(0,QIcon(":/img/img/tick.png"));
        }
    }
    // Eventos sin utilizar
    if (eventosUtilizados.contains(false)) {
        // chequeo cuales son los eventos sin utilizar
        QList<QTreeWidgetItem *> childItemEventos;
        for(k = 0; k < cantEventos; ++k) {
            if (eventosUtilizados[k] == false)  {
                QTreeWidgetItem *itemEvento =
                        new QTreeWidgetItem(QStringList(m_datosXml->getContenidoEvento(EVENTO+QString::number(k))));
                itemEvento->setFlags(Qt::ItemIsEnabled);
                itemEvento->setIcon(0,QIcon(":/img/img/error.png"));
                childItemEventos << itemEvento;
            }
        }
        itemEvSinUsar->addChildren(childItemEventos);
        itemEvSinUsar->setIcon(0,QIcon(":/img/img/error.png"));
        itemElemSinUsar->setIcon(0,QIcon(":/img/img/error.png"));
        ui->treeWidgetVerificar->expandAll();
    } else {
        itemEvSinUsar->setIcon(0,QIcon(":/img/img/tick.png"));
    }
    // Acciones sin utilizar
    if (accionesUtilizadas.contains(false)) {
        // chequeo cuales son los eventos sin utilizar
        QList<QTreeWidgetItem *> childItemAcciones;
        for(k = 0; k < cantAcciones; ++k) {
            if (accionesUtilizadas[k] == false)  {
                QTreeWidgetItem *itemAction =
                        new QTreeWidgetItem(QStringList(m_datosXml->getContenidoAccion(ACCION+QString::number(k))));
                itemAction->setFlags(Qt::ItemIsEnabled);
                itemAction->setIcon(0,QIcon(":/img/img/error.png"));
                childItemAcciones << itemAction;
            }
        }
        itemActSinUsar->addChildren(childItemAcciones);
        itemActSinUsar->setIcon(0,QIcon(":/img/img/error.png"));
        itemElemSinUsar->setIcon(0,QIcon(":/img/img/error.png"));
        ui->treeWidgetVerificar->expandAll();
    } else {
        itemActSinUsar->setIcon(0,QIcon(":/img/img/tick.png"));
    }
    // Variables sin utilizar
    if (variablesUtilizadas.contains(false)) {
        // chequeo cuales son los eventos sin utilizar
        QList<QTreeWidgetItem *> childItemVariables;
        for(k = 0; k < cantVar; ++k) {
            if (variablesUtilizadas[k] == false)  {
                QTreeWidgetItem *itemVar =
                        new QTreeWidgetItem(QStringList(m_datosXml->getContenidoVariable(VARIABLE+QString::number(k))));
                itemVar->setFlags(Qt::ItemIsEnabled);
                itemVar->setIcon(0,QIcon(":/img/img/error.png"));
                childItemVariables << itemVar;
            }
        }
        itemVarSinUsar->addChildren(childItemVariables);
        itemVarSinUsar->setIcon(0,QIcon(":/img/img/error.png"));
        itemElemSinUsar->setIcon(0,QIcon(":/img/img/error.png"));
        ui->treeWidgetVerificar->expandAll();
    } else {
        itemVarSinUsar->setIcon(0,QIcon(":/img/img/tick.png"));
    }
    // Ctes sin utilizar
//    if (constantesUtilizadas.contains(false)) {
//        // chequeo cuales son los eventos sin utilizar
//        QList<QTreeWidgetItem *> childItemCtes;
//        for(k = 0; k < cantConst; ++k) {
//            if (constantesUtilizadas[k] == false)  {
//                QTreeWidgetItem *itemConst =
//                        new QTreeWidgetItem(QStringList(m_datosXml->getContenidoConstante(CONSTANTE+QString::number(k))));
//                itemConst->setFlags(Qt::ItemIsEnabled);
//                itemConst->setIcon(0,QIcon(":/img/img/error.png"));
//                childItemCtes << itemConst;
//            }
//        }
//        itemConstSinUsar->addChildren(childItemCtes);
//        itemConstSinUsar->setIcon(0,QIcon(":/img/img/error.png"));
//        itemElemSinUsar->setIcon(0,QIcon(":/img/img/error.png"));
//        ui->treeWidgetVerificar->expandAll();
//    } else {
//        itemConstSinUsar->setIcon(0,QIcon(":/img/img/tick.png"));
//    }
}

void MainWindow::on_actionGenerarCodigo_triggered()
{
    guardarDatos();

    // Chequeo si están definidos todos los estados iniciales
    for(int j=0; j < m_sceneList.count(); ++j) {
        if(m_datosXml->getEstadoInicial(MAQUINA+QString::number(j)) == "-1") {
            QMessageBox::warning(this, QLatin1String("Advertencia"), m_sceneList.at(j)->name()
                                 +QLatin1String(": No se encuentra definido el Estado Inicial"));
            return;
        }
    }

    if (m_permisosCodigo.exec() == QFileDialog::Accepted) {
        struct Elementos nE;
        QList< struct MdeE > nM;
        QString crp;

        leerUmf(nM, nE);
        m_funcionesCodigo.SetValores(nM, nE);

        if (m_permisosCodigo.getSeleccioDeCodigo() == "Switch Case")
            crp = m_funcionesCodigo.GeneracionDeCodigo_switch(m_permisosCodigo);
        else
            crp = m_funcionesCodigo.GeneracionDeCodigo_pFuncion(m_permisosCodigo);

        QMessageBox::information(this, QString::fromUtf8("Generar Código"),
                                 QString::fromUtf8("Se encuentra disponible el código en ") +
                                 crp);
    }
}

void MainWindow::leerUmf(QList<struct MdeE> &Maquinas, struct Elementos &Elm)
{
    struct Transiciones trs;
    struct MdeE auxiliar;
    struct Estados auxiliarEstados;
    struct Comentario auxCom;
    QStringList vpm;
    // Variables
    int cant= m_datosXml->cantidadVariables();
    Elm.nVariables.clear();
    for(int k = 0 ; k < cant ; k++ ) {
        Elm.nVariables << m_datosXml->getContenidoVariable(VARIABLE+QString::number(k));
        auxCom.textoComentario = m_datosXml->getComentarioArchivo(VARIABLE+QString::number(k));
        if (auxCom.textoComentario != "") {
            auxCom.perteneceA = m_datosXml->getContenidoVariable(VARIABLE+QString::number(k));
            Elm.nComentarios << auxCom;
        }
    }
    Elm.nVariables << "";

    int cantEventos = m_datosXml->cantidadEventos();
    int cantAcciones = m_datosXml->cantidadAcciones();

    Elm.nEventos.clear();
    for(int i = 0 ; i < cantEventos ; i++) {
        Elm.nEventos << m_datosXml->getContenidoEvento(EVENTO+QString::number(i));
        auxCom.textoComentario = m_datosXml->getComentarioArchivo(EVENTO+QString::number(i));
        if (auxCom.textoComentario != "") {
            auxCom.perteneceA = m_datosXml->getContenidoEvento(EVENTO+QString::number(i));
            Elm.nComentarios << auxCom;
        }
    }
    Elm.nEventos << "";

    Elm.nAcciones.clear( );
    Elm.nMacros.clear( );
    Elm.nTemporizadores.clear( );

    for(int i = 0 ; i < cantAcciones ; i++ )
    {
        QString acc;
        acc = m_datosXml->getContenidoAccion(ACCION+QString::number(i));
        auxCom.textoComentario = m_datosXml->getComentarioArchivo(ACCION+QString::number(i));
        if ( acc[0] == 'f') {
            Elm.nAcciones << m_datosXml->getContenidoAccion(ACCION+QString::number(i));
            if (auxCom.textoComentario != "") {
                auxCom.perteneceA = acc.replace("()", "");
                Elm.nComentarios << auxCom;
            }
        } else if (acc[0] == 'm') {
            Elm.nMacros << m_datosXml->getContenidoAccion(ACCION+QString::number(i));
            if (auxCom.textoComentario != "") {
                auxCom.perteneceA = acc;
                Elm.nComentarios << auxCom;
            }
        } else if (acc[0] == 't') {
            Elm.nTemporizadores << m_datosXml->getContenidoAccion(ACCION+QString::number(i));
            if (auxCom.textoComentario != "") {
                QStringList auxStrList = acc.split('_');
                auxCom.perteneceA = auxStrList.at(0);
                for (int k = 1; k < auxStrList.length() - 2; k++) auxCom.perteneceA.append("_" + auxStrList[k]);
                Elm.nComentarios << auxCom;
            }
        }
    }
    Elm.nAcciones.sort();
    Elm.nMacros.sort();
    Elm.nTemporizadores.sort();

    Elm.nAcciones << "";
    Elm.nMacros << "";
    Elm.nTemporizadores << "";

    int cantConstantes = m_datosXml->cantidadConstantes();

    Elm.nConstantes.clear();
    for(int i = 0 ; i < cantConstantes ; i++ ) {
        Elm.nConstantes << m_datosXml->getContenidoConstante(CONSTANTE+QString::number(i));
        auxCom.textoComentario = m_datosXml->getComentarioArchivo(CONSTANTE+QString::number(i));
        if (auxCom.textoComentario != "") {
            auxCom.perteneceA = m_datosXml->getContenidoConstante(CONSTANTE+QString::number(i));
            Elm.nComentarios << auxCom;
        }
    }
    Elm.nConstantes << "";


    // Cargo los datos del proyecto
    int cantMaquinas, cantEstados, cantTransiciones;
    QString name;

    cantMaquinas = m_datosXml->cantidadMaquinas();

    QStringList auxEstados;
    Elm.nMaquinas.clear();
    for (int k = 0; k < cantMaquinas; ++k)
    {
        auxiliar.NombreMaquina = m_datosXml->getContenidoMaquina(MAQUINA+QString::number(k), NOMBRE_MAQUINA);
        Elm.nMaquinas << auxiliar.NombreMaquina ;

        cantEstados = m_datosXml->cantidadEstados(MAQUINA+QString::number(k));

        QString evento,ProxEvento;
        QStringList accion;

       auxiliarEstados.NombreEstado = "RESET";
       trs.idEvento = "-";
       trs.idAcciones = m_datosXml->getContenidoInicial(ID_ACCIONES, MAQUINA+QString::number(k));
       trs.idProxEstado = m_datosXml->getEstadoInicial(MAQUINA+QString::number(k));
       auxiliarEstados.trn << trs;

       trs.idEvento = "";
       trs.idAcciones = "";
       trs.idProxEstado = "";
       auxiliarEstados.trn << trs;

       auxiliar.est << auxiliarEstados;
       auxiliarEstados.trn.clear();

       auxEstados.clear();
       vpm.clear();
       for (int i = 0 ; i < cantEstados ; ++i)
       {
            auxiliarEstados.NombreEstado = m_datosXml->getContenidoDiagrama(ESTADO+QString::number(i),NOMBRE_ESTADO, MAQUINA+QString::number(k));
            auxEstados << auxiliarEstados.NombreEstado;

            cantTransiciones = m_datosXml->cantidadTransiciones(ESTADO+QString::number(i), MAQUINA+QString::number(k));

            for (int j = 0; j < cantTransiciones ; ++j)
            {
                QString pxe = m_datosXml->getContenidoTransicion(ESTADO+QString::number(i),ID_TRANS+QString::number(j), ID_PROX_ESTADO,MAQUINA+QString::number(k));//.right(1).toInt();

                trs.idProxEstado = m_datosXml->getContenidoDiagrama( pxe , NOMBRE_ESTADO , MAQUINA+QString::number(k));
                trs.idEvento = m_datosXml->getContenidoTransicion(ESTADO+QString::number(i),ID_TRANS+QString::number(j), ID_EVENTO,MAQUINA+QString::number(k));
                trs.idAcciones = (m_datosXml->getContenidoTransicion(ESTADO+QString::number(i),ID_TRANS+QString::number(j), ID_ACCIONES,MAQUINA+QString::number(k)));//.split(",");

                if (  trs.idEvento.contains("==", Qt::CaseInsensitive)
                        ||trs.idEvento.contains("<=", Qt::CaseInsensitive )
                        ||  trs.idEvento.contains("<=", Qt::CaseInsensitive)
                        ||  trs.idEvento.contains(">=", Qt::CaseInsensitive)
                        || trs.idEvento.contains(">", Qt::CaseInsensitive)
                        || trs.idEvento.contains("<", Qt::CaseInsensitive)
                        || trs.idEvento.contains("!=", Qt::CaseInsensitive) )

                {
                    Elm.nExpreciones << trs.idEvento;
                }

                for  ( int r = 0 ; Elm.nEventos[r] != ""; r++ )
                {
                    if ( trs.idEvento.contains(Elm.nEventos[r], Qt::CaseInsensitive) )
                        trs.idEvento.replace(Elm.nEventos[r],Elm.nEventos[r] + "()");

                }

                for ( int q = 0 ; q < Elm.nVariables[q].length() ; q++ )
                {
                    if ( trs.idEvento.contains(Elm.nVariables[q], Qt::CaseInsensitive) )
                    {
                        if ( !vpm.contains( Elm.nVariables[q] , Qt::CaseInsensitive) )
                            vpm << Elm.nVariables[q];
                    }
                }

                auxiliarEstados.trn << trs;

            }

            auxCom.textoComentario = m_datosXml->getComentarioArchivo(ESTADO+QString::number(i));
            if (auxCom.textoComentario != "") {
                auxCom.perteneceA = auxiliarEstados.NombreEstado;
                Elm.nComentarios << auxCom;
            }

            trs.idEvento = "";
            trs.idAcciones = "";
            trs.idProxEstado = "";
            auxiliarEstados.trn << trs;

            auxiliar.est << auxiliarEstados;
            auxiliarEstados.trn.clear();
        }

        auxEstados << "";
        Elm.nEtados << auxEstados;
        auxEstados.clear();

        auxiliarEstados.NombreEstado = "";
        auxiliar.est << auxiliarEstados;
        auxiliar.variables_x_maquina = vpm;

        Maquinas << auxiliar ; // ------ Primera Maquina Cargada!
        auxiliar.est.clear();
    }
    auxEstados << "";
    Elm.nEtados << auxEstados;

    Elm.nMaquinas << "";
    auxiliar.NombreMaquina = "";
    Maquinas << auxiliar ; // ------ Ultimo!

    // Datos Generales

    Elm.Fecha  = m_datosXml->getContenidoGeneral(MODIFICACION);
    Elm.Autor = m_datosXml->getContenidoGeneral(AUTORES);
    Elm.Proyecto = m_datosXml->getContenidoGeneral(NOMBRE);
    Elm.Carpetas = m_datosXml->getContenidoGeneral(CARPETA);
}

std::list<QString> MainWindow::obtenerNombresArchivos() {
    // Abrir un diálogo para que el usuario seleccione un directorio
    QString dir = QFileDialog::getExistingDirectory(nullptr, "Seleccionar directorio", "/");

    // Crear un objeto QDir para acceder a los archivos dentro del directorio seleccionado
    QDir directorio(dir);

    // Obtener una lista de nombres de archivos dentro del directorio
    QStringList listaArchivos = directorio.entryList(QDir::Files);

    // Convertir la lista de QString a una lista de std::string
    std::list<QString> listaArchivosStd;
    for (QString archivo : listaArchivos) {
        listaArchivosStd.push_back(dir+"/"+archivo);
    }

    return listaArchivosStd;
}

void MainWindow::on_actionGenerarDocumentacion_triggered() {
    Documentacion docManager;
    int i;

    //Guardo los datos del proyecto
    guardarDatos();

    //Recupero datos basicos
    QString tituloProy = m_datosXml->getContenidoGeneral(NOMBRE);
    docManager.setTituloProyecto(tituloProy);

    QString pathProy = m_datosXml->getContenidoGeneral(CARPETA);
    docManager.setPathProyecto(pathProy);

    QString fechaProy = m_datosXml->getContenidoGeneral(MODIFICACION);
    docManager.setFecha(fechaProy);

    QString autoresProy = m_datosXml->getContenidoGeneral(AUTORES);
    docManager.setAutores(autoresProy);

    QString pathSalidaOdt = pathProy+"/doc_"+tituloProy+"/documentacion.odt";

    //Exporto imagenes de diagramas y copio logo Utn a carpeta proyecto
    std::list<QString> diagramas = exportarImagenesDiagramas();
    QString logoUtn = pathProy+"/doc_"+tituloProy+"/logoUTN.png";
    QFile::copy(":/img/img/logoUTN.png",logoUtn);
    docManager.setLogoUtn(logoUtn);

    //Cargo Maquinas de estados
    int cantMaq = m_sceneList.count();
    for(int i=0; i<cantMaq; i++) {
        QString nombreMaq = m_datosXml->getContenidoMaquina(MAQUINA+QString::number((i)),NOMBRE_MAQUINA);
        QString path = pathProy+"/doc_"+tituloProy+"/capturaMdE" + QString::number(i) + ".png";
        /*
            Aca generar tabla de estados y transiciones
        */
        MaquinaDeEstados maq  = {nombreMaq, path, tableOdt(2,2)};
        docManager.addMaquina(maq);
    }

    //Cargo lista de eventos
    std::list<QString> eventos;
    int cantEventos = m_datosXml->cantidadEventos();
    for(i = 0; i < cantEventos; i++) {
        QString evento1 = m_datosXml->getContenidoEvento(EVENTO+QString::number(i));
        eventos.push_back(evento1);
    }
    if(i==0)
        eventos.push_front(QString("No hay"));
    docManager.setEventos(eventos);

    //Cargo lista acciones
    std::list<QString> acciones;
    int cantAcciones = m_datosXml->cantidadAcciones();
    for(i = 0; i < cantAcciones; i++) {
        QString accion1 = m_datosXml->getContenidoAccion(ACCION+QString::number(i));
        if(accion1.contains('=')==false) {
            acciones.push_back(accion1);
        }
    }
    if(i==0)
        acciones.push_front(QString("No hay"));
    docManager.setAcciones(acciones);

    //Cargo lista variables
    std::list<QString> variables;int cantVariables = m_datosXml->cantidadVariables();
    for(i = 0; i < cantVariables; i++) {
        QString variable1 = m_datosXml->getContenidoVariable(VARIABLE+QString::number(i));
        variables.push_back(variable1);
    }
    if(i==0)
        variables.push_front(QString("No hay"));
    docManager.setVariables(variables);

    //Cargo info teorica
    docManager.setIntroTeorica(true);
    InfoTeorica info;
    info.estados = "Estados: definicion correspondiente";
    info.reset = "Reset: definicion correspondiente";
    info.eventos = "Evetos: definicion correspondiente";
    info.transiciones = "Transicion: definicion correspondiente";
    info.acciones = "Acciones: definicion correspondiente";
    docManager.setInfoTeorica(info);






    //Cargo codigos
    /*************************************************************
    ------------------------PROVISORIO----------------------------
    *************************************************************/
    // Crea un QMessageBox con dos botones
    QMessageBox msgBox;
    msgBox.setText("Si desea incluir las implementaciones en C seleccione la ruta donde se encuentran los códigos generados desde el botón open. De lo contrario cierre esta ventana");
    msgBox.setStandardButtons(QMessageBox::Open | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Open);
    int result = msgBox.exec();
    if(result == QMessageBox::Open ) {
        docManager.setCodigos(true);
        std::list<QString> pathsCodes = obtenerNombresArchivos();
        if(pathsCodes.size()==0)
            docManager.setCodigos(false);
        docManager.setPathsCodigos(pathsCodes);
    } else {
        docManager.setCodigos(false);
    }
    /*************************************************************
    ------------------------PROVISORIO----------------------------
    *************************************************************/









    //Genero documentos .html y .odt
    //docManager.generarDocumentacionFormatoHtml();
    docManager.generarDocumentacionFormatoOdt(DOCUMENTACION_EXE_PATH, pathSalidaOdt);

    //Doy aviso de que se generó todo ok
    QMessageBox::information(this, QString::fromUtf8("Generar Documentación"),
                             QString::fromUtf8("Se ha creado con éxito la documentación del proyecto."));
}

std::list<QString> MainWindow::exportarImagenesDiagramas() {
    int i;
    QString tituloProy = m_datosXml->getContenidoGeneral(NOMBRE);
    QString pathProy = m_datosXml->getContenidoGeneral(CARPETA);
    std::list<QString> pathsList;

    // genera la carpeta doc
    QDir().mkpath(pathProy+"/doc_"+tituloProy);

    int cantMaq = m_sceneList.count();

    // Recorro cada escena para encontrar el boundingRect de todos los items
    for(i = 0 ; i < cantMaq ; i++) {
        DiagramScene *scene = m_sceneList.at(i);
        QRectF rect;
        foreach(DiagramItem *item, scene->itemList()) {
          rect = rect.united(item->sceneBoundingRect());
          foreach(DiagramTransition *arrow, item->transitions()) {
              rect = rect.united(arrow->boundingRect());
              // Agergar lógica para incorporar el texto de la transición!
          }
        }

        // Agrego 100 px de márgen
        rect.adjust(-100, -100, 100, 100);

        // Genero una imagen con fondo blanco y uso directamente el render de la escena
        QImage img(rect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
        img.fill(Qt::white);
        QPainter painter(&img);
        painter.setRenderHint(QPainter::Antialiasing);
        scene->render(&painter, img.rect(), rect.toRect());

        // Guardo la imagen en .png
        QString path = pathProy+"/doc_"+tituloProy+"/capturaMdE" + QString::number(i) + ".png";
        img.save(path);

        //Guardo paths para despues generar Doc
        pathsList.push_back(path);
    }
    return pathsList;
}

void MainWindow::on_actionConfigurarDepuracion_triggered()
{
    DialogConfigurarDebug dialogConfiguracion(this, m_debugTiempoReal, m_generarArchivoDebugLog,
                                    m_replayDebugFileName);
    dialogConfiguracion.setWindowTitle("Proceso de Depuración");

    if (dialogConfiguracion.exec() == QFileDialog::Accepted) {
        if ((m_debugTiempoReal = dialogConfiguracion.debugTiempoRealEnabled()) == true) {
            m_generarArchivoDebugLog = dialogConfiguracion.generarArchivoLogEnabled();
            m_portName = dialogConfiguracion.getPortName();
            //valor devuelto correspondiente al nombre del puerto
            if(m_portName != "")
            {
                ui->actionDepurar->setEnabled(true);
                ui->actionDepurar->setText("Depurar ["+m_portName+"]");
            }
            else
            {
                ui->actionDepurar->setText("Depurar");
                // Deshabilito Debug si no se especifica un Puerto válido
                ui->actionDepurar->setDisabled(true);
            }
        } else {
            m_replayDebugFileName = dialogConfiguracion.getArchivoDebugLogName();
            if(!m_replayDebugFileName.isEmpty()) {
                ui->actionDepurar->setEnabled(true);
                ui->actionDepurar->setText("Depurar [Replay Debug Log]");
            }
        }
    }
}

void MainWindow::on_actionDepurar_triggered()
{
    if(m_uiMode != MainWindow::Depurar) {
        guardarDatos();

		int cantMaq = m_sceneList.count();
        // Chequeo si están definidos todos los estados iniciales
        for(int j=0; j < cantMaq; ++j) {
            if(m_datosXml->getEstadoInicial(MAQUINA+QString::number(j)) == "-1") {
                QMessageBox::warning(this, QLatin1String("Advertencia"), m_sceneList.at(j)->name()
                                     +QLatin1String(": No se encuentra definido el Estado Inicial"));
                ui->actionDepurar->setChecked(false);
                return;
            }
            // Genero los checkbox para seleccionar las MdE
            m_checkBoxMaquinasDebug.append(new QCheckBox(m_datosXml->getContenidoMaquina(MAQUINA+QString::number(j), NOMBRE_MAQUINA), this));
            m_checkBoxMaquinasDebug.at(j)->setChecked(true);
            ui->verticalLayoutMaquinasDebug->addWidget(m_checkBoxMaquinasDebug.at(j));
            connect(m_checkBoxMaquinasDebug.at(j), SIGNAL(clicked()), this, SLOT(on_checkBoxMaquinasDebug_clicked()));
        }

        // Debug Tiempo Real con Conexión Serie
        if(m_debugTiempoReal) {	 
            // Creamos y configuramos el Puerto Serie
            m_serialPortDebug = new QSerialPort(m_portName);
            m_serialPortDebug->setBaudRate(QSerialPort::Baud9600);
            m_serialPortDebug->setFlowControl(QSerialPort::NoFlowControl);
            m_serialPortDebug->setParity(QSerialPort::EvenParity);
            m_serialPortDebug->setDataBits(QSerialPort::Data8);
            m_serialPortDebug->setStopBits(QSerialPort::OneStop);
            // Abrimos el puerto en modo lectura-escritura
            if (m_serialPortDebug->open(QIODevice::ReadWrite) == true) {
                //Conectamos la señal de datos RX
                connect(m_serialPortDebug, SIGNAL(readyRead()), this, SLOT(on_datosRecibidosSerial()));

                // genero archivo debug log y arranco timerCounter para registrar tiempo
                m_timerCounter.restart();
                if(m_generarArchivoDebugLog) {
                    m_archivoDebugLog.setFileName(m_datosXml->getContenidoGeneral(CARPETA)+"/"+
                                                  m_datosXml->getContenidoGeneral(NOMBRE)+"_"+
                                                  QDate::currentDate().toString("dd_MM_yyyy")+"_"+
                                                  QTime::currentTime().toString("hh_mm_ss")+".umfdbg");

                    if (m_archivoDebugLog.open(QIODevice::ReadWrite | QIODevice::Text)) {

                    } else {
                        // Si hubo un error en la apertura...
                        QMessageBox::critical(this, "Error", "No se puede crear el archivo "+
                                              m_archivoDebugLog.fileName());
                    }
                }


            } else {
                // Si hubo un error en la apertura...
                QMessageBox::critical(this, "Error", "No se puede abrir el puerto "+
                                      m_serialPortDebug->portName());
                delete m_serialPortDebug;
                m_serialPortDebug = nullptr;
                ui->actionDepurar->setChecked(false);
                return;
            }
        } else {
            // Debug Modo Diferido
            m_archivoDebugLog.setFileName(m_replayDebugFileName);
            // Cargo el archivo Debug Log y parseo los datos de las transiciones
            if(m_archivoDebugLog.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&m_archivoDebugLog);
                m_debugFileData.clear();
                while (!in.atEnd()) {
                  QString line = in.readLine();
                  m_debugFileData.append(line.split(","));
                }
                m_archivoDebugLog.close();
                // Cargo valores iniciales
                m_indexDebugData = 0;
                m_timeStampActual = 0;
                // Actualizo estado y ui
                m_debugMode = DebugEnPausa;
                ui->labelEstadoDebug->setText("Estado Debug: Listo");
                ui->progressBarDebug->setMaximum(m_debugFileData.last().at(DBG_ID_TIMESTAMP).toInt());
                ui->progressBarDebug->setValue(0);
            } else {
                // Si hubo un error en la apertura...
                QMessageBox::critical(this, "Error", "No se puede abrir el archivo "+
                                      m_replayDebugFileName);
            }
        }
        // Paso al modo Depuración...
        m_uiMode = MainWindow::Depurar;
        // Arranco timer inicial para el reset
        m_timerUpdateDebug.start(T_SIM_RESET_INIT);
        // Actualizo los controles de la ui para Depurar
        if (ui->dockWidgetVerificar->isVisible()) {
            ui->dockWidgetVerificar->hide();
            m_resetDockVerificar = true;
        }
        ui->actionTerminarDepuracion->setEnabled(true);
        if(!m_debugTiempoReal)
            ui->dockWidgetDebug->show();
        ui->dockWidgetProyecto->hide();
        ui->menuVer->removeAction(ui->dockWidgetProyecto->toggleViewAction());
        ui->actionNuevaMaquina->setDisabled(true);
        ui->actionEliminarMaquina->setDisabled(true);
        ui->actionRenombrarMaquina->setDisabled(true);
        ui->menuDefinir->setDisabled(true);
        ui->actionEstadoInicial->setDisabled(true);
        ui->actionSeleccionar->setDisabled(true);
        ui->actionNuevoEstado->setDisabled(true);
        ui->actionNuevaTransicion->setDisabled(true);
        ui->actionSimular->setDisabled(true);
        ui->actionGenerarCodigo->setDisabled(true);
        ui->actionGenerarDocumentacion->setDisabled(true);
        ui->actionVerificar->setDisabled(true);
        ui->actionAbrirProyecto->setDisabled(true);
        ui->actionNuevoProyecto->setDisabled(true);
        ui->actionCerrarProyecto->setDisabled(true);
        ui->actionGuardar->setDisabled(true);
        ui->actionGuardarComo->setDisabled(true);
        ui->actionEliminarElemento->setDisabled(true);
        ui->menuImportarRecursos->setDisabled(true);
        ui->actionConfigurarDepuracion->setDisabled(true);
        ui->tableWidgetSimAcciones->clearContents();
        ui->tableWidgetSimEventos->clearContents();

        m_indexEstadosActuales.clear();
        // Inicializo todas las máquinas y organizo las subWindows
        for(int j=0; j < cantMaq; ++j) {
            QMdiSubWindow *subWindow = ui->mdiArea->subWindowList().at(j);
            if (!subWindow->isVisible()) {
                subWindow->show();
                ui->mdiArea->tileSubWindows();
            }
            m_sceneList.at(j)->resetItem()->setActive(true);   // activo el estado de reset
            m_sceneList.at(j)->setMode(DiagramScene::Simulation);
            m_sceneList.at(j)->views().first()->setDragMode(QGraphicsView::NoDrag);
            m_indexEstadosActuales << -1;
        }

    } else {
        // Si ya estaba en modo Debug, no hago nada...
        ui->actionDepurar->setChecked(true);
    }
}

void MainWindow::on_actionTerminarDepuracion_triggered()
{
    ui->actionConfigurarDepuracion->setEnabled(true);
    ui->actionDepurar->setEnabled(true);
	ui->actionDepurar->setChecked(false);									 
    ui->actionTerminarDepuracion->setEnabled(false);

    // Cerrar Conexión Serie
    if(m_serialPortDebug != nullptr) {
        m_serialPortDebug->close();
        delete m_serialPortDebug;
        m_serialPortDebug = nullptr;
    }
    // Cerrar Archivo Debug Log
    if(m_archivoDebugLog.isOpen())
        m_archivoDebugLog.close();

    int cantMaq = m_sceneList.count();
    for(int j=0; j < cantMaq; ++j) {
        m_sceneList.at(j)->setMode(DiagramScene::MoveItem);
        m_sceneList.at(j)->views().first()->setDragMode(QGraphicsView::RubberBandDrag);
        if (m_indexEstadosActuales.at(j) != -1) {
            m_sceneList.at(j)->previousArrow()->setActive(false);
            m_sceneList.at(j)->setPreviousArrow(nullptr);
            m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->setActive(false);
        } else {
             m_sceneList.at(j)->resetItem()->setActive(false);
        }
    }

    m_uiMode = MainWindow::Editar;
    m_timerUpdateDebug.stop();

    // Actualizo controles ui
    foreach (QCheckBox *checkBox, m_checkBoxMaquinasDebug) {
        ui->verticalLayoutMaquinasDebug->removeWidget(checkBox);
        checkBox->deleteLater();
    }
    m_checkBoxMaquinasDebug.clear();

    ui->actionTerminarSimulacion->setDisabled(true);
    ui->actionConfigurarDepuracion->setEnabled(true);
    ui->actionSimular->setEnabled(true);
    ui->actionNuevaMaquina->setEnabled(true);
    ui->actionEliminarMaquina->setEnabled(true);
    ui->actionRenombrarMaquina->setEnabled(true);
    ui->menuDefinir->setEnabled(true);
    ui->actionEstadoInicial->setEnabled(true);
    ui->actionSeleccionar->setEnabled(true);
    ui->actionNuevoEstado->setEnabled(true);
    ui->actionNuevaTransicion->setEnabled(true);
    ui->actionGenerarCodigo->setEnabled(true);
    ui->actionGenerarDocumentacion->setEnabled(true);
    ui->actionEliminarElemento->setEnabled(true);
    ui->actionVerificar->setEnabled(true);
    ui->actionAbrirProyecto->setEnabled(true);
    ui->actionNuevoProyecto->setEnabled(true);
    ui->actionCerrarProyecto->setEnabled(true);
    ui->actionGuardar->setEnabled(true);
    ui->actionGuardarComo->setEnabled(true);
    ui->menuImportarRecursos->setEnabled(true);

    if(m_resetDockVerificar == true) {
        m_resetDockVerificar = false;
        ui->dockWidgetVerificar->show();
        tabifyDockWidget(ui->dockWidgetProyecto, ui->dockWidgetVerificar);
    }
    ui->tableWidgetSimAcciones->setRowCount(0);
    ui->tableWidgetSimEventos->setRowCount(0);
    ui->dockWidgetDebug->hide();
    ui->dockWidgetProyecto->show();
    ui->menuVer->addAction(ui->dockWidgetProyecto->toggleViewAction());
}

void MainWindow::on_pushButtonDebugStop_clicked()
{
    // Detengo la Depuración...
    m_debugMode = DebugFinalizado;
    ui->labelEstadoDebug->setText("Estado Debug: Detenido");
    ui->pushButtonDebugPlay->setChecked(false);

    // Detengo Timers y reinicio variables
    m_timerUpdateDebug.stop();
    m_timerReplayDebug.stop();
    m_timerProgresoDebug.stop();
    ui->progressBarDebug->setValue(0);
    m_indexDebugData = 0;
    m_timeStampActual = 0;

    // Vuelvo a inicializar todas las máquinas
    int cantMaq = m_sceneList.count(); 
    for(int j=0; j < cantMaq; ++j) {
        if (m_indexEstadosActuales.at(j) != -1) {
            m_sceneList.at(j)->previousArrow()->setActive(false);
            m_sceneList.at(j)->setPreviousArrow(nullptr);
            m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->setActive(false);
        }
    }
    // Activo todos los estados de reset
    m_indexEstadosActuales.clear();
    for(int j=0; j < cantMaq; ++j) {
        m_sceneList.at(j)->resetItem()->setActive(true);
        m_indexEstadosActuales << -1;
    }
}

void MainWindow::on_pushButtonDebugPlay_clicked()
{
    bool updateTimer = false;
    bool finalArchivo = false;

    // Si no estoy en modo Play
    if(m_debugMode != DebugRunning) {
        // Si había finalizado, reinicio indexDebug
        if(m_debugMode == DebugFinalizado) {
            m_indexDebugData = 0;
            m_timeStampActual = 0;
            ui->progressBarDebug->setValue(0);
        }
        // Paso a modo Play y actualizo ui
        m_debugMode = DebugRunning;
        ui->labelEstadoDebug->setText("Estado Debug: Corriendo");
        // Inicio la búsqueda de la próxima transición válida para disparar el Timer
        m_idMaquinaRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_MAQUINA).toInt();
        qDebug() << "[Play] indexDbgData = " << m_indexDebugData;
        // Verifico si la Máquina de la transición actual está seleccionada para el replay
        if(m_checkBoxMaquinasDebug.at(m_idMaquinaRx)->isChecked()) {
            updateTimer = true;
            qDebug() << "[Play] updateTimer MaqID = " << m_idMaquinaRx;
        } else {
            // Si no, busco la próxima transición que corresponda a una Máquina seleccionada
            do {
                if(m_indexDebugData < m_debugFileData.count()-1) {
                    m_indexDebugData++;
                    m_idMaquinaRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_MAQUINA).toInt();
                    qDebug() << "[Play] busco proxTr MaqID = " << m_idMaquinaRx << "indexDbgData = " << m_indexDebugData;
                }
                else {
                    // Si llegué al final del archivo
                    finalArchivo = true;
                    break;
                }
            } while (!m_checkBoxMaquinasDebug.at(m_idMaquinaRx)->isChecked());
            if(!finalArchivo)
                updateTimer = true;
        }
        // Si encontré una transición válida, disparo el timer para el Replay
        if(updateTimer == true) {//////
            // Cargo los datos de la transición actual
            m_idEstadoRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_ESTADO).toInt();
            m_idEventoRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_EVENTO).toInt();
            int timeStampNuevo = m_debugFileData.at(m_indexDebugData).at(DBG_ID_TIMESTAMP).toInt();
            qDebug() << "[Play] DebugData: "<< m_idMaquinaRx << m_idEstadoRx << m_idEventoRx << timeStampNuevo;
            // Arranco el timer para disparar la transición
            m_timerReplayDebug.setInterval(timeStampNuevo-m_timeStampActual);
            m_timerReplayDebug.start();
            // Arranco timer de actualización de progressBar
            m_timerProgresoDebug.setInterval(T_DBG_PROGRESO);
            m_timerProgresoDebug.start();
            qDebug() << "[Play] TimerStart = "<< (timeStampNuevo-m_timeStampActual);
            // Arranco el timer para disparar/////
        } else {
            // Si no encontré ninguna válida, terminó el Replay
            QMessageBox::information(this, "Depuración: Replay desde archivo...",
                                  "Se alcanzó el final del archivo actual.");
        }
    } else {
        // si ya estoy en Play, no hago nada...
        ui->pushButtonDebugPlay->setChecked(true);
    }

}

void MainWindow::on_pushButtonDebugPausa_clicked()
{
    // Si estaba en Play detengo timers
    if(m_debugMode == DebugRunning) {
        qDebug() << "[Pausa] timeStampActual= " << m_timeStampActual;
        // Actualizo timeStamp actual (+ intervalo actual - lo que faltaba) y detengo el Timer
        m_timeStampActual += (m_timerReplayDebug.interval()-m_timerReplayDebug.remainingTime());
        m_timerReplayDebug.stop();
        qDebug() << "[Pausa] timeStampPausado= " << m_timeStampActual;
        // Actualizo progressBar (+ 100ms - lo que faltaba) y detengo el Timer
        qDebug() << "[Pausa] remainingProgreso = " << m_timerProgresoDebug.remainingTime();
        ui->progressBarDebug->setValue(ui->progressBarDebug->value()+
                                       (T_DBG_PROGRESO-m_timerProgresoDebug.remainingTime()));
        m_timerProgresoDebug.stop();        
        ui->pushButtonDebugPlay->setChecked(false);
    }
    // Paso a modo Pausa
    m_debugMode = DebugEnPausa;
    ui->labelEstadoDebug->setText("Estado Debug: Pausado");
}

void MainWindow::on_pushButtonDebugStepFw_clicked()
{
    bool update = false;
    bool finalArchivo = false;

    // Si estoy en Pausa o Detenido
    if(m_debugMode != DebugRunning) {
        m_idMaquinaRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_MAQUINA).toInt();
        qDebug() << "[Step] indexDbgData = " << m_indexDebugData;
        // Verifico si la Máquina de la transición actual está seleccionada para el replay
        if(m_checkBoxMaquinasDebug.at(m_idMaquinaRx)->isChecked()) {
            update = true;
            qDebug() << "[Step] update MaqID = " << m_idMaquinaRx;
        } else {
            // Si no, busco la próxima transición que corresponda a una Máquina seleccionada
            do {
                if(m_indexDebugData < m_debugFileData.count()-1) {
                    m_indexDebugData++;
                    m_idMaquinaRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_MAQUINA).toInt();
                    qDebug() << "[Step] busco proxTr MaqID = " << m_idMaquinaRx << "indexDbgData = " << m_indexDebugData;
                }
                else {
                    finalArchivo = true;
                    break;
                }
            } while (!m_checkBoxMaquinasDebug.at(m_idMaquinaRx)->isChecked());
            if(!finalArchivo)
                update = true;
        }
        // Si encontré una transición válida, actualizo la escena
        if(update == true) {
            m_idEstadoRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_ESTADO).toInt();
            m_idEventoRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_EVENTO).toInt();
            int timeStampNuevo = m_debugFileData.at(m_indexDebugData).at(DBG_ID_TIMESTAMP).toInt();
            m_timeStampActual = timeStampNuevo;
            qDebug() << "[Step] Debug Data "<< m_idMaquinaRx << m_idEstadoRx << m_idEventoRx << timeStampNuevo;
            ui->progressBarDebug->setValue(timeStampNuevo);
            updateMonitorDebug();
            m_debugMode = DebugEnPausa;
            ui->labelEstadoDebug->setText("Estado Debug: Pausado");
            // Incremeto el index Debug para la próxima transición
            if(m_indexDebugData < m_debugFileData.count()-1) {
                m_indexDebugData++;
            } else {
                finalArchivo = true;
            }
        } else {
            finalArchivo = true;
            ui->progressBarDebug->setValue(ui->progressBarDebug->maximum());
        }
    } else {
        // Si estaba en Play, detengo la temporización del replay de transiciones y progreso
        m_timerReplayDebug.stop();
        m_timerProgresoDebug.stop();
        // Actualizo la escena con la transición actual (no espero al vencimiento del timer)
        updateMonitorDebug();
        // Paso a modo Pausa y actualizo ui
        m_debugMode = DebugEnPausa;
        ui->labelEstadoDebug->setText("Estado Debug: Pausado");
        ui->pushButtonDebugPlay->setChecked(false);
        int timeStampNuevo = m_debugFileData.at(m_indexDebugData).at(DBG_ID_TIMESTAMP).toInt();
        ui->progressBarDebug->setValue(timeStampNuevo);
        qDebug() << "[Step (desdePlay)] DebugData = " << m_idMaquinaRx << "indexDbgData = " << m_indexDebugData;
        // Incremeto el index Debug para la próxima transición
        if(m_indexDebugData < m_debugFileData.count()-1) {
            m_indexDebugData++;
        } else {
            // Si llegué al final del archivo
            finalArchivo = true;
        }
    }
    // Mensaje aviso final de archivo debug
    if(finalArchivo) {
        m_debugMode = DebugFinalizado;
        ui->labelEstadoDebug->setText("Estado Debug: Finalizado");
        // Si se llegó al final del archivo de debug log
        QMessageBox::information(this, "Depuración: Replay desde archivo...",
                              "Se alcanzó el final del archivo actual.");
    }
}

void MainWindow::on_doubleSpinBoxEscalaTiempoDebug_valueChanged(double arg1)
{

}

void MainWindow::on_checkBoxMaquinasDebug_clicked()
{
    // Por ahora no hago nada...
    QCheckBox *checkBox = qobject_cast<QCheckBox*>(sender());

    int indexMaq = m_checkBoxMaquinasDebug.indexOf(checkBox);

    qDebug() << checkBox->text() << checkBox->isChecked();

}

void MainWindow::on_replayDebugTimeout()
{
    bool updateTimer = false;
    bool finalArchivo = false;

    // Actualizo timeStamp actual (con la transición actual)
    m_timeStampActual = m_debugFileData.at(m_indexDebugData).at(DBG_ID_TIMESTAMP).toInt();

    qDebug() << "[replayTimeout] timeStamp " << m_timeStampActual << "indexDbgData" << m_indexDebugData;

    // Actualizo escena
    updateMonitorDebug();

    // Busco la próxima transición válida para setear el nuevo intervalo del timer
    if(m_indexDebugData < m_debugFileData.count()-1) {
        m_indexDebugData++;
        qDebug() << "[replayTimeout] next indexDbgData = " << m_indexDebugData;
        m_idMaquinaRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_MAQUINA).toInt();
        // Verifico si la Máquina de la transición actual está seleccionada para el replay
        if(m_checkBoxMaquinasDebug.at(m_idMaquinaRx)->isChecked()) {
            updateTimer = true;
        } else {
            // Si no, busco la próxima transición que corresponda a una Máquina seleccionada
            do {
                if(m_indexDebugData < m_debugFileData.count()-1) {
                    m_indexDebugData++;
                    qDebug() << "[replayTimeout] busco proxTr indxDbgData " << m_indexDebugData;
                    m_idMaquinaRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_MAQUINA).toInt();
                }
                else {
                    // Si llegué al final del archivo
                    finalArchivo = true;
                    break;
                }
            } while (!m_checkBoxMaquinasDebug.at(m_idMaquinaRx)->isChecked());
            if(!finalArchivo)
                updateTimer = true;
        }
        // Si encontré una transición válida, vuelvo a disparar el timer
        if(updateTimer == true) {
            qDebug() << "[replayTimeout] updateTimer indexDbg = " << m_indexDebugData;
            m_idEstadoRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_ESTADO).toInt();
            m_idEventoRx = m_debugFileData.at(m_indexDebugData).at(DBG_ID_EVENTO).toInt();
            int timeStamp = m_debugFileData.at(m_indexDebugData).at(DBG_ID_TIMESTAMP).toInt();
            qDebug() << "[replayTimeout] timeStampSiguiente: " << timeStamp << "tiempoActual" << m_timeStampActual
                     << "nuevoTimeout" << (timeStamp-m_timeStampActual);
            if(timeStamp-m_timeStampActual >= 0) {
                m_timerReplayDebug.stop();
                m_timerReplayDebug.setInterval(timeStamp-m_timeStampActual);
                m_timerReplayDebug.start();
            } else {
                // Error en la cuenta para el Timer
                m_debugMode = DebugFinalizado;
                ui->labelEstadoDebug->setText("Estado Debug: Error");
                m_timerReplayDebug.stop();
                QMessageBox::information(this, "Depuración: Replay desde archivo...",
                                      "Se produjo un error en la secuencia de Debug.");
            }

        }
    } else {
        // Si llegué al final del archivo
        finalArchivo = true;
    }
    if(finalArchivo) {
        // Actualizo estado y ui
        m_debugMode = DebugFinalizado;
        ui->labelEstadoDebug->setText("Estado Debug: Finalizado");
        ui->pushButtonDebugPlay->setChecked(false);
        m_timerReplayDebug.stop();
        m_timerProgresoDebug.stop();
        ui->progressBarDebug->setValue(ui->progressBarDebug->maximum());
        QMessageBox::information(this, "Depuración: Replay desde archivo...",
                              "Se alcanzó el final del archivo actual.");
    }
}

void MainWindow::on_progresoDebugTimeout()
{
    if(m_debugMode == DebugRunning) {
        ui->progressBarDebug->setValue(ui->progressBarDebug->value()+T_DBG_PROGRESO);
        // Vuelvo a disparar el timer de actualización de progressBar
        m_timerProgresoDebug.setInterval(T_DBG_PROGRESO);
        m_timerProgresoDebug.start();
    }
}

void MainWindow::on_datosRecibidosSerial()
{
    QByteArray bytesRx;
    int cant = static_cast<int>(m_serialPortDebug->bytesAvailable()); // devuelve la cantidad de bytes disponibles
    bytesRx.resize(cant);

    m_serialPortDebug->read(bytesRx.data(), bytesRx.size()); // guarda en bytesRx los datos recibidos

    m_datosRecibidos.append(bytesRx);

    procesarDatosRecibidosSerial();
}


void MainWindow::procesarDatosRecibidosSerial()
{
    static unsigned int estadoRx = ESPERO_MENSAJE;

    for (int i = 0; i < m_datosRecibidos.count(); i++) {
        char dato = m_datosRecibidos.at(i);
        switch(estadoRx) {
        case ESPERO_MENSAJE:
            if(dato == '$') {
                estadoRx = RECIBO_DATOS_MAQ;
            }
            break;
        case RECIBO_DATOS_MAQ:
            m_idMaquinaRx= dato;
            estadoRx = RECIBO_DATOS_ESTADO;
            break;

        case RECIBO_DATOS_ESTADO:
            // offset de -1 para ajustar idEvento a la lógica actual
            m_idEstadoRx = dato - 1;  // RESET pasa de 0 a -1 
            estadoRx = RECIBO_DATOS_EVENTO;
            break;

        case RECIBO_DATOS_EVENTO:
            m_idEventoRx = dato;
            estadoRx = FIN_DE_TRAMA;
            break;

        case FIN_DE_TRAMA:
            if(dato == '#') {
                // Acepto la trama y actualizo el modelo como monitor
                updateMonitorDebug();
                qDebug() << "RX: "<< m_idMaquinaRx << m_idEstadoRx << m_idEventoRx << m_timerCounter.elapsed();
                if (m_generarArchivoDebugLog && m_archivoDebugLog.isOpen()) {
                   debugData.append(QString::number(m_idMaquinaRx)+","+QString::number(m_idEstadoRx)+","+
                                    QString::number(m_idEventoRx)+","+QString::number(m_timerCounter.elapsed())+"\n");

                    m_archivoDebugLog.write(debugData);
                    debugData.clear();
                }
            } else {
                // descarto trama
                // genero algún log??
            }
            estadoRx = ESPERO_MENSAJE;
            break;
        }
    }
    m_datosRecibidos.clear();
}

void MainWindow::updateMonitorDebug()
{
    // Chequeo para diferenciar entre estados comunes ( >=0 ) / estado inicial ( -1 )
    if(m_idEstadoRx >= 0) {
        DiagramItem *itemActual = m_sceneList.at(m_idMaquinaRx)->itemList().at(m_idEstadoRx);
        // Busco el evento recibido en el diagrama para actualizar la transición
        QString evento = m_datosXml->getContenidoTransicion(ESTADO+QString::number(m_idEstadoRx),
                                                            ID_TRANS+QString::number(m_idEventoRx), ID_EVENTO,
                                                            MAQUINA+QString::number(m_idMaquinaRx));

        foreach (DiagramTransition *arrow, itemActual->transitions()) {
            if ((arrow->startItem() == itemActual) && (arrow->event() == evento)) {
                int idProx = m_sceneList.at(m_idMaquinaRx)->itemList().indexOf(arrow->endItem());
                // Actualizo la escena
                if(m_indexEstadosActuales.at(m_idMaquinaRx) != -1) {
                    m_sceneList.at(m_idMaquinaRx)->itemList().at(m_indexEstadosActuales.at(m_idMaquinaRx))->setActive(false);
                } else {
                    m_sceneList.at(m_idMaquinaRx)->resetItem()->setActive(false);
                }
                m_indexEstadosActuales[m_idMaquinaRx] = idProx;
                m_sceneList.at(m_idMaquinaRx)->itemList().at(idProx)->setActive(true);
                if(m_sceneList.at(m_idMaquinaRx)->previousArrow() != nullptr)
                    m_sceneList.at(m_idMaquinaRx)->previousArrow()->setActive(false);
                arrow->setActive(true);
                m_sceneList.at(m_idMaquinaRx)->setPreviousArrow(arrow);
                break;
            } // Termino la transición
        }
    } else {
        // Si fue un evento de RESET, disparo la transicion inicial
        DiagramTransition *arrow = m_sceneList.at(m_idMaquinaRx)->resetItem()->transitions().at(0);
        int idProx = m_sceneList.at(m_idMaquinaRx)->itemList().indexOf(arrow->endItem());
        if(m_indexEstadosActuales.at(m_idMaquinaRx) == -1) {
            m_sceneList.at(m_idMaquinaRx)->resetItem()->setActive(false);
        } else {
            m_sceneList.at(m_idMaquinaRx)->itemList().at(m_indexEstadosActuales.at(m_idMaquinaRx))->setActive(false);
        }
        m_indexEstadosActuales[m_idMaquinaRx] = idProx;
        m_sceneList.at(m_idMaquinaRx)->itemList().at(idProx)->setActive(true);
        if(m_sceneList.at(m_idMaquinaRx)->previousArrow() != nullptr)
           m_sceneList.at(m_idMaquinaRx)->previousArrow()->setActive(false);
        arrow->setActive(true);
        m_sceneList.at(m_idMaquinaRx)->setPreviousArrow(arrow);
    } // Termino la transición
}

void MainWindow::on_updateDebugTimeout()
{
    int cantMaq = m_sceneList.count();
    for(int j = 0; j < cantMaq; ++j) {
        // Si existe una transición previa, ya no estoy en el estado inicial...
        if(m_sceneList.at(j)->previousArrow() != nullptr) {
            // Actualizo el estado actual para hacerlo titilar
            if (m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->isActive()) {
                m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->setActive(false);
                m_sceneList.at(j)->update();
                m_timerUpdateDebug.start(T_SIM_ESTADO_INACTIVO);
            } else {
                m_sceneList.at(j)->itemList().at(m_indexEstadosActuales.at(j))->setActive(true);
                m_sceneList.at(j)->update();
                m_timerUpdateDebug.start(T_SIM_ESTADO_ACTIVO);
            }
        } else {
            // Si estoy en el estado inicial, en modo Debug por ahora no hago nada particular
            if (m_sceneList.at(j)->resetItem()->isActive()) {
            }
        }
    }
}

void MainWindow::on_nuevaConexionSimTCP()
{
    QTcpSocket *clientSocket = m_serverSim.nextPendingConnection();
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(on_datosRecibidosSimTCP()));
    connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,
        SLOT(on_cambioEnClientSocketSim(QAbstractSocket::SocketState)));
    m_clientSocketsSim.push_back(clientSocket);
    qDebug() << clientSocket->peerAddress().toString() + " se conectó al servidor !\n";
}

void MainWindow::on_cambioEnClientSocketSim(QAbstractSocket::SocketState socketState)
{
    if (socketState == QAbstractSocket::UnconnectedState) {
       QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
       m_clientSocketsSim.removeOne(sender);
    }
}

void MainWindow::on_datosRecibidosSimTCP()
{
    QTcpSocket *sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray data = sender->readAll();
    QString auxiliar(data);
    qDebug() << "Servidor leyó: " << data;

    // Nueva implementación
    QStringList separacion = auxiliar.split("@");
    qDebug() << separacion.at(0);

    for(int a = 0 ; a < separacion.length() ; a++) {
        if(separacion.at(a)!= "") {
            QString eventoRecibido , eR;
            QStringList auxLista = separacion.at(a).split(":");
            eventoRecibido = auxLista.at(1);

            if ( m_simular_con_Infotronic_virtual == true )
            {
                //// Aca tengo que cambiar el evento propietario x el que llego
                // La entrada puede ser un pulsador
                eR = m_Infotronic->GetPulsadoresConvertido(eventoRecibido);

                if ( eR == "") // ... o un estado de una de las entradas digitales
                    eR = m_Infotronic->GetEntradasConvertido(eventoRecibido);

                if ( eR == "") // ... o un estado de una de las entradas digitales
                    eR = m_Infotronic->GeteTemporizadoresConvertido(eventoRecibido);

                if ( eR != "" )
                    eventoRecibido = eR;
            }

            procesarDatosRecibidosSimTCP( eventoRecibido );
        }
    }
}

void MainWindow::procesarDatosRecibidosSimTCP(QString eventoRecibido)
{
    qDebug() << eventoRecibido ;

    int count = ui->tableWidgetSimEventos->rowCount();
    int fila = -1;

    QTableWidgetItem *item = nullptr;

    // Voy a buscar el evento
    for(int row = 0; row < count ; row++) {
       item = ui->tableWidgetSimEventos->item(row, 0);
       if(item->text() == eventoRecibido) {
           // Encontré el evento
           fila = row;
           break;
       }
    }

    if(fila != -1 && m_uiMode == Simular) {
        // Paso el evento recibido como si se hubiera clickeado
        on_tableWidgetSimEventos_cellDoubleClicked(fila, 0);
    }
}

void MainWindow::on_actionImportarRecursosDesdeArchivo_triggered()
{
    // Chequeo si hay que guardar cambios...
    if(m_datosXml != nullptr) {
        int rv = maybeSave();
        if(rv == QMessageBox::Cancel)
            return;
        else if (rv == QMessageBox::Save)
            guardarDatos();
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Importar Recursos Desde Archivo"),
                                                    QDir::homePath(), tr("Proyectos(*.h)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error al Importar Recursos"),
                             tr("No se pudo abrir el archivo \n%1").arg(fileName));
        return;
    }

    QString auxiliar ="";

    QTextStream instream(&file);
    QStringList misDefines;
    QStringList misAcciones;

    while (!instream.atEnd())
    {
        QString line = instream.readLine();

        if(line.contains("#define") == true)
        {
            QStringList auxDefine = line.split("define");
            if(auxDefine.length() > 1)
            {
                line = auxDefine[1].replace('\t',' ');
                QStringList auxDefine2 = line.split(" ");
                if(auxDefine2.length() > 0)
                {
                    qDebug() << auxDefine2.at(1) << endl;
                    misDefines.append(auxDefine2.at(1));
                }
            }
        }
        if(line.contains(";")== true)
        {
            if(line.contains(";") == true)
            {
                QStringList auxAccion = line.split("(");
                if(auxAccion.length() > 1)
                {
                    line = auxAccion[0].replace('\t',' ');

                    QStringList auxAccion2 = line.split(" ");
                    if(auxAccion2.length() > 1)
                    {
                        qDebug() << auxAccion2.at(1) << endl;
                        misAcciones.append(auxAccion2.at(1));
                    }
                }
            }
        }
    }

    file.close();
    //levanto los recursos del proyecto
    QStringList listaAcciones = m_datosXml->getNombresAccionesDefinidas();
    QStringList listaDefines = m_datosXml->getNombresAccionesDefinidas();

    //si no se encuentra en el proyecto actual los agrego
    bool marcaDefine = false;
    for(int i = 0; i < misDefines.length() ; i++)
    {
        for(int j = 0 ;j < listaDefines.length();j++)
        {
            if(listaDefines[j]==misDefines[i])
                marcaDefine = true;
        }
        if(marcaDefine == false)
            m_datosXml->addNuevaConstante(misDefines[i]);

        marcaDefine = false;
    }

    bool marcaAccion = false;
    for(int i = 0; i < misAcciones.length() ; i++)
    {
        for(int j = 0 ;j < listaAcciones.length();j++)
        {
            if(listaAcciones[j]==misAcciones[i])
                marcaAccion = true;
        }
        if(marcaAccion == false)
        {
            if(misAcciones[i].contains("(")== true)
                m_datosXml->addNuevaAccion(misAcciones[i]);
            else {
                m_datosXml->addNuevaAccion(misAcciones[i]+ "()");
            }
        }
        marcaAccion = false;
    }

    guardarDatos();
    updateExploradorProyecto();
}

void MainWindow::on_actionImportarRecursosDesdeOtroProyecto_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Importar Proyecto"),
                                                        QDir::homePath(), tr("Proyectos(*.umf)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error al Importar el Proyecto"),
                             tr("No se pudo abrir el archivo \n%1").arg(fileName));
        return;
    }

    XmlHandler *m_copiaDatosXml = new  XmlHandler();
    if (!m_copiaDatosXml->load(file)) {
        QMessageBox::warning(this, tr("Error al Importar el Proyecto"),
                            tr("No se pudo procesar el archivo \n%1").arg(fileName));
        return;
    }

    file.close();

    //levanto los recursos del proyecto a importar
    QStringList datosAcciones =  m_copiaDatosXml->getNombresAccionesDefinidas();
    QStringList datosEventos = m_copiaDatosXml->getNombresEventosDefinidos();
    QStringList datosVariables = m_copiaDatosXml->getNombresVariablesDefinidas();
    QStringList datosConstantes =  m_copiaDatosXml->getNombresConstantesDefinidas();

    //levanto los recursos del proyecto de origen
    QStringList listaAcciones =  m_datosXml->getNombresAccionesDefinidas();
    QStringList listaEventos = m_datosXml->getNombresEventosDefinidos();
    QStringList listaVariables = m_datosXml->getNombresVariablesDefinidas();
    QStringList listaConstantes =  m_datosXml->getNombresConstantesDefinidas();

    //si no se encuentra en el proyecto actual los agrego
    bool marcaDefine = false;
    for(int i = 0; i < datosConstantes.length() ; i++)
    {
        for(int j = 0 ;j < listaConstantes.length();j++)
        {
            if(listaConstantes[j]==datosConstantes[i])
                marcaDefine = true;
        }
        if(marcaDefine == false)
            m_datosXml->addNuevaConstante(datosConstantes[i]);

        marcaDefine = false;
    }

    bool marcaAccion = false;
    for(int i = 0; i < datosAcciones.length() ; i++)
    {
        for(int j = 0 ;j < listaAcciones.length();j++)
        {
            if(listaAcciones[j]==datosAcciones[i])
                marcaAccion = true;
        }
        if(marcaAccion == false)
        {
            if(datosAcciones[i].contains("(")== true)
                m_datosXml->addNuevaAccion(datosAcciones[i]);
            else {
                m_datosXml->addNuevaAccion(datosAcciones[i]+ "()");
            }
        }
        marcaAccion = false;
    }

    bool marcaEvento = false;
    for(int i = 0; i < datosEventos.length() ; i++)
    {
        for(int j = 0 ;j < listaEventos.length();j++)
        {
            if(listaEventos[j]==datosEventos[i])
                marcaEvento = true;
        }
        if(marcaEvento == false)
            m_datosXml->addNuevoEvento(datosEventos[i]);

        marcaEvento = false;
    }

    bool marcaVariable = false;
    for(int i = 0; i < datosVariables.length() ; i++)
    {
        for(int j = 0 ;j < listaVariables.length();j++)
        {
            if(listaVariables[j]==datosVariables[i])
                marcaVariable = true;
        }
        if(marcaVariable == false)
            m_datosXml->addNuevaVariable(datosVariables[i]);

        marcaVariable = false;
    }

    guardarDatos();
    updateExploradorProyecto();
}

int MainWindow::maybeSave()
{
    if (m_modificado == true) {
        QMessageBox msgBox;
        msgBox.addButton(tr("No"), QMessageBox::DestructiveRole);
        QPushButton *si = msgBox.addButton(tr("Si"), QMessageBox::AcceptRole);
        QPushButton *cancelar = msgBox.addButton(tr("Cancelar"),QMessageBox::RejectRole);

        msgBox.setText("El proyecto fue modificado.\n ¿Desea guardar los cambios antes de cerrarlo?");
        msgBox.setWindowTitle(tr("uModelFactory"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();

        if(msgBox.clickedButton() == cancelar)
            return QMessageBox::Cancel;

        else if (msgBox.clickedButton() == si)
            return QMessageBox::Save;
    }

    return QMessageBox::Discard;
}

void MainWindow::guardarDatos()
{
    m_datosXml->setContenidoGeneral(MODIFICACION,
                                     QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"));
    m_datosXml->borrarMaquinas();
    // Guarda los datos del diagrama
    int cantMaquinas = m_sceneList.count();
    for (int k = 0; k < cantMaquinas; ++k) {
        DiagramScene *scene = m_sceneList.at(k);
        m_datosXml->addNuevaMaquina(MAQUINA+QString::number(k));
        m_datosXml->addInfoMaquina(MAQUINA+QString::number(k), scene->name());
        for (int i = 0; i < scene->itemList().count(); ++i) {
            // Genero la estructura para cada estado
            DiagramItem *item = scene->itemList().at(i);

            m_datosXml->addNuevoEstado(ESTADO+QString::number(i), MAQUINA+QString::number(k));

            m_datosXml->addInfoEstado(ESTADO+QString::number(i), item->name(),
                                            item->color().name(), QString::number(item->pos().x()),
                                            QString::number(item->pos().y()), QString::number(item->rect().width()),
                                            QString::number(item->rect().height()), MAQUINA+QString::number(k));
            // Estructura para las transiciones
            int j=0, idProx;
            foreach (DiagramTransition *arrow, item->transitions()) {
                if(arrow->startItem() == item) {
                    idProx = scene->itemList().indexOf(arrow->endItem());
                    QString mid_pos = QString::number(arrow->midItem()->scenePos().x())+","
                            +QString::number(arrow->midItem()->scenePos().y());
                    QString text_pos = QString::number(arrow->textRelativePos().x())+","
                            +QString::number(arrow->textRelativePos().y());

                    m_datosXml->addNuevaTransicion(ESTADO+QString::number(i), ID_TRANS+QString::number(j),
                                                   arrow->event(), arrow->actions().join(","),
                                                   ESTADO+QString::number(idProx), mid_pos, text_pos,
                                                   MAQUINA+QString::number(k));
                    ++j;
                }
            }
        }
        // Estructura del estado inicial
        DiagramItem *initItem = scene->resetItem();
        if(initItem != nullptr) {
            DiagramTransition * arrow = initItem->transitions().first();
            int idProx = scene->itemList().indexOf(arrow->endItem());
            QString mid_pos = QString::number(arrow->midItem()->scenePos().x())+","
                    +QString::number(arrow->midItem()->scenePos().y());
            QString text_pos = QString::number(arrow->textRelativePos().x())+","
                    +QString::number(arrow->textRelativePos().y());

            m_datosXml->setInfoEstadoInicial(QString::number(initItem->pos().x()), QString::number(initItem->pos().y()),
                                             arrow->actions().join(","), ESTADO+QString::number(idProx),
                                             mid_pos, text_pos, MAQUINA+QString::number(k));
        } else
            m_datosXml->borrarEstadoInicial(MAQUINA+QString::number(k));
    }

    // Guardo el archivo XML
    m_datosXml->save();
    m_modificado = false;
    if(windowTitle().endsWith("*"))
        setWindowTitle(windowTitle().remove("*"));
}

void MainWindow::uiDisable()
{
    ui->actionNuevaMaquina->setDisabled(true);
    ui->actionEliminarMaquina->setDisabled(true);
    ui->actionRenombrarMaquina->setDisabled(true);
    ui->menuDefinir->setDisabled(true);
    ui->actionNuevoEstado->setDisabled(true);
    ui->actionEstadoInicial->setDisabled(true);
    ui->actionEliminarElemento->setDisabled(true);
    ui->actionGuardar->setDisabled(true);
    ui->actionGuardarComo->setDisabled(true);
    ui->actionGenerarCodigo->setDisabled(true);
    ui->actionGenerarDocumentacion->setDisabled(true);
    ui->actionSeleccionar->setDisabled(true);
    ui->actionNuevaTransicion->setDisabled(true);
    ui->actionCerrarProyecto->setDisabled(true);
    ui->actionSimular->setDisabled(true);
    ui->actionTerminarSimulacion->setDisabled(true);
    ui->actionVerificar->setDisabled(true);
    ui->actionMosaico->setDisabled(true);
    ui->actionCascada->setDisabled(true);
    ui->menuImportarRecursos->setDisabled(true);
    ui->actionConfigurarDepuracion->setDisabled(true);
    ui->actionDepurar->setDisabled(true);
    ui->actionTerminarDepuracion->setDisabled(true);

    m_sceneScaleCombo->setDisabled(true);
    m_zoomLabel->setDisabled(true);

    ui->treeWidgetProyecto->clear();
    ui->dockWidgetProyecto->setDisabled(true);
    ui->tableWidgetTransiciones->clearContents();
    ui->tableWidgetTransiciones->setRowCount(0);
    ui->dockWidgetTransiciones->setDisabled(true);
    ui->dockWidgetTransiciones->setWindowTitle("Tabla de Estados y Transiciones");
    ui->dockWidgetVerificar->setDisabled(true);
    ui->dockWidgetVerificar->close();
    //nuevo!
    m_resetDockVerificar = false;
    ui->dockWidgetSimulacion->hide();
    ui->dockWidgetTransiciones->hide();
    ui->dockWidgetDebug->hide();

    ui->dockWidgetAyuda->hide();

    ui->mdiArea->setDisabled(true);
}

void MainWindow::uiEnable()
{
    ui->actionGenerarCodigo->setEnabled(true);
    ui->actionGenerarDocumentacion->setEnabled(true);
    ui->actionSimular->setEnabled(true);
    ui->actionVerificar->setEnabled(true);
    ui->actionSeleccionar->setEnabled(true);
    ui->actionSeleccionar->setChecked(true);
    ui->actionNuevoEstado->setEnabled(true);
    ui->actionNuevaTransicion->setEnabled(true);
    ui->actionEstadoInicial->setEnabled(true);
    ui->actionMosaico->setEnabled(true);
    ui->actionCascada->setEnabled(true);
    ui->actionEliminarElemento->setEnabled(true);
    ui->actionEliminarMaquina->setEnabled(true);
    ui->actionRenombrarMaquina->setEnabled(true);
    ui->menuDefinir->setEnabled(true);
    ui->menuImportarRecursos->setEnabled(true);
    ui->actionConfigurarDepuracion->setEnabled(true);

    m_zoomLabel->setEnabled(true);
    m_sceneScaleCombo->setEnabled(true);

    ui->dockWidgetTransiciones->setEnabled(true);
}

void MainWindow::uiReset()
{
    foreach (DiagramScene *scene, m_sceneList) {
        scene->clear();
        delete scene;
    }
    foreach (QMdiSubWindow *subWindow, ui->mdiArea->subWindowList()) {
        ui->mdiArea->removeSubWindow(subWindow);
        delete subWindow;
    }
    m_sceneList.clear();

    uiDisable();

    m_modificado = false;
    this->setWindowTitle("uModel Factory | UTN-FRBA");
}

void MainWindow::readSettings()
{
    QSettings settings("UTN-FRBA", "umodelfactory");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings("UTN-FRBA", "umodelfactory");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::initSubWindow(const QString &name)
{
    DiagramScene *scene = new DiagramScene(name, m_itemMenu);
    connect(scene, SIGNAL(itemInserted(DiagramItem*)),
            this, SLOT(on_itemInserted(DiagramItem*)));
    connect(scene, SIGNAL(transitionInserted(DiagramTransition*)),
            this, SLOT(on_arrowInserted(DiagramTransition*)));
    connect(scene, SIGNAL(itemSelected(QGraphicsItem*)),
            this, SLOT(on_itemSelected(QGraphicsItem*)));
    connect(scene, SIGNAL(itemDoubleClicked(int)),
            this, SLOT(on_itemDoubleClicked(int)));
    connect(scene, SIGNAL(rightClicked()),
            this, SLOT(on_actionSeleccionar_triggered()));
    connect(scene, SIGNAL(transitionDoubleClicked(DiagramTransition*)),
            this, SLOT(on_arrowDoubleClicked(DiagramTransition*)));

    connect(scene, SIGNAL(selectionChanged()),
            this, SLOT(on_sceneSelectionChanged()));

    // Seteo el tamaño inicial de la escena
    scene->initSceneRect(-SCENE_INITIAL_WIDTH/2, -SCENE_INITIAL_HEIGHT/2,
                         SCENE_INITIAL_WIDTH, SCENE_INITIAL_HEIGHT);
    // Agrego la nueva escena a la lista
    m_sceneList.append(scene);
    // Genero una nueva vista y la configuro
    QGraphicsView *view = new QGraphicsView;
    view->setScene(scene);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setMouseTracking(true);
    view->viewport()->setMouseTracking(true);
    view->centerOn(-SCENE_INITIAL_WIDTH/2, -SCENE_INITIAL_HEIGHT/2);
    // Genero una nueva subwindow, la configuro y la muestro
    SubWindow *subWindow = new SubWindow();
    subWindow->setWidget(view);
    subWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    subWindow->setWindowTitle(name);
    subWindow->setWindowIcon(QIcon(":/img/uModel.ico"));
    ui->mdiArea->addSubWindow(subWindow, Qt::SubWindow);
    QRect r = ui->mdiArea->viewport()->rect();
    r.setHeight(static_cast<int>(r.height()*0.75));
    r.setWidth(static_cast<int>(r.width()*0.75));
    subWindow->setGeometry(r);
    if (ui->mdiArea->subWindowList().count() > 1) {
        subWindow->showNormal();
        ui->mdiArea->tileSubWindows();
    } else {
        subWindow->showMaximized();
    }
}

void MainWindow::initExploradorProyecto()
{
    ui->treeWidgetProyecto->clear();
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->treeWidgetProyecto, TreeItemType::Proyecto);
    rootItem->setText(0, m_datosXml->getContenidoGeneral(NOMBRE));
    rootItem->setFlags(Qt::ItemIsEnabled);
    rootItem->setIcon(0, QIcon(":/img/img/chart_organisation.png"));
    ui->treeWidgetProyecto->addTopLevelItem(rootItem);

    QTreeWidgetItem *item;
    item = new QTreeWidgetItem(rootItem, TreeItemType::Sistema);
    item->setText(0, tr("SISTEMA"));
    item->setFlags(Qt::ItemIsEnabled);
    item->setIcon(0, QIcon(":/img/img/plugin.png"));
    rootItem->addChild(item);
    item = new QTreeWidgetItem(rootItem, TreeItemType::Eventos);
    item->setText(0, tr("EVENTOS"));
    item->setFlags(Qt::ItemIsEnabled);
    item->setIcon(0, QIcon(":/img/img/plugin.png"));
    rootItem->addChild(item);
    item = new QTreeWidgetItem(rootItem, TreeItemType::Acciones);
    item->setText(0, tr("ACCIONES"));
    item->setFlags(Qt::ItemIsEnabled);
    item->setIcon(0, QIcon(":/img/img/plugin.png"));
    rootItem->addChild(item);
    item = new QTreeWidgetItem(rootItem, TreeItemType::Variables);
    item->setText(0, tr("VARIABLES"));
    item->setFlags(Qt::ItemIsEnabled);
    item->setIcon(0, QIcon(":/img/img/plugin.png"));
    rootItem->addChild(item);
    item = new QTreeWidgetItem(rootItem, TreeItemType::Constantes);
    item->setText(0, tr("CONSTANTES"));
    item->setFlags(Qt::ItemIsEnabled);
    item->setIcon(0, QIcon(":/img/img/plugin.png"));
    rootItem->addChild(item);

    ui->treeWidgetProyecto->expandAll();
}

void MainWindow::initTablaTransiciones()
{
    ui->tableWidgetTransiciones->setColumnCount(4);
    ui->tableWidgetTransiciones->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetTransiciones->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetTransiciones->horizontalHeader()->setFont(QFont("Segoe UI"));
    ui->tableWidgetTransiciones->verticalHeader()->hide();

    QTableWidgetItem *headerItem;
    headerItem = new QTableWidgetItem(tr("ESTADO ACTUAL"));
    headerItem->setIcon(QIcon(":/img/img/button.png"));
    headerItem->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidgetTransiciones->setHorizontalHeaderItem(0, headerItem);
    headerItem = new QTableWidgetItem(tr("ESTADO FUTURO"));
    headerItem->setIcon(QIcon(":/img/img/button.png"));
    headerItem->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidgetTransiciones->setHorizontalHeaderItem(1, headerItem);
    headerItem = new QTableWidgetItem(tr("EVENTOS"));
    headerItem->setIcon(QIcon(":/img/img/events.png"));
    headerItem->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidgetTransiciones->setHorizontalHeaderItem(2, headerItem);
    headerItem = new QTableWidgetItem(tr("ACCIONES"));
    headerItem->setIcon(QIcon(":/img/img/external.png"));
    headerItem->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidgetTransiciones->setHorizontalHeaderItem(3, headerItem);
}

void MainWindow::initTablaSimulacion()
{
    ui->tableWidgetSimEventos->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidgetSimEventos->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidgetSimAcciones->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidgetSimAcciones->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::updateUi()
{
    if(!windowTitle().endsWith("*"))
        setWindowTitle(windowTitle()+"*");
}

void MainWindow::updateTablaTransiciones(DiagramScene *scene)
{
    int row = 0;
    QStringList list;

    ui->dockWidgetTransiciones->setWindowTitle("Tabla de Estados y Transiciones [ "+scene->name()+" ]");
    ui->tableWidgetTransiciones->clearContents();
    ui->tableWidgetTransiciones->setRowCount(0);

    if(scene->resetItem()!=nullptr) {
        list.clear();
        list << "-";
        list << scene->resetItem()->transitions().first()->endItem()->name().toUpper();
        list << " (reset) ";
        list << scene->resetItem()->transitions().first()->actions().join(",");
        ui->tableWidgetTransiciones->insertRow(row);
        for(int col = 0; col < 4 ; ++col) {
            QTableWidgetItem *newItem = new QTableWidgetItem(list.at(col));
            newItem->setTextAlignment(Qt::AlignCenter);
            ui->tableWidgetTransiciones->setItem(row, col, newItem);
        }
        row++;
    }   
    for (int i = 0; i < scene->itemList().count(); ++i) {
        // Obtengo los datos de cada estado
        DiagramItem *item = scene->itemList().at(i);
        // Loop para recorrer todas las transiciones del estado
        foreach (DiagramTransition *arrow, item->transitions()) {
            // Chequeo si la transicion saliente (si es entrante, no hago nada)
            if (arrow->startItem() == item) {
                list.clear();
                list << arrow->startItem()->name().toUpper();
                list << arrow->endItem()->name().toUpper();
                list << arrow->event();
                list << arrow->actions().join(",");
                if (list.last().isEmpty())
                    list.last().append("-");
                ui->tableWidgetTransiciones->insertRow(row);
                for(int col = 0; col < 4 ; ++col) {
                    QTableWidgetItem *newItem = new QTableWidgetItem(list.at(col));
                    newItem->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidgetTransiciones->setItem(row, col, newItem);
                }
                row++;
            }
        }
    }
    ui->tableWidgetTransiciones->resizeRowsToContents();
}

void MainWindow::updateExploradorProyecto(const TreeItemType &type)
{
    int cant;
    QStringList list;
    QTreeWidgetItem *parentItem;

    // Actualizo el nombre del proyecto
    if (type == TreeItemType::Proyecto) {
        ui->treeWidgetProyecto->topLevelItem(0)->setText(0, m_datosXml->getContenidoGeneral(NOMBRE));
    }
    // Agrego los Estados de cada Máquina
    if (type == TreeItemType::Sistema || type == TreeItemType::Proyecto) {
        parentItem = ui->treeWidgetProyecto->topLevelItem(0)->child(0);
        qDeleteAll(parentItem->takeChildren());
        if (!m_sceneList.isEmpty()) {
            foreach (DiagramScene *scene, m_sceneList) {
                foreach (DiagramItem *item, scene->itemList()) {
                    if(item->isEstadoInicial())
                        list.append(item->name().toUpper()+" (Inicial)");
                    else
                        list.append(item->name().toUpper());
                }
                addItemMaquina(parentItem, scene->name(), list, ":/img/img/button.png");
                list.clear();
            }
        } else {
            QTreeWidgetItem *maquinaItem = new QTreeWidgetItem(parentItem, TreeItemType::Maquina);
            maquinaItem->setText(0, " - ");
            maquinaItem->setFlags(Qt::ItemIsEnabled);
            maquinaItem->setIcon(0, QIcon(":/img/img/application.png"));
            parentItem->addChild(maquinaItem);
        }
        ui->treeWidgetProyecto->expandItem(parentItem);
    }
    // Agrego los Eventos
    if (type == TreeItemType::Eventos || type == TreeItemType::Proyecto) {
        list.clear();
        cant = m_datosXml->cantidadEventos();
        parentItem = ui->treeWidgetProyecto->topLevelItem(0)->child(1);
        for(int k=0; k < cant; ++k) {
            list.append(m_datosXml->getContenidoEvento(EVENTO+QString::number(k)));
        }
        addItemProyecto(parentItem, list, TreeItemType::ItemEvento, ":/img/img/events.png");
    }
    // Agrego las Acciones
    if (type == TreeItemType::Acciones || type == TreeItemType::Proyecto) {
        list.clear();
        cant = m_datosXml->cantidadAcciones();
        parentItem = ui->treeWidgetProyecto->topLevelItem(0)->child(2);
        for(int k=0; k < cant; ++k) {
            list.append(m_datosXml->getContenidoAccion(ACCION+QString::number(k)));
        }
        addItemProyecto(parentItem, list, TreeItemType::ItemAccion, ":/img/img/external.png");
    }
    // Agrego las Variables
    if (type == TreeItemType::Variables || type == TreeItemType::Proyecto) {
        list.clear();
        cant = m_datosXml->cantidadVariables();
        parentItem = ui->treeWidgetProyecto->topLevelItem(0)->child(3);
        for(int k=0; k < cant; ++k) {
            list.append(m_datosXml->getContenidoVariable(VARIABLE+QString::number(k)));
        }
        addItemProyecto(parentItem, list, TreeItemType::ItemVariable, ":/img/img/brick.png");
    }
    // Agrego las Constantes
    if (type == TreeItemType::Constantes || type == TreeItemType::Proyecto) {
        list.clear();
        cant = m_datosXml->cantidadConstantes();
        parentItem = ui->treeWidgetProyecto->topLevelItem(0)->child(4);
        QStringList auxDatosConstante;
        for(int k=0; k < cant; ++k) {
            auxDatosConstante = m_datosXml->getContenidoConstante(CONSTANTE+QString::number(k)).split(",");
            list.append(auxDatosConstante.first() + " (" + auxDatosConstante.last() + ")");
        }
        addItemProyecto(parentItem, list, TreeItemType::ItemConstante, ":/img/img/non_derivative.png");
    }
}

void MainWindow::addItemProyecto(QTreeWidgetItem *parent, const QStringList &list, const TreeItemType &type, const QString &icon)
{
    qDeleteAll(parent->takeChildren());

    if (!list.isEmpty()) {
        QList<QTreeWidgetItem *> treeItems;
        foreach (QString str, list) {
            QTreeWidgetItem *newTreeItem = new QTreeWidgetItem(parent, type);
            newTreeItem->setText(0, str);
            newTreeItem->setFlags(Qt::ItemIsEnabled);
            newTreeItem->setIcon(0, QIcon(icon));
            treeItems.append(newTreeItem);
        }
        parent->addChildren(treeItems);
    } else {
        QTreeWidgetItem *newTreeItem = new QTreeWidgetItem(parent, type);
        newTreeItem->setText(0, " - ");
        newTreeItem->setFlags(Qt::ItemIsEnabled);
        newTreeItem->setIcon(0,QIcon(icon));
        parent->addChild(newTreeItem);
    }
    ui->treeWidgetProyecto->expandItem(parent);
}

void MainWindow::addItemMaquina(QTreeWidgetItem *parent, const QString &name,
                                const QStringList &list, const QString &icon)
{
    QTreeWidgetItem *maquinaItem = new QTreeWidgetItem(parent, TreeItemType::Maquina);
    maquinaItem->setText(0, name);
    maquinaItem->setFlags(Qt::ItemIsEnabled);
    maquinaItem->setIcon(0,QIcon(":/img/img/application.png"));
    parent->addChild(maquinaItem);

    if (!list.isEmpty()) {
        QList<QTreeWidgetItem *> treeItems;
        foreach (QString str, list) {
            QTreeWidgetItem *newTreeItem = new QTreeWidgetItem(maquinaItem, TreeItemType::ItemEstado);
            newTreeItem->setText(0, str);
            newTreeItem->setFlags(Qt::ItemIsEnabled);
            newTreeItem->setIcon(0,QIcon(icon));
            treeItems.append(newTreeItem);
        }
        maquinaItem->addChildren(treeItems);
    } else {
        QTreeWidgetItem *newTreeItem = new QTreeWidgetItem(maquinaItem, TreeItemType::ItemEstado);
        newTreeItem->setText(0, " - ");
        newTreeItem->setFlags(Qt::ItemIsEnabled);
        maquinaItem->addChild(newTreeItem);
    } 
    ui->treeWidgetProyecto->expandItem(maquinaItem);
}

QString MainWindow::getNameNuevaMaquina()
{
    bool ok = false;
    QRegExp regExp("[A-Za-z0-9-_ñÑ ]+");
    QIcon icon(QIcon(":/img/img/application_add.png"));
    QString name = RegExpInputDialog::getText(this, tr("Nueva Máquina de Estados"),
                                              tr("Definir el nombre:"),
                                              uniqueNameMaquina(DEFAULT_NAME_NUEVA_MAQUINA),
                                              regExp, &ok, icon, Qt::Dialog);
    name.replace(" ", "_");
    if (ok && !name.isEmpty())
        name = uniqueNameMaquina(name);
    else
        name = QString();

    return name;
}

QString MainWindow::getNameNuevoEstado()
{
    bool ok = false;
    QRegExp regExp("[A-Za-z0-9-_ñÑ ]+");
    QIcon icon(":/img/img/add_box.png");
    QString name = RegExpInputDialog::getText(this, tr("Nuevo Estado"),
                                              tr("Definir el nombre:"),
                                              uniqueNameEstado(DEFAULT_NAME_NUEVO_ESTADO),
                                              regExp, &ok, icon, Qt::Dialog);
    name.replace(" ", "_");
    if (ok && !name.isEmpty())
        name = uniqueNameEstado(name);
    else
        name = QString();

    return name;
}

int MainWindow::indexOfMaquina(const QString &nameMaquina) const
{
    int cantMaquinas = m_sceneList.count();
    for (int i = 0; i < cantMaquinas; ++i) {
        if (!m_sceneList.at(i)->name().compare(nameMaquina, Qt::CaseInsensitive))
        return i;
    }
    return -1;
}

int MainWindow::indexOfEstado(const QString &nameMaquina, const QString &nameEstado) const
{
    int cantMaquinas = ui->mdiArea->subWindowList().count();
    for (int i = 0; i < cantMaquinas; ++i) {
        QMdiSubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->subWindowList().at(i));
        QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
        DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
        if (!scene->name().compare(nameMaquina, Qt::CaseInsensitive)) {
            for (int i = 0; i < scene->itemList().count(); ++i) {
                if (!scene->itemList().at(i)->name().compare(nameEstado, Qt::CaseInsensitive))
                    return i;
            }
            break;
        }
    }
    return -1;
}

QString MainWindow::uniqueNameMaquina(const QString &name) const
{
    QString unique;
    for (int i = 1; ; ++i) {
        unique = name;
        if (i == 1 && !name.compare(DEFAULT_NAME_NUEVA_MAQUINA, Qt::CaseInsensitive))
            unique += QString::number(i);
        else if (i > 1)
            unique += QString::number(i);
        if (indexOfMaquina(unique) == -1)
            break;
    }
    return unique;
}

QString MainWindow::uniqueNameEstado(const QString &name) const
{
    QMdiSubWindow *subWindow = static_cast<SubWindow *>(ui->mdiArea->activeSubWindow());
    QGraphicsView *view = static_cast<QGraphicsView *>(subWindow->widget());
    DiagramScene *scene = static_cast<DiagramScene *>(view->scene());
    QString nameMaquina = scene->name();
    QString unique;
    for (int i = 1; ; ++i) {
        unique = name;
        if (i == 1 && !name.compare(DEFAULT_NAME_NUEVO_ESTADO, Qt::CaseInsensitive))
            unique += QString::number(i);
        else if (i > 1)
            unique += QString::number(i);
        if (indexOfEstado(nameMaquina, unique) == -1)
            break;
    }
    return unique;
}


void MainWindow::on_actionInterfaz_Virtual_triggered()
{
    if(m_uiMode == MainWindow::Simular)
    {
        ui->actionInterfaz_Virtual->setChecked(true);
        return;
    }

    InterfazInfotronicVirtual IIV;
    IIV.setWindowTitle("Convertir");
    QList< struct MdeE > M;
    struct Elementos E ;

    leerUmf( M , E );

    IIV.Infotronic( &E , m_Infotronic );

    if ( IIV.exec() == true )
    {
        for ( int i = 0 ; i < m_Infotronic->Get_max_salidas_digitales( ) ; i++)
            m_Infotronic->SetRelaysInfotronic2019(i,m_Infotronic->GetSalidasDigitales( i ));

        for ( int i = 0 ; i < m_Infotronic->Get_max_pulsadores( ) ; i++)
            m_Infotronic->SetPulsadoresInfotronic2019(i,m_Infotronic->GetPulsadores( i ));

        for ( int i = 0 ; i < m_Infotronic->Get_max_rgb( ) ; i++)
            m_Infotronic->SetRGBInfotronic2019(i,m_Infotronic->GetRGB(i));

        for ( int i = 0 ; i < m_Infotronic->Get_max_entradas_digitales( ) ; i++)
            m_Infotronic->SetEntradasInfotronic2019(i,m_Infotronic->GetEntradasDigitales( i ));

        for ( int i = 0 ; i < m_Infotronic->Get_max_leds() ; i++ )
            m_Infotronic->SetLEDS_EXP2Infotronic2019(i,m_Infotronic->GetLED(i ));

        ui->actionInterfaz_Virtual->setChecked(true);

        ui->actionSimular->setDisabled(true);

        on_actionSimular_triggered();

        m_simular_con_Infotronic_virtual = true ;
    }
    else
    {
         m_simular_con_Infotronic_virtual = false ;
         ui->actionInterfaz_Virtual->setChecked(false);
    }
    return;
}
void MainWindow::on_timerevent( void)
{
    int numero;
    TimerRun.timerevent( m_timers );
    struct timer d;
    while ( (numero = TimerRun.get_endtimer ( d )) != 0 )
    {
        QString a = d.evento;
        TimerRun.set_endtimer ( );
        procesarDatosRecibidosSimTCP(d.evento);
    }
}
//parte de comentarios

void MainWindow::on_actionAgregar_comentario_triggered()
{
    //aca tengo que agregar el comentario y setearlo
    DialogComentario dialog(&m_datosXml, commentElementId, this);
    dialog.exec();
}

void MainWindow::on_actionEliminar_comentario_triggered()
{
    int ret = QMessageBox::warning(this, "Advertencia", "Esta seguro? Esta operacion no se puede deshacer", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) m_datosXml->clearComentario(commentElementId, true);
}

//parte de tabla de variables

void MainWindow::BorrarTablaVariables()
{
    ui->tableWidgetVariables->clearContents();
    ui->tableWidgetVariables->setRowCount(0);
}

void MainWindow::OcultarTablaVariables()
{
    ui->tableWidgetVariables->hide();
    ui->dockWidgetVariables->hide();
}

void MainWindow::MostrarTablaVariables()
{
    ui->tableWidgetVariables->show();
    ui->dockWidgetVariables->show();
}


void MainWindow::InicializarTablaVariables()
{
    ui->tableWidgetVariables->setColumnCount(2);
    ui->tableWidgetVariables->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetVariables->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetVariables->horizontalHeader()->setFont(QFont("Segoe UI"));
    ui->tableWidgetVariables->verticalHeader()->hide();

    QTableWidgetItem *headerItem;
    headerItem = new QTableWidgetItem(tr("Variable"));
    headerItem->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidgetVariables->setHorizontalHeaderItem(0, headerItem);
    headerItem = new QTableWidgetItem(tr("Valor"));
    headerItem->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidgetVariables->setHorizontalHeaderItem(1, headerItem);
}



void MainWindow::on_actionAyudaDesactivada_triggered(bool checked)
{
    if(checked) {
        ui->dockWidgetAyuda->hide();
        ayudaDinamica.setNivelAyuda(AyudaDinamica::DESACTIVADA);
    }
}


void MainWindow::on_actionAyudaUsuarioInicial_triggered(bool checked)
{
    if(checked) {
        ui->dockWidgetAyuda->show();
        ayudaDinamica.setNivelAyuda(AyudaDinamica::AYUDA_USR_BASICO);
    }
}

void MainWindow::on_actionAyudaUsuarioAvanzado_triggered(bool checked)
{
    if(checked) {
        ui->dockWidgetAyuda->show();
        ayudaDinamica.setNivelAyuda(AyudaDinamica::AYUDA_USR_AVANZADO);
    }
}

