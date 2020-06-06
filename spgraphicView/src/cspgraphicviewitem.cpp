#include "cspgraphicviewitem.h"
#include "cspgraphicviewmouseeventinfo.h"
#include "cspgraphicstructmodel.h"
#include <QPainter>
#include <QDebug>

CSpGraphicViewItem::CSpGraphicViewItem(CSpGraphicViewItem *parentItem):
    QObject(nullptr)
{
     _parentItem = parentItem;
     _draggable  = false;
     _draggable  = false;
     _dragType   = InGl;


     _resizeable   = false;
     _resizeType   = Resize_Hor;


     //_z             = 1.0;

     _checkable     = true;
     _checked       = false;

     _enableState   = state_Normal;


     setBackgroundColor(QColor(Qt::transparent));
     setBorderColor(QColor(119,119,119));

     connect(this,&CSpGraphicViewItem::sig_SelectChanged,[=](bool selected){
         if(_graphView != nullptr)
         {
             emit _graphView->itemSelectChanged(this,selected);
         }
     });
}

CSpGraphicViewItem::~CSpGraphicViewItem()
{

}

//inline bool CGraphicViewItem::glState(PaintReson reson)
//{
//    if(reson == Normal_Paint)
//        return isGlItem();

//    return _isOperating;
//}

//inline QRectF CGraphicViewItem::rect(PaintReson reson)
//{
//    if(_isOperating && (reson != Normal_Paint))
//    {
//        return boundRectOpera();
//    }
//    return boundRect();
//}

//QRectF CGraphicViewItem::selfRect(PaintReson reson)
//{
//    QRectF rct = rect(reson);
//    rct.translate(-rct.topLeft());
//    return rct;
//}

//QRectF CGraphicViewItem::glRect(PaintReson reson)
//{
//    QRectF rct = rect(reson);
//    if(Normal_Paint != reson)
//        return rct;

//    return QRectF(mapToGl(QPointF(0.0,0.0)),rct.size()).toRect();
//}

CSpGraphicViewItem::ItemEnableState CSpGraphicViewItem::enableState()
{
    return  _enableState;
}

bool CSpGraphicViewItem::operaAccept()
{
    if(_isOperating)
    {
        if(_graphView != nullptr)
        {
            if(_graphView->_mouseMoveOpera == CSpGraphicView::DragMove)
            {
                return _graphView->_dragItemInfo->allAccept();
            }
            else if(_graphView->_mouseMoveOpera == CSpGraphicView::ResizeMove)
            {
                return _graphView->_resizeItemInfo->accept;
            }
        }
    }
    return true;
}

void CSpGraphicViewItem::paintSelfTopIcon(QPainter *painter,
                                        const CSpGraphicViewPaintEventInfo *paintInfo)
{
    Q_UNUSED(painter);
    Q_UNUSED(paintInfo);
}
void CSpGraphicViewItem::drawRect(QPainter *painter,const QRectF &rctf)
{
//    qreal penWidth = painter->pen().widthF();
//    painter->drawRect(QRectF(rctf.left() + penWidth,rctf.top() + penWidth,rctf.width() - 2.0*penWidth,
//                             rctf.height() - 2.0*penWidth));
    painter->drawRect(rctf);
}

void CSpGraphicViewItem::paint(QPainter* painter, const CSpGraphicViewPaintEventInfo *paintInfo)
{   
    if(_graphView == nullptr)
    {
        qWarning()<<metaObject()->className()<<"'s _graphView == nullptr ! paint Error !!!!!!!";
        return;
    }
    painter->save();
    paintTopIcon(painter,paintInfo);
    painter->restore();

    painter->save();
    painter->setClipRect(paintInfo->visiRectF.adjusted(-1,-1,1,1));
    paintSelf(painter,paintInfo);
    painter->restore();

    painter->save();
    painter->setClipRect(paintInfo->visiRectF.adjusted(-1,-1,1,1));
    paintChildren(painter,paintInfo->visiRectF);
    painter->restore();

    paintMore(painter,paintInfo);
}

void CSpGraphicViewItem::paintTopIcon(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo)
{
    //绘制top Item的offset区域(icon区域)
    if(this->isTopItem())
    {
        painter->save();
        //painter->setOpacity(Opacity);
        QRectF boundRect = paintInfo->rectF;
        int offsetWidth = 0;
        CSpGraphicStructModel* pMode = _graphView->structModel();
        if(pMode != nullptr)
        {
            offsetWidth = qRound(pMode->offsetWidth());
            if(offsetWidth > 0)
            {
                if(pMode->model() == CSpGraphicStructModel::List_Model)
                {
                    painter->translate(paintInfo->visiRectF.left()-offsetWidth,0);

                    CSpGraphicViewPaintEventInfo info;
                    info.rectF       = QRectF(QPointF(0,0),QSizeF(offsetWidth,boundRect.height()));
                    info.visiRectF   = info.rectF;
                    info.pViewPort   = graphicView()->viewPort();
                    info.reson       = paintInfo->reson;
                    info.isEnable    = true;
                    info.enableState = enableState();
                    info.isChecked   = (isCheckable() && isChecked());
                    paintSelfTopIcon(painter,&info);
                }
                else if(pMode->model() == CSpGraphicStructModel::Row_Model)
                {
                    painter->translate(0,paintInfo->visiRectF.top()-offsetWidth);

                    CSpGraphicViewPaintEventInfo info;
                    info.rectF       = QRectF(QPointF(0,0),QSizeF(boundRect.width(),offsetWidth));
                    info.visiRectF   = info.rectF;
                    info.pViewPort   = graphicView()->viewPort();
                    info.reson       = paintInfo->reson;
                    info.isEnable    = true;
                    info.enableState = enableState();
                    info.isChecked   = (isCheckable() && isChecked());
                    paintSelfTopIcon(painter,&info);
                }
            }
        }
        painter->restore();
    }
}

