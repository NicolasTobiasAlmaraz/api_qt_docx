#include "converciondedatos.h"
#include <QString>

//"
//"ApagarRele1"
//"PrenderRele2"
//"ApagarRele2"
//"PrenderRele3"
//"ApagarRele3"
//"PrenderRele4"
//"ApagarRele4"




//"eRele1_abierto"
//"eRele1_cerrado"
//"eRele2_abierto"
//"eRele2_cerrado"
//"eRele3_abierto"
//"eRele3_cerrado"
//"eRele4_abierto"
//"eRele4_cerrado"


//"ApagarLed0"
//"PrenderLed0"
//"ApagarLed1"
//"PrenderLed1"
//"ApagarLed2"
//"PrenderLed2"
//"ApagarLed3"
//"PrenderLed3"

ConvercionDeDatos::ConvercionDeDatos()
{
    SalidasDigitales = nullptr;
    EntradasDigitales = nullptr;
    EntradasAnalogicas = nullptr;
    Pulsadores = nullptr;
    RGB = nullptr;

    Temporizadores = nullptr;
    eTemporizadores = nullptr;

    m_max_salidas_digitales = 0;
    m_max_entradas_digitales = 0;
    m_max_entradas_analogicas = 0;
    m_max_pulsadores = 0;
    m_max_rgb = 0;
    m_pagina = 0;
    m_check = 0;

    for ( int i = 0 ; i < 8 ; i++)
        RELAYS[i][0] = "";

    RELAYS[0][1] = "ApagarRele1";
    RELAYS[1][1] = "PrenderRele1";
    RELAYS[2][1] = "ApagarRele2";
    RELAYS[3][1] = "PrenderRele2";
    RELAYS[4][1] = "ApagarRele3";
    RELAYS[5][1] = "PrenderRele3";
    RELAYS[6][1] = "ApagarRele4";
    RELAYS[7][1] = "PrenderRele4";

    for ( int i = 0 ; i < 5 ; i++)
        PULSADORES[i][0] = "";
    PULSADORES[0][1] = "Tecla0";
    PULSADORES[1][1] = "Tecla1";
    PULSADORES[2][1] = "Tecla2";
    PULSADORES[3][1] = "Tecla3";
    PULSADORES[4][1] = "Tecla4";

    for ( int i = 0 ; i < 6 ; i++)
        LEDS_RGB[i][0] = "";
    LEDS_RGB[0][1] = "ApagarLedRed";
    LEDS_RGB[1][1] = "PrenderLedRed";
    LEDS_RGB[2][1] = "ApagarLedGreen";
    LEDS_RGB[3][1] = "PrenderLedGreen";
    LEDS_RGB[4][1] = "ApagarLedBlue";
    LEDS_RGB[5][1] = "PrenderLedBlue";

    for ( int i = 0 ; i < 6 ; i++)
        ENTRADAS[i][0] = "";

    ENTRADAS[0][1] = "Sensor1";
    ENTRADAS[1][1] = "!Sensor1";
    ENTRADAS[2][1] = "Sensor2";
    ENTRADAS[3][1] = "!Sensor2";
    ENTRADAS[4][1] = "Sensor3";
    ENTRADAS[5][1] = "!Sensor3";

    LEDS_EXP2[0][1] = "ApagarLed0";
    LEDS_EXP2[1][1] = "PrenderLed0";
    LEDS_EXP2[2][1] = "ApagarLed1";
    LEDS_EXP2[3][1] = "PrenderLed1";
    LEDS_EXP2[4][1] = "ApagarLed2";
    LEDS_EXP2[5][1] = "PrenderLed2";
    LEDS_EXP2[6][1] = "ApagarLed3";
    LEDS_EXP2[7][1] = "PrenderLed3";
    LEDS_EXP2[8][1] = "ApagarLed4";
    LEDS_EXP2[9][1] = "PrenderLed4";
    LEDS_EXP2[10][1] = "ApagarLed5";
    LEDS_EXP2[11][1] = "PrenderLed5";
    LEDS_EXP2[12][1] = "ApagarLed6";
    LEDS_EXP2[13][1] = "PrenderLed6";
    LEDS_EXP2[14][1] = "ApagarLed7";
    LEDS_EXP2[15][1] = "PrenderLed7";
}

