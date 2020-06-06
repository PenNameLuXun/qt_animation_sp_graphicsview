#include "cspgraphicviewlistwidget.h"
#include "cspgraphicviewitem.h"
#include "cspgraphicviewindexitem.h"

CSpGraphicViewListWidget::CSpGraphicViewListWidget(QWidget *parent):
    CSpGraphicView (parent)
{
    _type = ListView;

    _model.setModel(CSpGraphicStructModel::List_Model);
    _model.setSpaceY(0);

    connect(&_model,&CSpGraphicStructModel::modelChanged,this,[=](){
        refresh();
    });

}

CSpGraphicViewListWidget::~CSpGraphicViewListWidget()
{

}

CSpGraphicStructModel *CSpGraphicViewListWidget::structModel()
{
    return &_model;
}

void CSpGraphicViewListWidget::clear()
{
    clearAllChecked();
    for(int i = 0;i<_topItemList.size();++i)
    {
        CSpGraphViewIndexItem* pItem = _topItemList[i];
        pItem->deleteSelf();
    }
    _topItemList.clear();

    updateItemsStructureAndGeometry_helper();
}
void CSpGraphicViewListWidget::insertItem(CSpGraphViewIndexItem *pItem, int index, bool refresh)
{
    if(index < 0)
    {
        index = _topItemList.size();
    }
    else
    {
        if(index > _topItemList.size())
        {
            index = _topItemList.size();
        }
    }
    pItem->_index = index;

    CSpGraphicView::addItem(pItem,refresh);
}

void CSpGraphicViewListWidget::removeItem(CSpGraphViewIndexItem *pItem, bool dele, bool refresh)
{
    CSpGraphicView::removeItem(pItem,dele,refresh);
}

void CSpGraphicViewListWidget::removeItem(int index, bool dele, bool refresh)
{
    if(index>=0 && index < _topItemList.size())
    {
        CSpGraphicViewItem* pItem = _topItemList[index];
        CSpGraphicView::removeItem(pItem,dele,refresh);
    }
}

bool CSpGraphicViewListWidget::addTopItem_virtual(CSpGraphicViewItem *pTopItem)
{
    CSpGraphViewIndexItem* pListItem = static_cast<CSpGraphViewIndexItem*>(pTopItem);
    _topItemList.insert(pListItem->index(),pListItem);

    _ref_index_b = pListItem->index();
    _ref_index_e = -1;

    return true;
}

bool CSpGraphicViewListWidget::removeTopItem_virtual(CSpGraphicViewItem *pTopItem)
{
    CSpGraphViewIndexItem* pListItem = static_cast<CSpGraphViewIndexItem*>(pTopItem);
    int index = pListItem->index();
    if(index != -1)
    {
        _topItemList.removeAt(index);

        _ref_index_b = index;
        _ref_index_e = -1;

        return true;
    }
    return false;
}

int CSpGraphicViewListWidget::topCount()
{
    return _topItemList.size();
}

int CSpGraphicViewListWidget::dataCount()
{
    int count = 0;
    for(int i = 0;i<_topItemList.size();++i)
    {
        count += _topItemList[i]->childrenCount();
    }
    return count;
}

int CSpGraphicViewListWidget::virLogicScrollHeight()
{
    int h = 0;
    if(!_topItemList.isEmpty())
    {
        if(_model.model() == CSpGraphicStructModel::List_Model)
        {
            h = static_cast<int>(_topItemList.last()->boundRect().bottom());
        }
        else if(_model.model() == CSpGraphicStructModel::Icon_Model)
        {
            h = static_cast<int>(_topItemList.last()->boundRect().bottom());
        }
        else if(_model.model() == CSpGraphicStructModel::Row_Model)
        {
            h = viewPort()->height();
        }
    }
    return h;
}

int CSpGraphicViewListWidget::virLogicScrollWidth()
{
    int w = 0;
    if(!_topItemList.isEmpty())
    {
        if(_model.model() == CSpGraphicStructModel::List_Model)
        {
            w = viewPort()->width();
        }
        else if(_model.model() == CSpGraphicStructModel::Icon_Model)
        {
            w = viewPort()->width();
        }
        else if(_model.model() == CSpGraphicStructModel::Row_Model)
        {
            w = static_cast<int>(_topItemList.last()->boundRect().right());
        }
    }
    return w;
}
bool CSpGraphicViewListWidget::posInBoundRectF(const QPointF &glPos, const QRectF &boundRctF)
{
    if(_model.model() == CSpGraphicStructModel::List_Model)
    {
        if((glPos.y()>=boundRctF.top() && glPos.y() < boundRctF.bottom()))
        {
            return true;
        }
        return false;
    }
    else if(_model.model() == CSpGraphicStructModel::Icon_Model)
    {
        if(boundRctF.contains(glPos))
        {
            return true;
        }
        return false;
    }
    else if(_model.model() == CSpGraphicStructModel::Row_Model)
    {
        if((glPos.x()>=boundRctF.left() && glPos.x() < boundRctF.right()))
        {
            return true;
        }
        return false;
    }

    return false;
}
CSpGraphicViewItem *CSpGraphicViewListWidget::itemAt(const QPointF &glPos, QPointF *out)
{
    CSpGraphicViewItem* ret = nullptr;

    int indexB = 0;
    int indexE = _topItemList.size() - 1;

    QRectF rctLocal = QRectF(glPos,QSizeF(1,1));
    _model.getIndex(rctLocal,viewPort()->width(),_topItemList.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* p = _topItemList[i];
        QRectF         rctF = p->boundRect();
        if(posInBoundRectF(glPos,rctF))
        {
            return p->ItemAt(QPointF(glPos) - rctF.topLeft(),out);
        }
    }

    return ret;
}