void CSpGraphicViewItem::paintSelf(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo)
{
    painter->setPen(paintInfo->borderColor);
    painter->setBrush(paintInfo->bgColor);
    painter->drawRect(paintInfo->visiRectF);
}

void CSpGraphicViewItem::paintChildren(QPainter *painter, const QRectF &rctVisible)
{
    QList<CSpGraphicViewItem*> notFirstToDraw;
    for(int i = 0;i<_childrenItems.size();++i)
    {
        CSpGraphicViewItem* pItem = _childrenItems[i];
        if(!pItem->isChecked())
        {
            s_paintItem(pItem,painter,rctVisible);
        }
        else
        {
            notFirstToDraw.append(pItem);
        }
    }

    for(int i = 0;i<notFirstToDraw.size();++i)
    {
        CSpGraphicViewItem* pItem = notFirstToDraw[i];
        s_paintItem(pItem,painter,rctVisible);
    }
}

void CSpGraphicViewItem::paintMore(QPainter* painter, const CSpGraphicViewPaintEventInfo *paintInfo)
{
    Q_UNUSED(painter);
    Q_UNUSED(paintInfo);
}

void CSpGraphicViewItem::updateItemsStructureAndGeometry()
{

}

void CSpGraphicViewItem::s_clearItemChildren(CSpGraphicViewItem *pItem, bool deleThem)
{
    pItem->removeAllChidren(deleThem);
}

void CSpGraphicViewItem::mousePressEvent(CSpGraphicViewMouseEventInfo *mouseInfo)
{
    Q_UNUSED(mouseInfo);
}

void CSpGraphicViewItem::mouseReleaseEvent(CSpGraphicViewMouseEventInfo *mouseInfo)
{
    Q_UNUSED(mouseInfo);
}

void CSpGraphicViewItem::mouseMoveEvent(CSpGraphicViewMouseEventInfo *mouseInfo)
{
    Q_UNUSED(mouseInfo);
}

void CSpGraphicViewItem::mouseDoubleClickEvent(CSpGraphicViewMouseEventInfo *mouseInfo)
{
    //emit mouseInfo->pGraphViewWidget->itemDoubleClicked(this);
    Q_UNUSED(mouseInfo);
}

void CSpGraphicViewItem::enterEvent(CSpGraphicViewMouseEventInfo *mouseInfo)
{
    Q_UNUSED(mouseInfo);
}

void CSpGraphicViewItem::leaveEvent(CSpGraphicViewMouseEventInfo *mouseInfo)
{
    Q_UNUSED(mouseInfo);
}

void CSpGraphicViewItem::moveEvent(CSpGraphicViewItemMoveEventInfo *moveInfo)
{
    Q_UNUSED(moveInfo);
}

void CSpGraphicViewItem::resizeEvent(const QSizeF &oldSz, const QSizeF &newSz)
{
    Q_UNUSED(oldSz);
    Q_UNUSED(newSz);
}

void CSpGraphicViewItem::childAddedEvent(CSpGraphicViewChildEventInfo *childEventInfo)
{
    Q_UNUSED(childEventInfo);
}

void CSpGraphicViewItem::childRemovedEvent(CSpGraphicViewChildEventInfo *childEventInfo)
{
    Q_UNUSED(childEventInfo);
}

void CSpGraphicViewItem::doDragMove(CSpGraphViewDragItemInfo *drageMoveInfo)
{
    const QPointF move = drageMoveInfo->newGlPos - drageMoveInfo->oldGlPos;

    QRectF operRct = boundRectOpera();
    setBoundRectOpera(QRectF(operRct.topLeft() + move,operRct.size()));

    drageMoveInfo->accept    = true;
    drageMoveInfo->acceptPos = true;
}

void CSpGraphicViewItem::doSelectMove(CSpGraphicViewMouseMoveSelectEventInfo *selectInfo)
{
    Q_UNUSED(selectInfo);

    graphicView()->select(this);

}

