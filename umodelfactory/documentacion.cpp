#include "documentacion.h"

const textDocx Documentacion::newLine = textDocx("");

Documentacion::Documentacion() {
    //Paths
    mExePath = "genDocx.exe";
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
    mManagerDocx = nullptr;
}

void Documentacion::generarDocumentacionFormatoDocx(QString exePath, QString outPath) {
    mExePath = exePath;
    mOutPath = outPath;

    //Genero manager y le configuro el page set up, header y footer
    mManagerDocx = new writerDocx( pageSetUp(exePath, outPath) );

    //Escribo titulo del proyecto
    escribirTitulo(mNombreProyecto, 14);

    //Escribo intro teorica
    if(fIntroTeorica)
        escribirIntroTeorica();

    if(fDescripcion)
        escribirDescripcion();

   //Lista de eventos, acciones, variables
    escribirEventosAccionesVariables();

    //Escribo cada maquina de estados
    std::list<MaquinaDeEstados> maquinas = mMaquinas;
    escribirTitulo("Máquinas de estados", 12);
    int cantMaq = maquinas.size();
    for(int i=0 ; i<cantMaq ; i++) {
        MaquinaDeEstados maq = maquinas.front();
        maquinas.pop_front();

        //Titulo
        escribirSubtitulo(maq.titulo);

        //Diagrama
        //ajustarImagenAncho(maq.diagrama, 750); //Ajusto ancho de la hoja
        imageDocx img(maq.diagrama, imageDocx::NONE, imageDocx::CENTER);
        img.scaleToWidth(750);
        escribirDiagrama(img);

        //Tabla
        if(fTabla) {
            escribirTablaEstados( generarTablaEstados(maq.tablaEstTranc) );
        }
    }

    //Codigo
    if(fCodigos)
        escribirCodigoFuente();
    mManagerDocx->generateDocx();
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

    QTextCharFormat texto;
    texto.setForeground(Qt::red);
    texto.setFontWeight(QFont::Bold);
    texto.setFontPointSize(12);

    QTextCharFormat textoContenido;
    textoContenido.setForeground(Qt::black);
    textoContenido.setFontPointSize(12);

    QTextTableFormat tableFormat;
    tableFormat.setBorder(1);
    tableFormat.setBorderBrush(Qt::black);
    tableFormat.setBorderStyle(QTextTableFormat::BorderStyle_Solid);

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

    if(fDescripcion) {
        m_cursor.insertText("Descripion del proyecto:\n",texto);
        m_cursor.insertText(mDescripcion,textoContenido);
        m_cursor.insertText(QObject::tr("\n"));
        m_cursor.insertText(QObject::tr("\n"));
    }

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
        m_cursor.insertText(QObject::tr("\n"));
        m_cursor.insertText(QObject::tr("\n"));

        if(fTabla) {
            //Inserto tabla
            m_cursor.insertText(QLatin1String("Tabla de estados y transiciones - ") + nombreMaq ,texto);
            std::list<std::list<QString>> filas = maq.tablaEstTranc;
            std::list<QString> fila;

            //Agrego encabezado de la tabla
            std::list<QString> encabezado;
            encabezado.push_back("Estado Actual");
            encabezado.push_back("Estado Futuro");
            encabezado.push_back("Eventos");
            encabezado.push_back("Acciones");
            filas.push_front(encabezado);

            int numFilas = filas.size();
            int numColumnas = filas.front().size();
            m_cursor.insertTable(numFilas, numColumnas, tableFormat);

            // Completo tabla
            for (int filaActual=0 ; filaActual<numFilas ; filaActual++) {
                fila = filas.front();
                filas.pop_front();
                for (int columnaActual=0 ; columnaActual<numColumnas ; columnaActual++) {
                    QTextCharFormat formatoAux = textoContenido;
                    if(filaActual == 0)
                        formatoAux = aux;
                    m_cursor.insertText(fila.front(), formatoAux);
                    fila.pop_front();
                    m_cursor.movePosition(QTextCursor::NextCell);
                }
                //m_cursor.movePosition(QTextCursor::NextRow);
            }
            //Salgo dela tabla
            m_cursor.movePosition(QTextCursor::End);
            m_cursor.insertText(QObject::tr("\n"));
            m_cursor.insertText(QObject::tr("\n"));
        }
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

tableDocx Documentacion::generarTablaEstados(std::list<std::list<QString>> filas) {
    tableDocx table(filas.size()+1,4);

    //Encabezado
    std::list <QString> encabezado;
    encabezado.push_back("Estado actual");
    encabezado.push_back("Estado futuro");
    encabezado.push_back("Eventos");
    encabezado.push_back("Acciones");
    for(int columna=0; columna<4; columna++) {
        QString aux = encabezado.front();
        encabezado.pop_front();
        textDocx textElement(aux);
        textElement.setAlign(textDocx::ALIGN_CENTER);
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
            textDocx textElement(aux);
            tableElement elemento(textElement);
            elemento.setPos(fila,columna);
            table.addElement(elemento);
        }
        fila++;
    }
    return table;
}

