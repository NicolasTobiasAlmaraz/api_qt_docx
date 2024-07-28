#include <QCoreApplication>

#include "qt_docx_api.h"

#define EXE_PATH            "C:/Users/User/Desktop/api_qt_docx/example/resources/exe/genDocx.exe"
#define LOGO_UTN_PATH       "C:/Users/User/Desktop/api_qt_docx/example/resources/img/logoUTN.png"
#define FOTO_SL_PATH        "C:/Users/User/Desktop/api_qt_docx/example/resources/img/hinchada.jpg"
#define SALIDA_DOCX_PATH    "C:/Users/User/Desktop/api_qt_docx/example/output/ejemplo.docx"

#define R "FF0000"
#define B "0000FF"

int main() {
    //Header
    headerFooterDocx header;
    imageDocx logoUtn(LOGO_UTN_PATH,imageDocx::PARALEL,imageDocx::LEFT);
    header.addElement(logoUtn);
    textDocx autor("Autor: Nicolas Almaraz");
    autor.setAlign(textDocx::ALIGN_RIGHT);
    header.addElement(autor);

    //Footer
    headerFooterDocx footer;
    textDocx footer_text("Aguante San Lorenzo!");
    footer.addElement(footer_text);

    //Hoja
    pageDocx hojaA4;

    //Creo Documento
    writerDocx handlerDocx(EXE_PATH,SALIDA_DOCX_PATH,hojaA4,header,footer);

    //Titulo
    textDocx titulo("La Gloriosa Hinchada de San Lorenzo");
    titulo.setLetterSize(15);
    titulo.setAlign(textDocx::ALIGN_CENTER);
    titulo.setBold(true);
    titulo.setUnderline(true);
    titulo.setFillColor(B);
    handlerDocx.writeText(titulo);

    //Descripcion
    textDocx descripcion("El club atletico San Lorenzo de Almagro tiene la mejor hinchada del mundo. Tiene las mejores canciones que se puedan imaginar. A continuacion adjunto una imagen para que vean");
    descripcion.setLetterSize(13);
    handlerDocx.writeText(descripcion);

    //Subtitulo
    textDocx subt1("Imagen de la hinchada");
    subt1.setLetterSize(14);
    subt1.setItalic(true);
    subt1.setUnderline(true);
    handlerDocx.writeText(subt1);

    //Imagen
    imageDocx foto(FOTO_SL_PATH,imageDocx::NONE,imageDocx::CENTER);
    int ancho = foto.getWidth();
    foto.scaleToWidth((int)ancho*0.35);
    handlerDocx.writeImage(foto);

    //Subtitulo 2
    textDocx subt2("Mejores canciones:");
    subt2.setLetterSize(14);
    subt2.setItalic(true);
    subt2.setUnderline(true);
    handlerDocx.writeText(subt2);

    //Lista
    textDocx cancion1("La mas fiel");
    cancion1.setFillColor(B);
    cancion1.setLetterSize(13);
    textDocx cancion2("Vamos ciclon, vamos a ganar");
    cancion2.setFillColor(R);
    cancion2.setLetterSize(13);
    textDocx cancion3("Yo soy del barrio de Boedo");
    cancion3.setFillColor(B);
    cancion3.setLetterSize(13);
    textDocx cancion4("Vamos San Lorenzo te voy a aletar");
    cancion4.setFillColor(R);
    cancion4.setLetterSize(13);
    textDocx cancion5("Hay una cosa que nunca van a entender");
    cancion5.setFillColor(B);
    cancion5.setLetterSize(13);

    listDocx listaCanciones;
    listaCanciones.addElement(cancion1);
    listaCanciones.addElement(cancion2);
    listaCanciones.addElement(cancion3);
    listaCanciones.addElement(cancion4);
    listaCanciones.addElement(cancion5);
    handlerDocx.writeList(listaCanciones);

    //Subtitulo 4
    textDocx estadio("Estadio:");
    estadio.setItalic(true);
    estadio.setUnderline(true);
    estadio.setLetterSize(14);
    handlerDocx.writeText(estadio);

    //Tabla
    tableDocx cancha(3,3);
    tableElement platea_sur((textDocx)"Platea Sur");
    platea_sur.setPos(0,1);
    tableElement popular_local((textDocx)"Popular Local");
    popular_local.setPos(1,0);
    tableElement platea_norte((textDocx)"Platea Norte");
    platea_norte.setPos(2,1);
    tableElement campo_de_juego((textDocx)"Campo de juego");
    campo_de_juego.setPos(1,1);
    tableElement popular_visitante((textDocx)"Popular Visitante");
    popular_visitante.setPos(1,2);

    cancha.addElement(platea_sur);
    cancha.addElement(platea_norte);
    cancha.addElement(campo_de_juego);
    cancha.addElement(popular_local);
    cancha.addElement(popular_visitante);

    handlerDocx.writeTable(cancha);

    //Escribo documento
    handlerDocx.generateDocx();
}
