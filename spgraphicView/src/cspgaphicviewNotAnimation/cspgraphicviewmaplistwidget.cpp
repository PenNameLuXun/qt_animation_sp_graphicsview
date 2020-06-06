#include "cspgraphicviewmaplistwidget.h"
#include "cspgraphicviewitem.h"
#include "cspgraphicviewindexitem.h"
#include "cspgraphicviewmouseeventinfo.h"
#include <QGridLayout>
#include <QDebug>
#include <QWheelEvent>
#include <QPainter>
CSpGraphicViewMapListWidget::CSpGraphicViewMapListWidget(QWidget *parent):
    CSpGraphicViewListWidget(parent)
{
    //setModel(List_Model);
}

CSpGraphicViewMapListWidget::~CSpGraphicViewMapListWidget()
{

}

int CSpGraphicViewMapListWidget::topCount()
{
    return _topItemBottomMap.size();
}

int CSpGraphicViewMapListWidget::dataCount()
{
    int count = 0;
    for(auto it = _topItemBottomMap.begin();it != _topItemBottomMap.end();++it)
    {
        CSpGraphicViewItem* p = it.value();

        count+=p->childrenCount();
    }
    return count;
}



int CSpGraphicViewMapListWidget::virLogicScrollHeight()
{
    int h = 0;
    if(!_topItemBottomMap.isEmpty())
    {
        if(_model.model() == CSpGraphicStructModel::Row_Model)
        {
            h = viewPort()->height();
        }
        else
        {
            h = _topItemBottomMap.lastKey();
        }
    }

    return h;
}

int CSpGraphicViewMapListWidget::virLogicScrollWidth()
{
    int w = 0;
    if(!_topItemBottomMap.isEmpty())
    {
        if(_model.model() == CSpGraphicStructModel::Row_Model)
        {
            return _topItemBottomMap.lastKey();
        }
        else
        {
            w = viewPort()->width();
        }
    }
    return w;
}

CSpGraphicViewItem *CSpGraphicViewMapListWidget::itemAt(const QPointF &glPos, QPointF *out)
{
    int bS = static_cast<int>(glPos.y());
    if(_model.model() == CSpGraphicStructModel::Icon_Model)
    {
        auto itb = _topItemBottomMap.upperBound(bS - _model.iconItemSz().height());
        auto ite = _topItemBottomMap.upperBound(bS + _model.iconItemSz().height());

        for(auto it = itb;it != ite;++it)
        {
            CSpGraphicViewItem* pTopItem = it.value();
            if(posInBoundRectF(glPos,pTopItem->boundRect()))
            {
                return pTopItem->ItemAt(QPointF(glPos) - pTopItem->boundRect().topLeft(),out);
            }
        }
    }
    else if(_model.model() == CSpGraphicStructModel::Row_Model)
    {
        bS = static_cast<int>(glPos.x());
    }
    auto it = _topItemBottomMap.upperBound(bS);
    if(it != _topItemBottomMap.end())
    {
        if(posInBoundRectF(glPos,it.value()->boundRect()))
        {
            return it.value()->ItemAt(QPointF(glPos) - it.value()->boundRect().topLeft(),out);
        }
    }
    return nullptr;
}

CSpGraphicViewItem *CSpGraphicViewMapListWidget::topItemAt(const QPointF &glPos, QPointF *out)
{
    int bS = static_cast<int>(glPos.y());
    if(_model.model() == CSpGraphicStructModel::Icon_Model)
    {
        auto itb = _topItemBottomMap.upperBound(bS - _model.iconItemSz().height());
        auto ite = _topItemBottomMap.upperBound(bS + _model.iconItemSz().height());

        for(auto it = itb;it != ite;++it)
        {
            CSpGraphicViewItem* pTopItem = it.value();
            if(posInBoundRectF(glPos,pTopItem->boundRect()))
            {
                if(out != nullptr)
                {
                    *out = (glPos - pTopItem->boundRect().topLeft());
                }
                return pTopItem;
            }
        }
    }
    else if(_model.model() == CSpGraphicStructModel::Row_Model)
    {
        bS = static_cast<int>(glPos.x());
    }
    auto it = _topItemBottomMap.lowerBound(bS);
    if(it != _topItemBottomMap.end())
    {
        if(posInBoundRectF(glPos,it.value()->boundRect()))
        {
            if(out != nullptr)
            {
                *out = (glPos - it.value()->boundRect().topLeft());
            }
            return it.value();
        }
    }
    return nullptr;
}

QSet<CSpGraphicViewItem *> CSpGraphicViewMapListWidget::itemAt(const QRectF &glRctF)
{

    int aS = static_cast<int>(glRctF.top());
    int bS = static_cast<int>(glRctF.bottom());
    if(_model.model() == CSpGraphicStructModel::Icon_Model)
    {
        aS -= _model.iconItemSz().height();
        bS +=  _model.iconItemSz().height();
    }
    else if(_model.model() == CSpGraphicStructModel::Row_Model)
    {
        aS = static_cast<int>(glRctF.left());
        bS = static_cast<int>(glRctF.right());
    }

    QSet<CSpGraphicViewItem *> items;
    auto itb = _topItemBottomMap.lowerBound(aS);
    if(itb == _topItemBottomMap.end()){return items;}

    auto ite = _topItemBottomMap.lowerBound(bS);
    if(ite != _topItemBottomMap.end()){++ite;}

    for(auto it = itb;it != ite;++it)
    {
        CSpGraphicViewItem* pTopItem = it.value();
        QRectF insterRctF = pTopItem->boundRect().intersected(glRctF);
        if(insterRctF.isValid())
        {
            insterRctF.translate(-pTopItem->boundRect().topLeft());
            items.unite(pTopItem->ItemAt(insterRctF));
        }
    }
    return items;
}

