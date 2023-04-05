#include "textodt.h"

//Fonts
const QString textOdt::FONT_CALIBRI = "Calibri";
const QString textOdt::FONT_ARIAL = "Arial";
const QString textOdt::FONT_TIMES_NEW_ROMAN = "Times New Roman";

// Align
const QString textOdt::ALIGN_LEFT = "left";
const QString textOdt::ALIGN_CENTER = "center";
const QString textOdt::ALIGN_RIGHT = "right";
const QString textOdt::ALIGN_JUSTIFY = "justify";

textOdt::textOdt(QString txt) {
    mContent = txt;
    mLetterSize = 11;
    mFont = FONT_CALIBRI;
    mAlign = ALIGN_LEFT;
    mUnderline = false;
    mBold = false;
    mItalic = false;
    mStrikethrough = false;
    mFillColor = "#000000";
    mBackgroundColor = "#FFFFFF";
}
