#include "documentacion.h"

const textOdt Documentacion::newLine = textOdt("");

Documentacion::Documentacion() {
    //Paths
    mExePath = "genOdt.exe";
    mOutPath = "documentacion.odt";

    //Config archivo
    mAutores = "Vacio";
    mFecha = "dd/mm/yy";
    mNombreProyecto = "Proyecto Sin Nombre";

    //Los opcionales todos en true
    fCodigos = true;
    fIntroTeorica = true;
    fTabla = true;

    //Manager
    mManagerOdt = nullptr;
}

void Documentacion::generarDocumentacionFormatoOdt(QString exePath, QString outPath) {
    mExePath = exePath;
    mOutPath = outPath;

    //Genero manager y le configuro el page set up, header y footer
    mManagerOdt = new writerOdt( pageSetUp(exePath, outPath) );

    //Escribo titulo del proyecto
    escribirTitulo(mNombreProyecto, 14);

    //Escribo intro teorica
    if(fIntroTeorica)
        escribirIntroTeorica();

   //Lista de eventos, acciones, variables
    escribirEventosAccionesVariables();

    //Escribo cada maquina de estados
    std::list<MaquinaDeEstados> maquinas = mMaquinas;
    escribirTitulo("Maquinas de estados", 12);
    int cantMaq = maquinas.size();
    for(int i=0 ; i<cantMaq ; i++) {
        MaquinaDeEstados maq = maquinas.front();
        maquinas.pop_front();

        //Titulo
        escribirSubtitulo(maq.titulo);

        //Diagrama
        ajustarImagenAncho(maq.diagrama, 750); //Ajusto ancho de la hoja
        imageOdt img(maq.diagrama+"_resized.png", imageOdt::NONE, imageOdt::CENTER);
        escribirDiagrama(img);

        //Tabla
        if(fTabla) {
            escribirTablaEstados(maq.tablaEstTranc);
        }
    }

    //Codigo
    if(fCodigos)
        escribirCodigoFuente();
    mManagerOdt->generateOdt();
}

