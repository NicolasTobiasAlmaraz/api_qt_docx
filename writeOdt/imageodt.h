#ifndef IMAGEODT_H
#define IMAGEODT_H

#include <QString>

class imageOdt {

public:
    //Constructor
    imageOdt(QString imgPath, int height, int widht);

    //Get
    QString inline getImgPath(){return mImgPath;}
    int inline getHeight(){return mHeight;}
    int inline getWidht(){return mWidth;}

private:
    QString mImgPath;
    int mHeight;
    int mWidth;
};

#endif // IMAGEODT_H