QList<CSpGraphicViewItem *> CSpGraphicViewMapListWidget::allItemAt(const QPointF &glPos)
{
    QList<CSpGraphicViewItem *> items;

    int bS = static_cast<int>(glPos.y());
    if(_model.model() == CSpGraphicStructModel::Icon_Model)
    {
        auto itb = _topItemBottomMap.upperBound(bS - _model.iconItemSz().height());
        auto ite = _topItemBottomMap.upperBound(bS + _model.iconItemSz().height());

        for(auto it = itb;it != ite;++it)
        {
            CSpGraphicViewItem* pTopItem = it.value();
            if(posInBoundRectF(glPos,pTopItem->boundRect()))
            {
                items.append(pTopItem->allItemAt(QPointF(glPos) - pTopItem->boundRect().topLeft()));
                items.append(pTopItem);
            }
        }
        return items;
    }
    else if(_model.model() == CSpGraphicStructModel::Row_Model)
    {
        bS = static_cast<int>(glPos.x());
    }


    auto it = _topItemBottomMap.upperBound(bS);
    if(it != _topItemBottomMap.end())
    {
        if(posInBoundRectF(glPos,it.value()->boundRect()))
        {
            items.append(it.value()->allItemAt(QPointF(glPos) - it.value()->boundRect().topLeft()));
            items.append(it.value());
        }
    }
    return items;
}

void CSpGraphicViewMapListWidget::drawItems(QPainter *painter)
{
    for(auto it = _it[0];(it !=_topItemBottomMap.end()) && (it != _it[1]);++it)
    {
        CSpGraphicViewItem* pTopItem = (*it);
        paintItem(pTopItem,painter,viewportRectF(),Normal_Paint);
    }
}

void CSpGraphicViewMapListWidget::updateItemsStructureAndGeometry(bool isResize)
{
    Q_UNUSED(isResize);

    _topItemBottomMap.clear();

    if(_model.model() == CSpGraphicStructModel::List_Model)
    {
        int y = 0;
        for(int i = 0;i<_topItemList.size();++i)
        {
            CSpGraphViewIndexItem* pItem = _topItemList[i];
            pItem->_rct = QRectF(0,y,viewPort()->width(),pItem->_rct.height());
            pItem->_index = i;
            y = static_cast<int>(pItem->boundRect().bottom());

            _topItemBottomMap.insert(y,pItem);
        }

        refreshVScrollBarRange();

        int vValue = verValue();

        onVerScrollBarValueChanged(vValue);
    }
    else if(_model.model() == CSpGraphicStructModel::Row_Model)
    {
        int x = 0;
        for(int i = 0;i<_topItemList.size();++i)
        {
            CSpGraphViewIndexItem* pItem = _topItemList[i];
            pItem->_rct = QRectF(x,0,pItem->_rct.width(),viewPort()->height());
            pItem->_index = i;
            x = static_cast<int>(pItem->boundRect().right());
            _topItemBottomMap.insert(x,pItem);
        }

        refreshHScrollBarRange();

        int vValue = horValue();

        onHorScrollBarValueChanged(vValue);
    }
    else if(_model.model() == CSpGraphicStructModel::Icon_Model)
    {
        qreal space = _model.spaceX();

        int w = _model.iconItemSz().width();
        int h = _model.iconItemSz().height();

        if(!_model.iconSpaceXStatic())
        {
            qreal dNum  = static_cast<qreal>(viewPort()->width()) / static_cast<qreal>(w);
            int   iNum  = static_cast<int>(dNum);
            qreal leftW = static_cast<qreal>(viewPort()->width()) - static_cast<qreal>(iNum*w);
            space = leftW/static_cast<qreal>(iNum + 1);
        }

        QPointF pos = QPointF(space,_model.spaceY());

        for(int i = 0;i<_topItemList.size();++i)
        {
            CSpGraphViewIndexItem* pItem = _topItemList[i];
            pItem->_rct = QRectF(pos,_model.iconItemSz());

            pos = pItem->boundRect().topRight();

            pos.setX(pos.x() + space);

            if(pos.x() + w  > viewPort()->width())
            {
                pos.setY(pos.y() + h + _model.spaceY());
                pos.setX(space);
            }
            _topItemBottomMap.insert(static_cast<int>(pItem->boundRect().bottom()),pItem);
        }
        refreshVScrollBarRange();

        int vValue = verValue();

        onVerScrollBarValueChanged(vValue);
    }
}

void CSpGraphicViewMapListWidget::onVerScrollBarValueChanged(int value)
{
    _it[0] = _topItemBottomMap.lowerBound(value);

    _it[1] = _topItemBottomMap.lowerBound(value + viewPort()->height() + (_model.model() == CSpGraphicStructModel::Icon_Model?_model.iconItemSz().height():0));


    if(_it[1] != _topItemBottomMap.end())++_it[1];
}
void CSpGraphicViewMapListWidget::onHorScrollBarValueChanged(int value)
{
    _it[0] = _topItemBottomMap.lowerBound(value);

    _it[1] = _topItemBottomMap.lowerBound(value + viewPort()->width());

    if(_it[1] != _topItemBottomMap.end())++_it[1];
}
