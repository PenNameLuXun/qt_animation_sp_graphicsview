#include "cspgraviewbtn.h"
#include <QPainter>

CSpGraviewBtn::CSpGraviewBtn(CSpGraphicViewItem *parentItem):
    CSpGraphicViewItem(parentItem)
{
    connect(this,&CSpGraviewBtn::sig_Clicked,this,[=](){
        if(isEnabled())
        {
            QMetaObject::invokeMethod(this,"btnClicked",Qt::QueuedConnection,Q_ARG(bool,isNcChecked()));
            //emit btnClicked(isNcChecked());
        }
    },Qt::QueuedConnection);
}

bool CSpGraviewBtn::isNcCheckable()
{
    return _ncCheckable;
}

void CSpGraviewBtn::setNcCheckable(bool b)
{
    _ncCheckable = b;
    update();
}

bool CSpGraviewBtn::isNcChecked()
{
    return _ncChecked;
}

void CSpGraviewBtn::setNcChecked(bool b)
{
    _ncChecked = b;
    update();
}

bool CSpGraviewBtn::isEnabled()
{
    return _enable;
}

void CSpGraviewBtn::setEnabled(bool b)
{
    _enable = b;
    update();
}

QRectF CSpGraviewBtn::boundRect()
{
    return _rct;
}

void CSpGraviewBtn::setBoundRect(const QRectF &rctF)
{
    _rct = rctF;
}

void CSpGraviewBtn::setImage(const QStringList& s, const QSize &sz)
{
    _imgSz = sz;
    for(int i = 0;i<s.size();++i)
    {
        if(i>= StateCount)
            break;

        _img[i] = s[i];
    }

    if(s.size() < StateCount)
    {
        if(s.size() > NcCheckedHover)
        {
            _img[NcCheckedPressed] = s[NcChecked];
        }
        else if(s.size() > NcChecked)
        {
            _img[NcCheckedHover] = s[NcChecked];
            _img[NcCheckedPressed] = s[NcChecked];
        }
        else if(s.size() > Dis)
        {
            _img[NcChecked]        = s[Normal];
            _img[NcCheckedHover]   = s[Hover];
            _img[NcCheckedPressed] = s[Normal];
        }
        else if(s.size() > Pressed)
        {
            _img[Dis]              = s[Normal];
            _img[NcChecked]        = s[Normal];
            _img[NcCheckedHover]   = s[Hover];
            _img[NcCheckedPressed] = s[Pressed];
        }
        else if(s.size() > Hover)
        {
            _img[Pressed]          = s[Normal];
            _img[Dis]              = s[Normal];
            _img[NcChecked]        = s[Normal];
            _img[NcCheckedHover]   = s[Hover];
            _img[NcCheckedPressed] = s[Normal];
        }
        else if(s.size() > Normal)
        {
            _img[Hover]            = s[Normal];
            _img[Pressed]          = s[Normal];
            _img[Dis]              = s[Normal];
            _img[NcChecked]        = s[Normal];
            _img[NcCheckedHover]   = s[Normal];
            _img[NcCheckedPressed] = s[Normal];
        }
    }

    update();
}

void CSpGraviewBtn::setBgColor(const QList<QColor> &bgLisColor)
{
    for(int i = 0;i<bgLisColor.size();++i)
    {
        if(i>= StateCount)
            break;

        _color[i] = bgLisColor[i];
    }
    if(bgLisColor.size() < StateCount)
    {
        if(bgLisColor.size() > NcCheckedHover)
        {
            _color[NcCheckedPressed] = bgLisColor[NcChecked];
        }
        else if(bgLisColor.size() > NcChecked)
        {
            _color[NcCheckedHover] = bgLisColor[NcChecked];
            _color[NcCheckedPressed] = bgLisColor[NcChecked];
        }
        else if(bgLisColor.size() > Dis)
        {
            _color[NcChecked]        = bgLisColor[Normal];
            _color[NcCheckedHover]   = bgLisColor[Hover];
            _color[NcCheckedPressed] = bgLisColor[Normal];
        }
        else if(bgLisColor.size() > Pressed)
        {
            _color[Dis]              = bgLisColor[Normal];
            _color[NcChecked]        = bgLisColor[Normal];
            _color[NcCheckedHover]   = bgLisColor[Hover];
            _color[NcCheckedPressed] = bgLisColor[Pressed];
        }
        else if(bgLisColor.size() > Hover)
        {
            _color[Pressed]          = bgLisColor[Normal];
            _color[Dis]              = bgLisColor[Normal];
            _color[NcChecked]        = bgLisColor[Normal];
            _color[NcCheckedHover]   = bgLisColor[Hover];
            _color[NcCheckedPressed] = bgLisColor[Normal];
        }
        else if(bgLisColor.size() > Normal)
        {
            _color[Hover]            = bgLisColor[Normal];
            _color[Pressed]          = bgLisColor[Normal];
            _color[Dis]              = bgLisColor[Normal];
            _color[NcChecked]        = bgLisColor[Normal];
            _color[NcCheckedHover]   = bgLisColor[Normal];
            _color[NcCheckedPressed] = bgLisColor[Normal];
        }
    }
}

