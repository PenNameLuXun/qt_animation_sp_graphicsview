#include "cspgraphicviewlistitem.h"
#include "cspgraphicviewlistwidget.h"
#include "cspgraphicview.h"
#include "cspgraphicviewindexitem.h"
#include <QPainter>
#include <QDebug>

CSpGraphicViewListItem::CSpGraphicViewListItem(CSpGraphicViewItem *parentItem):
    CSpGraphicViewItem(parentItem)
{
    _model = new CSpGraphicStructModel;
    connect(_model,&CSpGraphicStructModel::modelChanged,this,&CSpGraphicViewListItem::onModelChanged);
}

CSpGraphicViewListItem::~CSpGraphicViewListItem()
{

}

CSpGraphicStructModel *CSpGraphicViewListItem::structModel()
{
    return _model;
}

void CSpGraphicViewListItem::insertItem(CSpGraphViewIndexItem *pItem, int index, bool refresh)
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

void CSpGraphicViewListItem::removeItem(CSpGraphViewIndexItem *pItem, bool dele,bool refresh)
{
    CSpGraphicViewItem::removeItem(pItem,dele,refresh);
}

void CSpGraphicViewListItem::removeItem(int index, bool dele, bool refresh)
{
    if(index>=0 && index<_items.size())
    {
        CSpGraphViewIndexItem* pItem = _items[index];

        removeItem(pItem,dele,refresh);
    }
}

void CSpGraphicViewListItem::onModelChanged()
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
                graphicView()->refresh();
                graphicView()->viewPort()->update();
            }
        }
    }
}

int CSpGraphicViewListItem::childrenCount()
{
    return _items.size();
}

CSpGraphicViewItem *CSpGraphicViewListItem::firstChild()
{
    if(_items.isEmpty())
        return nullptr;
    return _items.first();
}

CSpGraphicViewItem *CSpGraphicViewListItem::lastChild()
{
    if(_items.isEmpty())
        return nullptr;
    return _items.last();
}

CSpGraphicViewItem *CSpGraphicViewListItem::nextChild(CSpGraphicViewItem *pItem)
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

CSpGraphicViewItem *CSpGraphicViewListItem::prevChild(CSpGraphicViewItem *pItem)
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

CSpGraphicViewItem *CSpGraphicViewListItem::ItemAt(const QPointF pos, QPointF *out)
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

QSet<CSpGraphicViewItem *> CSpGraphicViewListItem::ItemAt(const QRectF &rctF)
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

QList<CSpGraphicViewItem *> CSpGraphicViewListItem::allItemAt(const QPointF pos)
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

int CSpGraphicViewListItem::itemsBoundingHeight()
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

int CSpGraphicViewListItem::itemsBoundingWidth()
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

bool CSpGraphicViewListItem::addItem_virtual(CSpGraphicViewItem *pItem)
{
    CSpGraphViewIndexItem* pIndexItem = static_cast<CSpGraphViewIndexItem*>(pItem);

    _items.insert(pIndexItem->index(),pIndexItem);

    //_tempIndex = pIndexItem->index();

    return true;
}

bool CSpGraphicViewListItem::removeItem_virtual(CSpGraphicViewItem *pItem)
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

void CSpGraphicViewListItem::setChildrenGraphView(CSpGraphicView *pGv)
{
    for(int i = 0;i<_items.size();++i)
    {
        CSpGraphicViewItem* pItem = _items[i];
        s_setItemGraphView(pItem,pGv);
    }
}

void CSpGraphicViewListItem::removeAllChidren(bool dele)
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

void CSpGraphicViewListItem::paintChildren(QPainter *painter,const QRectF &rctVisible)
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

void CSpGraphicViewListItem::updateItemsStructureAndGeometry()
{
    for(int i = 0;i<_items.size();++i)
    {
        CSpGraphViewIndexItem* pItem = _items[i];

        pItem->_index = i;
    }
    if(graphicView() != nullptr)
    graphicView()->refresh();
}



