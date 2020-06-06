#include "cspgraphicviewanimationindexitem.h"
#include "cspgraphicstructmodel.h"
#include "cspgraphicviewanimationlistwidget.h"

CSpGraphicViewAnimationIndexItem::CSpGraphicViewAnimationIndexItem(CSpGraphicViewItem *pFatherItem):
    CSpGraphicViewAnimationItem(pFatherItem)
{

}

CSpGraphicViewAnimationIndexItem::~CSpGraphicViewAnimationIndexItem()
{

}
int CSpGraphicViewAnimationIndexItem::index()
{
    return _index;
}

void CSpGraphicViewAnimationIndexItem::setBoundRectOpera(const QRectF &rct)
{
    _rectOperating = rct;

    if(isTopItem())
    {
        CSpGraphicView* pView = graphicView();
        if(pView != nullptr)
        {
            if(pView->graphType() == CSpGraphicView::ListView)
            {
                CSpGraphicViewAnimationListWidget* pListView = static_cast<CSpGraphicViewAnimationListWidget*>(pView);
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
//    else
//    {
//        CGraphicViewItem* pParentItem = parentItem();
//        if(pParentItem != nullptr)
//        {
//            if(pParentItem->structModel() != nullptr)
//            {
//                QRectF parentGlBound = pParentItem->glBoundRect();
//                if(pParentItem->structModel()->model() == CGraphicStructModel::List_Model)
//                {
//                    _rectOperating = QRectF(QPointF(parentGlBound.left(),rct.top()),rct.size());
//                }
//                else if(pParentItem->structModel()->model() == CGraphicStructModel::Row_Model)
//                {
//                    _rectOperating = QRectF(QPointF(rct.left(),parentGlBound.right()),rct.size());
//                }
//            }
//        }
//    }
}

void CSpGraphicViewAnimationIndexItem::setIndexItemIndex(CSpGraphicViewAnimationIndexItem *pItem, int index)
{
    pItem->_index = index;
}
