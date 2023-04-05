#include <writerodt.h>
#include <QString>
#include <list>
#include <QFile>
#include <QTextStream>

//--------------------------------------------------
//------PATHS RELATIVOS
//--------------------------------------------------

//Paths relativos a Qt
#define EXE_PATH        "../../EjecutableWindows/genOdt.exe"
#define CODES_DIR_PATH  "../../../ProyectoCosechadoraAgro/Resultados_CosechadoraAgro/switch/SinDepuracionSinComentariosExtendidos"

//Paths relativos a Python
#define OUT_PATH        "../../ProyectoCosechadoraAgro/doc_CosechadoraAgro/doc_CosechadoraAgro.odt"
#define IMAGES_DIR_PATH "../../ProyectoCosechadoraAgro/doc_CosechadoraAgro"


//--------------------------------------------------
//------PATHS ABSOLUTOS
//--------------------------------------------------
////Paths relativos a Qt
//#define EXE_PATH        "D:/UTN/uModelFactory/uModelFactory-Documentacion/uModelFactory-Doc/EjecutableWindows/genOdt.exe"

////Paths relativos a Python
//#define OUT_PATH        "D:/UTN/uModelFactory/uModelFactory-Documentacion/ProyectoCosechadoraAgro/doc_CosechadoraAgro/doc_CosechadoraAgro.odt"
//#define IMAGES_DIR_PATH "D:/UTN/uModelFactory/uModelFactory-Documentacion/ProyectoCosechadoraAgro/doc_CosechadoraAgro"
//#define CODES_DIR_PATH  "D:/UTN/uModelFactory/uModelFactory-Documentacion/ProyectoCosechadoraAgro/doc_CosechadoraAgro/Resultados_CosechadoraAgro/switch/SinDepuracionSinComentariosExtendidos"

typedef struct {
    //Header
    QString autores;
    QString fecha;
    QString pathLogoUtn;

    //Nombre proyecto
    QString titulo;

    //Info teorica
    bool fIntroTeorica;
    QString introTeoricaEstados;
    QString introTeoricaTransiciones;
    QString introTeoricaEventos;
    QString introTeoricaAcciones;
    QString introTeoricaReset;

    //Contenido
    std::list <QString> nombresMaq;
    std::list <imageOdt> imagenesMaq;
    bool fTablaEstadosTransiciones;
    std::list <tableOdt> tablasEstTranc;

    //Codigo
    bool fCodigos;
    std::list <QString> pathsCodigos;

    //Listado de eventos
    std::list <std::list <QString>> eventosMacros;
    std::list <std::list <QString>> eventosBooleanos;
    std::list <std::list <QString>> eventosTemporizados;

    //Listado variables y acciones
    std::list <QString> variables;
    std::list <QString> acciones;
} CONTENIDO_DOCUMENTACION;


void writeDocument(QString exePath, QString outPath, CONTENIDO_DOCUMENTACION configDoc);
writerOdt pageSetUp(CONTENIDO_DOCUMENTACION configDoc, QString exePath, QString outPath);
void escribirTitulo(writerOdt* manager, CONTENIDO_DOCUMENTACION configDoc);
void escribirIntroTeorica(writerOdt* manager, CONTENIDO_DOCUMENTACION configDoc);
void escribirTituloMaqGeneral(writerOdt* manager);
void escribirTituloMaq(writerOdt* manager, QString titulo);
void escribirDiagrama(writerOdt* manager, imageOdt diagrama);
void escribirTablaEstados(writerOdt* manager, tableOdt maqTabla);
void escribirCodigoFuente(writerOdt* manager, CONTENIDO_DOCUMENTACION configDoc);
tableOdt generarTablaEstados(std::list<std::list<QString>> filas);

textOdt newLine("");

