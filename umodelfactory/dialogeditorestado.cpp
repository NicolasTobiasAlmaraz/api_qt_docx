#include "dialogeditorestado.h"
#include "ui_dialogeditorestado.h"
#include "diagramscene.h"
#include "diagramitem.h"
#include "diagramtransition.h"

#include <QMessageBox>
#include <QDebug>

// Colores combo-box
static const QStringList colorNames = QStringList() << "azure" << "white"
                                                    << "lightblue" << "royalblue"
                                                    << "lightgreen" << "yellowgreen"
                                                    << "tan" << "orange" << "red";

DialogEditorEstado::DialogEditorEstado(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditorEstado)
{
    ui->setupUi(this);
    QRegExp regExp("[A-Za-z0-9-_ñÑ ]+");
    QRegExpValidator  *validator = new QRegExpValidator(regExp, this);
    ui->lineEditNombre->setValidator(validator);
    ui->lineEditNombre->clearFocus();
    ui->pushButtonEliminar->setDisabled(true);
}

DialogEditorEstado::~DialogEditorEstado()
{
    delete ui;
}

void DialogEditorEstado::setDatos(int indexEstado, XmlHandler *&datos, DiagramScene *&scene)
{
    m_index = indexEstado;
    m_scene = scene;
    m_datos = datos;

    DiagramItem *item = m_scene->itemList().at(indexEstado);
    // Carga el nombre del estado seleccionado
    ui->lineEditNombre->setText(item->name());

    // Carga el color del estado seleccionado
    for (int j = 0; j < colorNames.count(); j++) {
        const QColor color(colorNames.at(j));
        ui->comboBoxColores->addItem("", color);
        const QModelIndex idx = ui->comboBoxColores->model()->index(j, 0);
        ui->comboBoxColores->model()->setData(idx, color, Qt::BackgroundColorRole);
        if(item->color() == color)
            ui->comboBoxColores->setCurrentIndex(j);
    }
    ui->labelColorEstado->setStyleSheet("background-color: "+item->color().name());

    // Carga las transiciones del estado seleccionado
    int idx = 0;
    foreach (DiagramTransition *arrow, item->transitions()) {
        if (arrow->startItem() == item) {
            ui->listWidgetTransiciones->insertItem(idx, item->name() + " -> " + arrow->event()
                                             + " / " + arrow->actions().join(",")
                                             + " -> " + arrow->endItem()->name());
        ++idx;
        }
    }

    // conecto las funciones para editar nombre y color
    connect(ui->comboBoxColores, SIGNAL(currentIndexChanged(int)), this, SLOT(updateColorEstado(int)));
    connect(ui->lineEditNombre, SIGNAL(textEdited(QString)), this, SLOT(updateNombreEstado()));
    m_modificado = false;
}

void DialogEditorEstado::updateNombreEstado()
{
    bool nombreRepetido = false;
    QString nuevoNombre = ui->lineEditNombre->text();
    DiagramItem *item = m_scene->itemList().at(m_index);

    if (nuevoNombre.isEmpty()) {
        ui->labelAdvertenciaNombre->setText(tr("Debe definir un nombre para el Estado!"));
        return;
    }

    if (nuevoNombre.toUpper() != item->name().toUpper()) {
        for (int j=0; j < m_scene->itemList().count(); ++j) {
            if (m_scene->itemList().at(j)->name().toUpper() == nuevoNombre.toUpper()) {
                nombreRepetido = true;
                break;
            }
        }
    }

    if (nombreRepetido == false) {
        ui->labelAdvertenciaNombre->setText("");
        item->setName(nuevoNombre);            // vuelve a cargar los nombres de los estados
        m_modificado = true;
        // vuelve a cargar la lista de transiciones
        ui->listWidgetTransiciones->clear();
        int idx = 0;
        foreach (DiagramTransition *arrow, item->transitions()) {
            if (arrow->startItem() == item) {
                ui->listWidgetTransiciones->insertItem(idx, item->name() + " -> " + arrow->event()
                                                 + " / " + arrow->actions().join(",")
                                                 + " -> " + arrow->endItem()->name());
            ++idx;
            }
        }
    } else {
        ui->labelAdvertenciaNombre->setText(tr("Ya existe un Estado con el mismo nombre!"));
    }
}

void DialogEditorEstado::on_pushButtonEliminar_clicked()
{
    // Borra la transición del diagrama
    int idx = 0;
    int idTr = ui->listWidgetTransiciones->currentRow();
    DiagramItem *item = m_scene->itemList().at(m_index);

    QMessageBox msgBox;
    QPushButton *si = msgBox.addButton(tr("Si"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("No"), QMessageBox::RejectRole);
    msgBox.setText("¿Está seguro que desea eliminar la Transición?");
    msgBox.setWindowIcon(QIcon(":/img/img/cross.png"));
    msgBox.setWindowTitle(tr("Eliminar Transición"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    if (msgBox.clickedButton() == si) {
        m_modificado = true;
        foreach (DiagramTransition *arrow, item->transitions()) {
            if (arrow->startItem() == item) {
                if (idx == idTr) {
                    arrow->startItem()->removeTransition(arrow);
                    arrow->endItem()->removeTransition(arrow);
                    m_scene->removeItem(arrow);
                    break;
                }
                else
                    ++idx;
            }
        }
        // Vuelve a cargar las transiciones del estado seleccionado
        idx = 0;
        ui->listWidgetTransiciones->clear();
        foreach (DiagramTransition *arrow, item->transitions()) {
            if (arrow->startItem() == item) {
                ui->listWidgetTransiciones->insertItem(idx, item->name() + " -> " + arrow->event()
                                                 + " / " + arrow->actions().join(",")
                                                 + " -> " + arrow->endItem()->name());
            ++idx;
            }
        }
        // Deshabilito 'Remover' hasta que no se seleccione otra trans...
        ui->pushButtonEliminar->setDisabled(true);
    }
}

void DialogEditorEstado::on_pushButtonAceptar_clicked()
{
    this->accept();
}

void DialogEditorEstado::updateColorEstado(int idColor)
{
    DiagramItem *item = m_scene->itemList().at(m_index);
    QString prevColor = item->color().name();
    QString newColor = colorNames.at(idColor);

    if(prevColor != newColor) {
        item->setColor(newColor);
        m_modificado = true;
        ui->labelColorEstado->setStyleSheet("background-color: "+item->color().name());
    }
}

void DialogEditorEstado::on_listWidgetTransiciones_itemSelectionChanged()
{
    ui->pushButtonEliminar->setEnabled(true);
}