void CSpGraphicViewItem::doResizeMove(CSpGraphViewResizeItemInfo *selectInfo)
{
    QRectF bouRctF = boundRectOpera();
    QPointF move   = selectInfo->newGlPos - selectInfo->oldGlPos;
    ResizeType tp  = static_cast<CSpGraphicViewItem::ResizeType>(selectInfo->resizeTp);

    if(tp == CSpGraphicViewItem::Resize_Left)
    {
        setBoundRectOpera(
                    QRectF(
                        bouRctF.topLeft() + QPointF(move.x(),0),
                           QSizeF(bouRctF.width() - move.x(),bouRctF.height())
                           )
                                      );
    }
    else if(tp == CSpGraphicViewItem::Resize_Right)
    {

        setBoundRectOpera(
                    QRectF(
                        bouRctF.topLeft(),
                           QSizeF(bouRctF.width() + move.x(),bouRctF.height())
                           )
                                      );
    }
    else if(tp == CSpGraphicViewItem::Resize_Top)
    {
        setBoundRectOpera(
                    QRectF(
                        bouRctF.topLeft() + QPointF(0,move.y()),
                           QSizeF(bouRctF.width(),bouRctF.height() - move.y())
                           )
                                      );
    }
    else if(tp == CSpGraphicViewItem::Resize_Bottom)
    {
        setBoundRectOpera(
                    QRectF(
                        bouRctF.topLeft(),
                           QSizeF(bouRctF.width(),bouRctF.height() + move.y())
                           )
                                      );
    }
    //qDebug()<<"bound0 = "<<bouRctF<<"bound1 = "<<boundRectOpera();
    selectInfo->accept = true;
    selectInfo->acceptPos = true;
}

void CSpGraphicViewItem::doResizeEnd(CSpGraphViewResizeItemInfo * resizeItemInfo)
{
    if(resizeItemInfo->accept)
    {
        CSpGraphicViewItem* pItemParent = this->parentItem();
        QRectF resultRct = this->boundRectOpera();
        if(pItemParent != nullptr)
        {
            resultRct = QRectF(pItemParent->mapFromGl(resultRct.topLeft()),resultRct.size());
        }
        this->setBoundRect(resultRct);
    }
}

int CSpGraphicViewItem::childrenCount()
{
    return  _childrenItems.size();
}

CSpGraphicViewItem *CSpGraphicViewItem::firstChild()
{
//    if(_childrenItems.isEmpty())
//        return nullptr;
//    return *(_childrenItems.begin());

    if(_childrenItems.isEmpty())
        return nullptr;

    return _childrenItems.first();
}

CSpGraphicViewItem *CSpGraphicViewItem::lastChild()
{
//    if(_childrenItems.isEmpty())
//        return nullptr;
//    return *(_childrenItems.end() - 1);

    if(_childrenItems.isEmpty())
        return nullptr;

    return _childrenItems.last();
}

CSpGraphicViewItem *CSpGraphicViewItem::nextChild(CSpGraphicViewItem *pItem)
{
//    if(_childrenItems.isEmpty())
//        return nullptr;

//    auto it = _childrenItems.find(pItem);
//    if(it != _childrenItems.end())
//    {
//        ++it;
//        if(it != _childrenItems.end())
//            return (*it);
//    }
//    return nullptr;

    if(_childrenItems.isEmpty())
        return nullptr;

    int index = _childrenItems.indexOf(pItem);
    if(index != -1)
    {
        int nexIndex = index + 1;
        if(nexIndex >= 0 && nexIndex < _childrenItems.size())
            return _childrenItems[nexIndex];
    }
    return nullptr;
}

CSpGraphicViewItem *CSpGraphicViewItem::prevChild(CSpGraphicViewItem *pItem)
{
//    if(_childrenItems.isEmpty())
//        return nullptr;

//    auto it = _childrenItems.find(pItem);
//    if(it != _childrenItems.end() &&
//            it != _childrenItems.begin())
//    {
//        --it;return (*it);
//    }
//    return nullptr;


    if(_childrenItems.isEmpty())
        return nullptr;

    int index = _childrenItems.indexOf(pItem);
    if(index != -1)
    {
        int preIndex = index - 1;
        if(preIndex >= 0 && preIndex < _childrenItems.size())
            return _childrenItems[preIndex];
    }
    return nullptr;

}

CSpGraphicViewItem *CSpGraphicViewItem::ItemAt(const QPointF pos, QPointF *out)
{
    /*倒着遍历(最上层在末尾)*/
    for(int i = _childrenItems.size() - 1;i>=0;--i)
    {
        CSpGraphicViewItem* pItem = _childrenItems[i];
        QPointF posInLocal;
        if(pItem->contains(pos,false,&posInLocal))
        {
            return pItem->ItemAt(posInLocal,out);
        }
    }
    if(out != nullptr)
    {
        *out = pos;
    }
    return this;
}

QSet<CSpGraphicViewItem *> CSpGraphicViewItem::ItemAt(const QRectF &rctF)
{
    QSet<CSpGraphicViewItem*> result;
    for(int i = 0;i<_childrenItems.size();++i)
    {
        CSpGraphicViewItem* pItem = _childrenItems[i];
        QRectF boundRct  = pItem->boundRect();
        QRectF interRctF = boundRct.intersected(rctF);
        if(interRctF.isValid())
        {
            interRctF.translate(-boundRct.topLeft());
            result.unite(pItem->ItemAt(interRctF));
        }
    }

    if(result.isEmpty())
    {
        result.insert(this);
    }

    return result;
}

QList<CSpGraphicViewItem *> CSpGraphicViewItem::allItemAt(const QPointF pos)
{
    QList<CSpGraphicViewItem *> items;

    /*倒着遍历(最上层在末尾)*/
    for(int i = _childrenItems.size() - 1;i>=0;--i)
    {
        CSpGraphicViewItem* pItem = _childrenItems[i];
        QPointF posInLocal;
        if(pItem->contains(pos,false,&posInLocal))
        {
            items.append(pItem->allItemAt(posInLocal));
            items.append(pItem);
        }
    }
    return items;
}