int main(void) {
    CONTENIDO_DOCUMENTACION configDoc;

    //Cargo configuracion

    //Header
    configDoc.autores = "Autores: Almaraz Nicolas, Gonzalez Nahuel, Nirino Felipe";
    configDoc.fecha = "Fecha: 23/03/2023";
    configDoc.pathLogoUtn = QString(IMAGES_DIR_PATH)+"/logoUTN.png";

    //Nombre proyecto
    configDoc.titulo = "Cosechadora Agro Industrial";

    //Info Teorica
    configDoc.fIntroTeorica = true;
    configDoc.introTeoricaEstados = "Estados: son instancias o etapas de un proceso";
    configDoc.introTeoricaTransiciones = "Transiciones: son el paso de un estado a otro";
    configDoc.introTeoricaEventos = "Eventos: son las condicioens que hacen que se ejecute una trasicion";
    configDoc.introTeoricaAcciones = "Acciones: son las ordenes que se ejecutan cada vez que se dispara cierta transicion";
    configDoc.introTeoricaReset = "Estado de reset: es el estado que fija las condiciones iniciales del sistema";

    //Contenido
    std::list<QString> listMaq;
    listMaq.push_back("Maquina de estados 1");
    listMaq.push_back("Maquina de estados 2");
    listMaq.push_back("Maquina de estados 3");
    configDoc.nombresMaq = listMaq;

    //Diagramas
    std::list<imageOdt> listDiagramas;
    QString pathDir = IMAGES_DIR_PATH;
    imageOdt img0(pathDir+"/capturaMdE0.png",681/2,954/2,imageOdt::NONE,imageOdt::CENTER);
    imageOdt img1(pathDir+"/capturaMdE1.png",713/2,1411/2,imageOdt::NONE,imageOdt::CENTER);
    imageOdt img2(pathDir+"/capturaMdE2.png",674/2,833/2,imageOdt::NONE,imageOdt::CENTER);
    listDiagramas.push_back(img0);
    listDiagramas.push_back(img1);
    listDiagramas.push_back(img2);
    configDoc.imagenesMaq = listDiagramas;

    //Tabla estados y tranc
    std::list<std::list<QString>> filasMaq0;
    std::list<std::list<QString>> filasMaq1;
    std::list<std::list<QString>> filasMaq2;
    std::list<QString> fila0Maq0, fila1Maq0, fila2Maq0;
    std::list<QString> fila0Maq1, fila1Maq1, fila2Maq1;
    std::list<QString> fila0Maq2, fila1Maq2, fila2Maq2;

    //Completo tabla Maq0
    //Por ahora los lleno en todos igual para probar
    fila0Maq0.push_back("REPOSO");
    fila0Maq0.push_back("s_HayLugar");
    fila0Maq0.push_back("m_RetirarFruta");
    fila0Maq0.push_back("RETIRAR_FRUTA");

    fila1Maq0.push_back("RETIRAR_FRUTA");
    fila1Maq0.push_back("s_FrutaRetirada");
    fila1Maq0.push_back("m_GuardarFruta");
    fila1Maq0.push_back("DEPOSITAR_FRUTA");

    fila2Maq0.push_back("DEPOSITAR_FRUTA");
    fila2Maq0.push_back("s_FrutaGuardada");
    fila2Maq0.push_back("m_ChequearConfig y m_ChequearAlmacenamiento");
    fila2Maq0.push_back("AVANZAR");

    filasMaq0.push_back(fila0Maq0);
    filasMaq0.push_back(fila1Maq0);
    filasMaq0.push_back(fila2Maq0);

    tableOdt tablaMaq0 = generarTablaEstados(filasMaq0);
    std::list <tableOdt> tablasEstTranc;
    tablasEstTranc.push_back(tablaMaq0);
    tablasEstTranc.push_back(tablaMaq0);
    tablasEstTranc.push_back(tablaMaq0);
    configDoc.fTablaEstadosTransiciones = true;
    configDoc.tablasEstTranc = tablasEstTranc;

    //Codigo
    std::list<QString> listCodes;
    listCodes.push_back(QString(CODES_DIR_PATH)+"/AP_FuncionesMDE.c");
    listCodes.push_back(QString(CODES_DIR_PATH)+"/AP_FuncionesMDE.h");
    listCodes.push_back(QString(CODES_DIR_PATH)+"/AP_Inicializacion.c");
    listCodes.push_back(QString(CODES_DIR_PATH)+"/AP_Inicializacion.h");
    listCodes.push_back(QString(CODES_DIR_PATH)+"/AP_MDE.c");
    listCodes.push_back(QString(CODES_DIR_PATH)+"/AP_MDE.h");
    listCodes.push_back(QString(CODES_DIR_PATH)+"/main.c");
    configDoc.pathsCodigos = listCodes;
    configDoc.fCodigos = true;

    //Escribo Doc
    writeDocument(EXE_PATH, OUT_PATH, configDoc);
    return 0;
}