void Documentacion::generarDocumentacionFormatoHtml() {
    int i = 0;

    QString tituloProy = mNombreProyecto;
    QString pathProy = mPathProyecto;
    QString fechaProy = mFecha;
    QString autoresProy = mAutores;
    int cantMaq = mMaquinas.size();

    QTextDocument * const m_document(new QTextDocument());
    QTextCursor m_cursor(m_document);

    QTextCharFormat titulo;
    titulo.setForeground(Qt::darkBlue);
    titulo.setFontWeight(QFont::Bold);
    titulo.setFontPointSize(20);
    //titulo.AlignMiddle;
    //titulo.setVerticalAlignment(titulo.AlignMiddle);

    QTextCharFormat texto;
    texto.setForeground(Qt::red);
    texto.setFontWeight(QFont::Bold);
    //texto.setFontWeight(12);
    texto.setFontPointSize(12);
    //texto.VerticalAlignment = texto.AlignMiddle;

    QTextCharFormat aux;
    aux.setForeground(Qt::black);
    aux.setFontPointSize(12);
    aux.setFontWeight(QFont::Bold);

    m_cursor.insertImage(pathProy+"/doc_"+tituloProy+"/logoUTN.png");

    m_cursor.insertText(QObject::tr("\n"));
    m_cursor.insertText(QObject::tr("\n"));

    m_cursor.insertText("uModel Factory - UTN FRBA\n",titulo);
    m_cursor.insertText(QObject::tr("\n"));

    m_cursor.insertText(QLatin1String("Software didactico-profesional para modelado, depuracion e implementacion de aplicaciones industriales sobre sistemas embebidos"),aux);
    m_cursor.insertText(QObject::tr("\n"));
    m_cursor.insertText(QObject::tr("\n"));

    m_cursor.insertText("Proyecto: "+ tituloProy + "\n",aux);
    m_cursor.insertText("Autor: "+ autoresProy + "\n",aux);
    m_cursor.insertText(QLatin1String("Modificacion: ")+ fechaProy + "\n",aux);

    m_cursor.insertText(QObject::tr("\n"));
    m_cursor.insertText(QObject::tr("\n"));

    QTextCharFormat formato;
    aux.setForeground(Qt::black);
    aux.setFontPointSize(12);
    aux.setFontWeight(QFont::Bold);

    std::list<MaquinaDeEstados> maquinas = mMaquinas;
    for(int k = 0 ; k < cantMaq ; k++)
    {
        //Inserto Nombre Maquina
        MaquinaDeEstados maq = maquinas.front();
        maquinas.pop_front();
        QString nombreMaq = maq.titulo;
        m_cursor.insertText(QLatin1String("Diagrama de estados - ") + nombreMaq ,texto);
        m_cursor.insertText(QObject::tr("\n"));
        m_cursor.insertText(QObject::tr("\n"));
        m_cursor.insertText(QObject::tr("\n"));

        //Inserto Diagrama máquina
        m_cursor.insertImage(maq.diagrama);

        //*****************************************************************
        //ahora inserto la tabla de estados y transiciones de esa maquina
        //*****************************************************************
        m_cursor.insertText(QObject::tr("\n"));
        m_cursor.insertText(QObject::tr("\n"));
    }

    m_cursor.insertText(QObject::tr("\n"));
    m_cursor.insertText(QObject::tr("\n"));
    m_cursor.insertText(QObject::tr("\n"));

    //Lista de eventos
    std::list<QString> eventos = mEventos;
    int cantEventos = eventos.size();
    m_cursor.insertText(QLatin1String("Listado de eventos"),texto);
    m_cursor.insertText(QObject::tr("\n"));
    for(i=0; i < cantEventos; i++) {
        QString evento1 = eventos.front();
        eventos.pop_front();
        m_cursor.insertText("void " + evento1 +  "() ;",aux);
        m_cursor.insertText(QObject::tr("\n"));

    }

    //Lista de acciones
    std::list<QString> acciones = mAcciones;
    int cantAcciones = acciones.size();
    m_cursor.insertText(QLatin1String("\n\nListado de acciones"),texto);
    m_cursor.insertText(QObject::tr("\n"));
    for(i=0; i<cantAcciones; i++) {
        QString accion1 = acciones.front();
        acciones.pop_front();
        m_cursor.insertText("void " + accion1 +  "() ;",aux);
        m_cursor.insertText(QObject::tr("\n"));
        //esto es para que no agregue las acciones que involucran variables como acciones
        if(accion1.contains('=')==false){
            m_cursor.insertText(QLatin1String("void ")  + accion1 + " ;",aux);
            m_cursor.insertText(QObject::tr("\n"));
        }
    }

    //Lista de variables
    std::list<QString> variables = mVariables;
    int cantVariables = variables.size();
    m_cursor.insertText(QLatin1String("\n\nListado de variables"),texto);
    m_cursor.insertText(QObject::tr("\n"));
    for(i=0; i<cantVariables; i++) {
        QString variable1 = variables.front();
        variables.pop_front();
        m_cursor.insertText(variable1, aux);
        m_cursor.insertText(QObject::tr("\n"));
    }

    //generacion del documento indexDoc.html que contiene la documentacion en formato WEB
    QTextDocumentWriter writer(pathProy+"/doc_"+tituloProy+"/indexDoc.html");
    writer.setFormat("HTML");
    writer.write(m_document);
}

