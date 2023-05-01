#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QMdiSubWindow>

class SubWindow : public QMdiSubWindow
{
    Q_OBJECT

public:
    explicit SubWindow(QWidget *parent = 0);
    double currentScale (void) { return scale; }
    void setScale (qreal const newScale) { scale = newScale; }

protected slots:
    void closeEvent(QCloseEvent *closeEvent);
    void resizeEvent(QResizeEvent *resizeEvent);

private:
    double scale;

};

#endif // SUBWINDOW_H
