#include "cspgraphicviewanimationlistwidget.h"
#include "cspgraphicviewitem.h"
#include "cspgraphicviewanimationindexitem.h"

CSpGraphicViewAnimationListWidget::CSpGraphicViewAnimationListWidget(QWidget *parent):
    CSpGraphicViewAnimation(parent)
{
    _type = ListView;

    _model.setModel(CSpGraphicStructModel::List_Model);
    _model.setSpaceY(0);

    connect(&_model,&CSpGraphicStructModel::modelChanged,this,[=](){
        this->CSpGraphicViewAnimation::refresh();
    });
}

CSpGraphicViewAnimationListWidget::~CSpGraphicViewAnimationListWidget()
{

}

CSpGraphicStructModel *CSpGraphicViewAnimationListWidget::structModel()
{
    return &_model;
}

void CSpGraphicViewAnimationListWidget::refreshList()
{
    refresh();
}
void CSpGraphicViewAnimationListWidget::clear()
{
    clearAllChecked();
    for(int i = 0;i<_topItemList.size();++i)
    {
        CSpGraphicViewAnimationIndexItem* pItem = _topItemList[i];
        pItem->deleteSelf();
    }
    _topItemList.clear();

    refreshStructAndGeometry();
    refresh();
}
void CSpGraphicViewAnimationListWidget::insertItem(CSpGraphicViewAnimationIndexItem *pItem, int index, bool refresh)
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
    CSpGraphicViewAnimationIndexItem::setIndexItemIndex(pItem,index);

    CSpGraphicView::addItem(pItem,refresh);
}

void CSpGraphicViewAnimationListWidget::removeItem(CSpGraphicViewAnimationIndexItem *pItem, bool dele, bool refresh)
{
    CSpGraphicView::removeItem(pItem,dele,refresh);
}

void CSpGraphicViewAnimationListWidget::removeItem(int index, bool dele, bool refresh)
{
    if(index>=0 && index < _topItemList.size())
    {
        CSpGraphicViewItem* pItem = _topItemList[index];
        CSpGraphicView::removeItem(pItem,dele,refresh);
    }
}

void CSpGraphicViewAnimationListWidget::moveItem(int srcIndex, int desIndex, bool finished)
{
    if(srcIndex == desIndex)
    {
        return;
    }
    if(srcIndex >=0 && srcIndex< _topItemList.size() &&
            desIndex >=0 && desIndex< _topItemList.size())
    {
        CSpGraphicViewAnimationIndexItem* pSrcItem = _topItemList.takeAt(srcIndex);

        if(desIndex > srcIndex)
        {
            --desIndex;
        }

        _topItemList.insert(desIndex,pSrcItem);

        if(finished)
        /*更新一下*/
        updateItemsStructureAndGeometry();
    }

}

bool CSpGraphicViewAnimationListWidget::addTopItem_virtual(CSpGraphicViewItem *pTopItem)
{
    CSpGraphicViewAnimationIndexItem* pListItem = static_cast<CSpGraphicViewAnimationIndexItem*>(pTopItem);
    _topItemList.insert(pListItem->index(),pListItem);

    _ref_index_b = pListItem->index();
    _ref_index_e = -1;

    return true;
}

