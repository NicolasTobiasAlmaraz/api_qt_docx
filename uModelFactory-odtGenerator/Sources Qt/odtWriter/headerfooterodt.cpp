#include "headerfooterodt.h"

headerFooterOdt::headerFooterOdt() {
    mVoid = true;
}

void headerFooterOdt::addElement(textOdt text) {
    mVoid = false;
    imageOdt image("",0,0,"","");
    DATA_ELEMENT aux = {text,image,TEXT};
    mElements.push_back(aux);
}

void headerFooterOdt::addElement(imageOdt image) {
    mVoid = false;
    textOdt text("");
    DATA_ELEMENT aux = {text,image,IMAGE};
    mElements.push_back(aux);
}
