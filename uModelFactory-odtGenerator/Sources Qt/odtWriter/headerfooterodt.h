#ifndef HEADERFOOTERODT_H
#define HEADERFOOTERODT_H

#include <textodt.h>
#include <imageodt.h>
#include <list>

#define TEXT    false
#define IMAGE   true

typedef struct {
    textOdt text;
    imageOdt image;
    bool type;
} DATA_ELEMENT;

class headerFooterOdt
{
public:
    headerFooterOdt();
    void addElement(textOdt);
    void addElement(imageOdt);
    std::list <DATA_ELEMENT> inline getElements(){return mElements;}
    bool inline isVoid(){return mVoid;}
private:
    bool mVoid;
    std::list <DATA_ELEMENT> mElements;
};

#endif // HEADERFOOTERODT_H
