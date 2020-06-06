#include "cspgraphicviewanimationlistitem.h"
#include "cspgraphicstructmodel.h"
#include "cspgraphicviewitem.h"
#include "cspgraphicviewanimationindexitem.h"

CSpGraphicViewAnimationListItem::CSpGraphicViewAnimationListItem(CSpGraphicViewItem *pFatherItem):
    CSpGraphicViewAnimationItem(pFatherItem)
{
    _model = new CSpGraphicStructModel;
    connect(_model,&CSpGraphicStructModel::modelChanged,this,&CSpGraphicViewAnimationListItem::onModelChanged);
}
CSpGraphicStructModel *CSpGraphicViewAnimationListItem::structModel()
{
    return _model;
}

void CSpGraphicViewAnimationListItem::insertItem(CSpGraphicViewAnimationIndexItem *pItem, int index, bool refresh)
{
    if(index < 0)
    {
        index = _items.size();
    }
    else
    {
        if(index > _items.size())
        {
            index = _items.size();
        }
    }
    CSpGraphicViewAnimationIndexItem::setIndexItemIndex(pItem,index);

    CSpGraphicViewItem::addItem(pItem,refresh);
}

void CSpGraphicViewAnimationListItem::removeItem(CSpGraphicViewAnimationIndexItem *pItem, bool dele,bool refresh)
{
    CSpGraphicViewItem::removeItem(pItem,dele,refresh);
}

void CSpGraphicViewAnimationListItem::removeItem(int index, bool dele, bool refresh)
{
    if(index>=0 && index<_items.size())
    {
        CSpGraphicViewAnimationIndexItem* pItem = _items[index];

        removeItem(pItem,dele,refresh);
    }
}

void CSpGraphicViewAnimationListItem::onModelChanged()
{
    if(_model->model() == CSpGraphicStructModel::Icon_Model)
    {
        if(_model->model() == CSpGraphicStructModel::Icon_Model)
        {
            int h = _model->realHeight_IconModel(static_cast<int>(boundRect().width()),_items.size());

            setBoundRect(QRectF(boundRect().topLeft(),QSizeF(boundRect().width(),h)));
        }

        updateItemsStructureAndGeometry();

        if(graphicView() != nullptr)
        {
            if(graphicView()->structModel() != nullptr)
            {
                graphicView()->refreshStructAndGeometry();
                graphicView()->viewPort()->update();
            }
        }
    }
}

int CSpGraphicViewAnimationListItem::childrenCount()
{
    return _items.size();
}

CSpGraphicViewItem *CSpGraphicViewAnimationListItem::firstChild()
{
    if(_items.isEmpty())
        return nullptr;
    return _items.first();
}

CSpGraphicViewItem *CSpGraphicViewAnimationListItem::lastChild()
{
    if(_items.isEmpty())
        return nullptr;
    return _items.last();
}

CSpGraphicViewItem *CSpGraphicViewAnimationListItem::nextChild(CSpGraphicViewItem *pItem)
{
    if(_items.isEmpty())
        return nullptr;
    int index =  (static_cast<CSpGraphicViewAnimationIndexItem*>(pItem))->index();
    ++index;
    if(index >=0&&index<_items.size())
    {
        return _items[index];
    }
    return nullptr;
}

CSpGraphicViewItem *CSpGraphicViewAnimationListItem::prevChild(CSpGraphicViewItem *pItem)
{
    if(_items.isEmpty())
        return nullptr;
    int index =  (static_cast<CSpGraphicViewAnimationIndexItem*>(pItem))->index();
    --index;
    if(index >=0&&index<_items.size())
    {
        return _items[index];
    }
    return nullptr;
}

CSpGraphicViewItem *CSpGraphicViewAnimationListItem::ItemAt(const QPointF pos, QPointF *out)
{
    int indexB = 0;
    int indexE = _items.size() - 1;

    QRectF rctLocal = QRectF(pos,QSizeF(1,1));
    _model->getIndex(rctLocal,static_cast<int>(boundRect().width()),_items.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* pItem = _items[i];
        if(pItem->boundRect().contains(pos))
        {
            return pItem->ItemAt(pos - pItem->boundRect().topLeft(),out);
        }
    }

    if(out != nullptr)
    {
        *out = pos;
    }
    return this;
}

QSet<CSpGraphicViewItem *> CSpGraphicViewAnimationListItem::ItemAt(const QRectF &rctF)
{
    QSet<CSpGraphicViewItem *> setItems;
    int indexB = 0;
    int indexE = _items.size() - 1;

    _model->getIndex(rctF,static_cast<int>(boundRect().width()),_items.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* pItem = _items[i];
        QRectF boundRct  = pItem->boundRect();
        QRectF interRctF = boundRct.intersected(rctF);
        if(interRctF.isValid())
        {
            interRctF.translate(-boundRct.topLeft());
            setItems.unite(pItem->ItemAt(interRctF));
        }
    }
    if(setItems.isEmpty())
    {
        setItems.insert(this);
    }
    return setItems;
}

QList<CSpGraphicViewItem *> CSpGraphicViewAnimationListItem::allItemAt(const QPointF pos)
{
    QList<CSpGraphicViewItem *> items;
    int indexB = 0;
    int indexE = _items.size() - 1;

    QRectF rctLocal = QRectF(pos,QSizeF(1,1));
    _model->getIndex(rctLocal,static_cast<int>(boundRect().width()),_items.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* pItem = _items[i];
        QRectF boundRct = pItem->boundRect();
        if(boundRct.contains(pos))
        {
            items.append(pItem->allItemAt(pos - boundRct.topLeft()));
            items.append(pItem);
        }
    }
    return items;
}

