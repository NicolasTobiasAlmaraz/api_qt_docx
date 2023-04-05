/**
*	\file       AP_FuncionesMDE.h
*	\brief      Prototipos, defines, etc. de los Eventos y Acciones asociadas a la M de E
*	\details    Descripcion detallada del archivo
*	\author     Almaraz
*	\date       27-03-2023 08:54:15
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

int s_Config ( void );
int s_ConfigCosecha ( void );
int s_ConfigPlantado ( void );
int s_ErrorConfig ( void );
int s_ErrorProceso ( void );
int s_HaySemillas ( void );
int s_HayFertilizante ( void );
int s_TierraRemovida ( void );
int s_FertilizanteTirado ( void );
int s_TierraAgujereada ( void );
int s_SemillasPlantadas ( void );
int s_AgujeroTapado ( void );
int s_PlantacionPendiente ( void );
int s_HayLugar ( void );
int s_FrutaRetirada ( void );
int s_FrutaGuardada ( void );
int s_CosechasPendientes ( void );

/*-------------------------------------------- ACCIONES ----------------------------------------------------*/


/*------------------------------------------TEMPORIZADORES -------------------------------------------------*/


#endif /* INC_AP_FUNCIONESMDEE_H_ */
