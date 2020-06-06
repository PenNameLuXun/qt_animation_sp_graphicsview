#include "csprealslider.h"

#include <QPainter>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPropertyAnimation>

CSpRealSlider::CSpRealSlider(QWidget *parent,EDirection dire):
    QWidget(parent),direction(dire)
{
    setMouseTracking(true);
}

CSpRealSlider::~CSpRealSlider()
{

}

void CSpRealSlider::setDirection(CSpRealSlider::EDirection dire)
{
    direction = dire;
    update();
}

qreal CSpRealSlider::value()
{
    return _value;
}

void CSpRealSlider::setValue(qreal value)
{
    if(!_enableAnimation)
    {
        if(_animationSetValue != nullptr &&_animationSetValue->state() == QPropertyAnimation::Running)
        {
            _animationSetValue->stop();
        }
        setValue_direction(value);
    }
    else
    {
        setValue_Animation(value);
    }
}

void CSpRealSlider::enableAnimation(bool b)
{
    _enableAnimation = b;
}

void CSpRealSlider::setValue_Animation(qreal value, int duration)
{
    QPropertyAnimation* pAnimation = getAnimation();

    pAnimation->setStartValue(this->value());
    pAnimation->setEndValue(qMax(0.0,qMin(value,max())));
    pAnimation->setDuration(duration);

    pAnimation->setCurrentTime(0);

    pAnimation->start();
}

qreal CSpRealSlider::min()
{
    return _min;
}

qreal CSpRealSlider::max()
{
    return _max;
}

void CSpRealSlider::setMin(qreal min)
{
    qreal oldMin = _min;
    _min = qMax(min,0.0);

    if(!qFuzzyIsNull(oldMin - _min))
    {
        emit rangeChanged(this->min(),this->max());

        qreal newValue = qMax(qMin(this->value(),this->max()),this->min());

        if(!qFuzzyIsNull(newValue - this->value()))
        {
            _value = newValue;
            emit valueChanged(_value);
        }
    }

    update();
}

void CSpRealSlider::setMax(qreal max)
{
    qreal oldMax = _max;
    _max = qMax(max,0.0);

    if(!qFuzzyIsNull(oldMax - _max))
    {
        emit rangeChanged(this->min(),this->max());

        qreal newValue = qMax(qMin(this->value(),this->max()),this->min());

        if(!qFuzzyIsNull(newValue - this->value()))
        {
            _value = newValue;
            emit valueChanged(_value);
        }
    }

    update();
}

void CSpRealSlider::setRange(qreal min, qreal max)
{
    qreal oldMin = _min;
    qreal oldMax = _max;
    _min = qMax(min,0.0);
    _max = qMax(max,0.0);

    if(!qFuzzyIsNull(oldMin - _min) || !qFuzzyIsNull(oldMax - _max))
    {
        emit rangeChanged(this->min(),this->max());

        qreal newValue = qMax(qMin(this->value(),this->max()),this->min());

        if(!qFuzzyIsNull(newValue - this->value()))
        {
            _value = newValue;
            emit valueChanged(_value);
        }
    }
    update();
}

bool CSpRealSlider::isSingleStepBtnVisble()
{
    return _isBeginAndEndBtnExist;
}

void CSpRealSlider::setSingleStepBtnVisble(bool b)
{
    _isBeginAndEndBtnExist = b;
    update();
}

qreal CSpRealSlider::singleStep()
{
    return _singleStep;
}

void CSpRealSlider::setSingleStep(qreal step)
{
    _singleStep = step;
    update();
}

qreal CSpRealSlider::pageStep()
{
    return _pageStep;
}

void CSpRealSlider::setPageStep(qreal pageStep)
{
    _pageStep = pageStep;
    update();
}

void CSpRealSlider::setClickGroove(CSpRealSlider::EClickGrooveDo type)
{
    _clickGrooveDo = type;
}

