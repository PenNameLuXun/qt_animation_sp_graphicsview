#include "cspgraphicviewanimation.h"
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

CSpGraphicViewAnimation::CSpGraphicViewAnimation(QWidget *parent):
    CSpGraphicView (parent)
{

}

CSpGraphicViewAnimation::~CSpGraphicViewAnimation()
{
}

bool CSpGraphicViewAnimation::isAnimationView()
{
    return true;
}

void CSpGraphicViewAnimation::blockAnimations(bool block)
{
    blocked = block;
}

void CSpGraphicViewAnimation::collectChildAnimalBegin()
{
    if(viewPort() != nullptr && viewPort()->isVisible() && !blocked)
    {
        isCollectingAnimal = true;
    }
}

void CSpGraphicViewAnimation::collectChildAnimalEnd()
{
    if(animations != nullptr && !blocked)
    {
        animations->start();
    }
    isCollectingAnimal = false;
}

bool CSpGraphicViewAnimation::isCollectingChildAnimations()
{
    return isCollectingAnimal && !blocked;
}

void CSpGraphicViewAnimation::addChildAnimation(QPropertyAnimation *animation)
{
    if(animation == nullptr)
        return;

    //动画个数或者参数有变化(调用addChildAnimation就证明有变化或者个数改变了) 那么先暂停 再在后面重新开始
    if(getAnimations()->state() == QPropertyAnimation::Running)
    {
        getAnimations()->pause();
        for(int i = 0;i<getAnimations()->animationCount();++i)
        {
            QPropertyAnimation* pAnimation = qobject_cast<QPropertyAnimation*>(getAnimations()->animationAt(i));
            if(pAnimation == animation)
                continue;
            if(pAnimation != nullptr && pAnimation->state() == QPropertyAnimation::Running)
            {
                pAnimation->setStartValue(pAnimation->currentValue());
                pAnimation->setDuration(animation->duration());
                pAnimation->setCurrentTime(0);
            }
        }
    }

    if(getAnimations()->indexOfAnimation(animation) == -1)
    getAnimations()->addAnimation(animation);
}

void CSpGraphicViewAnimation::onChildAnimationsFinished()
{
}

int CSpGraphicViewAnimation::childAnimationCount()
{
    if(animations == nullptr)
        return 0;

    return animations->animationCount();
}

QParallelAnimationGroup *CSpGraphicViewAnimation::getAnimations()
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
