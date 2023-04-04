#ifndef TABLEODT_H
#define TABLEODT_H

#include <tableelement.h>
#include <list>

class tableOdt {

public:
    static const int APPEND;
    static const int INDEX_ERROR;
    static const int REPLACE;

    //Construtor
    tableOdt(int rows, int columns);

    //Agrega un elemento a la lista de elementos
    //En caso de haber un elemento con la misma coordenada entonces lo reemplaza
    int addElement(tableElement item);

    //Devuelve la cantidad de filas que tiene la tabla
    int inline getRows(){return mRows;}

    //Devuelve la cantidad de columnas que tiene la tabla
    int inline getColumns(){return mColumns;}

    //Deuelve el contenido de las celdas de la tabla
    std::list<tableElement> getElements(){return listElements;}

private:
    int mRows;
    int mColumns;
    std::list <tableElement> listElements;

};

#endif // TABLEODT_H