QRectF CSpGraphicViewItem::selfBoundRect()
{
    return QRectF(QPointF(0.0,0.0),boundRect().size());
}

QRectF CSpGraphicViewItem::glBoundRect()
{
    return QRectF(mapToGl(QPointF(0.0,0.0)),boundRect().size()).toRect();
}

CSpGraphicViewItem *CSpGraphicViewItem::parentItem()
{
    return  _parentItem;
}

void CSpGraphicViewItem::setParentItem(CSpGraphicViewItem *parent)
{
    if(parent == parentItem())
    {
        return;
    }

    if(parent == nullptr)
    {
        if(parentItem() != nullptr)
        {
            parentItem()->removeItem(this);
        }
    }
    else
    {
        if(parentItem() != nullptr)
        {
            parentItem()->removeItem(this,false,true,false);
        }
        else
        {
            if(isTopItem())
            {
                graphicView()->removeItem(this,false,false);
            }
        }
        parent->addItem(this);
    }
}

void CSpGraphicViewItem::addItem(CSpGraphicViewItem *pItem, bool refreshStruct)
{
    if(addItem_virtual(pItem))
    {
        pItem->_isGlItem   = false;
        pItem->_isTopItem  = false;
        pItem->_parentItem = this;

        s_setItemGraphView(pItem,_graphView);

        CSpGraphicViewChildEventInfo info;
        info.type = CSpGraphicViewChildEventInfo::Added;
        info.pItem = pItem;
        info.pGraphViewWidget = _graphView;      

        if(refreshStruct)
        updateItemsStructureAndGeometry();

        childAddedEvent(&info);
    }
}

void CSpGraphicViewItem::removeItem(CSpGraphicViewItem *pItem, bool dele, bool refreshStruct, bool clearAllMark)
{
    if(removeItem_virtual(pItem))
    {
        CSpGraphicViewChildEventInfo info;
        info.type = CSpGraphicViewChildEventInfo::Removed;
        info.pItem = pItem;
        info.pGraphViewWidget = _graphView;
        childRemovedEvent(&info);

        if(clearAllMark)
        graphicView()->clearAllMarkOfItem(pItem,dele);

        pItem->_isGlItem   = true;
        pItem->_isTopItem  = false;
        pItem->_parentItem = nullptr;

        s_setItemGraphView(pItem,nullptr);

        if(dele)
        {
            pItem->deleteSelf();
        }
//        else
//        {
//            pItem->_isGlItem   = true;
//            pItem->_isTopItem  = false;
//            pItem->_parentItem = nullptr;

//            s_setItemGraphView(pItem,nullptr);
//        }
        if(refreshStruct)
        updateItemsStructureAndGeometry();
    }
}

void CSpGraphicViewItem::removeAllChidren(bool deleThem)
{
//    for(auto it =  _childrenItems.begin();it !=  _childrenItems.end();++it)
//    {
//        CGraphicViewItem* pItem = (*it);

//        CGraphicViewChildEventInfo info;
//        info.type = CGraphicViewChildEventInfo::Removed;
//        info.pItem = pItem;
//        info.pGraphViewWidget = _graphView;
//        childRemovedEvent(&info);

//        s_clearItemChildren(pItem,deleThem);

//        s_clearItemMarks(pItem,deleThem);

//        if(deleThem)
//            pItem->deleteSelf();
//    }

//    _childrenItems.clear();
//    _childrenItemsWithZ.clear();

    for(int i = 0;i<_childrenItems.size();++i)
    {
        CSpGraphicViewItem* pItem = _childrenItems[i];
        CSpGraphicViewChildEventInfo info;
        info.type = CSpGraphicViewChildEventInfo::Removed;
        info.pItem = pItem;
        info.pGraphViewWidget = _graphView;
        childRemovedEvent(&info);

        s_clearItemChildren(pItem,deleThem);

        s_clearItemMarks(pItem,deleThem);

        if(deleThem)
            pItem->deleteSelf();
    }
    _childrenItems.clear();
}

int CSpGraphicViewItem::itemsBoundingHeight()
{
//    qreal max = 0;
//    for(auto it =  _childrenItems.begin();it !=  _childrenItems.end();++it)
//    {
//        qreal bottom = (*it)->boundRect().bottom();
//        if(bottom > max)
//        {
//            max = bottom;
//        }
//    }
//    return static_cast<int>(max + 0.5);

    qreal max = 0;
    for(int i = 0;i<_childrenItems.size();++i)
    {
        CSpGraphicViewItem* pItem = _childrenItems[i];
        qreal bottom = pItem->boundRect().bottom();

        if(bottom > max)
        {
            max = bottom;
        }
    }
    return qRound(max);
}

int CSpGraphicViewItem::itemsBoundingWidth()
{
//    qreal max = 0;
//    for(auto it =  _childrenItems.begin();it !=  _childrenItems.end();++it)
//    {
//        qreal right = (*it)->boundRect().right();
//        if(right > max)
//        {
//            max = right;
//        }
//    }
//    return static_cast<int>(max + 0.5);

    qreal max = 0;
    for(int i = 0;i<_childrenItems.size();++i)
    {
        CSpGraphicViewItem* pItem = _childrenItems[i];
        qreal right = pItem->boundRect().right();

        if(right > max)
        {
            max = right;
        }
    }
    return qRound(max);
}

