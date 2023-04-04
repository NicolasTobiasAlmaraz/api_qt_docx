#ifndef TEXTODT_H
#define TEXTODT_H

#include <QString>

class textOdt {
public:
    static const int R=0;
    static const int G=1;
    static const int B=2;

    // Fonts
    static const QString FONT_CALIBRI;
    static const QString FONT_ARIAL;
    static const QString FONT_TIMES_NEW_ROMAN;

    // Align
    static const QString ALIGN_LEFT;
    static const QString ALIGN_CENTER;
    static const QString ALIGN_RIGHT;
    static const QString ALIGN_JUSTIFY;

    // Constructor
    textOdt(QString txt);

    // Set
    void inline setContent(QString text){mContent = text;}
    void inline setLetterSize(int size){mLetterSize = size;}
    void inline setFont(QString font){mFont = font;}
    void inline setAlign(QString align){mAlign = align;}
    void inline setUnderline(bool underline){mUnderline = underline;}
    void inline setBold(bool bold){mBold = bold;}
    void inline setItalic(bool italic){mItalic = italic;}
    void inline setStrikethrough(bool strikethrough){mStrikethrough = strikethrough;}
    void inline setFillColor(QString color) {mFillColor=color;} //Protocolo de QString: "#RRGGBB"
    void inline setBackgroundColor(QString color) {mBackgroundColor=color;}

    // Get
    QString inline getContent(){return mContent;}
    int inline getLetterSize(){return mLetterSize;}
    QString inline getFont(){return mFont;}
    QString inline getAlign(){return mAlign;}
    bool inline getUnderline(){return mUnderline;}
    bool inline getBold(){return mBold;}
    bool inline getItalic(){return mItalic;}
    bool inline getStrikethrough(){return mStrikethrough;}
    QString inline getFillColor(){return mFillColor;}
    QString inline getBackgroundColor(){return mBackgroundColor;}

private:
    QString mContent;
    int mLetterSize;
    QString mFont;
    QString mAlign;
    bool mUnderline;
    bool mBold;
    bool mItalic;
    bool mStrikethrough;
    QString mFillColor;
    QString mBackgroundColor;
};

#endif // TEXTODT_H