void CSpGraviewBtn::setText(const QString &txt, int sp, EAligment al)
{
    _text = txt;
    _space = sp;
    _algMent = al;
    update();
}

QString CSpGraviewBtn::text()
{
    return _text;
}

void CSpGraviewBtn::setAligment(CSpGraviewBtn::EAligment al)
{
    _algMent = al;
    update();
}

CSpGraviewBtn::EAligment CSpGraviewBtn::aligment()
{
    return _algMent;
}

void CSpGraviewBtn::setDirection(CSpGraviewBtn::ELayDirection di)
{
    _direction = di;
    update();
}

CSpGraviewBtn::ELayDirection CSpGraviewBtn::direction()
{
    return _direction;
}

void CSpGraviewBtn::setTextPosPriority(CSpGraviewBtn::ETextPosPriority pri)
{
    _textPosPri = pri;
    update();
}

CSpGraviewBtn::ETextPosPriority CSpGraviewBtn::textPosPriority()
{
    return _textPosPri;
}

void CSpGraviewBtn::setSpace(int sp)
{
    _space = sp;
    update();
}

int CSpGraviewBtn::space()
{
    return _space;
}

void CSpGraviewBtn::setMarginsF(const QMarginsF &mrg)
{
    _marginF = mrg;
    update();
}

QMarginsF CSpGraviewBtn::marginsF()
{
    return _marginF;
}

void CSpGraviewBtn::setTextFont(const QFont &font)
{
    if(_pFont != nullptr)
    {
        delete _pFont;
    }
    _pFont = new QFont(font);
    update();
}

void CSpGraviewBtn::setTextFont(const QString &family, int pixSize, int weight, bool italic)
{
    if(_pFont != nullptr)
    {
        delete _pFont;
    }
    _pFont = new QFont;

    _pFont->setFamily(family);
    _pFont->setPixelSize(pixSize);
    _pFont->setWeight(weight);
    _pFont->setItalic(italic);
    update();
}

void CSpGraviewBtn::setTextColor(const QList<QColor> &txtColor)
{
    for(int i = 0;i<txtColor.size();++i)
    {
        if(i>= StateCount)
            break;

        _textColor[i] = txtColor[i];
    }
    if(txtColor.size() < StateCount)
    {
        if(txtColor.size() > NcCheckedHover)
        {
            _textColor[NcCheckedPressed] = txtColor[NcChecked];
        }
        else if(txtColor.size() > NcChecked)
        {
            _textColor[NcCheckedHover] = txtColor[NcChecked];
            _textColor[NcCheckedPressed] = txtColor[NcChecked];
        }
        else if(txtColor.size() > Dis)
        {
            _textColor[NcChecked]        = txtColor[Normal];
            _textColor[NcCheckedHover]   = txtColor[Hover];
            _textColor[NcCheckedPressed] = txtColor[Normal];
        }
        else if(txtColor.size() > Pressed)
        {
            _textColor[Dis]              = txtColor[Normal];
            _textColor[NcChecked]        = txtColor[Normal];
            _textColor[NcCheckedHover]   = txtColor[Hover];
            _textColor[NcCheckedPressed] = txtColor[Pressed];
        }
        else if(txtColor.size() > Hover)
        {
            _textColor[Pressed]          = txtColor[Normal];
            _textColor[Dis]              = txtColor[Normal];
            _textColor[NcChecked]        = txtColor[Normal];
            _textColor[NcCheckedHover]   = txtColor[Hover];
            _textColor[NcCheckedPressed] = txtColor[Normal];
        }
        else if(txtColor.size() > Normal)
        {
            _textColor[Hover]            = txtColor[Normal];
            _textColor[Pressed]          = txtColor[Normal];
            _textColor[Dis]              = txtColor[Normal];
            _textColor[NcChecked]        = txtColor[Normal];
            _textColor[NcCheckedHover]   = txtColor[Normal];
            _textColor[NcCheckedPressed] = txtColor[Normal];
        }
    }
}

