#include "cspgraphicviewanimationitem.h"
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include "cspgraphicview.h"
#include "cspgraphicviewanimation.h"

CSpGraphicViewAnimationItem::CSpGraphicViewAnimationItem(CSpGraphicViewItem *pFatherItem):
    CSpGraphicViewItem (pFatherItem)
{

}

CSpGraphicViewAnimationItem::~CSpGraphicViewAnimationItem()
{

}

QRectF CSpGraphicViewAnimationItem::boundRect()
{
    return _rect;
}

void CSpGraphicViewAnimationItem::setBoundRect(const QRectF &rct)
{
    if(graphicView() != nullptr)
    {
        CSpGraphicView* pView = graphicView();
        if(pView->isVisible())
        {
            if(isGlItem())
            {
                if(pView->isAnimationView())
                {
                    CSpGraphicViewAnimation* pAnimationView = static_cast<CSpGraphicViewAnimation*>(pView);
                    if(pAnimationView->isCollectingChildAnimations())
                    {
                        pAnimationView->addChildAnimation(setAnimationTo(rct));
                        return;
                    }
                }
            }
            else
            {
                CSpGraphicViewItem* parentItem = this->parentItem();
                if(parentItem->isAnimationItem())
                {
                    CSpGraphicViewAnimationItem* pParentAnimationItem = static_cast<CSpGraphicViewAnimationItem*>(parentItem);
                    if(pParentAnimationItem->isCollectingChildAnimations())
                    {
                        pParentAnimationItem->addChildAnimation(setAnimationTo(rct));
                        return;
                    }
                }
            }
        }
    }
    if(_animation != nullptr)
    {_animation->setEndValue(rct);_animation->stop();}
    CResizeHelper helper(this);
    _rect = rct;
}

void CSpGraphicViewAnimationItem::setBoundRect_Animation(const QRectF &begin, const QRectF &end, const int duration)
{
    if(graphicView() == nullptr)
    {
        CResizeHelper helper(this);
        _rect = end;
        return;
    }

    CSpGraphicView* pView = graphicView();
    if(isGlItem())
    {
        if(pView->isAnimationView())
        {
            CSpGraphicViewAnimation* pAnimationView = static_cast<CSpGraphicViewAnimation*>(pView);
            pAnimationView->collectChildAnimalBegin();
            setBoundRect(end);
            if(_animation != nullptr)
            {
                _animation->setStartValue(begin);
                _animation->setDuration(duration);
            }
            pAnimationView->collectChildAnimalEnd();
        }
    }
    else
    {
        CSpGraphicViewItem* parentItem = this->parentItem();
        if(parentItem->isAnimationItem())
        {
            CSpGraphicViewAnimationItem* pParentAnimationItem = static_cast<CSpGraphicViewAnimationItem*>(parentItem);
            pParentAnimationItem->collectChildAnimalBegin();
            setBoundRect(end);
            if(_animation != nullptr)
            {
                _animation->setStartValue(begin);
                _animation->setDuration(duration);
            }
            pParentAnimationItem->collectChildAnimalEnd();
        }
    }
}

QRectF CSpGraphicViewAnimationItem::endBoundRect()
{
    if(_animation == nullptr)
        return boundRect();

    return _animation->endValue().toRectF();
}

void CSpGraphicViewAnimationItem::blockChildAnimations(bool block)
{
    blockedChild = block;
}

void CSpGraphicViewAnimationItem::setBoundRectAnimation(const QRectF &rct)
{
    //qDebug()<<"_rect ======= "<<_rect<<"rct ======= "<<rct<<"this = "<<this<<"testPrintSomething = "<<testPrintSomething();
    CResizeHelper helper(this);
    _rect = rct;
    if(graphicView() != nullptr)
    {
        graphicView()->viewPort()->update();
    }
}

QPropertyAnimation* CSpGraphicViewAnimationItem::setAnimationTo(const QRectF &endValue)
{
    if(_animation != nullptr)
    {
        if(_animation->endValue() == endValue)
        {
            return nullptr;
        }
    }
    else
    {
        if(boundRect() == endValue)
        {
            return nullptr;
        }
    }

    if(_animation == nullptr)
    {
        _animation = new QPropertyAnimation(this,"boundRectAnimation");
        _animation->setStartValue(boundRect());
        _animation->setEndValue(endValue);
        _animation->setDuration(300);
    }
    else
    {
        if(_animation->state() == QPropertyAnimation::Running)
        {
            if(_animation->endValue().toRectF() != endValue)
            {
                _animation->setStartValue(boundRect());
                _animation->setEndValue(endValue);
                _animation->setDuration(300);
                _animation->setCurrentTime(0);
            }
        }
        else
        {
            _animation->setStartValue(boundRect());
            _animation->setEndValue(endValue);
            _animation->setDuration(300);
            _animation->setCurrentTime(0);
        }
    }
    return _animation;
}

