/**
*	\file       AP_MDE.c
*	\brief      Maquina/s de Estado
*	\details    Descripcion detallada del archivo
*	\author     Almaraz
*	\date       27-03-2023 08:54:15
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
//Maquina: Cosecha
#define		Reposo		1	// < Maquina: Cosecha >
#define		Avanzar		2	// < Maquina: Cosecha >
#define		Retirar_Fruta		3	// < Maquina: Cosecha >
#define		Depositar_Fruta		4	// < Maquina: Cosecha >

//Maquina: Plantacion
#define		Reposo		1	// < Maquina: Plantacion >
#define		Agujerear_Tierra		2	// < Maquina: Plantacion >
#define		Plantar_Semilla		3	// < Maquina: Plantacion >
#define		Remover_Tierra		4	// < Maquina: Plantacion >
#define		Tirar_Fertilizante		5	// < Maquina: Plantacion >
#define		Tapar_Agujero		6	// < Maquina: Plantacion >
#define		Avanzar		7	// < Maquina: Plantacion >
#define		Cargar_Insumos		8	// < Maquina: Plantacion >

//Maquina: General
#define		Esperando Configuracion		1	// < Maquina: General >
#define		Configurada		2	// < Maquina: General >
#define		Plantacion_de_semilla		3	// < Maquina: General >
#define		Cosechado		4	// < Maquina: General >

/************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 ************************************************************************************************************/
#define     m_AgujerearTierra()

#define     m_ApagarActuadores()

#define     m_Avanzar()

#define     m_Avanzar()

#define     m_ChequearAlmacenamiento()

#define     m_ChequearConfig()

#define     m_ChequearFertilizante()

#define     m_ChequearSemillas()

#define     m_Cosechar()

#define     m_GuardarFruta()

#define     m_LeerConfig()

#define     m_Plantar()

#define     m_RemoverTierra()

#define     m_RetirarFruta()

#define     m_TaparAgujero()

#define     m_TirarFertilizante()

#define     m_TirarSemillas()


/************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 ************************************************************************************************************/
static int Cosecha ( int );
static int Plantacion ( int );
static int General ( int );

/************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 ************************************************************************************************************/

/************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 ************************************************************************************************************/

/**
*	\fn      static int Cosecha ( int Estado )
*	\brief   Coloque aqui su descripcion
*	\details Amplie la descripcion
*	\author  Almaraz
*	\date    27-03-2023 08:54:15
*   \param   [in] Estado: caso dentro de la maquina (estado actual)
*	\return  int : estado siguiente
*/

static int Cosecha ( int  Estado )
{
    switch ( Estado )
    {

        case RESET :
            m_ChequearAlmacenamiento();
            m_LeerConfig();

            Estado = Reposo;

            break;

        case Reposo :
            if ( s_HayLugar() )
            {
                m_RetirarFruta();

                Estado = Retirar_Fruta;
            }

            break;

        case Avanzar :
            if ( !s_CosechasPendientes() || !s_HayLugar() )
            {

                Estado = Reposo;
            }

            if ( s_HayLugar() && s_CosechasPendientes() )
            {
                m_RetirarFruta();

                Estado = Retirar_Fruta;
            }

            break;

        case Retirar_Fruta :
            if ( s_FrutaRetirada() )
            {
                m_GuardarFruta();

                Estado = Depositar_Fruta;
            }

            break;

        case Depositar_Fruta :
            if ( s_FrutaGuardada() )
            {
                m_Avanzar();
                m_ChequearConfig();
                m_ChequearAlmacenamiento();

                Estado = Avanzar;
            }

            break;

        default:
            Estado = RESET ;
            break;

    }
    return Estado ;
}

/**
*	\fn      static int Plantacion ( int Estado )
*	\brief   Coloque aqui su descripcion
*	\details Amplie la descripcion
*	\author  Almaraz
*	\date    27-03-2023 08:54:15
*   \param   [in] Estado: caso dentro de la maquina (estado actual)
*	\return  int : estado siguiente
*/

static int Plantacion ( int  Estado )
{
    switch ( Estado )
    {

        case RESET :
            m_ApagarActuadores();
            m_ChequearSemillas();
            m_ChequearFertilizante();
            m_ChequearConfig();

            Estado = Reposo;

            break;

        case Reposo :
            if ( s_HaySemillas() && s_HayFertilizante() && s_PlantacionPendiente() )
            {
                m_RemoverTierra();

                Estado = Remover_Tierra;
            }

            if ( !s_HaySemillas() || !s_HayFertilizante() )
            {

                Estado = Cargar_Insumos;
            }

            break;

        case Agujerear_Tierra :
            if ( s_TierraAgujereada() )
            {
                m_TirarSemillas();

                Estado = Plantar_Semilla;
            }

            break;

        case Plantar_Semilla :
            if ( s_SemillasPlantadas() )
            {
                m_TaparAgujero();

                Estado = Tapar_Agujero;
            }

            break;

        case Remover_Tierra :
            if ( s_TierraRemovida() )
            {
                m_TirarFertilizante();

                Estado = Tirar_Fertilizante;
            }

            break;

        case Tirar_Fertilizante :
            if ( s_FertilizanteTirado() )
            {
                m_AgujerearTierra();

                Estado = Agujerear_Tierra;
            }

            break;

        case Tapar_Agujero :
            if ( s_AgujeroTapado() )
            {
                m_Avanzar();
                m_ChequearFertilizante();
                m_ChequearSemillas();

                Estado = Avanzar;
            }

            break;

        case Avanzar :
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
*	\date    27-03-2023 08:54:15
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
*	\date    27-03-2023 08:54:15
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

