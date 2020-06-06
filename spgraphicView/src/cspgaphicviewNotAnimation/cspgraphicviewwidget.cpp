#include "cspgraphicviewwidget.h"
#include "cspgraphicviewitem.h"
#include "cspgraphicviewmouseeventinfo.h"
#include <QGridLayout>
#include <QDebug>
#include <QWheelEvent>
#include <QPainter>
#include <QStyleOption>
CSpGraphicViewWidget::CSpGraphicViewWidget(QWidget *parent):
    CSpGraphicView(parent)
{
}

CSpGraphicViewWidget::~CSpGraphicViewWidget()
{

}

void CSpGraphicViewWidget::setBgColor(const QColor &color)
{
    _bgColor = color;
    viewPort()->update();
}

bool CSpGraphicViewWidget::addTopItem(CSpGraphicViewItem *pTopItem)
{
    _topItems.insert(pTopItem->z(),pTopItem);

    QRectF rctF = pTopItem->boundRect();

    if(_maxTopBottom < rctF.bottom())
        _maxTopBottom = rctF.bottom();


    if(_maxTopRight < rctF.right())
        _maxTopRight = rctF.right();

    return true;
}

bool CSpGraphicViewWidget::removeTopItem(CSpGraphicViewItem *pTopItem)
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

int CSpGraphicViewWidget::topCount()
{
    return _topItems.size();
}

int CSpGraphicViewWidget::dataCount()
{
    int count = 0;
    for(auto it = _topItems.begin();it != _topItems.end();++it)
    {
        CSpGraphicViewItem* p = it.value();

        count+=p->childrenCount();
    }
    return count;
}



int CSpGraphicViewWidget::virLogicScrollHeight()
{
    return static_cast<int>(_maxTopBottom);
}

int CSpGraphicViewWidget::virLogicScrollWidth()
{
    return static_cast<int>(_maxTopRight);
}

CSpGraphicViewItem *CSpGraphicViewWidget::itemAt(const QPointF &glPos, QPointF *out)
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

CSpGraphicViewItem *CSpGraphicViewWidget::topItemAt(const QPointF &glPos, QPointF *out)
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

QSet<CSpGraphicViewItem *> CSpGraphicViewWidget::itemAt(const QRectF &glRctF)
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

QList<CSpGraphicViewItem *> CSpGraphicViewWidget::allItemAt(const QPointF &glPos)
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

void CSpGraphicViewWidget::drawItems(QPainter *painter)
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

void CSpGraphicViewWidget::updateItemsStructureAndGeometry(bool isResize)
{
    Q_UNUSED(isResize);
}

void CSpGraphicViewWidget::drawBackground(QPainter *painter, const QSize &viewSize)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(_bgColor);
    painter->drawRect(QRect(QPoint(0,0),viewSize));
}