QString CSpGraphicViewAnimationItem::testPrintSomething()
{
    return QString();
}
void CSpGraphicViewAnimationItem::collectChildAnimalBegin(/*bool pauseIfRunning*/)
{
    if(graphicView() != nullptr)
    {
        if(graphicView()->viewPort() != nullptr && graphicView()->viewPort()->isVisible() && !blockedChild)
        {
            isCollectingAnimal = true;
//            if(pauseIfRunning && getAnimations()->state() == QParallelAnimationGroup::Running)
//            {
//                getAnimations()->pause();
//            }
        }
    }
}

void CSpGraphicViewAnimationItem::collectChildAnimalEnd()
{
    if(animations != nullptr)
    {
//        if(animations->state() == QParallelAnimationGroup::Paused)
//        {
//            if(!blockedChild)
//            animations->resume();
//        }
//        else
//        {
//            if(!blockedChild)
//            animations->start();
//        }
        if(!blockedChild)
        {
            if(animations->state() == QPropertyAnimation::Running)
            animations->setCurrentTime(0);

            //qDebug()<<"animations current time = "<<animations->currentTime();
            animations->start();
        }
    }
    isCollectingAnimal = false;
}

bool CSpGraphicViewAnimationItem::isCollectingChildAnimations()
{
    return isCollectingAnimal && !blockedChild;
}

void CSpGraphicViewAnimationItem::addChildAnimation(QPropertyAnimation *animation)
{
    if(animation == nullptr)
        return;


    //动画个数或者参数有变化(调用addChildAnimation就证明有变化或者个数改变了) 那么先暂停 再在后面重新开始
    if(getAnimations()->state() == QPropertyAnimation::Running)
    {
        //getAnimations()->pause();
        for(int i = 0;i<getAnimations()->animationCount();++i)
        {
            QPropertyAnimation* pAnimation = qobject_cast<QPropertyAnimation*>(getAnimations()->animationAt(i));
            if(pAnimation == animation)
                continue;
            if(pAnimation != nullptr && pAnimation->state() == QPropertyAnimation::Running)
            {
//                CGraphicViewItem* pRes = qobject_cast<CGraphicViewItem*>(pAnimation->targetObject());


                pAnimation->setStartValue(pAnimation->currentValue());
                pAnimation->setDuration(animation->duration());
                pAnimation->setCurrentTime(0);

//                if(pRes != nullptr)
//                {
//                    qDebug()<<"pRes name = "<<pRes->toolTip()<<"setStartBound = "<<pAnimation->startValue().toRectF();
//                }
            }
        }
    }

    if(getAnimations()->indexOfAnimation(animation) == -1)
    {
        getAnimations()->addAnimation(animation);
    }
}

int CSpGraphicViewAnimationItem::childAnimationCount()
{
    if(animations == nullptr)
        return 0;

    return animations->animationCount();
}

QPropertyAnimation *CSpGraphicViewAnimationItem::selfAnimation()
{
    return _animation;
}

void CSpGraphicViewAnimationItem::onChildAnimationsFinished()
{
    //qDebug()<<"CGraphicViewAnimationItem::onChildAnimationsFinished()---------";
}

void CSpGraphicViewAnimationItem::onSelfAnimationsFinished()
{

}

bool CSpGraphicViewAnimationItem::isAnimationItem()
{
    return true;
}

QParallelAnimationGroup *CSpGraphicViewAnimationItem::getAnimations()
{
    if(animations == nullptr)
    {
        animations = new QParallelAnimationGroup;
        connect(animations,&QParallelAnimationGroup::finished,this,[=]()
        {
            if(this->childAnimationCount() > 0)
            {
                while(animations->animationCount() > 0)
                {
                    animations->takeAnimation(0);
                }
                onChildAnimationsFinished();
            }
        });
    }
    return animations;
}
