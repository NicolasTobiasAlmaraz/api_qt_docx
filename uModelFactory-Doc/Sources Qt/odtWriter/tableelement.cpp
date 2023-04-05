#include "tableelement.h"

tableElement::tableElement(textOdt text) {
    mColumn = 0;
    mRow = 0;
    mText = new textOdt(text);
}
