/**
*	\file       main.c
*	\brief      Proyecto: CosechadoraAgro
*	\details    Descripcion detallada del archivo
*	\author     Almaraz
*	\date       27-03-2023 08:54:15
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

