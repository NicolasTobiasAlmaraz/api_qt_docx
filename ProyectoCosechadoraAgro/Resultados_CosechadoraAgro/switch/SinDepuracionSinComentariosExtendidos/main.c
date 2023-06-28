/**
*	\file       main.c
*	\brief      Proyecto: CosechadoraAgro
*	\details    Descripcion detallada del archivo
*	\author     Nicolas
*	\date       28-06-2023 11:33:30
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

