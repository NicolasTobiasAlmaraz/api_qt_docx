#ifndef PAGEODT_H
#define PAGEODT_H


class pageOdt {
public:
    pageOdt();

    //Set
    void inline setMarginTop(int margin){mMarginTop = margin;}
    void inline setMarginLeft(int margin){mMarginLeft = margin;}
    void inline setMarginRight(int margin){mMarginRight = margin;}
    void inline setMarginBottom(int margin){mMarginBottom = margin;}
    void inline setSizeWidth(int size){mSizeWidth = size;}
    void inline setSizeHeight(int size){mSizeHeight = size;}

    //Get
    int inline getMarginTop(){return mMarginTop;}
    int inline getMarginLeft(){return mMarginLeft;}
    int inline getMarginRight(){return mMarginRight;}
    int inline getMarginBottom(){return mMarginBottom;}
    int inline getSizeWidth(){return mSizeWidth;}
    int inline getSizeHeight(){return mSizeHeight;}

private:
    //Margins
    int mMarginTop;
    int mMarginLeft;
    int mMarginRight;
    int mMarginBottom;

    //Size
    int mSizeWidth;
    int mSizeHeight;
};

#endif // PAGEODT_H
