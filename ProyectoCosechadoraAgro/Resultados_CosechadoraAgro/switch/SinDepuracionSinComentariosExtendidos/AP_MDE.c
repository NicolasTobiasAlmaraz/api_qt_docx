/**
*	\file       AP_MDE.c
*	\brief      Maquina/s de Estado
*	\details    Descripcion detallada del archivo
*	\author     Nicolas
*	\date       28-06-2023 11:33:30
*/
/************************************************************************************************************
 *** INCLUDES
 ************************************************************************************************************/
#include "AP_MDE.h"
#include "AP_FuncionesMDE.h"

/************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 ************************************************************************************************************/
#define		RESET		0	
//Maquina: MdE2
#define		Tapando Tierra		1	// < Maquina: MdE2 >
#define		Posicionando		2	// < Maquina: MdE2 >
#define		Removiendo		3	// < Maquina: MdE2 >
#define		PLANTANDO		4	// < Maquina: MdE2 >
#define		Sin_Semillas		5	// < Maquina: MdE2 >

//Maquina: MdE3
#define		cosechando		1	// < Maquina: MdE3 >
#define		plantando		2	// < Maquina: MdE3 >
#define		sin_configuracion		3	// < Maquina: MdE3 >

//Maquina: MdE4
#define		led_On		1	// < Maquina: MdE4 >
#define		led_Off		2	// < Maquina: MdE4 >

/************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 ************************************************************************************************************/
#define     m_agujerearPiso()

#define     m_apagarLed()

#define     m_avanzar()

#define     m_prenderLed()

#define     m_tapar()

#define     m_tirarSemilla()


/************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 ************************************************************************************************************/
static int MdE2 ( int );
static int MdE3 ( int );
static int MdE4 ( int );

/************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 ************************************************************************************************************/
int plantaciones ;		//

/************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 ************************************************************************************************************/

/**
*	\fn      static int MdE2 ( int Estado )
*	\brief   Coloque aqui su descripcion
*	\details Amplie la descripcion
*	\author  Nicolas
*	\date    28-06-2023 11:33:30
*   \param   [in] Estado: caso dentro de la maquina (estado actual)
*	\return  int : estado siguiente
*/

static int MdE2 ( int  Estado )
{
    switch ( Estado )
    {

        case RESET :
            plantaciones = 0;

            Estado = Posicionando;

            break;

        case Tapando Tierra :
            if ( s_agujeroTapado() )
            {
                m_avanzar();
                plantaciones += 1;

                Estado = Posicionando;
            }

            break;

        case Posicionando :
            if ( s_enPosicion() )
            {
                m_agujerearPiso();

                Estado = Removiendo;
            }

            break;

        case Removiendo :
            if ( s_agujereado() && !s_sinSemillas() )
            {
                m_tirarSemilla();

                Estado = PLANTANDO;
            }

            if ( s_sinSemillas() )
            {

                Estado = Sin_Semillas;
            }

            break;

        case PLANTANDO :
            if ( s_semillaTirada() )
            {
                m_tapar();

                Estado = Tapando Tierra;
            }

            break;

        case Sin_Semillas :
            break;

        default:
            Estado = RESET ;
            break;

    }
    return Estado ;
}

/**
*	\fn      static int MdE3 ( int Estado )
*	\brief   Coloque aqui su descripcion
*	\details Amplie la descripcion
*	\author  Nicolas
*	\date    28-06-2023 11:33:30
*   \param   [in] Estado: caso dentro de la maquina (estado actual)
*	\return  int : estado siguiente
*/

static int MdE3 ( int  Estado )
{
    switch ( Estado )
    {

        case RESET :

            Estado = sin_configuracion;

            break;

        case cosechando :
            if ( s_finCosecha() )
            {

                Estado = sin_configuracion;
            }

            break;

        case plantando :
            if ( s_finPlantar() )
            {

                Estado = sin_configuracion;
            }

            break;

        case sin_configuracion :
            if ( s_configCosecha() )
            {

                Estado = cosechando;
            }

            if ( s_configPlantar() )
            {

                Estado = plantando;
            }

            break;

        default:
            Estado = RESET ;
            break;

    }
    return Estado ;
}

