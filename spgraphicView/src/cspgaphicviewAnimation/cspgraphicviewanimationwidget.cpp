#include "cspgraphicviewanimationwidget.h"
#include "cspgraphicviewitem.h"
#include "cspgraphicviewmouseeventinfo.h"
#include <QGridLayout>
#include <QDebug>
#include <QWheelEvent>
#include <QPainter>
#include <QStyleOption>

CSpGraphicViewAnimationWidget::CSpGraphicViewAnimationWidget(QWidget *parent):
    CSpGraphicViewAnimation(parent)
{

}

CSpGraphicViewAnimationWidget::~CSpGraphicViewAnimationWidget()
{

}
void CSpGraphicViewAnimationWidget::setBgColor(const QColor &color)
{
    _bgColor = color;
    viewPort()->update();
}

void CSpGraphicViewAnimationWidget::addItem(CSpGraphicViewItem *pItem, bool refresh)
{
    CSpGraphicView::addItem(pItem,refresh);
}

void CSpGraphicViewAnimationWidget::removeItem(CSpGraphicViewItem *pTopItem, bool dele, bool refresh)
{
    CSpGraphicView::removeItem(pTopItem,dele,refresh);
}
bool CSpGraphicViewAnimationWidget::addTopItem_virtual(CSpGraphicViewItem *pTopItem)
{
    _topItems.insert(pTopItem->z(),pTopItem);

    QRectF rctF = pTopItem->boundRect();

    if(_maxTopBottom < rctF.bottom())
        _maxTopBottom = rctF.bottom();


    if(_maxTopRight < rctF.right())
        _maxTopRight = rctF.right();

    return true;
}

bool CSpGraphicViewAnimationWidget::removeTopItem_virtual(CSpGraphicViewItem *pTopItem)
{
    auto it = _topItems.find(pTopItem->z(),pTopItem);
    if(it != _topItems.end())
    {
        _topItems.erase(it);

        qreal maxBottom = 0;
        qreal maxRight  = 0;
        for(auto it = _topItems.begin();it != _topItems.end();++it)
        {
            QRectF boundRct = it.value()->boundRect();
            if(boundRct.bottom() > maxBottom)
                maxBottom = boundRct.bottom();

            if(boundRct.right() > maxRight)
                maxRight = boundRct.right();
        }
        _maxTopBottom = maxBottom;
        _maxTopRight  = maxRight;


        return true;
    }
    return false;
}

int CSpGraphicViewAnimationWidget::topCount()
{
    return _topItems.size();
}

int CSpGraphicViewAnimationWidget::dataCount()
{
    int count = 0;
    for(auto it = _topItems.begin();it != _topItems.end();++it)
    {
        CSpGraphicViewItem* p = it.value();

        count+=p->childrenCount();
    }
    return count;
}



int CSpGraphicViewAnimationWidget::virLogicScrollHeight()
{
    return static_cast<int>(_maxTopBottom);
}

int CSpGraphicViewAnimationWidget::virLogicScrollWidth()
{
    return static_cast<int>(_maxTopRight);
}

CSpGraphicViewItem *CSpGraphicViewAnimationWidget::itemAt(const QPointF &glPos, QPointF *out)
{
    if(!_topItems.isEmpty())
    {
        for(auto it = _topItems.end() - 1;it != _topItems.begin() - 1;--it)
        {
            CSpGraphicViewItem* pTop = it.value();
            QRectF boundRct = pTop->boundRect();
            if(boundRct.contains(glPos))
            {
                return pTop->ItemAt(glPos - boundRct.topLeft(),out);
            }
        }
    }
    return nullptr;
}

CSpGraphicViewItem *CSpGraphicViewAnimationWidget::topItemAt(const QPointF &glPos, QPointF *out)
{
    if(!_topItems.isEmpty())
    {
        for(auto it = _topItems.end() - 1;it != _topItems.begin() - 1;--it)
        {
            CSpGraphicViewItem* pTop = it.value();
            QRectF boundRct = pTop->boundRect();
            if(boundRct.contains(glPos))
            {
                if(out != nullptr)
                {
                    *out = (glPos - boundRct.topLeft());
                }
                return pTop;
            }
        }
    }
    return nullptr;
}

QSet<CSpGraphicViewItem *> CSpGraphicViewAnimationWidget::itemAt(const QRectF &glRctF)
{

    QSet<CSpGraphicViewItem *> items;

    if(!_topItems.isEmpty())
    {
        for(auto it = _topItems.end() - 1;it != _topItems.begin() - 1;--it)
        {
            CSpGraphicViewItem* pTop = it.value();
            QRectF boundRct = pTop->boundRect();
            QRectF insterRctF = boundRct.intersected(glRctF);
            if(insterRctF.isValid())
            {
                insterRctF.translate(-boundRct.topLeft());
                items.unite(pTop->ItemAt(insterRctF));
            }
        }
    }

    return items;
}

QList<CSpGraphicViewItem *> CSpGraphicViewAnimationWidget::allItemAt(const QPointF &glPos)
{


    QList<CSpGraphicViewItem *> items;

    if(!_topItems.isEmpty())
    {
        for(auto it = _topItems.end() - 1;it != _topItems.begin() - 1;--it)
        {
            CSpGraphicViewItem* pTop = it.value();
            QRectF boundRct = pTop->boundRect();
            if(boundRct.contains(glPos))
            {
                items.append(pTop->allItemAt(glPos - boundRct.topLeft()));
                items.append(pTop);
            }
        }
    }

    return items;
}

void CSpGraphicViewAnimationWidget::drawItems(QPainter *painter)
{
    if(!_topItems.isEmpty())
    {
        for(auto it = _topItems.end() - 1;it != _topItems.begin() - 1;--it)
        {
            CSpGraphicViewItem* pTopItem = it.value();
            paintItem(pTopItem,painter,viewportRectF());
        }
    }
}

void CSpGraphicViewAnimationWidget::updateItemsStructureAndGeometry(bool isResize)
{
    Q_UNUSED(isResize)
}

void CSpGraphicViewAnimationWidget::drawBackground(QPainter *painter, const QSize &viewSize)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(_bgColor);
    painter->drawRect(QRect(QPoint(0,0),viewSize));
}