void ConvercionDeDatos::NewInfotronic( unsigned long N1 , unsigned long N2 , unsigned long N3 , unsigned long N4 , unsigned long N5 , unsigned long N6, unsigned long N7 )
{
    if ( SalidasDigitales )
    {
        delete [] SalidasDigitales;
        delete [] EntradasDigitales;
        delete [] EntradasAnalogicas;
        delete [] Pulsadores;
        delete [] RGB;
        delete [] Leds;
        delete [] Nombres;

        delete Temporizadores;
        delete eTemporizadores;
    }

    m_max_salidas_digitales = (int ) N1;
    m_max_entradas_digitales = (int )N2;
    m_max_entradas_analogicas = (int )N3;
    m_max_pulsadores = (int )N4;
    m_max_rgb = (int )N5;
    m_max_Leds = (int )N6;
    m_max_Nombres = (int )N7;

    SalidasDigitales = new QString [ N1 ] ;
    EntradasDigitales = new QString [ N2 ] ;
    EntradasAnalogicas  = new QString [ N3 ] ;
    Pulsadores  = new QString [ N4 ] ;
    RGB  = new QString [ N5 ] ;
    Temporizadores = new QList<QStringList>;
    eTemporizadores = new QList<QStringList>;
    Leds = new QString[ N6 ];
    Nombres = new QString[ N7 ];
}

int ConvercionDeDatos::Get_max_salidas_digitales( void )
{
    return m_max_salidas_digitales;
}
int ConvercionDeDatos::Get_max_entradas_digitales( void )
{
    return m_max_entradas_digitales;
}
int ConvercionDeDatos::Get_max_entradas_analogicas( void )
{
    return  m_max_entradas_analogicas;
}
int ConvercionDeDatos::Get_max_pulsadores( void )
{
    return (int ) m_max_pulsadores;

}
int ConvercionDeDatos::Get_max_rgb( void )
{
    return m_max_rgb;
}
int ConvercionDeDatos::Get_max_leds( void )
{
    return m_max_Leds;
}

void ConvercionDeDatos::SetSalidasDigitales( int i, QString salida)
{
    SalidasDigitales[ i ] = salida ;
}

void ConvercionDeDatos::SetEntradasDigitales( int i, QString entrada)
{
    EntradasDigitales[ i ] = entrada ;
}
void ConvercionDeDatos::SetEntradasAnalogicas( int i, QString entrada)
{
    EntradasAnalogicas[ i ] = entrada ;
}
void ConvercionDeDatos::SetPulsadores( int i, QString pulsador)
{
    Pulsadores[ i ] = pulsador ;
}
void ConvercionDeDatos::SetRGB( int i , QString rgb)
{
    RGB[ i ] = rgb ;
}

void ConvercionDeDatos::SetLED( int i , QString l)
{
    Leds[ i ] = l ;
}

void ConvercionDeDatos::Setpagina( int i )
{
    m_pagina = i;
}

QString ConvercionDeDatos::GetEntradasDigitales( int i )
{
    return EntradasDigitales[ i ] ;
}

QString ConvercionDeDatos::GetSalidasDigitales( int i )
{
    return SalidasDigitales[ i ] ;
}
QString ConvercionDeDatos::GetEntradasAnalogicas( int i )
{
    return EntradasAnalogicas[ i ] ;
}
QString ConvercionDeDatos::GetPulsadores( int i )
{
    return Pulsadores[ i ] ;
}
QString ConvercionDeDatos::GetRGB( int i )
{
    return RGB[ i ] ;
}

QString ConvercionDeDatos::GetLED( int i )
{
    return Leds[ i ] ;
}

int ConvercionDeDatos::Get_pagina(void )
{
    return m_pagina ;
}

int ConvercionDeDatos::GetCheckExpansion( void )
{
     return m_check;
}

void ConvercionDeDatos::ClearTemporizadores( void )
{
    Temporizadores->clear();
}

void ConvercionDeDatos::SetTemporizadores( QString viejo , QString nuevo )
{
     QStringList aux;
     aux << viejo << nuevo;
     Temporizadores->push_back(aux);
     return;
}

void ConvercionDeDatos::SeteTemporizadores( QString viejo , QString nuevo )
{
     QStringList aux;
     aux << viejo << nuevo;
     eTemporizadores->push_back(aux);
     return;
}

void ConvercionDeDatos::CleareTemporizadores( void )
{
    eTemporizadores->clear();
}

void ConvercionDeDatos::SetCheckExpansion( int check )
{
     m_check = check;
     return;
}

