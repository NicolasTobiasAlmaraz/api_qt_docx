#include <writerodtmanager.h>
#include <QString>

int main(void) {
    QString exePath = "C:/Users/User/Desktop/uModelFactory-odtGenerator/EjecutableWindows/main.exe";
    QString odtPath = "C:/Users/User/Desktop/uModelFactory-odtGenerator/EjecutableWindows/salidaOdt.odt";

    //Configuro tamaño de hoja y margenes (en cm)
    pageOdt pageSetUp;
    pageSetUp.setMarginTop(2);
    pageSetUp.setMarginLeft(3);
    pageSetUp.setMarginRight(2);
    pageSetUp.setMarginBottom(2);
    pageSetUp.setSizeHeight(31);
    pageSetUp.setSizeWidth(21);

    writerOdtManager manager(exePath, odtPath, pageSetUp);

    //Escribo texto en negrita centrado y subrayado
    textOdt texto1("Ejemplo Titulo uModelFactory");
    texto1.setBold(true);
    texto1.setAlign(textOdt::ALIGN_CENTER);
    texto1.setUnderline(true);
    texto1.setLetterSize(16);
    manager.writeText(texto1);

    //Escribo texto normal
    textOdt texto2("Ejemplo de parrafo con texto normal. Sigo escribiendo para hacer bulto.");
    textOdt texto3("Esto lo estoy escribiendo desde Qt.");
    textOdt texto4("El codigo de C++ escribe un XML que luego cuando uso el metodo generateOdt() llama al codigo de Python");
    textOdt texto5("El codigo de Pyton consume el XML y usa el modulo ODFPY para escribir el archivo ODT");
    manager.writeText(texto2);
    manager.writeText(texto3);
    manager.writeText(texto4);
    manager.writeText(texto5);

    //Pego una foto, 3x7cm
    imageOdt img("C:/Users/user/desktop/ejemploUtn.jpg",3,7);
    manager.writeImage(img);

    //Hago una tabla de 2x2
    tableOdt tabla(2,2);

    //Creo los elementos de cada celda
    tableElement aux00(textOdt("Arriba izquierda 00"));
    aux00.setPos(0,0);
    tableElement aux01(textOdt("Arriba derecha 01"));
    aux01.setPos(0,1);
    tableElement aux10(textOdt("Abajo izquierda 10"));
    aux10.setPos(1,0);
    tableElement aux11(textOdt("Abajo derecha 11"));
    aux11.setPos(1,1);

    //Agrego elementos a la tabla
    tabla.addElement(aux00);
    tabla.addElement(aux01);
    tabla.addElement(aux10);
    tabla.addElement(aux11);

    //Dibujo tabla
    manager.writeTable(tabla);

    //Escribo texto con otros ejemplos de formato
    textOdt texto6("Ejemplo texto con fondo de color tachado e italica. Alineado a la derecha");
    texto6.setAlign(textOdt::ALIGN_RIGHT);
    texto6.setItalic(true);
    texto6.setStrikethrough(true);
    texto6.setFillColor("#FFFFFF");
    texto6.setBackgroundColor("#3B6120");
    manager.writeText(texto6);

    //Dibujo otra tabla igual a la de antes
    manager.writeTable(tabla);


    manager.generateOdt();
    return 0;
}
