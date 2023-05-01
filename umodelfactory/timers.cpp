//#include "mainwindow.h"
#include "timers.h"
#include <stdio.h>
//#include "ui_mainwindow.h"

timers::timers()
{
    endtimer = 0;
    basedetiempo = 0;
}

timers::~timers()
{
    endtimer = false;
}

void timers::Set_basedetiempo(int bt)
{
    basedetiempo = bt;
    return;
}

int timers::Get_basedetiempo(void)
{
    return basedetiempo;
}

void timers::timerstart (struct timer &T )
{
    int suma = 0;
    int inx = 0;
    struct timer a;

    if ( !TimerRun.count() )
    {
       TimerRun.push_front( T );
       return ;
    }

    if (T.deltaT <= TimerRun.at(0).deltaT)
    {
       a = TimerRun.value( 0 );
       a.deltaT-=T.deltaT;
       TimerRun.replace(0,a);
       a = TimerRun.value( 0 );
       TimerRun.push_front( T );
       return ;
    }

    while (inx < TimerRun.count( ) && T.deltaT > suma)
    {
       suma += TimerRun.at( inx ).deltaT;
       inx++;
    }

    if ( TimerRun.count( ) == inx  && suma < T.deltaT )
    {
       T.deltaT -= suma;
       TimerRun.push_back( T );
       return ;
    }

    inx--;
    suma = suma - TimerRun.at( inx ).deltaT;

    T.deltaT = T.deltaT - suma;

    a = TimerRun.value( inx );
    a.deltaT -= T.deltaT;
    TimerRun.replace(inx,a);
    TimerRun.insert( inx , T );

    return ;
}


bool timers::timerevent( QTimer &timer )
{
    struct timer T , d;
    bool salida = 1 ;

    timer.start( basedetiempo );

    if ( TimerRun.count() )
    {
        d = TimerRun.value(0);

        if ( d.deltaT  != 0)
        {
            d.deltaT --;
            TimerRun.replace( 0 , d );
        }

        while( TimerRun.count() && !(TimerRun.value(0).deltaT ) )
        {
           // TimerRun.value(0).Evento();
            T = TimerRun.value(0);
            TimerRun.pop_front();
            TimerEnd << T;
            endtimer ++;
        }
        salida = 0;
    }
    return salida;
}

bool timers::timerstop( QString evento )
{
    struct timer T , d;
    bool salida = 1 ;

    for  (int i = 0 ; i < TimerRun.count( ) ; i++ )
    {
        if ( TimerRun.value(i).evento == evento )
        {
            salida = 0;
            if ( i != TimerRun.count( ) - 1)
            {
                d = TimerRun.value(i);
                TimerRun.removeAt(i);
                T = TimerRun.value(i);
                T.deltaT += d.deltaT;
                TimerRun.replace(i,T);
            }
            else
                TimerRun.removeAt(i);
        }
    }
    return salida;
}

void timers::timerclose( void )
{
    TimerRun.clear();
}

int timers:: get_endtimer ( struct timer &d )
{
    if (!endtimer)
        return 0;

    d = TimerEnd.value(0) ;
    TimerEnd.pop_front();
    return endtimer;

}

void timers:: set_endtimer ( void )
{
    endtimer --;
}

void timers::start_timer ( QTimer &timers)
{
    timers.start( 1 );
}