void writeDocument(QString exePath, QString outPath, CONTENIDO_DOCUMENTACION configDoc) {
    //Genero manager y le configuro el page set up, header y footer
    writerOdt manager = pageSetUp(configDoc, exePath, outPath);

    //Escribo titulo
    escribirTitulo(&manager, configDoc);

    //Escribo intro teorica
    if(configDoc.fIntroTeorica)
        escribirIntroTeorica(&manager, configDoc);

    //Escribo cada maquina de estados
    escribirTituloMaqGeneral(&manager);
    std::list<tableOdt> tablas = configDoc.tablasEstTranc;
    int cantMaq = configDoc.nombresMaq.size();
    for(int i=0 ; i<cantMaq ; i++) {
        //Titulo
        QString titulo = configDoc.nombresMaq.front();
        configDoc.nombresMaq.pop_front();
        escribirTituloMaq(&manager, titulo);

        //Diagrama
        imageOdt maqImagen = configDoc.imagenesMaq.front();
        configDoc.imagenesMaq.pop_front();
        escribirDiagrama(&manager, maqImagen);

        //Tabla est tranc
        if(configDoc.fTablaEstadosTransiciones) {
            tableOdt maqTabla = tablas.front();
            tablas.pop_front();
            escribirTablaEstados(&manager, maqTabla);
        }
    }

    //Codigo
    if(configDoc.fCodigos)
        escribirCodigoFuente(&manager, configDoc);


    manager.generateOdt();
}

tableOdt generarTablaEstados(std::list<std::list<QString>> filas) {
    tableOdt table(filas.size()+1,4);

    //Encabezado
    std::list <QString> encabezado;
    encabezado.push_back("Estado actual");
    encabezado.push_back("Evento");
    encabezado.push_back("Accion");
    encabezado.push_back("Estado futuro");
    for(int columna=0; columna<4; columna++) {
        QString aux = encabezado.front();
        encabezado.pop_front();
        textOdt textElement(aux);
        textElement.setAlign(textOdt::ALIGN_CENTER);
        textElement.setBold(true);
        tableElement elemento(textElement);
        elemento.setPos(0, columna);
        table.addElement(elemento);
    }

    //Contenido tabla
    int fila = 1;
    while(filas.size() != 0) {
        std::list<QString> datosFila = filas.front();
        filas.pop_front();
        for(int columna=0; columna<4; columna++) {
            QString aux = datosFila.front();
            datosFila.pop_front();
            textOdt textElement(aux);
            tableElement elemento(textElement);
            elemento.setPos(fila,columna);
            table.addElement(elemento);
        }
        fila++;
    }
    return table;
}

writerOdt pageSetUp(CONTENIDO_DOCUMENTACION configDoc,
                    QString exePath,
                    QString outPath) {
    //Configuro tamaño de hoja y margenes (en mm)
    pageOdt pageSetUp;
    pageSetUp.setMarginTop(20);
    pageSetUp.setMarginLeft(30);
    pageSetUp.setMarginRight(20);
    pageSetUp.setMarginBottom(20);
    pageSetUp.setSizeHeight(297);
    pageSetUp.setSizeWidth(210);

    //Configuro header
    headerFooterOdt header;
    imageOdt imgHeader(configDoc.pathLogoUtn,92/2,414/2,imageOdt::PARALEL,imageOdt::RIGHT);
    textOdt autores(configDoc.autores);
    textOdt fecha(configDoc.fecha);
    header.addElement(imgHeader);
    header.addElement(autores);
    header.addElement(fecha);
    header.addElement(newLine);

    //Configuro footer
    headerFooterOdt footer;
    textOdt uModelFactory("Generado con uModelFactory");
    uModelFactory.setAlign(textOdt::ALIGN_CENTER);
    footer.addElement(uModelFactory);

    //Creo documento
    writerOdt manager(exePath, outPath, pageSetUp, header, footer);
    return manager;
}

