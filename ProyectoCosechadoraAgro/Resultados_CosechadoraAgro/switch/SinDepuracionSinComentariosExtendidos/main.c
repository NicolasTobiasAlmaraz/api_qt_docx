/**
*	\file       main.c
*	\brief      Proyecto: CosechadoraAgro
*	\details    Descripcion detallada del archivo
*	\author     Almaraz
*	\date       01-05-2023 14:06:29
*/

/************************************************************************************************************
 *** INCLUDES
 ************************************************************************************************************/

#include "AP_MDE.h"
#include "AP_Inicializacion.h"

void main ( void )
{
    Inicializar( );

    while ( 1 )
    {
        MaquinaDeEstados();
    }
    return ;
}

