#ifndef CGRAPHICVIEWANIMATIONITEM_H
#define CGRAPHICVIEWANIMATIONITEM_H
#include "cspgraphicviewitem.h"

class QParallelAnimationGroup;
class QPropertyAnimation;

class GRAPHICVIEW_EXPORT CSpGraphicViewAnimationItem:public CSpGraphicViewItem
{
    Q_OBJECT
    Q_PROPERTY(QRectF boundRectAnimation READ boundRect WRITE setBoundRectAnimation)
public:
    CSpGraphicViewAnimationItem(CSpGraphicViewItem* pFatherItem = nullptr);

    ~CSpGraphicViewAnimationItem();

    QRectF boundRect();

    void   setBoundRect(const QRectF& rct);

    /*单独的动画接口从begin到end*/
    void   setBoundRect_Animation(const QRectF& begin,const QRectF& end,const int duration = 300);

    /*如果正在动画中 那么返回end值如果不在动画中返回boundRect*/
    QRectF endBoundRect();

    /*手动设置是否禁止孩子的动画(如果block传入true 那么直到被传入false之前都不会再产生动画)*/
    void blockChildAnimations(bool block);

    /*在需要动画的一系列操作前调用collectAnimalBegin以开始收集动画*/
    void collectChildAnimalBegin();

    /*在需要动画的一系列操作前调用collectAnimalEnd以结束收集动画(随后会开始运行动画)*/
    void collectChildAnimalEnd();

    /*是否正在收集动画*/
    bool isCollectingChildAnimations();

    /*添加动画*/
    void addChildAnimation(QPropertyAnimation* animation);

    /*孩子动画个数*/
    int  childAnimationCount();

    /*自己动画的指针(提供给外部接口可以定制动画的类型)*/
    QPropertyAnimation* selfAnimation();

public slots:
    virtual void onChildAnimationsFinished();
    virtual void onSelfAnimationsFinished();

private:
    bool   isAnimationItem();
    QParallelAnimationGroup* getAnimations();
    QPropertyAnimation*      setAnimationTo(const QRectF &endValue);


protected:
    virtual QString testPrintSomething();
private:
    bool                     isCollectingAnimal   = false;
    bool                     blockedChild         = false;
    QParallelAnimationGroup* animations           = nullptr;

    QPropertyAnimation*      _animation           = nullptr;

private:
    void   setBoundRectAnimation(const QRectF& rct);

    friend class QPropertyAnimation;

protected:
    QRectF _rect;

};

#endif // CGRAPHICVIEWANIMATIONITEM_H