QFont CSpGraviewBtn::textFont()
{
    QFont font(graphicView()->font());
    if(_pFont != nullptr)
    {
        font = *_pFont;
    }
    else
    {
        font.setPixelSize(13);
    }
    return font;
}

QSize CSpGraviewBtn::minSize()
{
    QSize retSz;
    if(text().isEmpty())
    {
        if(_img->isEmpty())
        {
            retSz = QSize(0,0);
        }
        else
        {
            //只有插图
            retSz = _imgSz;
        }
    }
    else
    {
        if(_img->isEmpty())
        {
            //只有文字
            QFont font(textFont(),graphicView()->viewPort());
            QFontMetrics meter(font,graphicView()->viewPort());
            retSz = QSizeF(meter.width(text()),meter.height()).toSize();
        }
        else
        {
            //有文字有插图
            QFont font(textFont(),graphicView()->viewPort());
            QFontMetrics meter(font,graphicView()->viewPort());
            QSizeF textSz;
            QSizeF imgAndText;
            if(direction() == Hor)
            {
                //水平
                textSz     = QSizeF(meter.width(text()),meter.height());
                imgAndText = QSizeF(_imgSz.width() + space() + textSz.width(),qMax(_imgSz.height(),qRound(textSz.height())));

            }
            else
            {
                //垂直
                textSz     = QSizeF(meter.width(text()),meter.height());
                imgAndText = QSizeF(qMax(_imgSz.width(),qRound(textSz.width())),_imgSz.height() + space() + textSz.height());
            }
            retSz = imgAndText.toSize();
        }
    }
    return retSz + QSizeF(_marginF.left()+_marginF.right(),_marginF.top()+_marginF.bottom()).toSize();
}

void CSpGraviewBtn::paintSelf(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo)
{
    painter->setRenderHint(QPainter::SmoothPixmapTransform,true);

    BtnPaintInfo info = getPaintInfo(ItemEnableState(paintInfo->enableState));

    painter->setPen(Qt::NoPen);
    painter->setBrush(info.bgColor);
    painter->drawRect(paintInfo->rectF);
    painter->setFont(textFont());

    if(direction() == Hor)
    {
        //水平绘制
        if(textPosPriority() == First)
        {
            //文字在首
            paintSelfHorFirst(painter,paintInfo,&info);
        }
        else
        {
            //文字在后
            paintSelfHorSecond(painter,paintInfo,&info);
        }
    }
    else
    {
        //垂直绘制
        if(textPosPriority() == First)
        {
            //文字在上
            paintSelfVerFirst(painter,paintInfo,&info);
        }
        else
        {
            //文字在下
            paintSelfVerSecond(painter,paintInfo,&info);
        }
    }
}

void CSpGraviewBtn::paintSelfHorFirst(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo, CSpGraviewBtn::BtnPaintInfo *pBtnPaintInfo)
{
    //水平布局 文字在前图像在后

    //先设置marginF
    QRectF contexRectF = paintInfo->rectF.adjusted(marginsF().left(),marginsF().top(),-marginsF().right(),-marginsF().bottom());

    QSizeF textSz(painter->fontMetrics().width(text()),painter->fontMetrics().height());

    QSizeF imgAndText(_imgSz.width() + space() + textSz.width(),qMax(_imgSz.height(),qRound(textSz.height())));

    QPointF first; QSizeF firstSz(textSz);
    QPointF second;QSizeF secondSz(_imgSz);
    if(_algMent & Left)
    {
        first.setX(0);
    }
    else if(_algMent & Right)
    {
        first.setX(contexRectF.width() - imgAndText.width());
    }
    else/* if(_algMent & CenterH)*/     //默认就是居中
    {
        first.setX((contexRectF.width() - imgAndText.width()) /2.0);
    }
    second.setX(first.x() + firstSz.width() + space());

    if(_algMent & Top)
    {
        first.setY((imgAndText.height() - firstSz.height())/2.0);
    }
    else if(_algMent & Bottom)
    {
        first.setY(contexRectF.height() - imgAndText.height() + (imgAndText.height() - firstSz.height())/2.0);
    }
    else/* if(_algMent & CenterV)*/        //默认就是居中
    {
        first.setY((contexRectF.height() - imgAndText.height())/2.0 + (imgAndText.height() - firstSz.height())/2.0);
    }
    second.setY(first.y() + (firstSz.height() - secondSz.height())/ 2.0);



    painter->save();
    painter->translate(contexRectF.topLeft());

    painter->setPen(pBtnPaintInfo->txtColor);

    painter->drawText(QRectF(first,firstSz),Qt::TextWordWrap,text());

    painter->drawPixmap(QRectF(second,secondSz).toRect(),pBtnPaintInfo->pix);

    painter->restore();
}

