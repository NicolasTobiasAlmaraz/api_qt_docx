#ifndef CONVERCIONDEDATOS_H
#define CONVERCIONDEDATOS_H
#include <QStringList>
#include <QString>
#include <QLineEdit>
#include "fuentes.h"

#define     MAX_LEDS_INFOTRONIC2019                     16
#define     MAX_NOMBRES_INFOTRONIC2019                  30
#define     MAX_SALIDAS_DIGITALES_INFOTRONIC2019        8
#define     MAX_ENTRADAS_DIGITALES_INFOTRONIC2019       6
#define     MAX_ENTRADAS_ANALOGICAS_INFOTRONIC2019      3
#define     MAX_PULSADORES_INFOTRONIC2019               5
#define     MAX_RGB_INFOTRONIC2019                      6
#define     MAX_CARTELES_EXP2_INFOTRONIC2019            14

class ConvercionDeDatos
{
    int m_max_salidas_digitales ;
    int m_max_entradas_digitales ;
    int m_max_entradas_analogicas ;
    int m_max_pulsadores ;
    int m_max_rgb ;
    int m_max_Leds ;
    int m_max_Nombres;
    int m_pagina ;
    int m_check ;

    QString * SalidasDigitales ;
    QString * EntradasDigitales ;
    QString * EntradasAnalogicas ;
    QString * Pulsadores ;
    QString * RGB ;
    QString * Leds ;
    QList <QStringList> *Temporizadores;
    QList <QStringList> *eTemporizadores;

    QString * Nombres ;

    QString RELAYS[8][2];
    QString PULSADORES[5][2];
    QString LEDS_RGB[6][2];
    QString LEDS_EXP2[16][2];

    QString ENTRADAS[6][2];

public:
    ConvercionDeDatos();

    void NewInfotronic( unsigned long N1 , unsigned long N2 , unsigned long N3 , unsigned long N4 , unsigned long N5 , unsigned long N6 , unsigned long N7 );
    void SetSalidasDigitales( int , QString );
    void SetEntradasDigitales( int , QString );
    void SetEntradasAnalogicas( int i, QString );
    void SetPulsadores( int i, QString );
    void SetRGB( int i , QString );
    void SetLED( int i , QString l);
    void Setpagina( int i );
    void SetTemporizadores( QString viejo , QString nuevo );
    void SeteTemporizadores( QString viejo , QString nuevo );
    void SetCheckExpansion( int check );
    void ClearTemporizadores(void);
    void CleareTemporizadores(void);

    QList < QStringList > * GetTemporizadores(  struct Elementos *elementos );
    QList < QStringList > * GeteTemporizadores(  );
    QString GetSalidasDigitales( int  );
    QString GetEntradasDigitales( int  );
    QString GetEntradasAnalogicas( int i );
    QString GetPulsadores( int i );
    QString GetRGB( int i );
    QString GetLED( int i );

    void SetLEDS_EXP2Infotronic2019( int inx , QString NombrePropio );
    void SetRelaysInfotronic2019( int inx , QString NombrePropio );
    void SetPulsadoresInfotronic2019( int inx , QString NombrePropio );    
    void SetRGBInfotronic2019( int inx , QString NombrePropio );  
    void SetEntradasInfotronic2019( int inx , QString NombrePropio );

    QString GetEntradasConvertido( QString NombrePropio);
    QString GetTemporizadoresConvertido( QString NombrePropio);
    QString GeteTemporizadoresConvertido( QString NombrePropio);
    QString GetLEDS_EXP2Convertido( QString NombrePropio);
    QString GetRelayConvertido( QString NombrePropio);
    QString GetPulsadoresConvertido( QString NombrePropio);
    QString GetRGBConvertido( QString NombrePropio);

    int Get_max_salidas_digitales( void );
    int Get_max_entradas_digitales( void );
    int Get_max_entradas_analogicas( void );
    int Get_max_pulsadores( void );
    int Get_max_rgb( void );
    int Get_max_leds( void );
    int Get_pagina( void );
    int GetCheckExpansion( void );

 };
#endif // CONVERCIONDEDATOS_H
