#include "imageodt.h"

const QString imageOdt::LEFT = "left";
const QString imageOdt::RIGHT = "right";
const QString imageOdt::CENTER = "center";
const QString imageOdt::PARALEL = "paralel";
const QString imageOdt::NONE = "none";

imageOdt::imageOdt(QString imgPath, int height, int width, QString anchor, QString align) {
    mImgPath = imgPath;
    mHeight = height;
    mWidth = width;
    mAlign = align;
    mAnchor = anchor;
}