CSpRealSlider::EClickGrooveDo CSpRealSlider::clickGroove()
{
    return _clickGrooveDo;
}

bool CSpRealSlider::isClickSetValueAnimationWay()
{
    return _clickAnimationToSetValue;
}

void CSpRealSlider::setClickSetValueAnimation(bool b)
{
    _clickAnimationToSetValue = b;
}

void CSpRealSlider::setToScrollBar(bool b)
{
    _isScrollBar = b;
    update();
}

bool CSpRealSlider::isScrollBar()
{
    return _isScrollBar;
}

void CSpRealSlider::setGroovePTFLen(qreal len)
{
    _dpiBaseInfo._groovePtfLen = len;
    updateDpi();
}

qreal CSpRealSlider::getGroovePTFLen()
{
    return _dpiBaseInfo._groovePtfLen;
}

void CSpRealSlider::setSingleStepBtnLen(qreal len)
{
    _dpiBaseInfo._singleStepBtnLen = len;
    updateDpi();
}

qreal CSpRealSlider::singleSteptnLen()
{
    return _dpiBaseInfo._singleStepBtnLen;
}

qreal CSpRealSlider::singleStepBtnPTFLen()
{
    return _dpiBaseInfo._singleStepBtnPtfLen;
}

void CSpRealSlider::setSingleStepBtnPTFLen(qreal len)
{
    _dpiBaseInfo._singleStepBtnPtfLen = len;
    updateDpi();
}

qreal CSpRealSlider::sliderPTFLen()
{
    return _dpiBaseInfo._sliderPtfLen;
}

void CSpRealSlider::setSliderPTFLen(qreal l)
{
    _dpiBaseInfo._sliderPtfLen = l;
    updateDpi();
}

qreal CSpRealSlider::sliderMinLen()
{
    return _dpiBaseInfo._minSliderLen;
}

void CSpRealSlider::setSliderMinLen(qreal len)
{
    _dpiBaseInfo._minSliderLen = len;
    updateDpi();
}

qreal CSpRealSlider::dpi()
{
    return _dpi;
}

void CSpRealSlider::setDpi(qreal dpi)
{
    _dpi = dpi;
    updateDpi();
}

void CSpRealSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    /* 先绘制背景色 */
    painter.save();
    paintBackground(&painter,rect(),state[CntCount]);
    painter.restore();

    if(_isBeginAndEndBtnExist)
    {
        painter.save();
        QRectF rectF = getControlRectF(SingleStepBtn_Begin);
        paintBeginBtn(&painter,rectF,state[SingleStepBtn_Begin]);
        painter.restore();
    }

    /* 绘制槽 */
    painter.save();
    QRectF rectFGroove = getControlRectF(Groove);
    //painter.translate(rectFGroove.topLeft());
    paintGroove(&painter,rectFGroove,state[Groove]);
    painter.restore();

    if(_isBeginAndEndBtnExist)
    {
        painter.save();
        QRectF rectF = getControlRectF(SingleStepBtn_End);
        //painter.translate(rectF.topLeft());
        paintEndBtn(&painter,rectF,state[SingleStepBtn_End]);
        painter.restore();
    }

    painter.save();
    QRectF rectFSlider = getControlRectF(Slider);
    //painter.translate(rectFSlider.topLeft());
    paintSlider(&painter,rectFSlider,state[Slider]);
    painter.restore();

    return QWidget::paintEvent(event);
}

void CSpRealSlider::paintBackground(QPainter *painter, const QRectF &rct, EContrlState state)
{
    Q_UNUSED(painter);
    Q_UNUSED(rct);
    Q_UNUSED(state);
}