bool CSpGraphicViewItem::contains(const QPointF &pos, bool posIsInLocal, QPointF *out)
{
    if(isTopItem() &&
            graphicView() != nullptr &&
            graphicView()->structModel() != nullptr &&
            graphicView()->structModel()->offsetWidth() > 0)
    {
        qreal  offWidth     = graphicView()->structModel()->offsetWidth();
        QRectF offBoundRect = posIsInLocal?selfBoundRect():boundRect();
        switch (graphicView()->structModel()->model())
        {
        case CSpGraphicStructModel::List_Model:
        {
            offBoundRect.adjust(-offWidth,0,0,0);
            break;
        }
        case CSpGraphicStructModel::Row_Model:
        {
            offBoundRect.adjust(0,-offWidth,0,0);
            break;
        }
        default:
            break;
        }
        if(out != nullptr)
        {
            posIsInLocal?(*out=pos):(*out = pos - offBoundRect.topLeft());
        }
        return offBoundRect.contains(pos);
    }


    QRectF offBoundRect = boundRect();
    if(posIsInLocal)
    {
        offBoundRect = selfBoundRect();
    }

    if(out != nullptr)
    {
        posIsInLocal?(*out=pos):(*out = pos - offBoundRect.topLeft());
    }

    return offBoundRect.contains(pos);
}

void CSpGraphicViewItem::s_clearItemMarks(CSpGraphicViewItem *pItem, bool willDelete)
{
    if(pItem->_graphView != nullptr)
    {
        pItem->_graphView->clearAllMarkOfItem(pItem,willDelete);
    }
    else
    {
        qWarning()<<"s_clearItemMarks pItem _graphView = nullptr !!!";
    }

    pItem->_parentItem  = nullptr;
    pItem->_graphView   = nullptr;
    pItem->_isGlItem    = true;
    pItem->_isTopItem   = false;
}

void CSpGraphicViewItem::s_setItemFather(CSpGraphicViewItem *pItem, CSpGraphicViewItem *pFather)
{
    if(pFather != nullptr)
    {
        pItem->_isGlItem  = false;
        pItem->_isTopItem = false;
        pItem->_parentItem = pFather;

        s_setItemGraphView(pItem,pFather->_graphView);
    }
    else
    {
        pItem->_isGlItem  = true;
        pItem->_isTopItem = false;
    }
}

void CSpGraphicViewItem::s_setItemGraphView(CSpGraphicViewItem *pItem, CSpGraphicView *pGv)
{
    pItem->_graphView = pGv;
    pItem->setChildrenGraphView(pGv);
}

void CSpGraphicViewItem::s_paintItem(CSpGraphicViewItem *pItem,
                                 QPainter *painter,
                                 const QRectF &rctVisible)
{
    pItem->_graphView->paintItem(pItem,painter,rctVisible,Normal_Paint);
}

QPixmap CSpGraphicViewItem::selfie(qreal Opacity)
{
    qreal dpi = graphicView()->viewPort()->devicePixelRatioF();
    QSizeF sz = boundRect().size();
    QPixmap pix(((sz + QSizeF(1.0/dpi*2.0,1.0/dpi * 2.0))*dpi).toSize());
    pix.setDevicePixelRatio(dpi);
    pix.fill(QColor(0,0,0,0));
    QPainter painter(&pix);
    painter.initFrom(graphicView()->viewPort());
    painter.setOpacity(Opacity);
    CSpGraphicViewPaintEventInfo info;
    info.rectF       = selfBoundRect();
    info.visiRectF   = info.rectF;
    info.enableState = enableState();
    info.isChecked   = isCheckable() && isChecked();
    info.pViewPort   = graphicView()->viewPort();
    info.reson       = Selfie_Paint;
    info.isEnable    = true;
    paint(&painter,&info);
    return pix;
}

bool CSpGraphicViewItem::addItem_virtual(CSpGraphicViewItem *pItem)
{
//    bool success = false;
//    auto ifound = _childrenItems.find(pItem);
//    if(ifound ==  _childrenItems.end())
//    {
//        _childrenItems.insert(pItem);
//        success = true;
//    }

//    auto itf =  _childrenItemsWithZ.find(pItem->z());
//    if(itf !=  _childrenItemsWithZ.end())
//    {
//        itf.value().insert(pItem);
//    }
//    else
//    {
//        QSet<CGraphicViewItem*> set;
//        set.insert(pItem);
//         _childrenItemsWithZ.insert(pItem->z(),set);
//    }

//    return success;

    int index = _childrenItems.indexOf(pItem);
    if(index == -1)
    {
        _childrenItems.append(pItem);
    }
    return (index == -1);
}

bool CSpGraphicViewItem::removeItem_virtual(CSpGraphicViewItem *pItem)
{
//    bool success = false;
//    auto itf =  _childrenItems.find(pItem);
//    if(itf !=  _childrenItems.end())
//    {
//         _childrenItems.erase(itf);
//         success = true;
//    }

//    auto itSetf =  _childrenItemsWithZ.find(pItem->z());
//    if(itSetf !=  _childrenItemsWithZ.end())
//    {
//        auto itf = itSetf.value().find(pItem);
//        if(itf != itSetf.value().end())
//        {
//            itSetf.value().erase(itf);
//        }
//    }
//    return success;

    int index = _childrenItems.indexOf(pItem);
    if(index != -1)
    {
        _childrenItems.removeAt(index);
    }

    return (index != -1);
}