CSpGraphicViewItem *CSpGraphicViewListWidget::topItemAt(const QPointF &glPos, QPointF *out)
{
    int indexB = 0;
    int indexE = _topItemList.size() - 1;

    QRectF rctLocal = QRectF(glPos,QSizeF(1,1));
    _model.getIndex(rctLocal,viewPort()->width(),_topItemList.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* p = _topItemList[i];
        QRectF         rctF = p->boundRect();
        if(posInBoundRectF(glPos,rctF))
        {
            if(out != nullptr)
            {
                *out = (glPos - rctF.topLeft());
            }
            return p;
        }
    }
    return nullptr;
}

QSet<CSpGraphicViewItem *> CSpGraphicViewListWidget::itemAt(const QRectF &glRctF)
{
    QSet<CSpGraphicViewItem *> setItems;

    int indexB = 0;
    int indexE = _topItemList.size() - 1;

    _model.getIndex(glRctF,viewPort()->width(),_topItemList.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* p = _topItemList[i];
        QRectF         rctF = p->boundRect();
        QRectF insterRctF = rctF.intersected(glRctF);
        if(insterRctF.isValid())
        {
            insterRctF.translate(-rctF.topLeft());
            setItems.unite(p->ItemAt(insterRctF));
        }
    }
    return setItems;
}

QList<CSpGraphicViewItem *> CSpGraphicViewListWidget::allItemAt(const QPointF &glPos)
{
    QList<CSpGraphicViewItem *> items;

    int indexB = 0;
    int indexE = _topItemList.size() - 1;

    QRectF rctLocal = QRectF(glPos,QSizeF(1,1));
    _model.getIndex(rctLocal,viewPort()->width(),_topItemList.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* p = _topItemList[i];
        QRectF         rctF = p->boundRect();
        if(posInBoundRectF(glPos,rctF))
        {
            items.append(p->allItemAt(QPointF(glPos) - rctF.topLeft()));
            items.append(p);
        }
    }
    return items;
}

void CSpGraphicViewListWidget::drawItems(QPainter *painter)
{
    int indexB = 0;
    int indexE = _topItemList.size() - 1;
    QRectF rctLocal = viewportRectF();
    _model.getIndex(rctLocal,static_cast<int>(rctLocal.width()),_topItemList.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* p = _topItemList[i];
        paintItem(p,painter,viewportRectF(),Normal_Paint);
    }
}

void CSpGraphicViewListWidget::updateItemsStructureAndGeometry(bool isResize)
{
    if(!isResize)
    {
        updateItemsStructureAndGeometry_helper(_ref_index_b,_ref_index_e);
    }
    else
    {
        updateItemsStructureAndGeometry_helper();
    }
}

void CSpGraphicViewListWidget::updateItemsStructureAndGeometry_helper(int indexB, int indexE)
{
    if(indexE == -1)
        indexE = _topItemList.size() - 1;

    if(_model.model() == CSpGraphicStructModel::List_Model)
    {
        int lastIndexB = indexB - 1;
        int y = 0;
        if(lastIndexB>=0 && lastIndexB<_topItemList.size())
        {
            y = static_cast<int>(_topItemList[lastIndexB]->boundRect().bottom()) + _model.spaceY();
        }
        for(int i = indexB;i<=indexE;++i)
        {
            CSpGraphViewIndexItem* pItem = _topItemList[i];
            QRectF rectf = QRectF(0,y,viewPort()->width(),pItem->boundRect().height());
            pItem->setBoundRect(rectf);
            pItem->_index = i;
            y = static_cast<int>(rectf.bottom()) + _model.spaceY();
        }
    }
    else if(_model.model() == CSpGraphicStructModel::Row_Model)
    {
        int lastIndexB = indexB - 1;
        int x = 0;
        if(lastIndexB>=0 && lastIndexB<_topItemList.size())
        {
            x = static_cast<int>(_topItemList[lastIndexB]->boundRect().right()) + _model.spaceX();
        }
        for(int i = indexB;i<=indexE;++i)
        {
            CSpGraphViewIndexItem* pItem = _topItemList[i];
            QRectF rectf = QRectF(x,0,pItem->boundRect().width(),viewPort()->height());
            pItem->setBoundRect(rectf);
            pItem->_index = i;
            x = static_cast<int>(rectf.right()) + _model.spaceX();
        }
    }
    else if(_model.model() == CSpGraphicStructModel::Icon_Model)
    {
        for(int i = 0;i<_topItemList.size();++i)
        {
            CSpGraphViewIndexItem* pItem = _topItemList[i];
            pItem->setBoundRect(_model.boundRectF_IconModel(viewPort()->width(),i,_topItemList.size()));
        }
    }

    _ref_index_b = 0;
    _ref_index_e = -1;
}

CSpGraphViewIndexItem *CSpGraphicViewListWidget::item(int index)
{
    if(index < 0 || index >= _topItemList.size())
        return nullptr;

    return _topItemList[index];
}