void CSpRealSlider::paintBeginBtn(QPainter *painter, const QRectF &rct,EContrlState state)
{
    QColor c(Qt::red);

    if(state == Hover)
    {
        c = c.light();
    }
    else if(state == Pressed)
    {
        c = c.dark();
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(c);
    painter->drawRect(rct);
}

void CSpRealSlider::paintEndBtn(QPainter *painter, const QRectF &rct,EContrlState state)
{
    QColor c(Qt::red);

    if(state == Hover)
    {
        c = c.light();
    }
    else if(state == Pressed)
    {
        c = c.dark();
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(c);
    painter->drawRect(rct);
}

void CSpRealSlider::paintGroove(QPainter *painter, const QRectF &rct,EContrlState state)
{
    QColor c(43,44,45);

    if(state == Hover)
    {
        c = c.light();
    }
    else if(state == Pressed)
    {
        c = c.dark();
    }

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(c);
    painter->drawRoundedRect(rct,4.0*_dpi,4.0*_dpi,Qt::AbsoluteSize);
}

void CSpRealSlider::paintSlider(QPainter *painter, const QRectF &rct,EContrlState state)
{
    QColor c(200,200,200);

    if(state == Hover)
    {
        c = c.light();
    }
    else if(state == Pressed)
    {
        c = c.dark();
    }
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(c);
    painter->drawRoundedRect(rct,4.0*_dpi,4.0*_dpi,Qt::AbsoluteSize);
}

void CSpRealSlider::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        for(int i = 0;i<CntCount;++i)
        {
            if(getControlRectF(EContrlType(i)).contains(event->pos()))
            {
                _pressedCntType   = EContrlType(i);
                _pressedInitPos   = event->pos();
                _pressedPos       = event->pos();
                _pressedSliderPos = getControlPos(Slider);
                state[i]          = Pressed;
                break;
            }
        }
        state[CntCount] = Pressed;
        update();
    }
    return QWidget::mousePressEvent(event);
}

void CSpRealSlider::mouseMoveEvent(QMouseEvent *event)
{
    switch (_pressedCntType) {
    case SingleStepBtn_Begin:
    {
        break;
    }
    case SingleStepBtn_End:
    {
        break;
    }
    case Groove:
    {
        break;
    }
    case Slider:
    {
        QPointF move  = event->pos() - _pressedPos;
        _pressedSliderPos += move;
        qreal value = calSliderValueBySliderPos(direction == Hor ? _pressedSliderPos.x():_pressedSliderPos.y());
        setValue(value);
        break;
    }
    default:
    {
        //证明没有点下
        for(int i = 0;i<CntCount + 1;++i)
        {
            if(getControlRectF(EContrlType(i)).contains(event->pos()))
            {
                state[i]          = Hover;
            }
            else
            {
                state[i]          = Normal;
            }
        }
        if(state[Slider] == Hover){state[Groove] = Normal;}
        update();
        break;
    }
    }
    _pressedPos = event->pos();
    return QWidget::mouseMoveEvent(event);
}

void CSpRealSlider::mouseReleaseEvent(QMouseEvent *event)
{
    switch (_pressedCntType) {
    case SingleStepBtn_Begin:
    {
        if(getControlRectF(SingleStepBtn_Begin).contains(event->pos()))
        {
            _clickAnimationToSetValue ? setValue_Animation(value() - singleStep()) : setValue(value() - singleStep());
        }
        break;
    }
    case SingleStepBtn_End:
    {
        if(getControlRectF(SingleStepBtn_End).contains(event->pos()))
        {
            _clickAnimationToSetValue ? setValue_Animation(value() + singleStep()) : setValue(value() + singleStep());
        }
        break;
    }
    case Groove:
    {
        if(getControlRectF(Groove).contains(event->pos()))
        {
            QRectF SliderRectF = getControlRectF(Slider);

            if(_clickGrooveDo == DoPageStep)
            {
                qreal move = pageStep();
                if(direction == Hor)
                {
                    if(SliderRectF.left() > event->pos().x())
                    {
                        move = -pageStep();
                    }
                }
                else
                {
                    if(SliderRectF.top() > event->pos().y())
                    {
                        move = -pageStep();
                    }
                }
                _clickAnimationToSetValue?setValue_Animation(value() + move):setValue(value() + move);
            }
            else if(_clickGrooveDo == DoSetDirectValue)
            {
                qreal sliderLen = calSliderLen();
                qreal pos       = (direction == Hor?event->x()- sliderLen/2.0:event->y() - sliderLen/2.0);

                qreal maxLen    = (direction == Hor?width()- sliderLen:height() - sliderLen);

                qreal value     = calSliderValueBySliderPos(qMax(qMin(pos,maxLen),0.0));
                _clickAnimationToSetValue?setValue_Animation(value):setValue(value);
            }
        }
        break;
    }
    case Slider:
    {
        break;
    }
    default:
        break;
    }

    _pressedCntType = CntCount;
    for(int i = 0;i<CntCount+1;++i)
    {
        if(getControlRectF(EContrlType(i)).contains(event->pos()))
        {
            state[i]          = Hover;
        }
        else
        {
            state[i]          = Normal;
        }
    }
    update();

    return QWidget::mouseReleaseEvent(event);
}

