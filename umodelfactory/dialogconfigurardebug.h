#ifndef DIALOGCONFIGURARDEBUG_H
#define DIALOGCONFIGURARDEBUG_H

#include <QDialog>

#include <QSerialPort>

namespace Ui {
class DialogConfigurarDebug;
}

class DialogConfigurarDebug : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfigurarDebug(QWidget *parent = nullptr);
    DialogConfigurarDebug(QWidget *parent = nullptr, bool dbgTiempoReal = true, bool generarArchivoLog = true, QString fileName = "");
    ~DialogConfigurarDebug();

    void enumerarPuertos();
    QString getPortName();
    QString getArchivoDebugLogName();
    bool generarArchivoLogEnabled();
    bool debugTiempoRealEnabled();

private slots:
    void on_pushButtonAceptar_clicked();
    void on_pushButtonCancelar_clicked();
    void on_pushButtonRefreshPorts_clicked();
    void on_pushButtonAbrir_clicked();

private:
    Ui::DialogConfigurarDebug *ui;
    QString m_portName;
    QString m_replayDebugFileName;
};

#endif // CONFIGURARDEBUG_H
