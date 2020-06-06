#ifndef CREALSLIDER_H
#define CREALSLIDER_H


#include <QWidget>
#include "spgraphicview_global.h"

class QPropertyAnimation;
class GRAPHICVIEW_EXPORT CSpRealSlider:public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal value READ value WRITE setValue)
public:
    enum EDirection{Hor,Ver};
    enum EContrlType{Slider,SingleStepBtn_Begin,SingleStepBtn_End,Groove,CntCount};
    enum EContrlState{Normal,Hover,Pressed,StateCount};
    enum EClickGrooveDo{DoPageStep,DoSetDirectValue,DoNothing};

    CSpRealSlider(QWidget *parent = Q_NULLPTR,EDirection dire = Ver);

    ~CSpRealSlider();

    /* 设置方向 */
    void  setDirection(EDirection dire);

    /* 当前值 */
    qreal value();

    /* 设置当前值 */
    Q_SLOT void  setValue(qreal value);

    /* 是否使value的设置animation化(默认是false) */
    void         enableAnimation(bool b);

    /* 设置当前值(带动画) */
    Q_SLOT void         setValue_Animation(qreal value,int duration = 300);

    /* 得到动画的管理指针 */
    QPropertyAnimation* getAnimation();

    /* 最小值 */
    qreal min();

    /* 设置最小值 */
    Q_SLOT void setMin(qreal min);

    /* 最大值 */
    qreal max();

    /* 设置最大值 */
    Q_SLOT void  setMax(qreal max);

    /* 设置范围 */
    Q_SLOT void  setRange(qreal min,qreal max);

    /* 单步按钮是否可见 */
    bool  isSingleStepBtnVisble();

    /* 设置单步按钮是否可见 */
    void  setSingleStepBtnVisble(bool b);

    /* 单步值 */
    qreal  singleStep();

    /* 设置单步值 */
    Q_SLOT void  setSingleStep(qreal step);

    /* 一页的值 */
    qreal pageStep();

    /* 设置一页的值 */
    Q_SLOT void  setPageStep(qreal pageStep);

    /* 设置点击groove时是执行pageStep移动还是直接设置被点击点所表示的value */
    void  setClickGroove(EClickGrooveDo type);

    /* 点击groove时是执行pageStep移动还是直接设置被点击点所表示的value */
    EClickGrooveDo clickGroove();

    /* 是否点击设置value的移动方式是动画方式 */
    bool           isClickSetValueAnimationWay();

    /* 设置点击设置value的移动方式是动画方式 */
    void           setClickSetValueAnimation(bool b);

    /* 设置为scrollbar  */
    void  setToScrollBar(bool b);

    /* 是否是scrollbar */
    bool  isScrollBar();

    /*---------------------- 显示相关 -----------------------*/

    /* 返回槽的ptf长度 */
    qreal getGroovePTFLen();

    /* 设置槽的ptf长度(ptf指垂直于父widget的方向的,比如当direction是水平时 垂直于水平就是 竖直方向 即为高度。 direction是竖直方向时 即为宽度) */
    void  setGroovePTFLen(qreal len);

    /* 单步按钮的长度 */
    qreal singleSteptnLen();

    /* 设置单步按钮的长度(和direction的方向一致) */
    void  setSingleStepBtnLen(qreal len);

    /* 返回单步按钮的ptf长度 */
    qreal singleStepBtnPTFLen();

    /* 设置单步按钮的ptf长度(ptf指垂直于父widget的方向的,比如当direction是水平时 垂直于水平就是 竖直方向 即为高度。 direction是竖直方向时 即为宽度) */
    void  setSingleStepBtnPTFLen(qreal len);

    /* 返回滑块的ptf长度 */
    qreal sliderPTFLen();

    /* 设置滑块的ptf长度(ptf指垂直于父widget的方向的,比如当direction是水平时 垂直于水平就是 竖直方向 即为高度。 direction是竖直方向时 即为宽度) */
    void  setSliderPTFLen(qreal l);

    /* 滑块的最小长度(和direction的方向一致) */
    qreal sliderMinLen();

    /* 设置滑块的最小长度 */
    void  setSliderMinLen(qreal len);

    /* dpi值 */
    qreal  dpi();

    /* 设置dpi */
    void   setDpi(qreal dpi);

signals:
    void valueChanged(qreal value);
    void rangeChanged(qreal min,qreal max);

protected:
    void paintEvent(QPaintEvent* event);

    virtual void paintBackground(QPainter* painter,const QRectF& rct,EContrlState state);
    virtual void paintBeginBtn(QPainter* painter,const QRectF& rct,EContrlState state);
    virtual void paintEndBtn(QPainter* painter,const QRectF& rct,EContrlState state);
    virtual void paintGroove(QPainter* painter,const QRectF& rct,EContrlState state);
    virtual void paintSlider(QPainter* painter,const QRectF& rct,EContrlState state);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    QSizeF  getControlSize(EContrlType contrl);
    QPointF getControlPos(EContrlType contrl);
    QRectF  getControlRectF(EContrlType contrl);

    /* 计算slider块的长度 */
    virtual qreal calSliderLen();

    /* 通过当前的位置计算出应该被设置到的值 */
    qreal calSliderValueBySliderPos(qreal sliderPos);
    qreal calSliderPosBySliderValue(qreal value);


    void  updateDpi();
private:
    qreal _value = 99;
    qreal _min   = 0;
    qreal _max   = 200;

    qreal _singleStep = 1;
    qreal _pageStep   = 10;

    Q_PROPERTY(qreal valueAnimation READ value WRITE setValue_direction)
    void                 setValue_direction(qreal value);
    bool                 _enableAnimation   = false;
    QPropertyAnimation*  _animationSetValue = nullptr;


    struct SDpiInfo{
        qreal _singleStepBtnLen    = 20;
        qreal _singleStepBtnPtfLen = -1;   //同_groovePtfLen解释

        qreal _groovePtfLen        = -1;   //水平时表示高度，-1表示和CRealScrollBar的高度一致 垂直时表示宽度，-1表示和CRealScrollBar的宽度一致

        qreal _minSliderLen        = 30;
        qreal _sliderPtfLen        = -1;   //同_groovePtfLen解释

        SDpiInfo operator *(qreal dpi) const
        {
            SDpiInfo ret(*this);
            ret._singleStepBtnLen*=dpi;
            ret._singleStepBtnPtfLen*=dpi;
            ret._groovePtfLen*=dpi;
            ret._minSliderLen*=dpi;
            ret._sliderPtfLen*=dpi;

            return ret;
        }
    };

    bool  _isBeginAndEndBtnExist = false;

    SDpiInfo _dpiBaseInfo;
    SDpiInfo _dpiInfo;

    EDirection direction;
    bool       _isScrollBar    = true;


    EContrlType  _pressedCntType = CntCount;
    QPointF      _pressedInitPos;
    QPointF      _pressedPos;
    QPointF      _pressedSliderPos;

    EContrlState state[CntCount+1] = {Normal,Normal,Normal,Normal,Normal};  //还有整个自身的state


    qreal        _dpi = 1.0;

    EClickGrooveDo _clickGrooveDo = DoPageStep;
    bool           _clickAnimationToSetValue = true;
};


#endif // CREALSLIDER_H