void CSpGraviewBtn::paintSelfHorSecond(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo, CSpGraviewBtn::BtnPaintInfo *pBtnPaintInfo)
{
    //水平布局 图像在前文字在后

    //先设置marginF
    QRectF contexRectF = paintInfo->rectF.adjusted(marginsF().left(),marginsF().top(),-marginsF().right(),-marginsF().bottom());


    QSizeF textSz(painter->fontMetrics().width(text()),painter->fontMetrics().height());

    QSizeF imgAndText(_imgSz.width() + space() + textSz.width(),qMax(_imgSz.height(),qRound(textSz.height())));

    QPointF first; QSizeF firstSz(_imgSz);
    QPointF second;QSizeF secondSz(textSz);
    if(_algMent & Left)
    {
        first.setX(0);
    }
    else if(_algMent & Right)
    {
        first.setX(contexRectF.width() - imgAndText.width());
    }
    else/* if(_algMent & CenterH)*/     //默认就是居中
    {
        first.setX((contexRectF.width() - imgAndText.width()) /2.0);
    }
    second.setX(first.x() + firstSz.width() + space());

    if(_algMent & Top)
    {
        first.setY((imgAndText.height() - firstSz.height())/2.0);
    }
    else if(_algMent & Bottom)
    {
        first.setY(contexRectF.height() - imgAndText.height() + (imgAndText.height() - firstSz.height())/2.0);
    }
    else/* if(_algMent & CenterV)*/        //默认就是居中
    {
        first.setY((contexRectF.height() - imgAndText.height())/2.0 + (imgAndText.height() - firstSz.height())/2.0);
    }
    second.setY(first.y() + (firstSz.height() - secondSz.height())/ 2.0);



    painter->save();
    painter->translate(contexRectF.topLeft());

    painter->drawPixmap(QRectF(first,firstSz).toRect(),pBtnPaintInfo->pix);

    painter->setPen(pBtnPaintInfo->txtColor);
    painter->drawText(QRectF(second,secondSz),Qt::TextWordWrap,text());

    painter->restore();
}

void CSpGraviewBtn::paintSelfVerFirst(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo, CSpGraviewBtn::BtnPaintInfo *pBtnPaintInfo)
{
    //垂直布局 图像在上文字在下

    //先设置marginF
    QRectF contexRectF = paintInfo->rectF.adjusted(marginsF().left(),marginsF().top(),-marginsF().right(),-marginsF().bottom());


    QSizeF textSz(painter->fontMetrics().width(text()),painter->fontMetrics().height());

    QSizeF imgAndText(qMax(_imgSz.width(),qRound(textSz.width())),_imgSz.height() + space() + textSz.height());

    QPointF first; QSizeF firstSz(textSz);
    QPointF second;QSizeF secondSz(_imgSz);

    if(_algMent & Top)
    {
        first.setY(0);
    }
    else if(_algMent & Bottom)
    {
        first.setY(contexRectF.height() - imgAndText.height());
    }
    else/* if(_algMent & CenterH)*/     //默认就是居中
    {
        first.setY((contexRectF.height() - imgAndText.height()) /2.0);
    }
    second.setY(first.y() + firstSz.height() + space());

    if(_algMent & Left)
    {
        first.setX((imgAndText.width() - firstSz.width())/2.0);
    }
    else if(_algMent & Right)
    {
        first.setX(contexRectF.width() - imgAndText.width() + (imgAndText.width() - firstSz.width())/2.0);
    }
    else/* if(_algMent & CenterV)*/        //默认就是居中
    {
        first.setX((contexRectF.width() - imgAndText.width())/2.0 + (imgAndText.width() - firstSz.width())/2.0);
    }
    second.setX(first.x() + (firstSz.width() - secondSz.width())/ 2.0);



    painter->save();
    painter->translate(contexRectF.topLeft());

    painter->drawPixmap(QRectF(second,secondSz).toRect(),pBtnPaintInfo->pix);

    painter->setPen(pBtnPaintInfo->txtColor);
    painter->drawText(QRectF(first,firstSz),Qt::TextWordWrap,text());

    painter->restore();
}