QList < QStringList > * ConvercionDeDatos::GetTemporizadores( struct Elementos *e)
{
    //// aca tengo que eliminar los timers que cambie de nombre
    //// antes de retornalo
    int salida = 0;

    Temporizadores->clear();
    eTemporizadores->clear();

    QStringList b= e->nTemporizadores;

    for ( int j = 0 ; j < b.count() - 1; j++) // El ultimo es ""
    {
        QStringList t = b[j].split("_");
        QString valt;
        int T;
        T = t[t.count() - 2].toInt();
        if ( t[t.count() - 1] == "D()" )
            T *= 100;
        else
            T *= 1000;
        if ( T > 99999 )
            T = 99999;

        valt = QString::number(T);
        QStringList aux , aux1;

        aux <<  b[j].replace("()","")  <<  ("IniciarTimer_" + valt);
        Temporizadores->push_back(aux);

        QString n = "e";

        for ( int m = 1 ; m < t.count()-2 ; m++)
        {
            n += "_";
            n += t[m];
        }
        aux1 << n << ("Timer_" + valt );
        eTemporizadores->push_back(aux1);

    }

    return Temporizadores;
}

QList < QStringList > * ConvercionDeDatos::GeteTemporizadores( void )
{
     return eTemporizadores;
}

void ConvercionDeDatos::SetRelaysInfotronic2019( int inx , QString NombrePropio )
{
     RELAYS[inx][0] = NombrePropio;
     return;
}

QString ConvercionDeDatos::GetRelayConvertido( QString NombrePropio)
{
    for (int i = 0 ; i < 8 ; i++)
    {
        if ( RELAYS[i][0] == NombrePropio )
            return RELAYS[i][1];
    }
    return "";
}

void ConvercionDeDatos::SetPulsadoresInfotronic2019( int inx , QString NombrePropio )
{
     PULSADORES[inx][0] = NombrePropio;
     return;
}

QString ConvercionDeDatos::GetPulsadoresConvertido( QString NombrePropio)
{
    for (int i = 0 ; i < 5 ; i++)
    {
        if ( PULSADORES[i][1] == NombrePropio )
            return PULSADORES[i][0];
    }
    return "";
}

void ConvercionDeDatos::SetRGBInfotronic2019( int inx , QString NombrePropio )
{
     LEDS_RGB[inx][0] = NombrePropio;
     return;
}

void ConvercionDeDatos::SetLEDS_EXP2Infotronic2019( int inx , QString NombrePropio )
{
     LEDS_EXP2[inx][0] = NombrePropio;
     return;
}

QString ConvercionDeDatos::GetLEDS_EXP2Convertido( QString NombrePropio)
{
    for (int i = 0 ; i < 16 ; i++)
    {
        QString a = LEDS_EXP2[i][0];
        if ( LEDS_EXP2[i][0] == NombrePropio )
            return LEDS_EXP2[i][1];
    }
    return "";
}

QString ConvercionDeDatos::GetRGBConvertido( QString NombrePropio)
{
    for (int i = 0 ; i < 6 ; i++)
    {
        QString a = LEDS_RGB[i][0];
        if ( LEDS_RGB[i][0] == NombrePropio )
            return LEDS_RGB[i][1];
    }
    return "";
}

void ConvercionDeDatos::SetEntradasInfotronic2019( int inx , QString NombrePropio )
{
     ENTRADAS[inx][0] = NombrePropio;
     return;
}

QString ConvercionDeDatos::GetEntradasConvertido( QString NombrePropio)
{
    for (int i = 0 ; i < 6 ; i++)
    {
        if ( ENTRADAS[i][1] == NombrePropio )
            return ENTRADAS[i][0];
    }
    return "";
}
QString ConvercionDeDatos::GeteTemporizadoresConvertido( QString NombrePropio)
{
    for (int i = 0 ; i <  eTemporizadores->count() ; i++ )
    {
         if ( eTemporizadores->at(i)[1] == NombrePropio )
            return eTemporizadores->at(i)[0];
    }
    return "";
}


QString ConvercionDeDatos::GetTemporizadoresConvertido( QString NombrePropio)
{
    for (int i = 0 ; i <  Temporizadores->count() ; i++ )
    {

        if ( Temporizadores->at(i)[0] == NombrePropio )
            return Temporizadores->at(i)[1];
    }
    return "";
}