/**
*	\fn      static int MdE4 ( int Estado )
*	\brief   Coloque aqui su descripcion
*	\details Amplie la descripcion
*	\author  Nicolas
*	\date    28-06-2023 11:33:30
*   \param   [in] Estado: caso dentro de la maquina (estado actual)
*	\return  int : estado siguiente
*/

static int MdE4 ( int  Estado )
{
    switch ( Estado )
    {

        case RESET :
            m_apagarLed();

            Estado = led_Off;

            break;

        case led_On :
            if ( e_tiempoOn() )
            {
                t_tiempoOn_001_S();
                m_apagarLed();

                Estado = led_Off;
            }

            break;

        case led_Off :
            if ( e_tiempoOn() )
            {
                t_tiempoOn_001_S();
                m_prenderLed();

                Estado = led_On;
            }

            break;

        default:
            Estado = RESET ;
            break;

    }
    return Estado ;
}

/************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 ************************************************************************************************************/

/**
*	\fn      void MaquinaDeEstados ( void )
*	\brief   Coloque aqui su descripcion
*	\details Amplie la descripcion
*	\author  Nicolas
*	\date    28-06-2023 11:33:30
*   \param   void
*	\return  void
*/
void MaquinaDeEstados ( void )
{
    static int estados_MdE2 = RESET;
    static int estados_MdE3 = RESET;
    static int estados_MdE4 = RESET;

    // Coloque su codigo aqui

    estados_MdE2 = MdE2( estados_MdE2 );
    estados_MdE3 = MdE3( estados_MdE3 );
    estados_MdE4 = MdE4( estados_MdE4 );


    return ;
}

anzar :
            if ( s_PlantacionPendiente() && s_HaySemillas() && s_HayFertilizante() )
            {
                m_RemoverTierra();

                Estado = Remover_Tierra;
            }

            if ( !s_PlantacionPendiente() )
            {

                Estado = Reposo;
            }

            if ( !s_HayFertilizante() && !s_HaySemillas() )
            {

                Estado = Cargar_Insumos;
            }

            break;

        case Cargar_Insumos :
            if ( s_HaySemillas() && s_HayFertilizante() )
            {
                m_ChequearConfig();

                Estado = Reposo;
            }

            break;

        default:
            Estado = RESET ;
            break;

    }
    return Estado ;
}

/**
*	\fn      static int General ( int Estado )
*	\brief   Coloque aqui su descripcion
*	\details Amplie la descripcion
*	\author  Almaraz
*	\date    01-05-2023 14:06:29
*   \param   [in] Estado: caso dentro de la maquina (estado actual)
*	\return  int : estado siguiente
*/

static int General ( int  Estado )
{
    switch ( Estado )
    {

        case RESET :
            m_ApagarActuadores();

            Estado = Esperando Configuracion;

            break;

        case Esperando Configuracion :
            if ( s_Config() )
            {
                m_LeerConfig();

                Estado = Configurada;
            }

            break;

        case Configurada :
            if ( s_Config()Cosecha )
            {
                m_Cosechar();

                Estado = Cosechado;
            }

            if ( s_Config()Plantado )
            {
                m_Plantar();

                Estado = Plantacion_de_semilla;
            }

            if ( s_ErrorConfig() )
            {

                Estado = Esperando Configuracion;
            }

            break;

        case Plantacion_de_semilla :
            if ( s_ErrorProceso() )
            {
                m_ApagarActuadores();

                Estado = Esperando Configuracion;
            }

            break;

        case Cosechado :
            if ( s_ErrorProceso() )
            {
                m_ApagarActuadores();

                Estado = Esperando Configuracion;
            }

            break;

        default:
            Estado = RESET ;
            break;

    }
    return Estado ;
}

/************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 ************************************************************************************************************/

/**
*	\fn      void MaquinaDeEstados ( void )
*	\brief   Coloque aqui su descripcion
*	\details Amplie la descripcion
*	\author  Almaraz
*	\date    01-05-2023 14:06:29
*   \param   void
*	\return  void
*/
void MaquinaDeEstados ( void )
{
    static int estados_Cosecha = RESET;
    static int estados_Plantacion = RESET;
    static int estados_General = RESET;

    // Coloque su codigo aqui

    estados_Cosecha = Cosecha( estados_Cosecha );
    estados_Plantacion = Plantacion( estados_Plantacion );
    estados_General = General( estados_General );


    return ;
}