tableOdt Documentacion::generarTablaEstados(std::list<std::list<QString>> filas) {
    tableOdt table(filas.size()+1,4);

    //Encabezado
    std::list <QString> encabezado;
    encabezado.push_back("Estado actual");
    encabezado.push_back("Estado futuro");
    encabezado.push_back("Eventos");
    encabezado.push_back("Acciones");
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

writerOdt Documentacion::pageSetUp(QString exePath, QString outPath) {
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
    ajustarImagenAncho(mLogoUtn, 200);
    imageOdt imgHeader(mLogoUtn+"_resized.png", imageOdt::PARALEL, imageOdt::RIGHT);
    textOdt autores("Autores: "+mAutores);
    textOdt fecha("Fecha: "+mFecha);
    header.addElement(imgHeader);
    header.addElement(autores);
    header.addElement(fecha);
    header.addElement(newLine);

    //Configuro footer
    headerFooterOdt footer;
    textOdt uModelFactory("Generado automaticamente con uModelFactory");
    uModelFactory.setAlign(textOdt::ALIGN_CENTER);
    footer.addElement(uModelFactory);

    //Creo documento
    writerOdt manager(exePath, outPath, pageSetUp, header, footer);
    return manager;
}

void Documentacion::escribirIntroTeorica() {
    escribirTitulo("Introduccion teorica",12);

    textOdt estados(mInfoTeorica.estados);
    mManagerOdt->writeText(estados);

    textOdt transiciones(mInfoTeorica.transiciones);
    mManagerOdt->writeText(transiciones);

    textOdt eventos(mInfoTeorica.eventos);
    mManagerOdt->writeText(eventos);

    textOdt acciones(mInfoTeorica.acciones);
    mManagerOdt->writeText(acciones);

    textOdt reset(mInfoTeorica.reset);
    mManagerOdt->writeText(reset);

    mManagerOdt->writeText(newLine);
}

void Documentacion::escribirTitulo(QString txt, int size) {
    textOdt titulo(txt);
    titulo.setAlign(textOdt::ALIGN_CENTER);
    titulo.setBold(true);
    titulo.setUnderline(true);
    titulo.setLetterSize(size);
    mManagerOdt->writeText(titulo);
    mManagerOdt->writeText(newLine);
}

void Documentacion::escribirSubtitulo(QString txt) {
    textOdt titulo(txt);
    titulo.setBold(true);
    titulo.setUnderline(true);
    titulo.setItalic(true);
    mManagerOdt->writeText(titulo);
    mManagerOdt->writeText(newLine);
}

void Documentacion::escribirDiagrama(imageOdt img) {
    escribirSubtitulo("Diagrama de estados:");
    mManagerOdt->writeText(newLine);
    //mManagerOdt->writeText(newLine);
    mManagerOdt->writeImage(img);
    //mManagerOdt->writeText(newLine);
    mManagerOdt->writeText(newLine);
}

void Documentacion::escribirTablaEstados(tableOdt tabla) {
    escribirSubtitulo("Tabla de estados y trancisiones:");
    mManagerOdt->writeTable(tabla);
    mManagerOdt->writeText(newLine);
}

void Documentacion::escribirEventosAccionesVariables() {
    escribirTitulo("Eventos, Acciones y Variables", 12);
    escribirLista("Eventos:", mEventos);
    escribirLista("Acciones:", mAcciones);
    escribirLista("Variables:", mVariables);
}

void Documentacion::escribirLista(QString subtitulo, std::list<QString> lista) {
    escribirSubtitulo(subtitulo);

    listOdt myList;
    while( lista.size()!=0 ) {
        QString txt = lista.front();
        lista.pop_front();
        myList.addElement( textOdt(txt) );
    }
    mManagerOdt->writeList(myList);
    mManagerOdt->writeText(newLine);
}

void Documentacion::escribirCodigoFuente() {
    textOdt titulo("Implementacion en C:");
    titulo.setBold(true);
    titulo.setLetterSize(12);
    titulo.setAlign(textOdt::ALIGN_CENTER);
    mManagerOdt->writeText(titulo);
    mManagerOdt->writeText(newLine);

    std::list<QString> listCodigos = mPathsCodigos;
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
        tituloCodigo.setLetterSize(14);
        tableElement TE_tituloCodigo(tituloCodigo);
        TE_tituloCodigo.setPos(0,0);
        tableOdt tableCode(1,1);
        tableCode.addElement(TE_tituloCodigo);
        mManagerOdt->writeTable(tableCode);

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
            mManagerOdt->writeText(line);
        }
        file.close();
        mManagerOdt->writeText(newLine);
    }
}

void Documentacion::ajustarImagenAncho(const QString& imagePath, int newWidth) {
    //Abro Imagen
    QImage image(imagePath);

    //Reescalo imagen
    QImage newImage = image.scaledToWidth(newWidth,Qt::SmoothTransformation);

    //Guardo en path nuevo
    QString newPath = imagePath;
    newPath.remove(imagePath.size()-4,4);
    QFile file(imagePath+"_resized.png");
    file.open(QIODevice::WriteOnly);
    newImage.save(&file);
    file.close();
}
