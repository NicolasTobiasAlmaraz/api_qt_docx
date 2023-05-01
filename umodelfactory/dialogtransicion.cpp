#include "dialogtransicion.h"
#include "ui_dialogtransicion.h"
#include "regexpinputdialog.h"
#include "xmlhandler.h"
#include "diagramscene.h"
#include "diagramtransition.h"

#include <QCompleter>
#include <QStringListModel>
#include <QMessageBox>
#include <QRegExp>
#include <QRegularExpression>

DialogTransicion::DialogTransicion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTransicion)
{
    ui->setupUi(this);

   // connect(ui->pushButtonCancelar, SIGNAL(clicked()), this, SLOT(reject()));

    ui->toolBoxEventos->setCurrentIndex(0);
    ui->toolBoxAcciones->setCurrentIndex(0);


    /// cambios en RegExp
//    QRegExp regExp("[a-zA-z0-9_+-/*]+");
//    //regExp.setPatternSyntax(QRegExp::Wildcard);
//    QRegExpValidator *validator = new QRegExpValidator(regExp);

    QRegExp regExpRValue("[a-zA-z0-9_+-/*#@%'&+]");
    //regExp.setPatternSyntax(QRegExp::Wildcard);
    QRegExpValidator *validatorRValue = new QRegExpValidator(regExpRValue);

    ui->lineEditValores->setValidator(validatorRValue);
    ui->lineEditValores_2->setValidator(validatorRValue);

    /// fin modificación


    ui->pushButtonAnd->setDisabled(true);
    ui->pushButtonOr->setDisabled(true);
    ui->pushButtonParentesisCerrar->setDisabled(true);
    m_difParentesis = 0;

    // Agregados Truji

    QRegExp rx( "[A-Za-z]+");
    QValidator *v = new QRegExpValidator(rx, this);
    ui->lineEditNombreTemporizador->setValidator(v);
    QRegExp rx1("[0-9]+");
    QValidator *v1 = new QRegExpValidator(rx1, this);
    ui->lineEditTiempoTemporizador->setValidator(v1);
    ui->lineEditTiempoTemporizador->setMaxLength(4);
    ui->radioButtonSegundos->setChecked(true);
    m_Borrar_Evento_Y_Timer = M_BORRAR_FASE_0;
}

DialogTransicion::~DialogTransicion()
{
    delete ui;
}

