#ifndef CGRAVIEWBTN_H
#define CGRAVIEWBTN_H
#include "cspgraphicviewitem.h"

class GRAPHICVIEW_EXPORT CSpGraviewBtn:public CSpGraphicViewItem
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(EAligment aligment READ aligment WRITE setAligment)
    Q_PROPERTY(ELayDirection direction READ direction WRITE setDirection)
    Q_PROPERTY(ETextPosPriority textPosPriority READ textPosPriority WRITE setTextPosPriority)
    Q_PROPERTY(int space READ space WRITE setSpace)
    Q_PROPERTY(QMarginsF marginsF READ marginsF WRITE setMarginsF)

public:
    CSpGraviewBtn(CSpGraphicViewItem*   parentItem = nullptr);

    enum EAligment{Left = 0x0001,Right = 0x0002,CenterH = 0x0004,
                   Top = 0x0008,Bottom = 0x0010,CenterV= 0x0020,

                   LeftCenterV    = Left|CenterV,
                   RightCenterV   = Right|CenterV,
                   TopCenterH     = Top|CenterH,
                   BottomCenterH  = Bottom|CenterH,

                   Center = CenterH|CenterV};
    enum ELayDirection{Hor,Ver};
    enum ETextPosPriority{First,Second};

    bool isNcCheckable();
    void setNcCheckable(bool b);

    bool isNcChecked();
    void setNcChecked(bool b);

    bool isEnabled();
    void setEnabled(bool b);

    QRectF boundRect();
    void   setBoundRect(const QRectF& rctF);

    void setImage(const QStringList& s,const QSize& sz);
    void setBgColor(const QList<QColor>& bgLisColor);


    void setText(const QString& txt,int _space = 6,EAligment al = Left);
    QString text();

    void setAligment(EAligment al);
    EAligment aligment();

    void setDirection(ELayDirection di);
    ELayDirection direction();

    void setTextPosPriority(ETextPosPriority pri);
    ETextPosPriority textPosPriority();

    void setSpace(int sp);
    int  space();

    void setMarginsF(const QMarginsF& mrg);
    QMarginsF marginsF();

    void setTextFont(const QFont& font);
    void setTextFont(const QString &family, int pixSize = 13, int weight = -1, bool italic = false);
    void setTextColor(const QList<QColor>& txtColor);

    inline QFont textFont();
    QSize minSize();
signals:
    void btnClicked(bool _ncChecked = false);
protected:
    struct BtnPaintInfo{QColor bgColor;QPixmap pix; QColor txtColor;};
    BtnPaintInfo getPaintInfo(ItemEnableState state = state_Normal);
protected:
    void paintSelf(QPainter* painter,const CSpGraphicViewPaintEventInfo *paintInfo);

    /*将这四个函数设置成可重载完全是为了实现自定的dpi实现(如果用qt自带的dpi完全不用重载了除非有特殊的需要)*/
    virtual void paintSelfHorFirst(QPainter* painter,const CSpGraphicViewPaintEventInfo *paintInfo,BtnPaintInfo* pBtnPaintInfo);
    virtual void paintSelfHorSecond(QPainter* painter,const CSpGraphicViewPaintEventInfo *paintInfo,BtnPaintInfo* pBtnPaintInfo);
    virtual void paintSelfVerFirst(QPainter* painter,const CSpGraphicViewPaintEventInfo *paintInfo,BtnPaintInfo* pBtnPaintInfo);
    virtual void paintSelfVerSecond(QPainter* painter,const CSpGraphicViewPaintEventInfo *paintInfo,BtnPaintInfo* pBtnPaintInfo);


    void onClicked();


protected:
    enum state{Normal,Hover,Pressed,Dis,NcChecked,NcCheckedHover,NcCheckedPressed,StateCount};
    QRectF  _rct;
    QString _img[StateCount];
    QSize   _imgSz;
    QColor  _color[StateCount];

    bool    _enable  = true;

    bool    _ncCheckable = false;
    bool    _ncChecked   = false;

    QColor    _textColor[StateCount];
    QString   _text;
    int       _space          = 0;
    QMarginsF _marginF        = QMarginsF(0,0,0,0);
    QFont*    _pFont          = nullptr;

    EAligment        _algMent    = Center;
    ELayDirection    _direction  = Hor;
    ETextPosPriority _textPosPri = Second;
};

#endif // CGRAVIEWBTN_H
