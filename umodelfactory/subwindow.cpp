#include "subwindow.h"
#include <QCloseEvent>

SubWindow::SubWindow(QWidget *parent) :
    QMdiSubWindow(parent)
{
    scale = 1.0;
}

void SubWindow::closeEvent(QCloseEvent *closeEvent)
{
    this->hide();
    closeEvent->ignore();
}

void SubWindow::resizeEvent(QResizeEvent *resizeEvent)
{
    QMdiSubWindow::resizeEvent(resizeEvent);
}