void DialogTransicion::setDatos(DiagramTransition* &arrow, XmlHandler* &datos, QList<DiagramScene *> &sceneList, DiagramScene* &scene, DialogTransicion::ModoTransicion modo)
{
    // Punteros
    m_arrow = arrow;
    m_datos = datos;
    m_sceneList = sceneList;
    m_scene = scene;
    // QCompleter
    QStringList stringListForCompleter;
    if (m_datos->cantidadVariables() > 0)
        stringListForCompleter << m_datos->getNombresVariablesDefinidas();
    if (m_datos->cantidadConstantes() > 0)
        stringListForCompleter << m_datos->getNombresConstantesDefinidas();
    m_completerModel = new QStringListModel(stringListForCompleter, this);
    m_completer = new QCompleter(m_completerModel, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    ui->lineEditValores->setCompleter(m_completer);
    ui->lineEditValores_2->setCompleter(m_completer);
    // Eventos
    for (int k=0; k < m_datos->cantidadEventos(); ++k)
        ui->listWidgetEventos->insertItem(k, m_datos->getContenidoEvento(EVENTO+QString::number(k)));
    // Variables
    for (int k=0; k < m_datos->cantidadVariables(); ++k) {
        QString varName = m_datos->getContenidoVariable(VARIABLE+QString::number(k));
        ui->listWidgetVariables->insertItem(k, varName);
        ui->listWidgetVariables_2->insertItem(k, varName);
    }
    // Acciones
    for (int k=0; k < m_datos->cantidadAcciones(); ++k)
    {
        QString accion;
        accion =  m_datos->getContenidoAccion(ACCION+QString::number(k));
        if ( accion[0] == 'f' && accion[1] == '_')
            ui->listWidgetAcciones->insertItem(k, m_datos->getContenidoAccion(ACCION+QString::number(k)));
    }
    // Timers
    for (int k=0; k < m_datos->cantidadAcciones(); ++k)
    {
        QString accion;
        accion =  m_datos->getContenidoAccion(ACCION+QString::number(k));
        if ( accion[0] == 't' && accion[1] == '_')
            ui->listWidgetTemporizadores->insertItem(k, m_datos->getContenidoAccion(ACCION+QString::number(k)));
    }

    // Macros
    for (int k=0; k < m_datos->cantidadAcciones(); ++k)
    {
        QString accion;
        accion =  m_datos->getContenidoAccion(ACCION+QString::number(k));
        if ( accion[0] == 'm' && accion[1] == '_')
            ui->listWidgetMacros->insertItem(k, m_datos->getContenidoAccion(ACCION+QString::number(k)));
    }
    // Cargo los datos de la transición actual
    if(m_arrow->startItem()->itemType() == DiagramItem::EstadoReset) {
        ui->toolBoxEventos->setDisabled(true);
        ui->labelEstadoActual->setText("RESET");
        ui->labelEstadoFuturo->setText(m_arrow->endItem()->name().toUpper());
        ui->lineEditCondicion->setText("(reset)");
        ui->groupBoxCondicion->setDisabled(true);
    } else {
        ui->labelEstadoActual->setText(m_arrow->startItem()->name().toUpper());
        ui->labelEstadoFuturo->setText(m_arrow->endItem()->name().toUpper());

        if(modo == DialogTransicion::EditarTransicion) {
            // encuentro el evento en el combo
            ui->lineEditCondicion->setText(m_arrow->event());
            // cambio las acciones disponibles
            ui->pushButtonElegirEvento->setDisabled(true);
            ui->pushButtonElegirExpresion->setDisabled(true);
            ui->pushButtonNot->setDisabled(true);
            ui->pushButtonParentesis->setDisabled(true);
            ui->pushButtonAnd->setEnabled(true);
            ui->pushButtonOr->setEnabled(true);
        }
    }
    if (modo == DialogTransicion::EditarTransicion) {
        // actualizo acciones disponibles
        foreach (QString action, m_arrow->actions()) {
            if (!action.isEmpty())
                ui->listWidgetAccionesTransicion->addItem(action);
        }
    }
}

void DialogTransicion::on_listWidgetEventos_itemDoubleClicked(QListWidgetItem *item)
{
    if (ui->pushButtonElegirEvento->isEnabled()) {
        // agrego el evento a la condición
        ui->lineEditCondicion->setText(ui->lineEditCondicion->text()+item->text());
        // cambio las acciones disponibles
        ui->pushButtonElegirEvento->setDisabled(true);
        ui->pushButtonElegirExpresion->setDisabled(true);
        ui->pushButtonNot->setDisabled(true);
        ui->pushButtonParentesis->setDisabled(true);
        ui->pushButtonAnd->setEnabled(true);
        ui->pushButtonOr->setEnabled(true);
        if (m_difParentesis > 0)
            ui->pushButtonParentesisCerrar->setEnabled(true);
    }
}

void DialogTransicion::on_pushButtonNuevoEvento_clicked()
{
    bool ok = false;
    QRegExp regExp("[A-Za-z_]+[A-Za-z0-9_]+");
    QIcon icon(":/img/img/events.png");
    QString name = RegExpInputDialog::getText(this, tr("Nuevo Evento"),
                                              tr("Definir el nombre:"),
                                              "", regExp, &ok, icon, Qt::Dialog);
    if (ok && !name.isEmpty()) {
        //name.append("( )");
        name = "s_" + name;
        if(!m_datos->getNombresEventosDefinidos().contains(name, Qt::CaseSensitive)) {
            m_datos->addNuevoEvento(name);
            ui->listWidgetEventos->addItem(name);
            ui->listWidgetEventos->setCurrentRow(ui->listWidgetEventos->count()-1);
        } else
            QMessageBox::information(this, tr("Definir Evento"),
                                     tr("Ya existe un evento definido con el mismo nombre."));
    }
}

void DialogTransicion::on_pushButtonBorrarEvento_clicked()
{
    if(ui->listWidgetEventos->count() == 0 ||
            ui->listWidgetEventos->currentItem() == nullptr)
        return;

    // Chequear si el evento a borrar está siendo utilizado (ADVERTENCIA!)       
    QString eventoActual = ui->listWidgetEventos->currentItem()->text();
    QStringList q = eventoActual.split("_");
    if ( q[0] == "e" && m_Borrar_Evento_Y_Timer == M_BORRAR_FASE_0)
    {
        QMessageBox::warning(this, "Borrar Evento de Temporizador"," El evento sera removido de forma automática solamente cuando se elimine su Temporizador asociado.");
        return;
    }
    QRegularExpression re("\\b"+eventoActual+"\\b");
    // Primero chequeo la transición actual
    QStringList triggers = ui->lineEditCondicion->text().split(QRegExp("\\&\\&|\\|\\|"), QString::SkipEmptyParts);
    foreach (QString str, triggers) {
        if (str.contains(re) && !str.contains(QRegExp("\\=|\\!\\=|\\<|\\>"))) {
            QMessageBox::warning(this, "Borrar Evento",
                                 "El evento forma parte de la transición actual.");
            return;
        }
    }
    // Después el resto de las transiciones
    int cantMaquinas = m_sceneList.count();
    for (int k = 0; k < cantMaquinas; ++k) {
        int cantItems = m_sceneList.at(k)->itemList().count();
        for (int i = 0; i < cantItems; ++i) {
            DiagramItem *item = m_sceneList.at(k)->itemList().at(i);
            foreach (DiagramTransition *arrow, item->transitions()) {
                // Para no volver a verificar la transición actual
                if(arrow != m_arrow) {
                    triggers = arrow->event().split(QRegExp("\\&\\&|\\|\\|"), QString::SkipEmptyParts);
                    foreach (QString str, triggers) {
                        if (str.contains(re) && !str.contains(QRegExp("\\=|\\!\\=|\\<|\\>"))) {
                            QMessageBox::warning(this, "Borrar Evento",
                                                 "El evento forma parte de otra transición ya definida. Primero debe remover la transición.");
                            return;
                        }
                    }
                }
            }
        }
    }
    // si no está siendo utilizado, elimino el evento seleccionado
    int rowActual = ui->listWidgetEventos->currentRow();
    m_datos->borrarEvento(rowActual);
    ui->listWidgetEventos->takeItem(rowActual);
    // Guardo los eventos eliminados por si se cancela el dialog
    m_eventosEliminados.append(eventoActual);
    m_Borrar_Evento_Y_Timer = M_BORRAR_FASE_2;
}

void DialogTransicion::on_pushButtonElegirEvento_clicked()
{
    if(ui->listWidgetEventos->currentItem() != nullptr) {
        // agrego el evento a la condición
        ui->lineEditCondicion->setText(ui->lineEditCondicion->text()+
                                       ui->listWidgetEventos->currentItem()->text());
        // cambio las acciones disponibles
        ui->pushButtonElegirEvento->setDisabled(true);
        ui->pushButtonElegirExpresion->setDisabled(true);
        ui->pushButtonNot->setDisabled(true);
        ui->pushButtonParentesis->setDisabled(true);
        ui->pushButtonAnd->setEnabled(true);
        ui->pushButtonOr->setEnabled(true);
        if (m_difParentesis > 0)
            ui->pushButtonParentesisCerrar->setEnabled(true);
    }
}

void DialogTransicion::on_pushButtonAnd_clicked()
{
    ui->lineEditCondicion->setText(ui->lineEditCondicion->text()+" && ");
    ui->pushButtonAnd->setDisabled(true);
    ui->pushButtonOr->setDisabled(true);
    ui->pushButtonParentesisCerrar->setDisabled(true);
    ui->pushButtonNot->setEnabled(true);
    ui->pushButtonParentesis->setEnabled(true);
    ui->pushButtonElegirEvento->setEnabled(true);
    ui->pushButtonElegirExpresion->setEnabled(true);
}

void DialogTransicion::on_pushButtonOr_clicked()
{
    ui->lineEditCondicion->setText(ui->lineEditCondicion->text()+" || ");
    ui->pushButtonAnd->setDisabled(true);
    ui->pushButtonOr->setDisabled(true);
    ui->pushButtonParentesisCerrar->setDisabled(true);
    ui->pushButtonNot->setEnabled(true);
    ui->pushButtonParentesis->setEnabled(true);
    ui->pushButtonElegirEvento->setEnabled(true);
    ui->pushButtonElegirExpresion->setEnabled(true);
}

void DialogTransicion::on_pushButtonNot_clicked()
{
    ui->lineEditCondicion->setText(ui->lineEditCondicion->text()+"!");
    ui->pushButtonNot->setDisabled(true);
    ui->pushButtonAnd->setDisabled(true);
    ui->pushButtonOr->setDisabled(true);
    ui->pushButtonParentesisCerrar->setDisabled(true);
    ui->pushButtonParentesis->setEnabled(true);
}

void DialogTransicion::on_pushButtonParentesis_clicked()
{
    ui->lineEditCondicion->setText(ui->lineEditCondicion->text()+"(");
    m_difParentesis++;
    ui->pushButtonParentesis->setDisabled(true);
    ui->pushButtonAnd->setDisabled(true);
    ui->pushButtonOr->setDisabled(true);
    ui->pushButtonNot->setEnabled(true);
}

void DialogTransicion::on_pushButtonParentesisCerrar_clicked()
{
    ui->lineEditCondicion->setText(ui->lineEditCondicion->text()+")");
    m_difParentesis--;
    if(m_difParentesis == 0)
        ui->pushButtonParentesisCerrar->setDisabled(true);
}

void DialogTransicion::on_pushButtonBorrarCondicion_clicked()
{
    ui->lineEditCondicion->clear();

    ui->pushButtonAnd->setDisabled(true);
    ui->pushButtonOr->setDisabled(true);
    ui->pushButtonAnd->setChecked(false);
    ui->pushButtonOr->setChecked(false);
    ui->pushButtonNot->setEnabled(true);
    ui->pushButtonParentesis->setEnabled(true);
    ui->pushButtonParentesisCerrar->setDisabled(true);
    m_difParentesis = 0;
    ui->pushButtonElegirEvento->setEnabled(true);
    ui->pushButtonElegirExpresion->setEnabled(true);
}

void DialogTransicion::on_pushButtonElegirAccion_clicked()
{
    int fila = ui->listWidgetAcciones->currentRow();
    if (fila != (-1)) {
        QString accionActual = ui->listWidgetAcciones->currentItem()->text();
        QList<QListWidgetItem *>  itemsList =
                ui->listWidgetAccionesTransicion->findItems(accionActual, Qt::MatchCaseSensitive);
        if (itemsList.isEmpty()) {
            ui->listWidgetAccionesTransicion->addItem(accionActual);
        } else {
            QMessageBox::information(this, tr("Advertencia"),
                                     tr("Este elemento ya fue agregado."));
        }
    } else
        QMessageBox::information(this, tr("Advertencia"),
                                 tr("No hay ningún elememento seleccionado."));
}

void DialogTransicion::on_listWidgetAcciones_itemDoubleClicked(QListWidgetItem *item)
{
    QString accionActual = item->text();
    QList<QListWidgetItem *>  itemsList =
            ui->listWidgetAccionesTransicion->findItems(accionActual, Qt::MatchCaseSensitive);
    if (itemsList.isEmpty()) {
        ui->listWidgetAccionesTransicion->addItem(accionActual);
    } else {
        QMessageBox::information(this, tr("Advertencia"),
                                 tr("Este elemento ya fue agregado."));
    }
}

void DialogTransicion::on_pushButtonBorrarItemAccion_clicked()
{
    int row = ui->listWidgetAccionesTransicion->currentRow();
    if (row != -1) {
        ui->listWidgetAccionesTransicion->takeItem(row);
    }
}

void DialogTransicion::on_pushButtonSubirItemAccion_clicked()
{
    int row = ui->listWidgetAccionesTransicion->currentRow();
    if (row > 0) {
        QListWidgetItem *currentItem = ui->listWidgetAccionesTransicion->takeItem(row);
        ui->listWidgetAccionesTransicion->insertItem(row-1, currentItem);
        ui->listWidgetAccionesTransicion->setCurrentRow(row-1);
    }
}

void DialogTransicion::on_pushButtonBajarItemAccion_clicked()
{
    int row = ui->listWidgetAccionesTransicion->currentRow();
    if (row >= 0 && row < ui->listWidgetAccionesTransicion->count()-1) {
        QListWidgetItem *currentItem = ui->listWidgetAccionesTransicion->takeItem(row);
        ui->listWidgetAccionesTransicion->insertItem(row+1, currentItem);
        ui->listWidgetAccionesTransicion->setCurrentRow(row+1);
    }
}

void DialogTransicion::on_pushButtonNuevaAccion_clicked()
{
    bool ok = false;
    //QRegExp regExp("[A-Za-z_]+[A-Za-z0-9_]+");
    QRegExp regExp("[A-Za-z0-9_]+");
    QIcon icon(":/img/img/external.png");
    QString name = RegExpInputDialog::getText(this, tr("Nueva Acción"),
                                              tr("Definir el nombre:"),
                                              "", regExp, &ok, icon, Qt::Dialog);
    if (ok && !name.isEmpty()) {
        name.append("()");
        if(!m_datos->getNombresAccionesDefinidas().contains(name, Qt::CaseSensitive)) {
            m_datos->addNuevaAccion("f_"+ name);
            ui->listWidgetAcciones->addItem("f_"+ name);
            ui->listWidgetAcciones->setCurrentRow(ui->listWidgetAcciones->count()-1);
        } else
            QMessageBox::information(this, tr("Definir Acción"),
                                     tr("Ya existe una acción definida con el mismo nombre."));
    }
}

void DialogTransicion::on_pushButtonBorrarAccion_clicked()
{
    if(ui->listWidgetAcciones->count() == 0 ||
            ui->listWidgetAcciones->currentItem() == nullptr)
        return;

    QString accionActual = ui->listWidgetAcciones->currentItem()->text();

    // Chequear si la acción a borrar está siendo utilizada (ADVERTENCIA)
    // Primero chequeo la transición actual
    int cantAcciones = ui->listWidgetAccionesTransicion->count();
    for (int i = 0; i < cantAcciones; ++i) {
        if( accionActual == ui->listWidgetAccionesTransicion->item(i)->text()) {
            QMessageBox::warning(this, "Borrar Acción",
                                 "La acción forma parte de la transición actual.");
            return;
        }

    }
    // Después el resto de las transiciones
    int cantMaquinas = m_sceneList.count();
    for (int k = 0; k < cantMaquinas; ++k) {
        int cantItems = m_sceneList.at(k)->itemList().count();
        for (int i = 0; i < cantItems; ++i) {
            DiagramItem *item = m_sceneList.at(k)->itemList().at(i);
            foreach (DiagramTransition *arrow, item->transitions()) {
                // Para no volver a verificar la transición actual
                if(arrow != m_arrow) {
                    if(arrow->actions().contains(accionActual)) {
                        QMessageBox::warning(this, tr("Borrar Acción"),
                                                 tr("La acción forma parte de otra transición ya definida.\nPrimero debe remover la transición."));
                        return;
                    }
                }
            }
        }
    }

    int rowActual;
    for ( rowActual = 0 ; rowActual < m_datos->getNombresAccionesDefinidas().count() ; rowActual ++)
    {
        if ( m_datos->getNombresAccionesDefinidas().at(rowActual) == accionActual)
            break;
    }
    // Elimino la acción seleccionada
    m_datos->borrarAccion(rowActual);
    ui->listWidgetAcciones->takeItem(ui->listWidgetAcciones->currentRow());
    // Guardo la acción eliminada por si se cancela el dialog
    m_accionesEliminadas.append(accionActual);

}

void DialogTransicion::on_listWidgetVariables_itemDoubleClicked(QListWidgetItem *item)
{
    ui->lineEditVariable->setText(item->text());
}

void DialogTransicion::on_listWidgetVariables_2_itemDoubleClicked(QListWidgetItem *item)
{
    ui->lineEditVariable_2->setText(item->text());
}

void DialogTransicion::on_pushButtonElegirExpresion_clicked()
{
    QString variable = ui->lineEditVariable->text();
    QString valor = ui->lineEditValores->text();


    if(!variable.isEmpty() && !valor.isEmpty()) {

        //solo dejo que puedan poner numeros
        bool ok = false;
        int dec = valor.toInt(&ok, 10);

        if(ok==true)
        {
        // agrego la expresión a la condición
        ui->lineEditCondicion->setText(ui->lineEditCondicion->text()+variable+
                                       " "+ui->comboBoxComparacion->currentText()+
                                       " "+valor);
        // cambio las acciones disponibles
        ui->pushButtonElegirEvento->setDisabled(true);
        ui->pushButtonElegirExpresion->setDisabled(true);
        ui->pushButtonNot->setDisabled(true);
        ui->pushButtonParentesis->setDisabled(true);
        ui->pushButtonAnd->setEnabled(true);
        ui->pushButtonOr->setEnabled(true);
        if (m_difParentesis > 0)
            ui->pushButtonParentesisCerrar->setEnabled(true);
        }
        else
        {
            //aviso que solo se pueden poner valores
            QMessageBox::information(this, tr("Asignar valor"),
                                     tr("Por el momento solo se pueden asignar valores para la simulación del modelo"));
        }
     }
}

void DialogTransicion::on_pushButtonElegirExpresion_2_clicked()
{
    QString variable = ui->lineEditVariable_2->text();
    QString valor = ui->lineEditValores_2->text();
    if(!variable.isEmpty() && !valor.isEmpty()) {

        //solo dejo que puedan poner numeros
        bool ok = false;
        int dec = valor.toInt(&ok, 10);

        if(ok==true)
        {


        // agrego la expresión a la condición
        ui->listWidgetAccionesTransicion->addItem(variable+" "+ui->comboBoxAsignacion->currentText()+" "+valor);
        // PRUEBA (NUEVO): Chequeo el campo valor para definir constantes nuevas!
        // VER expresiones con +,-,etc.
//        bool ok;
//        valor.toDouble(&ok);
//        if(ok == true)
//            return;
//        if(!m_datos->getNombresConstantesDefinidas().contains(valor, Qt::CaseSensitive)) {
//            m_datos->addNuevaConstante(valor);
//            // update QCompleter
//            m_completerModel->setStringList(m_completerModel->stringList() << valor);
//        }
        }
        else
        {
            //aviso que solo se pueden poner valores
            QMessageBox::information(this, tr("Asignar valor"),
                                     tr("Por el momento solo se pueden asignar valores para la simulación del modelo"));

        }
    }
}

void DialogTransicion::on_pushButtonNuevaVariable_clicked()
{
    bool ok = false;
    QRegExp regExp("[A-Za-z_]+[A-Za-z0-9_]+");
    QIcon icon(":/img/img/brick.png");
    QString name = RegExpInputDialog::getText(this, tr("Nueva Variable"),
                                              tr("Definir el nombre:"),
                                              "", regExp, &ok, icon, Qt::Dialog);
    if (ok && !name.isEmpty()) {
        if(!m_datos->getNombresVariablesDefinidas().contains(name, Qt::CaseSensitive)) {
            m_datos->addNuevaVariable(name);
            ui->listWidgetVariables->addItem(name);
            ui->listWidgetVariables_2->addItem(name);
            ui->listWidgetVariables->setCurrentRow(ui->listWidgetVariables->count()-1);
            ui->listWidgetVariables_2->setCurrentRow(ui->listWidgetVariables_2->count()-1);
            // update QCompleter
            m_completerModel->setStringList(m_completerModel->stringList() << name);
        } else
            QMessageBox::information(this, tr("Definir Variable"),
                                     tr("Ya existe una variable definida con el mismo nombre."));
    }
}

void DialogTransicion::on_pushButtonBorrarVariable_clicked()
{
    if(ui->listWidgetVariables->count() == 0 ||
            ui->listWidgetVariables->currentItem() == nullptr)
        return;

    borrarVariable(ui->listWidgetVariables->currentItem()->text(),
                   ui->listWidgetVariables->currentRow());
}

void DialogTransicion::on_pushButtonNuevaVariable_2_clicked()
{
    bool ok = false;
    QRegExp regExp("[A-Za-z_]+[A-Za-z0-9_]+");
    QIcon icon(":/img/img/brick.png");
    QString name = RegExpInputDialog::getText(this, tr("Nueva Variable"),
                                              tr("Definir el nombre:"),
                                              "", regExp, &ok, icon, Qt::Dialog);
    if (ok && !name.isEmpty()) {
        if(!m_datos->getNombresVariablesDefinidas().contains(name, Qt::CaseSensitive)) {
            m_datos->addNuevaVariable(name);
            ui->listWidgetVariables->addItem(name);
            ui->listWidgetVariables_2->addItem(name);
            ui->listWidgetVariables->setCurrentRow(ui->listWidgetVariables->count()-1);
            ui->listWidgetVariables_2->setCurrentRow(ui->listWidgetVariables_2->count()-1);
            // update QCompleter
            m_completerModel->setStringList(m_completerModel->stringList() << name);
        } else
            QMessageBox::information(this, tr("Definir Variable"),
                                     tr("Ya existe una variable definida con el mismo nombre."));
    }
}

void DialogTransicion::on_pushButtonBorrarVariable_2_clicked()
{
    if(ui->listWidgetVariables_2->count() == 0 ||
            ui->listWidgetVariables_2->currentItem() == nullptr)
        return;

    borrarVariable(ui->listWidgetVariables_2->currentItem()->text(),
                   ui->listWidgetVariables_2->currentRow());
}

void DialogTransicion::on_pushButtonAceptar_clicked()
{
    QStringList listAcciones;
    int cantAcciones = ui->listWidgetAccionesTransicion->count();
    for (int i = 0; i < cantAcciones; ++i) {
        listAcciones.append(ui->listWidgetAccionesTransicion->item(i)->text());
    }
    if(m_arrow->startItem()->itemType() == DiagramItem::EstadoReset) {
        m_evento = "-";
    } else {
        m_evento = ui->lineEditCondicion->text();
    }
    m_acciones = listAcciones.join(",");
    if (!m_evento.isEmpty()) {
        if (m_difParentesis == 0 && !m_evento.endsWith("!") && !m_evento.endsWith(" ")) {
            this->accept();
        } else
            QMessageBox::warning(this, tr("Advertencia"),
                                     tr("Disparador de la Transición: Expresión inválida."));
    } else
        QMessageBox::information(this, tr("Advertencia"),
                                 tr("Es necesario definir el evento que dispara la transición."));
}

void DialogTransicion::on_pushButtonCancelar_clicked()
{
    if (!m_eventosEliminados.isEmpty()) {
        foreach (QString ev, m_eventosEliminados) {
            if(!m_datos->getNombresEventosDefinidos().contains(ev, Qt::CaseSensitive)) {
                m_datos->addNuevoEvento(ev);
            }
        }
        m_eventosEliminados.clear();
    }

    if (!m_variablesEliminadas.isEmpty()) {
        foreach (QString var, m_variablesEliminadas) {
            if(!m_datos->getNombresVariablesDefinidas().contains(var, Qt::CaseSensitive)) {
                m_datos->addNuevaVariable(var);
            }
        }
        m_variablesEliminadas.clear();
    }

    if (!m_accionesEliminadas.isEmpty()) {
        foreach (QString acc, m_accionesEliminadas) {
            if(!m_datos->getNombresAccionesDefinidas().contains(acc, Qt::CaseSensitive)) {
                m_datos->addNuevaAccion(acc);
            }
        }
        m_accionesEliminadas.clear();
    }

    this->reject();
}

void DialogTransicion::borrarVariable(QString variableActual, int rowActual)
{
    bool flagBorrar = true;
    // Chequear si la variable a borrar está siendo utilizada en la transicion actual (ADVERTENCIA)
    QRegularExpression re("\\b"+variableActual+"\\b");
    QStringList triggers = ui->lineEditCondicion->text().split(QRegExp("\\&\\&|\\|\\|"), QString::SkipEmptyParts);
    foreach (QString str, triggers) {
        if (str.contains(re) && str.contains(QRegExp("\\=|\\!\\=|\\<|\\>"))) {
            QMessageBox::warning(this, tr("Borrar Variable"),
                                     tr("La variable forma parte de la transición actual."));
            return;
        }
    }
    int cantAcciones = ui->listWidgetAccionesTransicion->count();
    for (int i = 0; i < cantAcciones; ++i) {
        if (ui->listWidgetAccionesTransicion->item(i)->text().contains(variableActual)) {
            QMessageBox::warning(this, tr("Borrar Variable"),
                                     tr("La variable forma parte de la transición actual."));
            return;
        }
    }
    // Chequear si la variable a borrar está siendo utilizada en otra transición
    int cantMaquinas = m_sceneList.count();
    for (int k = 0; k < cantMaquinas; ++k) {
        int cantItems = m_sceneList.at(k)->itemList().count();
        for (int i = 0; i < cantItems; ++i) {
            DiagramItem *item = m_sceneList.at(k)->itemList().at(i);
            foreach (DiagramTransition *arrow, item->transitions()) {
                // Para no volver a verificar la transición actual
                if(arrow != m_arrow) {
                    //disparador de la transición
                    triggers = arrow->event().split(QRegExp("\\&\\&|\\|\\|"), QString::SkipEmptyParts);
                    foreach (QString str, triggers) {
                        if (str.contains(re) && str.contains(QRegExp("\\=|\\!\\=|\\<|\\>"))) {
                            flagBorrar = false;
                        }
                    }
                    //acciones de la transición
                    foreach (QString str, arrow->actions()) {
                        if (str.contains(re) && str.contains(QRegExp("\\="))) {
                            flagBorrar = false;
                        }
                    }
                }
            }
        }
    }
    // si está siendo utilizada -> ADVERTENCIA!
    if (flagBorrar == false) {
        QMessageBox::warning(this, "Borrar Variable",
                             "La variable forma parte de otra transición ya definida. "
                             "Primero debe remover la transición.");
        return;
    }
    // Si llegué acá, elimino la variable seleccionada
    m_datos->borrarVariable(rowActual);
    ui->listWidgetVariables->takeItem(rowActual);
    ui->listWidgetVariables_2->takeItem(rowActual);
    if(variableActual == ui->lineEditVariable->text())
        ui->lineEditVariable->clear();
    if(variableActual == ui->lineEditVariable_2->text())
        ui->lineEditVariable_2->clear();
    // update QCompleter
    QStringList stringListForCompleter = m_completerModel->stringList();
    stringListForCompleter.removeOne(variableActual);
    m_completerModel->setStringList(stringListForCompleter);
    // Guardo la variable eliminada por si se cancela el dialog
    m_variablesEliminadas.append(variableActual);
}

void DialogTransicion::on_pushButtonNuevoTemporizador_clicked()
{
    QString Temporizador , TemporizadorStop , base;
    if (ui->lineEditNombreTemporizador->text() != "" &&  ui->lineEditTiempoTemporizador->text() != "" )
    {
        if (ui->radioButtonDecimas->isChecked())
            base = "D";
        else
            base = "S";

        QString v ;
        v = ui->lineEditTiempoTemporizador->text();
        v = v.rightJustified(3, '0');
        Temporizador = "t_" + ui->lineEditNombreTemporizador->text() + "_" + v + "_" + base + "()";
        TemporizadorStop = "t_" + ui->lineEditNombreTemporizador->text() + "_" + v + "_" + "stop" + "()";

        if(!m_datos->getNombresAccionesDefinidas().contains(Temporizador, Qt::CaseSensitive))
        {
            m_datos->addNuevaAccion(Temporizador);
            m_datos->addNuevaAccion(TemporizadorStop);

            ui->listWidgetTemporizadores->addItem(Temporizador);
            ui->listWidgetTemporizadores->addItem(TemporizadorStop);

            ui->listWidgetAcciones->setCurrentRow(ui->listWidgetAcciones->count()-1);

            m_datos->addNuevoEvento("e_" + ui->lineEditNombreTemporizador->text());
            ui->listWidgetEventos->addItem("e_" + ui->lineEditNombreTemporizador->text());
            ui->listWidgetEventos->setCurrentRow(ui->listWidgetEventos->count()-1);
            ui->lineEditNombreTemporizador->setText("");
            ui->lineEditTiempoTemporizador->setText("");

        } else
            QMessageBox::information(this, tr("Definir Temporizador"),
                                     tr("Ya existe un Temporizador definido con el mismo nombre."));
    }
}

void DialogTransicion::on_pushButtonBorrarTemporizador_clicked()
{
    if(ui->listWidgetTemporizadores->count() == 0 ||
            ui->listWidgetTemporizadores->currentItem() == nullptr)
        return;

    QString accionActual = ui->listWidgetTemporizadores->currentItem()->text();

    // Chequear si la acción a borrar está siendo utilizada (ADVERTENCIA)
    // Primero chequeo la transición actual
    int cantAcciones = ui->listWidgetAccionesTransicion->count();
    for (int i = 0; i < cantAcciones; ++i) {
        if( accionActual == ui->listWidgetAccionesTransicion->item(i)->text()) {
            QMessageBox::warning(this, "Borrar Temporizador",
                                 "El Temporizador forma parte de la transición actual.");
            return;
        }

    }
    // Después el resto de las transiciones
    int cantMaquinas = m_sceneList.count();
    for (int k = 0; k < cantMaquinas; ++k) {
        int cantItems = m_sceneList.at(k)->itemList().count();
        for (int i = 0; i < cantItems; ++i) {
            DiagramItem *item = m_sceneList.at(k)->itemList().at(i);
            foreach (DiagramTransition *arrow, item->transitions()) {
                // Para no volver a verificar la transición actual
                if(arrow != m_arrow) {
                    if(arrow->actions().contains(accionActual)) {
                        QMessageBox::warning(this, tr("Borrar Temporizador"),
                                                 tr("El Temporizador forma parte de otra transición ya definida.\nPrimero debe remover la transición."));
                        return;
                    }
                }
            }
        }
    }

    int rowActual;

    QStringList temporizador = accionActual.split("_");
    QString q;
    q= "e_" + temporizador[1];

    for ( rowActual = 0 ; rowActual <  m_datos->getNombresEventosDefinidos().count() ; rowActual ++)
    {
        if (  m_datos->getNombresEventosDefinidos().at(rowActual) == "e_" + temporizador[1] )
            break;
    }

    ui->listWidgetEventos->setCurrentRow(rowActual);
    m_Borrar_Evento_Y_Timer = M_BORRAR_FASE_1;
    on_pushButtonBorrarEvento_clicked();

    if ( m_Borrar_Evento_Y_Timer == M_BORRAR_FASE_2 )
    {
        m_Borrar_Evento_Y_Timer = M_BORRAR_FASE_0;

        for ( rowActual = 0 ; rowActual <  m_datos->getNombresAccionesDefinidas().count() ; rowActual ++)
        {
            if (  m_datos->getNombresAccionesDefinidas().at(rowActual) == accionActual)
                break;
        }
        // Elimino la acción seleccionada
        m_datos->borrarAccion(rowActual);
        ui->listWidgetTemporizadores->takeItem(ui->listWidgetTemporizadores->currentRow());
        // Guardo la acción eliminada por si se cancela el dialog
        m_accionesEliminadas.append(accionActual);
    }
}

void DialogTransicion::on_pushButtonElegirTemporizador_clicked()
{
    int fila = ui->listWidgetTemporizadores->currentRow();
    if (fila != (-1)) {
        QString accionActual = ui->listWidgetTemporizadores->currentItem()->text();
        QList<QListWidgetItem *>  itemsList =
                ui->listWidgetAccionesTransicion->findItems(accionActual, Qt::MatchCaseSensitive);
        if (itemsList.isEmpty()) {
            ui->listWidgetAccionesTransicion->addItem(accionActual);
        } else {
            QMessageBox::information(this, tr("Advertencia"),
                                     tr("Este Temporizador ya fue agregado."));
        }
    } else
        QMessageBox::information(this, tr("Advertencia"),
                                 tr("No hay ningún Temporizador seleccionado."));
}

void DialogTransicion::on_pushButtonNuevaMacro_clicked()
{
    bool ok = false;
    //QRegExp regExp("[A-Za-z_]+[A-Za-z0-9_]+");
    QRegExp regExp("[A-Za-z0-9_]+");
    QIcon icon(":/img/img/external.png");
    QString name = RegExpInputDialog::getText(this, tr("Nueva Macro"),
                                              tr("Definir el nombre:"),
                                              "", regExp, &ok, icon, Qt::Dialog);
    if (ok && !name.isEmpty()) {
        name.append("()");
        if(!m_datos->getNombresAccionesDefinidas().contains(name, Qt::CaseSensitive)) {
            m_datos->addNuevaAccion("m_"+ name);
            ui->listWidgetMacros->addItem("m_"+ name);
            ui->listWidgetMacros->setCurrentRow(ui->listWidgetAcciones->count()-1);
        } else
            QMessageBox::information(this, tr("Definir Macro"),
                                     tr("Ya existe una macro definida con el mismo nombre."));
    }
}

void DialogTransicion::on_pushButtonBorrarMacro_clicked()
{
    if(ui->listWidgetMacros->count() == 0 ||
            ui->listWidgetMacros->currentItem() == nullptr)
        return;

    QString accionActual = ui->listWidgetMacros->currentItem()->text();

    // Chequear si la acción a borrar está siendo utilizada (ADVERTENCIA)
    // Primero chequeo la transición actual
    int cantAcciones = ui->listWidgetAccionesTransicion->count();
    for (int i = 0; i < cantAcciones; ++i) {
        if( accionActual == ui->listWidgetAccionesTransicion->item(i)->text()) {
            QMessageBox::warning(this, "Borrar Temporizador",
                                 "El Temporizador forma parte de la transición actual.");
            return;
        }

    }
    // Después el resto de las transiciones
    int cantMaquinas = m_sceneList.count();
    for (int k = 0; k < cantMaquinas; ++k) {
        int cantItems = m_sceneList.at(k)->itemList().count();
        for (int i = 0; i < cantItems; ++i) {
            DiagramItem *item = m_sceneList.at(k)->itemList().at(i);
            foreach (DiagramTransition *arrow, item->transitions()) {
                // Para no volver a verificar la transición actual
                if(arrow != m_arrow) {
                    if(arrow->actions().contains(accionActual)) {
                        QMessageBox::warning(this, tr("Borrar Temporizador"),
                                                 tr("El Temporizador forma parte de otra transición ya definida.\nPrimero debe remover la transición."));
                        return;
                    }
                }
            }
        }
    }

    int rowActual;
    for ( rowActual = 0 ; rowActual <  m_datos->getNombresAccionesDefinidas().count() ; rowActual ++)
    {
        if (  m_datos->getNombresAccionesDefinidas().at(rowActual) == accionActual)
            break;
    }
    // Elimino la acción seleccionada
    m_datos->borrarAccion(rowActual);
    ui->listWidgetMacros->takeItem(ui->listWidgetMacros->currentRow());
    // Guardo la acción eliminada por si se cancela el dialog
    m_accionesEliminadas.append(accionActual);
}

void DialogTransicion::on_pushButtonElegirMacro_clicked()
{
    int fila = ui->listWidgetMacros->currentRow();
    if (fila != (-1)) {
        QString accionActual = ui->listWidgetMacros->currentItem()->text();
        QList<QListWidgetItem *>  itemsList =
                ui->listWidgetAccionesTransicion->findItems(accionActual, Qt::MatchCaseSensitive);
        if (itemsList.isEmpty()) {
            ui->listWidgetAccionesTransicion->addItem(accionActual);
        } else {
            QMessageBox::information(this, tr("Advertencia"),
                                     tr("Esta Macro ya fue agregado."));
        }
    } else
        QMessageBox::information(this, tr("Advertencia"),
                                 tr("No hay ninguna Macro seleccionado."));
}

void DialogTransicion::on_listWidgetMacros_itemDoubleClicked(QListWidgetItem *item)
{
    QString accionActual = item->text();
    QList<QListWidgetItem *>  itemsList =
            ui->listWidgetAccionesTransicion->findItems(accionActual, Qt::MatchCaseSensitive);
    if (itemsList.isEmpty()) {
        ui->listWidgetAccionesTransicion->addItem(accionActual);
    } else {
        QMessageBox::information(this, tr("Advertencia"),
                                 tr("Esta Macro ya fue agregado."));
    }
}

void DialogTransicion::on_listWidgetTemporizadores_itemDoubleClicked(QListWidgetItem *item)
{
    QString accionActual = item->text();
    QList<QListWidgetItem *>  itemsList =
            ui->listWidgetAccionesTransicion->findItems(accionActual, Qt::MatchCaseSensitive);
    if (itemsList.isEmpty()) {
        ui->listWidgetAccionesTransicion->addItem(accionActual);
    } else {
        QMessageBox::information(this, tr("Advertencia"),
                                 tr("Este Temporizador ya fue agregado."));
    }
}