bool CSpGraphicViewAnimationListWidget::removeTopItem_virtual(CSpGraphicViewItem *pTopItem)
{
    CSpGraphicViewAnimationIndexItem* pListItem = static_cast<CSpGraphicViewAnimationIndexItem*>(pTopItem);
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

int CSpGraphicViewAnimationListWidget::topCount()
{
    return _topItemList.size();
}

int CSpGraphicViewAnimationListWidget::dataCount()
{
    int count = 0;
    for(int i = 0;i<_topItemList.size();++i)
    {
        count += _topItemList[i]->childrenCount();
    }
    return count;
}

int CSpGraphicViewAnimationListWidget::virLogicScrollHeight()
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

int CSpGraphicViewAnimationListWidget::virLogicScrollWidth()
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

    //return viewPort()->width();
}
bool CSpGraphicViewAnimationListWidget::posInBoundRectF(const QPointF &glPos, const QRectF &boundRctF)
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
CSpGraphicViewItem *CSpGraphicViewAnimationListWidget::itemAt(const QPointF &glPos, QPointF *out)
{
    CSpGraphicViewItem* ret = nullptr;

//    int indexB = 0;
//    int indexE = _topItemList.size() - 1;
//    QRectF rctLocal = viewportRectF();
//    _model.getIndex(rctLocal,static_cast<int>(rctLocal.width()),_topItemList.size(),indexB,indexE);

    int indexB = 0;
    int indexE = _topItemList.size() - 1;

    QRectF rctLocal = QRectF(glPos,QSizeF(1,1));
    _model.getIndex(rctLocal,viewPort()->width(),_topItemList.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* p = _topItemList[i];
        QPointF        posInItem;
        if(p->contains(glPos,false,&posInItem))
        {
            return p->ItemAt(posInItem,out);
        }
    }

    return ret;
}

CSpGraphicViewItem *CSpGraphicViewAnimationListWidget::topItemAt(const QPointF &glPos, QPointF *out)
{
//    int indexB = 0;
//    int indexE = _topItemList.size() - 1;
//    QRectF rctLocal = viewportRectF();
//    _model.getIndex(rctLocal,static_cast<int>(rctLocal.width()),_topItemList.size(),indexB,indexE);
    int indexB = 0;
    int indexE = _topItemList.size() - 1;

    QRectF rctLocal = QRectF(glPos,QSizeF(1,1));
    _model.getIndex(rctLocal,viewPort()->width(),_topItemList.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* p = _topItemList[i];
        QPointF        posInItem;
        if(p->contains(glPos,false,&posInItem))
        {
            if(out != nullptr)
            {
                *out = posInItem;
            }
            return p;
        }
    }
    return nullptr;
}

QSet<CSpGraphicViewItem *> CSpGraphicViewAnimationListWidget::itemAt(const QRectF &glRctF)
{
    QSet<CSpGraphicViewItem *> setItems;

//    int indexB = 0;
//    int indexE = _topItemList.size() - 1;
//    QRectF rctLocal = viewportRectF();
//    _model.getIndex(rctLocal,static_cast<int>(rctLocal.width()),_topItemList.size(),indexB,indexE);

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

QList<CSpGraphicViewItem *> CSpGraphicViewAnimationListWidget::allItemAt(const QPointF &glPos)
{
    QList<CSpGraphicViewItem *> items;
//    int indexB = 0;
//    int indexE = _topItemList.size() - 1;
//    QRectF rctLocal = viewportRectF();
//    _model.getIndex(rctLocal,static_cast<int>(rctLocal.width()),_topItemList.size(),indexB,indexE);

    int indexB = 0;
    int indexE = _topItemList.size() - 1;

    QRectF rctLocal = QRectF(glPos,QSizeF(1,1));
    _model.getIndex(rctLocal,viewPort()->width(),_topItemList.size(),indexB,indexE);

    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* p = _topItemList[i];
        QPointF        posInItem;
        if(p->contains(glPos,false,&posInItem))
        {
            items.append(p->allItemAt(posInItem));
            items.append(p);
        }
    }
    return items;
}

void CSpGraphicViewAnimationListWidget::drawItems(QPainter *painter)
{
    int indexB = 0;
    int indexE = _topItemList.size() - 1;
    QRectF rctLocal = viewportRectF();
    _model.getIndex(rctLocal,static_cast<int>(rctLocal.width()),_topItemList.size(),indexB,indexE);

    QList<CSpGraphicViewItem*> checkedItem;
    for(int i = indexB;i<=indexE;++i)
    {
        CSpGraphicViewItem* p = _topItemList[i];
        if(p->isCheckable() && p->isChecked())
        {
            checkedItem.append(p);
        }
        else
        {
            paintItem(p,painter,viewportRectF(),Normal_Paint);
        }
    }
    /*被选中的最后绘制保证在最上层*/
    for(int i = 0;i<checkedItem.size();++i)
    {
        CSpGraphicViewItem* p = checkedItem[i];
        paintItem(p,painter,viewportRectF(),Normal_Paint);
    }
}

