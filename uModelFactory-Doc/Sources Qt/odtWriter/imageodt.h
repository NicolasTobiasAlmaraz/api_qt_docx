#ifndef IMAGEODT_H
#define IMAGEODT_H

#include <QString>

class imageOdt {

public:
    static const QString LEFT;
    static const QString RIGHT;
    static const QString CENTER;
    static const QString PARALEL;
    static const QString NONE;

    //Constructor
    imageOdt(QString imgPath, int height, int widht, QString anchor, QString align);

    //Get
    QString inline getImgPath(){return mImgPath;}
    int inline getHeight(){return mHeight;}
    int inline getWidht(){return mWidth;}
    QString inline getAnchor(){return mAnchor;}
    QString inline getAlign(){return mAlign;}

private:
    QString mImgPath;
    int mHeight;
    int mWidth;
    QString mAlign;
    QString mAnchor;
};

#endif // IMAGEODT_H