void CSpRealSlider::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    for(int i = 0;i<CntCount;++i)
    {
        if(getControlRectF(EContrlType(i)).contains(mapFromGlobal(QCursor::pos())))
        {
            state[i]          = Hover;
            break;
        }
    }
    state[CntCount] = Hover;
    update();
}

void CSpRealSlider::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(state[CntCount] != Pressed)
    {
        for(int i = 0;i<CntCount;++i)
        {
            state[i]          = Normal;
        }
        state[CntCount] = Normal;
        update();
    }
}

QSizeF CSpRealSlider::getControlSize(CSpRealSlider::EContrlType contrl)
{
    QSizeF ret;
    switch (contrl) {
    case SingleStepBtn_Begin:
    case SingleStepBtn_End:
    {
        ret = direction == Hor? QSizeF(_dpiInfo._singleStepBtnLen,_dpiInfo._singleStepBtnPtfLen<0?height():_dpiInfo._singleStepBtnPtfLen):QSizeF(_dpiInfo._singleStepBtnPtfLen<0?width():_dpiInfo._singleStepBtnPtfLen,_dpiInfo._singleStepBtnLen);
        break;
    }
    case Groove:
    {
        qreal offset = (_isBeginAndEndBtnExist?2*_dpiInfo._singleStepBtnLen:0);
        ret = direction == Hor? QSizeF(width() - offset,_dpiInfo._groovePtfLen<0?height():_dpiInfo._groovePtfLen):QSizeF(_dpiInfo._groovePtfLen<0?width():_dpiInfo._groovePtfLen,height() - offset);
        break;
    }
    case Slider:
    {
        ret = direction == Hor?QSizeF(calSliderLen(),_dpiInfo._sliderPtfLen<0?height():_dpiInfo._sliderPtfLen):QSizeF(_dpiInfo._sliderPtfLen<0?width():_dpiInfo._sliderPtfLen,calSliderLen());
        break;
    }
    default:
        ret = size();
        break;
    }
    return ret;
}