//void CGraphicViewItem::removeAllChidren_pri(bool dele)
//{
//    for(auto it =  _childrenItems.begin();it !=  _childrenItems.end();++it)
//    {
//        CGraphicViewItem* pItem = (*it);

//        pItem->removeAllChidren_pri(dele);

//        graphicViewWidget()->clearAllMarkOfItem(pItem);

//        pItem->_parentItem  = nullptr;
//        pItem->_graphView   = nullptr;
//        pItem->_isGlItem  = true;
//        pItem->_isTopItem = false;

//        if(dele)
//        pItem->deleteSelf();
//    }

//     _childrenItems.clear();
//     _childrenItemsWithZ.clear();
//}

QPointF CSpGraphicViewItem::mapToParent(const QPointF pos)
{
    QPointF selfPos = boundRect().topLeft();
    return QPointF(pos.x() + selfPos.x(),pos.y() + selfPos.y());
}

QPointF CSpGraphicViewItem::mapFromGl(const QPointF pos)
{
    QPointF out(pos);
    CSpGraphicViewItem* p = this;
    while(p != nullptr)
    {
        out -= p->boundRect().topLeft();
        p    = p->parentItem();
    }
    return out;
}

QPointF CSpGraphicViewItem::mapToGl(const QPointF pos)
{
    CSpGraphicViewItem* p = this;
    QPointF out = pos;
    while(p != nullptr)
    {
        out = p->mapToParent(out);
        p   = p->parentItem();
    }
    return out;
}

bool CSpGraphicViewItem::isAncestors(CSpGraphicViewItem *pAncestors)
{
    if(pAncestors == nullptr){return false;}
    CSpGraphicViewItem* p = parentItem();
    while(p != nullptr)
    {
        if(p == pAncestors)
        {
            return true;
        }
        p  = p->parentItem();
    }
    return false;
}

CSpGraphicViewItem *CSpGraphicViewItem::getSameAncestorsWith(CSpGraphicViewItem *pItem)
{
    if(pItem == nullptr){return nullptr;}
    CSpGraphicViewItem* p = parentItem();
    QSet<CSpGraphicViewItem*> setA;
    while(p != nullptr)
    {
        setA.insert(p);
        p = p->parentItem();
    }

    CSpGraphicViewItem* p1 = pItem->parentItem();
    while(p1 != nullptr)
    {
        auto itf = setA.find(p1);
        if(itf != setA.end())
        {
            return p1;
        }
        p1 = p1->parentItem();
    }
    return nullptr;
}

bool CSpGraphicViewItem::isTopItem()
{
    return _isTopItem;
    //return inherits("CGraphicViewTopItem");
    //return (parent() == nullptr);
}

bool CSpGraphicViewItem::isGlItem()
{
    return _isGlItem;
}

CSpGraphicViewItem *CSpGraphicViewItem::topParentItem()
{
    CSpGraphicViewItem* p = this;
    while(p != nullptr)
    {
        if(p->isTopItem())
        {
            return p;
        }
        p = p->parentItem();
    }
    return nullptr;
}

CSpGraphicView *CSpGraphicViewItem::graphicView()
{
    return _graphView;
}

bool CSpGraphicViewItem::draggable()
{
    return  _draggable;
}

void CSpGraphicViewItem::setDragAble(bool b)
{
    _draggable = b;
}

bool CSpGraphicViewItem::isPosDragAble(const QPointF &pos)
{
    return contains(pos,true);
}

void CSpGraphicViewItem::dragBegin()
{
}

void CSpGraphicViewItem::dragMove()
{

}

void CSpGraphicViewItem::dragEnd()
{
}

CSpGraphicViewItem::DragType CSpGraphicViewItem::dragType()
{
    return  _dragType;
}

void CSpGraphicViewItem::setDragType(CSpGraphicViewItem::DragType tp)
{
     _dragType = tp;
}

CSpGraphicViewItem::ResizeType CSpGraphicViewItem::resizeType()
{
    return  _resizeType;
}

void CSpGraphicViewItem::setResizeType(CSpGraphicViewItem::ResizeType tp)
{
     _resizeType = tp;
}

bool CSpGraphicViewItem::resizeabel()
{
    return  _resizeable;
}

void CSpGraphicViewItem::setResizeAble(bool b)
{
     _resizeable = b;
}

