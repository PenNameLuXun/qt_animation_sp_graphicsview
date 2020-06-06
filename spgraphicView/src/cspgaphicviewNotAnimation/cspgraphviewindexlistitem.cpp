#include "cspgraphviewindexlistitem.h"
#include "cspgraphicview.h"
#include <QDebug>

CSpGraphViewIndexListItem::CSpGraphViewIndexListItem(CSpGraphicViewItem *parentItem):
    CSpGraphViewIndexItem(parentItem)
{
    _model = new CSpGraphicStructModel;
    connect(_model,&CSpGraphicStructModel::modelChanged,this,&CSpGraphViewIndexListItem::onModelChanged);

    setResizeAble(true);
    setResizeType(Resize_Hor);
}

CSpGraphViewIndexListItem::~CSpGraphViewIndexListItem()
{

}

CSpGraphicStructModel *CSpGraphViewIndexListItem::structModel()
{
    return _model;
}

void CSpGraphViewIndexListItem::insertItem(CSpGraphViewIndexItem *pItem, int index, bool refresh)
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
    pItem->_index = index;

    CSpGraphicViewItem::addItem(pItem,refresh);
}

void CSpGraphViewIndexListItem::removeItem(CSpGraphViewIndexItem *pItem, bool dele,bool refresh)
{
    CSpGraphicViewItem::removeItem(pItem,dele,refresh);
}

void CSpGraphViewIndexListItem::removeItem(int index, bool dele,bool refresh)
{
    if(index>=0 && index<_items.size())
    {
        CSpGraphViewIndexItem* pItem = _items[index];

        removeItem(pItem,dele,refresh);
    }
}

void CSpGraphViewIndexListItem::onModelChanged()
{
    if(_model->model() == CSpGraphicStructModel::Icon_Model)
    {
        if(_model->model() == CSpGraphicStructModel::Icon_Model)
        {
            int h = _model->realHeight_IconModel(static_cast<int>(_rct.width()),_items.size());

            _rct  = QRectF(_rct.left(),_rct.top(),_rct.width(),h);
        }

        updateItemsStructureAndGeometry();

        if(graphicView() != nullptr)
        {
            if(graphicView()->structModel() != nullptr)
            {
                graphicView()->refresh();
                //graphicView()->viewPort()->update();
            }
        }
    }
}

int CSpGraphViewIndexListItem::childrenCount()
{
    return _items.size();
}

CSpGraphicViewItem *CSpGraphViewIndexListItem::ItemAt(const QPointF pos, QPointF *out)
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

QSet<CSpGraphicViewItem *> CSpGraphViewIndexListItem::ItemAt(const QRectF &rctF)
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

QList<CSpGraphicViewItem *> CSpGraphViewIndexListItem::allItemAt(const QPointF pos)
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

bool CSpGraphViewIndexListItem::addItem_virtual(CSpGraphicViewItem *pItem)
{
    CSpGraphViewIndexItem* pIndexItem = static_cast<CSpGraphViewIndexItem*>(pItem);

    _items.insert(pIndexItem->index(),pIndexItem);

    //_tempIndex = pIndexItem->index();

    return true;
}

bool CSpGraphViewIndexListItem::removeItem_virtual(CSpGraphicViewItem *pItem)
{
    CSpGraphViewIndexItem* pIndexItem = static_cast<CSpGraphViewIndexItem*>(pItem);

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

void CSpGraphViewIndexListItem::setChildrenGraphView(CSpGraphicView *pGv)
{
    for(int i = 0;i<_items.size();++i)
    {
        CSpGraphicViewItem* pItem = _items[i];
        s_setItemGraphView(pItem,pGv);
    }
}
CSpGraphicViewItem *CSpGraphViewIndexListItem::firstChild()
{
    if(_items.isEmpty())
        return nullptr;
    return _items.first();
}

CSpGraphicViewItem *CSpGraphViewIndexListItem::lastChild()
{
    if(_items.isEmpty())
        return nullptr;
    return _items.last();
}

CSpGraphicViewItem *CSpGraphViewIndexListItem::nextChild(CSpGraphicViewItem *pItem)
{
    if(_items.isEmpty())
        return nullptr;
    int index =  (static_cast<CSpGraphViewIndexItem*>(pItem))->index();
    ++index;
    if(index >=0&&index<_items.size())
    {
        return _items[index];
    }
    return nullptr;
}

CSpGraphicViewItem *CSpGraphViewIndexListItem::prevChild(CSpGraphicViewItem *pItem)
{
    if(_items.isEmpty())
        return nullptr;
    int index =  (static_cast<CSpGraphViewIndexItem*>(pItem))->index();
    --index;
    if(index >=0&&index<_items.size())
    {
        return _items[index];
    }
    return nullptr;
}
void CSpGraphViewIndexListItem::removeAllChidren(bool dele)
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

void CSpGraphViewIndexListItem::paintChildren(QPainter *painter,const QRectF &rctVisible)
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

void CSpGraphViewIndexListItem::updateItemsStructureAndGeometry()
{
    if(_model->model() == CSpGraphicStructModel::List_Model)
    {
        int y = 0;
        for(int i = 0;i<_items.size();++i)
        {
            CSpGraphViewIndexItem* pItem = _items[i];
            QRectF rect(0,y,boundRect().width(),pItem->boundRect().height());
            pItem->setBoundRect(rect);
            pItem->_index = i;
            y = static_cast<int>(rect.bottom()) + _model->spaceY();
        }
    }
    else if(_model->model() == CSpGraphicStructModel::Row_Model)
    {
        int x = 0;
        for(int i = 0;i<_items.size();++i)
        {
            CSpGraphViewIndexItem* pItem = _items[i];
            QRectF rect(x,0,pItem->boundRect().width(),boundRect().height());
            pItem->setBoundRect(rect);
            pItem->_index = i;
            x = static_cast<int>(rect.right()) + _model->spaceX();
        }
    }
    else if(_model->model() == CSpGraphicStructModel::Icon_Model)
    {
        for(int i = 0;i<_items.size();++i)
        {
            CSpGraphViewIndexItem* pItem = _items[i];
            pItem->_index = i;
            pItem->setBoundRect(_model->boundRectF_IconModel(boundRect().width(),i,_items.size()));
        }
    }

//    if(graphicView() != nullptr)
//    graphicView()->refreshStruct();


//    for(int i = 0;i<_items.size();++i)
//    {
//        CGraphViewIndexItem* pItem = _items[i];
//        pItem->_index = i;
//    }

//    if(graphicView() != nullptr)
//    graphicView()->refreshStruct();
}
