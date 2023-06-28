/**
*	\file       AP_FuncionesMDE.h
*	\brief      Prototipos, defines, etc. de los Eventos y Acciones asociadas a la M de E
*	\details    Descripcion detallada del archivo
*	\author     Nicolas
*	\date       28-06-2023 11:33:30
*/

#ifndef INC_AP_FUNCIONESMDEE_H_
#define INC_AP_FUNCIONESMDEE_H_

/************************************************************************************************************
 *** INCLUDES GLOBALES
 ************************************************************************************************************/

/************************************************************************************************************
 *** DEFINES GLOBALES
 ************************************************************************************************************/
#define		TRUE		1
#define		FALSE		0
#define		ON		1
#define		OFF		0

/************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 ************************************************************************************************************/

/************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES GLOBALES
 ************************************************************************************************************/

/*-------------------------------------------- EVENTOS ----------------------------------------------------*/

int s_configCosecha ( void );
int s_configPlantar ( void );
int s_finCosecha ( void );
int s_finPlantar ( void );
int e_tiempoOn ( void );
int s_sinSemillas ( void );
int s_agujereado ( void );
int s_semillaTirada ( void );
int s_agujeroTapado ( void );
int s_enPosicion ( void );

/*-------------------------------------------- ACCIONES ----------------------------------------------------*/


/*------------------------------------------TEMPORIZADORES -------------------------------------------------*/

void t_tiempoOn ( void );
void t_tiempoOn ( void );

#endif /* INC_AP_FUNCIONESMDEE_H_ */
--------------------------------*/


/*------------------------------------------TEMPORIZADORES -------------------------------------------------*/


#endif /* INC_AP_FUNCIONESMDEE_H_ */