void CSpGraviewBtn::paintSelfVerSecond(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo, CSpGraviewBtn::BtnPaintInfo *pBtnPaintInfo)
{
    //垂直布局 图像在上文字在下

    //先设置marginF
    QRectF contexRectF = paintInfo->rectF.adjusted(marginsF().left(),marginsF().top(),-marginsF().right(),-marginsF().bottom());


    QSizeF textSz(painter->fontMetrics().width(text()),painter->fontMetrics().height());

    QSizeF imgAndText(qMax(_imgSz.width(),qRound(textSz.width())),_imgSz.height() + space() + textSz.height());

    QPointF first; QSizeF firstSz(_imgSz);
    QPointF second;QSizeF secondSz(textSz);

    if(_algMent & Top)
    {
        first.setY(0);
    }
    else if(_algMent & Bottom)
    {
        first.setY(contexRectF.height() - imgAndText.height());
    }
    else/* if(_algMent & CenterH)*/     //默认就是居中
    {
        first.setY((contexRectF.height() - imgAndText.height()) /2.0);
    }
    second.setY(first.y() + firstSz.height() + space());

    if(_algMent & Left)
    {
        first.setX((imgAndText.width() - firstSz.width())/2.0);
    }
    else if(_algMent & Right)
    {
        first.setX(contexRectF.width() - imgAndText.width() + (imgAndText.width() - firstSz.width())/2.0);
    }
    else/* if(_algMent & CenterV)*/        //默认就是居中
    {
        first.setX((contexRectF.width() - imgAndText.width())/2.0 + (imgAndText.width() - firstSz.width())/2.0);
    }
    second.setX(first.x() + (firstSz.width() - secondSz.width())/ 2.0);



    painter->save();
    painter->translate(contexRectF.topLeft());

    painter->drawPixmap(QRectF(first,firstSz).toRect(),pBtnPaintInfo->pix);

    painter->setPen(pBtnPaintInfo->txtColor);
    painter->drawText(QRectF(second,secondSz),Qt::TextWordWrap,text());

    painter->restore();
}

void CSpGraviewBtn::onClicked()
{
    if(isNcCheckable())
    {
        setNcChecked(!isNcChecked());
    }
}

CSpGraviewBtn::BtnPaintInfo CSpGraviewBtn::getPaintInfo(CSpGraphicViewItem::ItemEnableState state)
{
    CSpGraviewBtn::BtnPaintInfo info;
    if(_enable)
    {
        if(isNcCheckable()&& isNcChecked())
        {
            switch (state) {
            case state_Normal:
                info.pix.load(_img[NcChecked]);
                info.bgColor = _color[NcChecked];
                info.txtColor = _textColor[NcChecked];
                break;
            case state_Hover:
                info.pix.load(_img[NcCheckedHover]);
                info.bgColor = _color[NcCheckedHover];
                info.txtColor = _textColor[NcCheckedHover];
                break;
            case state_Pressed:
                info.pix.load(_img[NcCheckedPressed]);
                info.bgColor = _color[NcCheckedPressed];
                info.txtColor = _textColor[NcCheckedPressed];
                break;
            }
        }
        else
        {
            switch (state) {
            case state_Normal:
                info.pix.load(_img[Normal]);
                info.bgColor = _color[Normal];
                info.txtColor = _textColor[Normal];
                break;
            case state_Hover:
                info.pix.load(_img[Hover]);
                info.bgColor = _color[Hover];
                info.txtColor = _textColor[Hover];
                break;
            case state_Pressed:
                info.pix.load(_img[Pressed]);
                info.bgColor = _color[Pressed];
                info.txtColor = _textColor[Pressed];
                break;
            }
        }
    }
    else
    {
        info.bgColor = _color[Dis];
        info.pix.load(_img[Dis]);
        info.txtColor = _textColor[Dis];
    }
    return info;
}