QPointF CSpRealSlider::getControlPos(CSpRealSlider::EContrlType contrl)
{
    QPointF ret;
    switch (contrl) {
    case SingleStepBtn_Begin:
    {
        if(_isBeginAndEndBtnExist)
        {
            ret = QPointF(0,0);
        }
        else
        {
            ret = QPointF(-_dpiInfo._singleStepBtnLen,-_dpiInfo._singleStepBtnLen);
        }
        break;
    }
    case Groove:
    {
        if(direction == Hor)
        {
            ret = QPointF(_isBeginAndEndBtnExist?_dpiInfo._singleStepBtnLen:0,0);
        }
        else
        {
            ret = QPointF(0,_isBeginAndEndBtnExist?_dpiInfo._singleStepBtnLen:0);
        }
        break;
    }
    case SingleStepBtn_End:
    {
        if(_isBeginAndEndBtnExist)
        {
            if(direction == Hor)
            {
                ret = QPointF(_dpiInfo._singleStepBtnLen + getControlSize(Groove).width(),0);
            }
            else
            {
                ret = QPointF(0,_dpiInfo._singleStepBtnLen + getControlSize(Groove).height());
            }
        }
        else
        {
            ret = QPointF(-_dpiInfo._singleStepBtnLen,-_dpiInfo._singleStepBtnLen);
        }
        break;
    }

    case Slider:
    {
//        qreal groveLen  = (direction == Hor ? getControlSize(Groove).width() : getControlSize(Groove).height());
//        qreal grovePos  = (direction == Hor ? getControlPos(Groove).x() : getControlPos(Groove).y());

//        qreal contexLen = groveLen - calSliderLen();

//        qreal radio = ((value() - min()) / (max() - min()));
        qreal sliderPos = calSliderPosBySliderValue(this->value())/*grovePos + radio * contexLen*/;
        ret = (direction == Hor? QPointF(sliderPos,0):QPointF(0,sliderPos));
        break;
    }
    default:
        ret = QPointF(0,0);
        break;
    }
    return ret;
}

QRectF CSpRealSlider::getControlRectF(EContrlType contrl)
{
    return QRectF(getControlPos(contrl),getControlSize(contrl));
}

qreal CSpRealSlider::calSliderLen()
{
    qreal groveLen  = (direction == Hor ? getControlSize(Groove).width() : getControlSize(Groove).height());
    if(!_isScrollBar)
    {
        return qMin(_dpiInfo._minSliderLen,groveLen);
    }

    //scrollbar 要根据pageStep计算出slider的长度
    // calculate slider length
    qreal maxlen    = groveLen;
    qreal sliderlen = maxlen;
    qreal range     = max() - min();
    if (!qFuzzyIsNull(range))
    {
        sliderlen = (pageStep() * maxlen) / (range + pageStep());

        qreal slidermin = _dpiInfo._minSliderLen;
        if (sliderlen < slidermin)
            sliderlen = slidermin;
        if (sliderlen > maxlen)
            sliderlen = maxlen;
    }
    return sliderlen;
}

qreal CSpRealSlider::calSliderValueBySliderPos(qreal sliderPos)
{
    qreal groveLen  = (direction == Hor ? getControlSize(Groove).width() : getControlSize(Groove).height());
    qreal grovePos  = (direction == Hor ? getControlPos(Groove).x() : getControlPos(Groove).y());

    qreal contexLen = groveLen - calSliderLen();

    qreal value = ((sliderPos - grovePos) / contexLen) * (max() - min()) + min();


    return value;
}

qreal CSpRealSlider::calSliderPosBySliderValue(qreal value)
{
    qreal groveLen  = (direction == Hor ? getControlSize(Groove).width() : getControlSize(Groove).height());
    qreal grovePos  = (direction == Hor ? getControlPos(Groove).x() : getControlPos(Groove).y());

    qreal contexLen = groveLen - calSliderLen();

    qreal radio = ((value - min()) / (max() - min()));
    qreal sliderPos = grovePos + radio * contexLen;

    return sliderPos;
}

void CSpRealSlider::updateDpi()
{
    _dpiInfo = _dpiBaseInfo*_dpi;
    update();
}

void CSpRealSlider::setValue_direction(qreal value)
{
    qreal oldValue = _value;
    _value =  qMax(qMin(value,max()),min());

    if(!qFuzzyIsNull(_value - oldValue))
    {
        emit valueChanged(_value);
    }

    update();
}

QPropertyAnimation *CSpRealSlider::getAnimation()
{
    if(_animationSetValue == nullptr)
    {
        _animationSetValue = new QPropertyAnimation(this,"valueAnimation");
        connect(_animationSetValue,&QPropertyAnimation::finished,this,[=](){
            _animationSetValue->deleteLater();
            _animationSetValue = nullptr;
        });
    }
    return _animationSetValue;
}
