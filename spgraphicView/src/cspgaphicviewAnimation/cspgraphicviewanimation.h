#ifndef CGRAPHICVIEWANIMATION_H
#define CGRAPHICVIEWANIMATION_H
#include "cspgraphicview.h"
class QParallelAnimationGroup;
class GRAPHICVIEW_EXPORT CSpGraphicViewAnimation:public CSpGraphicView
{
    Q_OBJECT
public:
    CSpGraphicViewAnimation(QWidget* parent = nullptr);

    ~CSpGraphicViewAnimation();

    /* 该函数肯定返回true */
    bool isAnimationView();

    /* 手动设置是否禁止动画(如果block传入true 那么直到被传入false之前都不会再产生动画) */
    void blockAnimations(bool block);

    /* 在需要动画的一系列操作前调用collectAnimalBegin以开始收集动画 */
    void collectChildAnimalBegin();

    /* 在需要动画的一系列操作前调用collectAnimalEnd以结束收集动画(随后会开始运行动画) */
    void collectChildAnimalEnd();

    /* 是否正在收集动画 */
    bool isCollectingChildAnimations();

    /* 添加动画 */
    void addChildAnimation(QPropertyAnimation* animation);

    /* 孩子动画个数 */
    int  childAnimationCount();

public slots:
    virtual void onChildAnimationsFinished();

private:
    QParallelAnimationGroup* getAnimations();
private:
    bool                     isCollectingAnimal = false;
    bool                     blocked            = false;
    QParallelAnimationGroup* animations         = nullptr;
};

#endif // CGRAPHICVIEWANIMATION_H