writerDocx Documentacion::pageSetUp(QString exePath, QString outPath) {
    //Configuro tamaño de hoja y margenes (en mm)
    pageDocx pageSetUp;
    pageSetUp.setMarginTop(20);
    pageSetUp.setMarginLeft(30);
    pageSetUp.setMarginRight(20);
    pageSetUp.setMarginBottom(20);
    pageSetUp.setSizeHeight(297);
    pageSetUp.setSizeWidth(210);

    //Configuro header
    headerFooterDocx header;
    imageDocx imgHeader(mLogoUtn, imageDocx::PARALEL, imageDocx::RIGHT);
    imgHeader.scaleToWidth(200);
    textDocx autores("Autores: "+mAutores);
    textDocx fecha("Fecha: "+mFecha);
    header.addElement(imgHeader);
    header.addElement(autores);
    header.addElement(fecha);
    header.addElement(newLine);

    //Configuro footer
    headerFooterDocx footer;
    textDocx uModelFactory("Generado automáticamente con uModelFactory");
    uModelFactory.setAlign(textDocx::ALIGN_CENTER);
    footer.addElement(uModelFactory);

    //Creo documento
    writerDocx manager(exePath, outPath, pageSetUp, header, footer);
    return manager;
}

void Documentacion::escribirIntroTeorica() {
    escribirTitulo("Introducción teorica",12);

    textDocx estados(mInfoTeorica.estados);
    mManagerDocx->writeText(estados);

    textDocx transiciones(mInfoTeorica.transiciones);
    mManagerDocx->writeText(transiciones);

    textDocx eventos(mInfoTeorica.eventos);
    mManagerDocx->writeText(eventos);

    textDocx acciones(mInfoTeorica.acciones);
    mManagerDocx->writeText(acciones);

    textDocx reset(mInfoTeorica.reset);
    mManagerDocx->writeText(reset);

    mManagerDocx->writeText(newLine);
}

void Documentacion::escribirDescripcion() {
    escribirTitulo("Descripción del proyecto",12);
    // Dividir el texto por saltos de línea
    QStringList lineas = mDescripcion.split("\n");
    // Agregar cada línea a la lista
    for (const QString& linea : lineas) {
        textDocx texto = textDocx(linea);
        mManagerDocx->writeText(texto);
    }
}


void Documentacion::escribirTitulo(QString txt, int size) {
    textDocx titulo(txt);
    titulo.setAlign(textDocx::ALIGN_CENTER);
    titulo.setBold(true);
    titulo.setUnderline(true);
    titulo.setLetterSize(size);
    mManagerDocx->writeText(titulo);
    //mManagerOdt->writeText(newLine);
}

void Documentacion::escribirSubtitulo(QString txt) {
    textDocx titulo(txt);
    titulo.setBold(true);
    titulo.setUnderline(true);
    titulo.setItalic(true);
    mManagerDocx->writeText(titulo);
    //mManagerOdt->writeText(newLine);
}

void Documentacion::escribirDiagrama(imageDocx img) {
    escribirSubtitulo("Diagrama de estados:");
    mManagerDocx->writeText(newLine);
    //mManagerOdt->writeText(newLine);
    mManagerDocx->writeImage(img);
    //mManagerOdt->writeText(newLine);
    mManagerDocx->writeText(newLine);
}

void Documentacion::escribirTablaEstados(tableDocx tabla) {
    escribirSubtitulo("Tabla de estados y trancisiones:");
    mManagerDocx->writeTable(tabla);
    mManagerDocx->writeText(newLine);
}

void Documentacion::escribirEventosAccionesVariables() {
    escribirTitulo("Eventos, Acciones y Variables", 12);
    escribirLista("Eventos:", mEventos);
    escribirLista("Acciones:", mAcciones);
    escribirLista("Variables:", mVariables);
}

void Documentacion::escribirLista(QString subtitulo, std::list<QString> lista) {
    escribirSubtitulo(subtitulo);

    listDocx myList;
    while( lista.size()!=0 ) {
        QString txt = lista.front();
        lista.pop_front();
        myList.addElement( textDocx(txt) );
    }
    mManagerDocx->writeList(myList);
    mManagerDocx->writeText(newLine);
}

void Documentacion::escribirCodigoFuente() {
    textDocx titulo("Implementación en C:");
    titulo.setBold(true);
    titulo.setLetterSize(12);
    titulo.setAlign(textDocx::ALIGN_CENTER);
    mManagerDocx->writeText(titulo);
    mManagerDocx->writeText(newLine);

    std::list<QString> listCodigos = mPathsCodigos;
    while(listCodigos.size()!=0) {
        QString pathCodigo = listCodigos.front();
        listCodigos.pop_front();

        //Genero titulo del codigo a escribir
        QString nameFile = pathCodigo;
        int aux = pathCodigo.lastIndexOf("/");
        nameFile.remove(0,aux+1);
        textDocx tituloCodigo(nameFile);
        tituloCodigo.setAlign(textDocx::ALIGN_CENTER);
        tituloCodigo.setBold(true);
        tituloCodigo.setFont("Courier new");
        tituloCodigo.setLetterSize(14);
        tableElement TE_tituloCodigo(tituloCodigo);
        TE_tituloCodigo.setPos(0,0);
        tableDocx tableCode(1,1);
        tableCode.addElement(TE_tituloCodigo);
        mManagerDocx->writeTable(tableCode);

        //Abro archivos
        QFile file(pathCodigo);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        QString code;

        //Leo linea por linea y las escribo
        while(!in.atEnd()) {
            code = in.readLine();
            textDocx line(code);
            line.setFont("Courier new");
            line.setLetterSize(10);
            mManagerDocx->writeText(line);
        }
        file.close();
        mManagerDocx->writeText(newLine);
    }
}
