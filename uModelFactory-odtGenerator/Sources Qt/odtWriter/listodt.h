#ifndef LISTODT_H
#define LISTODT_H

#include <textodt.h>
#include <list>

class listOdt {

public:
    listOdt();

    //Agrega un elemento a la lista de elementos
    void addElement(textOdt item);

    //Deuelve ls lista
    std::list<textOdt> getElements(){return listElements;}

private:
    std::list <textOdt> listElements;
};

#endif // LISTODT_H