int CSpGraphicViewAnimationListItem::itemsBoundingHeight()
{
    int h = 0;
    if(!_items.isEmpty())
    {
        if(_model->model() == CSpGraphicStructModel::Row_Model)
        {
            h = static_cast<int>(boundRect().height() + 0.5);
        }
        else
        {
            h = static_cast<int>(_items.last()->boundRect().bottom()+0.5);
        }
    }
    return h;
}

int CSpGraphicViewAnimationListItem::itemsBoundingWidth()
{
    int w = 0;
    if(!_items.isEmpty())
    {
        if(_model->model() == CSpGraphicStructModel::Row_Model)
        {
            w = static_cast<int>(_items.last()->boundRect().right() + 0.5);
        }
        else
        {
            w = static_cast<int>(boundRect().width() + 0.5);
        }
    }
    return w;
}

bool CSpGraphicViewAnimationListItem::addItem_virtual(CSpGraphicViewItem *pItem)
{
    CSpGraphicViewAnimationIndexItem* pIndexItem = static_cast<CSpGraphicViewAnimationIndexItem*>(pItem);

    _items.insert(pIndexItem->index(),pIndexItem);

    //_tempIndex = pIndexItem->index();

    return true;
}

bool CSpGraphicViewAnimationListItem::removeItem_virtual(CSpGraphicViewItem *pItem)
{
    CSpGraphicViewAnimationIndexItem* pIndexItem = static_cast<CSpGraphicViewAnimationIndexItem*>(pItem);

    int index = pIndexItem->index();

    bool result = true;

    if(index >= 0 && index<_items.size())
    {
        _items.removeAt(index);
    }
    else
    {
        result = _items.removeOne(pIndexItem);
        if(!result)
            qWarning()<<"delete  failed !!  == "<<metaObject()->className();
    }

    //_tempIndex = index;

    return true;
}

void CSpGraphicViewAnimationListItem::setChildrenGraphView(CSpGraphicView *pGv)
{
    for(int i = 0;i<_items.size();++i)
    {
        CSpGraphicViewItem* pItem = _items[i];
        s_setItemGraphView(pItem,pGv);
    }
}

void CSpGraphicViewAnimationListItem::removeAllChidren(bool dele)
{
    for(int i = 0;i<_items.size();++i)
    {
        CSpGraphicViewItem* pItem = _items[i];

        CSpGraphicViewChildEventInfo info;
        info.type = CSpGraphicViewChildEventInfo::Removed;
        info.pItem = pItem;
        info.pGraphViewWidget = _graphView;
        childRemovedEvent(&info);

        s_clearItemChildren(pItem,dele);

        s_clearItemMarks(pItem,dele);

        if(dele)
            pItem->deleteSelf();
    }
    _items.clear();
}

void CSpGraphicViewAnimationListItem::paintChildren(QPainter *painter,const QRectF &rctVisible)
{
    int indexB = 0;
    int indexE = _items.size() - 1;

    QRectF rct = graphicView()->viewportRectF();
    QRectF rctLocal = QRectF(mapFromGl(rct.topLeft()),rct.size());
    _model->getIndex(rctLocal,static_cast<int>(rct.width()),_items.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* pItem = _items[i];

        s_paintItem(pItem,painter,rctVisible);
    }
}

void CSpGraphicViewAnimationListItem::updateItemsStructureAndGeometry()
{
//    for(int i = 0;i<_items.size();++i)
//    {
//        CGraphicViewAnimationIndexItem* pItem = _items[i];

//        CGraphicViewAnimationIndexItem::setIndexItemIndex(pItem,i);
//    }
//    if(graphicView() != nullptr)
//    graphicView()->refreshStruct();

    collectChildAnimalBegin();
    if(_model->model() == CSpGraphicStructModel::List_Model)
    {
        int y = 0;
        for(int i = 0;i<_items.size();++i)
        {
            CSpGraphicViewAnimationIndexItem* pItem = _items[i];
            QRectF rect(0,y,boundRect().width(),pItem->boundRect().height());
            pItem->setBoundRect(rect);
            CSpGraphicViewAnimationIndexItem::setIndexItemIndex(pItem,i);
            y = static_cast<int>(rect.bottom()) + _model->spaceY();
        }
    }
    else if(_model->model() == CSpGraphicStructModel::Row_Model)
    {
        int x = 0;
        for(int i = 0;i<_items.size();++i)
        {
            CSpGraphicViewAnimationIndexItem* pItem = _items[i];
            QRectF rect(x,0,pItem->boundRect().width(),boundRect().height());
            pItem->setBoundRect(rect);
            CSpGraphicViewAnimationIndexItem::setIndexItemIndex(pItem,i);
            x = static_cast<int>(rect.right()) + _model->spaceX();
        }
    }
    else if(_model->model() == CSpGraphicStructModel::Icon_Model)
    {
        for(int i = 0;i<_items.size();++i)
        {
            CSpGraphicViewAnimationIndexItem* pItem = _items[i];
            CSpGraphicViewAnimationIndexItem::setIndexItemIndex(pItem,i);
            pItem->setBoundRect(_model->boundRectF_IconModel(boundRect().width(),i,_items.size()));
        }
    }
    collectChildAnimalEnd();
}