CSpGraphicViewItem::ResizeType CSpGraphicViewItem::canResize(const QPointF& pos)
{
    if(!resizeabel())
        return Resize_None;

    const qreal tenDis = 4.0;
    QRectF                       selfRctF = selfBoundRect();
    CSpGraphicViewItem::ResizeType type     = resizeType();
    if(type == Resize_Left)
    {
        if (QRectF(QPointF(0.0,0.0),QSizeF(tenDis,selfRctF.height())).contains(pos))
        {
            return Resize_Left;
        }
        else
        {
            return Resize_None;
        }
    }
    else if(type == Resize_Right)
    {
        if (QRectF(QPointF(selfRctF.width() - tenDis,0.0),QSizeF(tenDis,selfRctF.height())).contains(pos))
        {
            return Resize_Right;
        }
        else
        {
            return Resize_None;
        }
    }
    else if(type == Resize_Hor)
    {
        if (QRectF(QPointF(0.0,0.0),QSizeF(tenDis,selfRctF.height())).contains(pos))
        {
            return Resize_Left;
        }
        else if(QRectF(QPointF(selfRctF.width() - tenDis,0.0),QSizeF(tenDis,selfRctF.height())).contains(pos))
        {
            return Resize_Right;
        }
        else
        {
            return Resize_None;
        }
    }
    else if(type == Resize_Top)
    {
        if (QRectF(QPointF(0.0,0.0),QSizeF(selfRctF.width(),tenDis)).contains(pos))
        {
            return Resize_Top;
        }
        else
        {
            return Resize_None;
        }
    }
    else if(type == Resize_Bottom)
    {
        if (QRectF(QPointF(0.0,selfRctF.height() - tenDis),QSizeF(selfRctF.width(),tenDis)).contains(pos))
        {
            return Resize_Bottom;
        }
        else
        {
            return Resize_None;
        }
    }
    else if(type == Resize_Ver)
    {
        if (QRectF(QPointF(0.0,0.0),QSizeF(selfRctF.width(),tenDis)).contains(pos))
        {
            return Resize_Top;
        }
        else if(QRectF(QPointF(0.0,selfRctF.height() - tenDis),QSizeF(selfRctF.width(),tenDis)).contains(pos))
        {
            return Resize_Bottom;
        }
        else
        {
            return Resize_None;
        }
    }
    return Resize_None;
}

void CSpGraphicViewItem::resizeBegin(ResizeType tp)
{
    Q_UNUSED(tp);
    //record();
}

void CSpGraphicViewItem::resizeMove(CSpGraphicViewItem::ResizeType tp)
{
    Q_UNUSED(tp);
}

void CSpGraphicViewItem::resizeEnd(ResizeType tp)
{
    Q_UNUSED(tp);
//    if( _parentItem == nullptr)
//    {
//        revoke();
//        setParentItem( _parentItem);
//    }
}

bool CSpGraphicViewItem::isCheckable()
{
    return  _checkable;
}

void CSpGraphicViewItem::setCheckable(bool b)
{
     _checkable = b;
}

bool CSpGraphicViewItem::isChecked()
{
    return  _checked;
}

void CSpGraphicViewItem::setChecked(bool b)
{
    if(!isCheckable())
        return;

    _checked = b;

    CSpGraphicViewItem* parent = parentItem();
    if(parent != nullptr)
    {
        if(_checked)
        {
            parent->moveChildItemToLast(this);
        }
    }
}

QColor CSpGraphicViewItem::backgroundColor()
{
    return _bgColor;
}

void CSpGraphicViewItem::setBackgroundColor(const QColor &c)
{
    _bgColor = c;
    update();
}

QColor CSpGraphicViewItem::borderColor()
{
    return _borderColor;
}

void CSpGraphicViewItem::setBorderColor(const QColor &c)
{
    _borderColor = c;
    update();
}

bool CSpGraphicViewItem::isOperating()
{
    return _isOperating;
}

OperatingTp CSpGraphicViewItem::OperatingType()
{
    if(_graphView == nullptr)
    {
        return CSpGraphicView::NoneOperation;
    }
    return _graphView->_mouseMoveOpera;
}

qreal CSpGraphicViewItem::z()
{
    return  _z;
}

void CSpGraphicViewItem::setZ(qreal zTemp)
{

//    CGraphicViewItem* parent = parentItem();
//    if(parent != nullptr)
//    {
//        auto itf = parent-> _childrenItemsWithZ.find( _z);
//        if(itf != parent-> _childrenItemsWithZ.end())
//        {
//            auto itfItem = itf.value().find(this);
//            if(itfItem != itf.value().end())
//            {
//                 itf.value().erase(itfItem);
//                 if(itf.value().isEmpty())
//                 {
//                     parent-> _childrenItemsWithZ.erase(itf);
//                 }
//            }
//        }

//        itf = parent-> _childrenItemsWithZ.find(zTemp);
//        if(itf != parent-> _childrenItemsWithZ.end())
//        {
//            itf.value().insert(this);
//        }
//        else
//        {
//            QSet<CGraphicViewItem*> set;
//            set.insert(this);
//            parent-> _childrenItemsWithZ.insert(zTemp,set);
//        }
//    }
     _z = zTemp;
}

void CSpGraphicViewItem::moveBy(qreal dx, qreal dy)
{
    moveBy(QPointF(dx,dy));
}

void CSpGraphicViewItem::moveBy(const QPointF &dPos)
{
    CSpGraphicViewItemMoveEventInfo moveInfo;
    moveInfo.oldPos = boundRect().topLeft();
    setBoundRect(QRectF(boundRect().topLeft() + dPos,boundRect().size()));
    moveInfo.newPos = boundRect().topLeft();

    moveEvent(&moveInfo);
}

