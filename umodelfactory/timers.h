#ifndef TEMPORIZADORES_H
#define TEMPORIZADORES_H
#include <QMainWindow>
#include <QTimer>
//#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include <QMessageBox>

//namespace Ui { class MainWindow; }

struct timer
{
    int deltaT;
    int tiempo;
    int n; //campo para test. Después lo saco.
    QString evento;
    void (* Evento) (void);
    timer (void ){ deltaT = tiempo = n = 0; evento = "" ; Evento = nullptr ;}
};

class timers
{
    int endtimer;

    QList < struct timer > TimerEnd;
    QList < struct timer > TimerRun;

    int basedetiempo;

public:
    timers();
    ~timers (void );
    void timerstart ( struct timer &T );
    bool timerstop( QString evento );
    void timerclose( void );
    void start_timer (QTimer & timers);
    int get_endtimer ( struct timer &d );
    void set_endtimer ( void );
    void Set_basedetiempo( int bt );
    int Get_basedetiempo( void );
    bool timerevent( QTimer & );
};

#endif // TEMPORIZADORES_H
