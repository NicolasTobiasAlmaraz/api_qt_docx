#include  "{NOMBRE_HEADER}"

void {NOMBRE_CONTROL}(void) 
{
	static char estado;		

	switch (estado)
	{
		case RESET:			 
			// esto lo propone el estado de RESET
			{DESARROLLO_RESET}
			break;

		case ENCENDIDO:
			estado = Encendido();
			break;

		case APAGADO:
			estado = Apagado();
			break;

		default:
			estado = RESET;
			break;
	}
}


char Encendido (void)
{
	char salida = ENCENDIDO;

If  (ifCambio())
	{
		Apagar();
		salida = APAGADO;
	}
	return salida;
}

char Apagado (void)
{
	char salida = APAGADO;

	If(ifCambio())
	{
		Encender();
		salida = ENCENDIDO;
	}
	return salida;
}

char ifCambio(void)
{

	//Return Estado del bit en cuestión leido del buffer limpio de ruido;
	//ACLARAR??!!
	{DESARROLLO_CAMBIO}
	
	return {ESTADO};
}

void Apagar(void)
{
	//Desactivo la salida en cuestión sobre el buffer de salidas
	{DESARROLLO_APAGAR}
	return; 
}

void Encender(void)
{
	//Desactivo la salida en cuestión sobre el buffer de salidas
	{DESARROLLO_ENCENDER}
	return; 
}


hola