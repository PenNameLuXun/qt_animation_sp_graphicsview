#include "cspgraphicviewIndexitem.h"
#include "cspgraphicviewlistwidget.h"
#include "cspgraphicview.h"
#include "cspgraphicstructmodel.h"
#include <QPainter>
#include <QDebug>

CSpGraphViewIndexItem::CSpGraphViewIndexItem(CSpGraphicViewItem* parentItem):
    CSpGraphicViewItem(parentItem)
{
    setDragAble(true);
}

CSpGraphViewIndexItem::~CSpGraphViewIndexItem()
{

}

int CSpGraphViewIndexItem::index()
{
    return _index;
}

QRectF CSpGraphViewIndexItem::boundRect()
{
    return _rct;
}

void CSpGraphViewIndexItem::setBoundRect(const QRectF &rctF)
{
    CResizeHelper helper(this);
    _rct = rctF;
}

void CSpGraphViewIndexItem::paintSelf(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo)
{
    painter->setBrush(QColor(255,0,0));

    if(isChecked())
    {
        painter->setBrush(QColor(255,0,255));

    }
    painter->drawRect(paintInfo->rectF);
    painter->drawText(selfBoundRect(),QString("index:%1").arg(index()));
}

void CSpGraphViewIndexItem::setBoundRectOpera(const QRectF &rct)
{
    _rectOperating = rct;

    if(isTopItem())
    {
        CSpGraphicView* pView = graphicView();
        if(pView != nullptr)
        {
            if(pView->graphType() == CSpGraphicView::ListView)
            {
                CSpGraphicViewListWidget* pListView = static_cast<CSpGraphicViewListWidget*>(pView);
                if(pListView->structModel()->model() == CSpGraphicStructModel::List_Model)
                {
                    _rectOperating = QRectF(QPointF(0,rct.top()),rct.size());
                }
                else if(pListView->structModel()->model() == CSpGraphicStructModel::Row_Model)
                {
                    _rectOperating = QRectF(QPointF(rct.left(),0),rct.size());
                }
            }

        }
    }
    else
    {
        CSpGraphicViewItem* pParentItem = parentItem();
        if(pParentItem != nullptr)
        {
            if(pParentItem->structModel() != nullptr)
            {
                QRectF parentGlBound = pParentItem->glBoundRect();
                if(pParentItem->structModel()->model() == CSpGraphicStructModel::List_Model)
                {
                    _rectOperating = QRectF(QPointF(parentGlBound.left(),rct.top()),rct.size());
                }
                else if(pParentItem->structModel()->model() == CSpGraphicStructModel::Row_Model)
                {
                    _rectOperating = QRectF(QPointF(rct.left(),parentGlBound.right()),rct.size());
                }
            }
        }
    }
}

void CSpGraphViewIndexItem::setIndexItemIndex(CSpGraphViewIndexItem *pItem, int index)
{
    pItem->_index = index;
}