void CSpGraphicViewItem::update()
{
    if(_graphView != nullptr)
    {
        QRectF cuF = (_isOperating?boundRectOpera():glBoundRect());
        QRect paintCuF = QRectF(_graphView->mapGlPosToPaintPos(cuF.topLeft()) - QPoint(1,1),cuF.size() +QSize(2,2)).toRect();
        if(isTopItem())
        {
            CSpGraphicStructModel* pMode = graphicView()->structModel();
            if(pMode != nullptr)
            {
                int offsetWidth = qRound(pMode->offsetWidth());;
                if(pMode->model() == CSpGraphicStructModel::List_Model)
                {
                    paintCuF.adjust(-offsetWidth,0,0,0);
                }
                else if(pMode->model() == CSpGraphicStructModel::Row_Model)
                {
                    paintCuF.adjust(0,-offsetWidth,0,0);
                }
            }
        }
        _graphView->viewPort()->update(paintCuF);
    }
}

void CSpGraphicViewItem::update(const QRectF &rctf)
{
    if(_graphView != nullptr)
    {
        //QRectF cuF = (_isOperating?boundRectOpera():glBoundRect());
        QRect  cuF     = QRectF(mapToGl(rctf.topLeft()),rctf.size()).toRect();
        QRect paintCuF = QRectF(_graphView->mapGlPosToPaintPos(cuF.topLeft()) - QPoint(1,1),cuF.size() +QSize(2,2)).toRect();
        _graphView->viewPort()->update(paintCuF);
    }
}

//qreal CGraphicViewItem::maxRight()
//{
//    qreal max = 0;
//    for(auto it =  _childrenItems.begin();it !=  _childrenItems.end();++it)
//    {
//        qreal right = (*it)->boundRect().right();
//        if(right > max)
//        {
//            max = right;
//        }
//    }
//    return max;
//}

void CSpGraphicViewItem::refresh()
{
    updateItemsStructureAndGeometry();
}

void CSpGraphicViewItem::setToolTip(const QString & toopTip)
{
    _toolTips = toopTip;

//    _toolTipPos = pos;
}

QString CSpGraphicViewItem::toolTip()
{
    return _toolTips;
}

//CGraphicViewItem::EToolTipPos CGraphicViewItem::tipPos()
//{
//    return _toolTipPos;
//}

void CSpGraphicViewItem::setToolTipDuration(int msec)
{
    _tooltipDur = msec;
}

int CSpGraphicViewItem::toolTipDuration() const
{
    return _tooltipDur;
}

void CSpGraphicViewItem::onClicked()
{

}

void CSpGraphicViewItem::deleteSelf()
{
    QObject::deleteLater();
}

void CSpGraphicViewItem::setChildrenGraphView(CSpGraphicView *pGv)
{
//    for(auto it = _childrenItems.begin();it != _childrenItems.end();++it)
//    {
//        CGraphicViewItem* p = (*it);

//        s_setItemGraphView(p,pGv);
//    }

    for(int i = 0;i<_childrenItems.size();++i)
    {
        CSpGraphicViewItem* p = _childrenItems[i];

        s_setItemGraphView(p,pGv);
    }
}

QRectF CSpGraphicViewItem::boundRectOpera()
{
    return _rectOperating;
}

void CSpGraphicViewItem::setBoundRectOpera(const QRectF &rct)
{
    _rectOperating = rct;
}

void CSpGraphicViewItem::doDragBegin(CSpGraphViewDragItemInfo *pInfo)
{
    Q_UNUSED(pInfo);
    setBoundRectOpera(QRectF(mapToGl(QPointF(0.0,0.0)),boundRect().size()));
}
void CSpGraphicViewItem::doDragEnd(CSpGraphViewDragItemInfo* pInfo)
{
    Q_UNUSED(pInfo);
//    if(pInfo->accept)
//    {
//        CGraphicViewItem* pItemParent = pInfo->pressedItemNewParent;
//        if(pInfo->pressedItem == this)
//        {
//            pItemParent = pInfo->pressedItemNewParent;
//        }
//        else
//        {
//            pItemParent  =  pInfo->pGraphViewWidget->itemAt(boundRectOpera().topLeft());
//        }

//        if(pItemParent != nullptr)
//        {
//            if(pItemParent->isAncestors(this) || pItemParent == this)
//            {
//                pItemParent = parentItem();
//            }

//            QPointF topLeft = pItemParent->mapFromGl(boundRectOpera().topLeft());
//            setBoundRect(QRectF(topLeft,boundRectOpera().size()));
//            setParentItem(pItemParent);
//        }
//    }
}

void CSpGraphicViewItem::doResizeBegin(CSpGraphViewResizeItemInfo *selectInfo)
{
    Q_UNUSED(selectInfo);
    setBoundRectOpera(QRectF(mapToGl(QPointF(0.0,0.0)),boundRect().size()));
    _isOperating   = true;
}


void CSpGraphicViewItem::dragMoveBy(CSpGraphViewDragItemInfo *drageMoveInfo)
{
    doDragMove(drageMoveInfo);
}

void CSpGraphicViewItem::moveChildItemToLast(CSpGraphicViewItem *pItem)
{
    if(pItem->parentItem() == this && (_childrenItems.indexOf(pItem) != -1))
    {
        _childrenItems.removeOne(pItem);
        _childrenItems.append(pItem);
    }
}