void CSpGraphicViewAnimationListWidget::updateItemsStructureAndGeometry(bool isResize)
{
    refresh(0,-1,isResize);
}

void CSpGraphicViewAnimationListWidget::onChildAnimationsFinished()
{
    refreshScrollBarRange();
    CSpGraphicViewAnimation::onChildAnimationsFinished();
}

void CSpGraphicViewAnimationListWidget::refresh(int indexB, int indexE, bool isResize)
{
    bool doAnimation = ((isResize && _animationWayDoResize) || !isResize);

    if(doAnimation)
    collectChildAnimalBegin();

    if(indexE == -1)
        indexE = _topItemList.size() - 1;

    bool  isSameSize = structModel()->itemSameSize();
    QSize unitSz     = structModel()->iconItemSz();

    if(_model.model() == CSpGraphicStructModel::List_Model)
    {
        qreal unitWidth = viewportRectF().width();
        int lastIndexB = indexB - 1;
        int y = 0;
        if(lastIndexB>=0 && lastIndexB<_topItemList.size())
        {
            if(isSameSize)
            {
                y = unitSz.height()*(lastIndexB+1) + _model.spaceY()*lastIndexB;
            }
            else
            {
                y = static_cast<int>(_topItemList[lastIndexB]->boundRect().bottom()) + _model.spaceY();
            }
        }
        for(int i = indexB;i<=indexE;++i)
        {
            CSpGraphicViewAnimationIndexItem* pItem = _topItemList[i];
            QRectF rectf = QRectF(0,y,unitWidth,isSameSize? unitSz.height():pItem->boundRect().height());
            pItem->setBoundRect(rectf);
            CSpGraphicViewAnimationIndexItem::setIndexItemIndex(pItem,i);
            y = static_cast<int>(rectf.bottom()) + _model.spaceY();
        }
    }
    else if(_model.model() == CSpGraphicStructModel::Row_Model)
    {
        qreal unitHeight = viewportRectF().height();
        int lastIndexB = indexB - 1;
        int x = 0;
        if(lastIndexB>=0 && lastIndexB<_topItemList.size())
        {
            if(isSameSize)
            {
                x = unitSz.width()*(lastIndexB+1) + _model.spaceX()*lastIndexB;
            }
            else
            {
                x = static_cast<int>(_topItemList[lastIndexB]->boundRect().right()) + _model.spaceX();
            }
        }
        for(int i = indexB;i<=indexE;++i)
        {
            CSpGraphicViewAnimationIndexItem* pItem = _topItemList[i];
            QRectF rectf = QRectF(x,0,isSameSize?unitSz.width():pItem->boundRect().width(),unitHeight);
            pItem->setBoundRect(rectf);
            CSpGraphicViewAnimationIndexItem::setIndexItemIndex(pItem,i);
            x = static_cast<int>(rectf.right()) + _model.spaceX();
        }
    }
    else if(_model.model() == CSpGraphicStructModel::Icon_Model)
    {
        for(int i = 0;i<_topItemList.size();++i)
        {
            CSpGraphicViewAnimationIndexItem* pItem = _topItemList[i];
            pItem->setBoundRect(_model.boundRectF_IconModel(viewPort()->width(),i,_topItemList.size()));
        }
    }

    if(doAnimation)
    collectChildAnimalEnd();

    _ref_index_b = 0;
    _ref_index_e = -1;
}

CSpGraphicViewAnimationIndexItem *CSpGraphicViewAnimationListWidget::item(int index)
{
    if(index < 0 || index >= _topItemList.size())
        return nullptr;

    return _topItemList[index];
}

void CSpGraphicViewAnimationListWidget::setResizeAnimation(bool b)
{
    _animationWayDoResize = b;
}
