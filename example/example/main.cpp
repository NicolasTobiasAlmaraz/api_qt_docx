#include <QCoreApplication>

#include "qt_docx_api.h"

int main(int argc, char *argv[]) {
    //QCoreApplication a(argc, argv);

    //Header
    headerFooterDocx header;
    header.addElement((textDocx)"Nicolas Almaraz Ejemplo");
    imageDocx logoUtn(":/img/resources/img/logoUTN.png",
                      imageDocx::NONE,
                      imageDocx::RIGHT);
    header.addElement(logoUtn);

    //Footer
    headerFooterDocx footer;
    header.addElement((textDocx)"Aguante San Lorenzo!");

    //Creo Documento
    pageDocx hojaA4;
    writerDocx handlerDocx(":/exe/resources/exe/exe_windows.exe",
                           "archivoSalida.docx",
                           hojaA4,
                           header,
                           footer);

    //Titulo
    textDocx titulo("La Gloriosa Hinchada de San Lorenzo");
    titulo.setAlign(textDocx::ALIGN_CENTER);
    titulo.setBold(true);
    titulo.setUnderline(true);
    titulo.setBackgroundColor(textDocx::B);
    titulo.setFillColor(textDocx::R);
    handlerDocx.writeText(titulo);

    //Descripcion
    textDocx descripcion("El club atletico San Lorenzo de Almagro tiene la mejor hinchada del mundo. Tiene las mejores canciones que se puedan imaginar. A continuacion adjunto una imagen para que vean");
    handlerDocx.writeText(descripcion);

    //Subtitulo
    textDocx subt1("Imagen de la hinchada");
    subt1.setItalic(true);
    subt1.setUnderline(true);
    handlerDocx.writeText(subt1);

    //Imagen
    imageDocx foto(":/img/resources/img/hinchada.jpg",
              imageDocx::NONE,
              imageDocx::CENTER);
    handlerDocx.writeImage(foto);

    //Subtitulo 2
    textDocx subt2("Mejores canciones:");
    subt1.setItalic(true);
    subt1.setUnderline(true);
    handlerDocx.writeText(subt2);

    //Lista
    textDocx cancion1("La mas fiel");
    cancion1.setFillColor(textDocx::B);
    textDocx cancion2("Vamos ciclon, vamos a ganar");
    cancion2.setFillColor(textDocx::R);
    textDocx cancion3("Yo soy del barrio de Boedo");
    cancion3.setFillColor(textDocx::B);
    textDocx cancion4("Vamos San Lorenzo te voy a aletar");
    cancion4.setFillColor(textDocx::R);
    textDocx cancion5("Hay una cosa que nunca van a entender");
    cancion4.setFillColor(textDocx::B);

    listDocx listaCanciones;
    listaCanciones.addElement(cancion1);
    listaCanciones.addElement(cancion2);
    listaCanciones.addElement(cancion3);
    listaCanciones.addElement(cancion4);
    listaCanciones.addElement(cancion5);
    handlerDocx.writeList(listaCanciones);

    //Subtitulo 4
    textDocx estadio("Estadio:");
    subt1.setItalic(true);
    subt1.setUnderline(true);
    handlerDocx.writeText(subt1);

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
    return 0;
}
