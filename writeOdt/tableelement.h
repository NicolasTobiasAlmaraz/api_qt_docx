#ifndef TABLEELEMENT_H
#define TABLEELEMENT_H

#include <imageodt.h>
#include <textodt.h>


class tableElement {

public:

    tableElement(imageOdt image);
    tableElement(textOdt text);
    //Set
    void inline setPos(int currentRow, int currentCol){ mColumn = currentCol; mRow = currentRow; }
    void inline setText(textOdt text){ mText = new textOdt(text);}

    //Get
    int inline getColumn(){return mColumn;}
    int inline getRow(){return mRow;}
    textOdt inline getText(){return *mText;}

private:
    int mColumn;
    int mRow;
    textOdt *mText;
};

#endif // TABLEELEMENT_H