void escribirTitulo(writerOdt* manager, CONTENIDO_DOCUMENTACION configDoc) {
    textOdt titulo(configDoc.titulo);
    titulo.setBold(true);
    titulo.setAlign(textOdt::ALIGN_CENTER);
    titulo.setUnderline(true);
    titulo.setLetterSize(14);
    manager->writeText(titulo);
    manager->writeText(newLine);
}

void escribirIntroTeorica(writerOdt* manager, CONTENIDO_DOCUMENTACION configDoc) {
    textOdt introTeoricaTitulo("Introduccion teorica");
    introTeoricaTitulo.setBold(true);
    introTeoricaTitulo.setAlign(textOdt::ALIGN_CENTER);
    introTeoricaTitulo.setLetterSize(12);
    manager->writeText(introTeoricaTitulo);

    textOdt estados(configDoc.introTeoricaEstados);
    manager->writeText(estados);

    textOdt transiciones(configDoc.introTeoricaTransiciones);
    manager->writeText(transiciones);

    textOdt eventos(configDoc.introTeoricaEventos);
    manager->writeText(eventos);

    textOdt acciones(configDoc.introTeoricaAcciones);
    manager->writeText(acciones);

    textOdt reset(configDoc.introTeoricaReset);
    manager->writeText(reset);

    manager->writeText(newLine);
}

void escribirTituloMaqGeneral(writerOdt* manager) {
    textOdt tituloMaqGeneral("Maquinas de estados");
    tituloMaqGeneral.setAlign(textOdt::ALIGN_CENTER);
    tituloMaqGeneral.setBold(true);
    tituloMaqGeneral.setLetterSize(12);
    manager->writeText(tituloMaqGeneral);
    manager->writeText(newLine);
}

void escribirTituloMaq(writerOdt* manager, QString titulo) {
    textOdt maqTitulo(titulo);
    maqTitulo.setItalic(true);
    maqTitulo.setUnderline(true);
    maqTitulo.setBold(true);
    manager->writeText(maqTitulo);
}

void escribirDiagrama(writerOdt* manager, imageOdt diagrama) {
    textOdt titulo("Diagrama de estados:");
    titulo.setItalic(true);
    manager->writeText(titulo);
    manager->writeImage(diagrama);
    manager->writeText(newLine);
}

void escribirTablaEstados(writerOdt* manager, tableOdt maqTabla) {
    textOdt titulo("Tabla de estados y trancisiones:");
    titulo.setItalic(true);
    manager->writeText(titulo);
    manager->writeTable(maqTabla);
    manager->writeText(newLine);
}

void escribirCodigoFuente(writerOdt* manager, CONTENIDO_DOCUMENTACION configDoc) {
    textOdt titulo("Implementacion en C:");
    titulo.setBold(true);
    titulo.setLetterSize(12);
    titulo.setAlign(textOdt::ALIGN_CENTER);
    manager->writeText(titulo);
    manager->writeText(newLine);

    std::list<QString> listCodigos = configDoc.pathsCodigos;
    while(listCodigos.size()!=0) {
        QString pathCodigo = listCodigos.front();
        listCodigos.pop_front();

        //Genero titulo del codigo a escribir
        QString nameFile = pathCodigo;
        int aux = pathCodigo.lastIndexOf("/");
        nameFile.remove(0,aux+1);
        textOdt tituloCodigo(nameFile);
        tituloCodigo.setAlign(textOdt::ALIGN_CENTER);
        tituloCodigo.setBold(true);
        tituloCodigo.setFont("Courier new");
        tableElement TE_tituloCodigo(tituloCodigo);
        TE_tituloCodigo.setPos(0,0);
        tableOdt tableCode(1,1);
        tableCode.addElement(TE_tituloCodigo);
        manager->writeTable(tableCode);

        //Abro archivos
        QFile file(pathCodigo);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        QString code;

        //Leo linea por linea y las escribo
        while(!in.atEnd()) {
            code = in.readLine();
            textOdt line(code);
            line.setFont("Courier new");
            line.setLetterSize(10);
            manager->writeText(line);
        }
        file.close();
        manager->writeText(newLine);
    }
}
