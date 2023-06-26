#ifndef DIALOGDOCUMENTACIONGENERADA_H
#define DIALOGDOCUMENTACIONGENERADA_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>

namespace Ui {
class DialogDocumentacionGenerada;
}

class DialogDocumentacionGenerada : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDocumentacionGenerada(QWidget *parent = nullptr, QString ruta="C:/");
    ~DialogDocumentacionGenerada();

private slots:
    void on_PB_ver_clicked();

    void on_PB_ok_clicked();

private:
    Ui::DialogDocumentacionGenerada *ui;
    QString mRuta;
};

#endif // DIALOGDOCUMENTACIONGENERADA_H
