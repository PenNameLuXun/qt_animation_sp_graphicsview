#include "cspgraphicview.h"
#include "cspgraphicviewitem.h"
#include "cspgraphicviewmouseeventinfo.h"
#include <QGridLayout>
#include <QDebug>
#include <QWheelEvent>
#include <QPainter>
#include <QStyleOption>
#include <QPropertyAnimation>
#include <QVariantAnimation>
#include "cspgraphicstructmodel.h"
#include "csprealslider.h"
#include <QScrollBar>
#include <QToolTip>
CSpGraphicView::CSpGraphicView(QWidget *parent, const QColor &bgColor):
    QWidget(parent)
{
    _horSlider = new CSpRealSlider(this,CSpRealSlider::Hor);
    connect(_horSlider,&CSpRealSlider::valueChanged,this,&CSpGraphicView::onHorScrollBarValueChanged_pri);
    connect(_horSlider,&CSpRealSlider::rangeChanged,this,&CSpGraphicView::onHorScrollBarRangeChanged_pri);
    _horSlider->setValue(0);
    _horSlider->setMax(0);
    _horSlider->setFixedHeight(10);

    _verSlider = new CSpRealSlider(this,CSpRealSlider::Ver);
    connect(_verSlider,&CSpRealSlider::valueChanged,this,&CSpGraphicView::onVerScrollBarValueChanged_pri);
    connect(_verSlider,&CSpRealSlider::rangeChanged,this,&CSpGraphicView::onVerScrollBarRangeChanged_pri);
    _verSlider->setValue(0);
    _verSlider->setMax(0);
    _verSlider->setFixedWidth(10);

    setViewPort(new QWidget(this));

    setBackgroundColor(bgColor);
    setViewPortBackgroundColor(bgColor);
}

void CSpGraphicView::setViewPort(QWidget *viewPort)
{
    if(_viewport == viewPort)
        return;

    if(viewPort != nullptr)
    {
        if(layout() != nullptr)
        {
            layout()->replaceWidget(_viewport,viewPort);
        }
        else
        {
            QGridLayout* gridLayout = new QGridLayout(this);
            gridLayout->setSpacing(6);
            gridLayout->setContentsMargins(0, 0, 0, 0);
            gridLayout->setObjectName(QStringLiteral("gridLayout"));

            gridLayout->addWidget(viewPort, 0, 0, 1, 1);


            int offsetX = 0;
            int offsetY = 0;
            CSpGraphicStructModel* pMode = structModel();
            if(pMode != nullptr)
            {
                if(pMode->model() == CSpGraphicStructModel::List_Model)
                {
                    offsetX = qRound(pMode->offsetWidth());
                }
                else if(pMode->model() == CSpGraphicStructModel::Row_Model)
                {
                    offsetY = qRound(pMode->offsetWidth());
                }
            }

            _verSliderLayOut = new QVBoxLayout();
            _verSliderLayOut->setSpacing(0);
            _verSliderLayOut->setObjectName(QStringLiteral("verSliderLayout"));
            _verSliderLayOut->setContentsMargins(0, offsetY, 0, 0);
            _verSliderLayOut->addWidget(_verSlider);
            gridLayout->addLayout(_verSliderLayOut, 0, 1, 1, 1);

            _horSliderLayOut = new QHBoxLayout();
            _horSliderLayOut->setSpacing(0);
            _horSliderLayOut->setObjectName(QStringLiteral("horSliderLayout"));
            _horSliderLayOut->setContentsMargins(offsetX, 0, 0, 0);
            _horSliderLayOut->addWidget(_horSlider);
            gridLayout->addLayout(_horSliderLayOut, 1, 0, 1, 1);
        }


        if(_viewport != nullptr)
        {
            _viewport->deleteLater();
        }

        _viewport = viewPort;

        viewPort->setMouseTracking(true);
        viewPort->installEventFilter(this);
    }
}

void CSpGraphicView::setHorRealSliderBar(CSpRealSlider *slider)
{
    if(_horSlider == slider)
        return;

    if(_horSlider != nullptr)
    {
        if(_horSliderLayOut != nullptr)
        {
            _horSliderLayOut->replaceWidget(_horSlider,slider);
        }
        _horSlider->deleteLater();
    }
    _horSlider = slider;
}

void CSpGraphicView::setVerRealSliderBar(CSpRealSlider *slider)
{
    if(_verSlider == slider)
        return;

    if(_verSlider != nullptr)
    {
        if(_verSliderLayOut != nullptr)
        {
            _verSliderLayOut->replaceWidget(_verSlider,slider);
        }
        _verSlider->deleteLater();
    }
    _verSlider = slider;
}

void CSpGraphicView::setBackgroundColor(const QColor &c)
{
    _color = c;
    viewPort()->update();
}

QColor CSpGraphicView::backgroundColor()
{
    return _color;
}

void CSpGraphicView::setViewPortBackgroundColor(const QColor &c)
{
    _viewBgColor = c;
    update();
}

QColor CSpGraphicView::viewPortBackgroundColor()
{
    return _viewBgColor;
}

CSpGraphicView::~CSpGraphicView()
{

}

QRectF CSpGraphicView::viewportRectF()
{
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        int offsetWidth = qRound(pMode->offsetWidth());;
        if(pMode->model() == CSpGraphicStructModel::List_Model)
        {
            return QRectF(this->horValue(),this->verValue(),_viewport->width() - offsetWidth,_viewport->height());
        }
        else if(pMode->model() == CSpGraphicStructModel::Row_Model)
        {
            return QRectF(this->horValue(),this->verValue(),_viewport->width(),_viewport->height() - offsetWidth);
        }
    }
    return QRectF(this->horValue(),this->verValue(),_viewport->width(),_viewport->height());
}

bool CSpGraphicView::isEmpty()
{
    return isTopEmpty();
}

bool CSpGraphicView::isTopEmpty()
{
    return (topCount() == 0);
}

bool CSpGraphicView::isDataEmpty()
{
    return (dataCount() == 0);
}

void CSpGraphicView::addItem(CSpGraphicViewItem *pItem, bool refresh)
{
    if(addTopItem_virtual(pItem))
    {
        pItem->_isTopItem  = true;
        pItem->_isGlItem   = true;
        pItem->_parentItem = nullptr;

        CSpGraphicViewItem::s_setItemGraphView(pItem,this);

        if(pItem->isCheckable())
        {
            makeCheckedFlag(pItem);
            makeDragFlag(pItem);
        }

        if(refresh)
        {
            updateItemsStructureAndGeometry();
            refreshScrollBarRange();
        }
        CSpGraphicViewChildEventInfo info;
        info.type  = CSpGraphicViewChildEventInfo::Added;
        info.pItem = pItem;
        info.pGraphViewWidget = this;
        childAddedEvent(&info);
    }
}

void CSpGraphicView::removeItem(CSpGraphicViewItem *pTopItem, bool dele, bool refresh, bool clearAllMark)
{
    if(dele)
    {
        pTopItem->removeAllChidren(true);
    }

    if(removeTopItem_virtual(pTopItem))
    {
        if(clearAllMark)
        CSpGraphicViewItem::s_clearItemMarks(pTopItem,dele);

        if(dele)
        {
            pTopItem->deleteSelf();
        }

        //updateKeyValue();

        if(refresh)
        {
            updateItemsStructureAndGeometry();
            refreshScrollBarRange();
        }

        CSpGraphicViewChildEventInfo info;
        info.type  = CSpGraphicViewChildEventInfo::Removed;
        info.pItem = pTopItem;
        info.pGraphViewWidget = this;
        childRemovedEvent(&info);
    }
}

QRectF CSpGraphicView::intersection(const QRectF &rctfA, const QRectF &rctfB)
{
    return rctfA.intersected(rctfB);
}

void CSpGraphicView::refresh()
{
    updateItemsStructureAndGeometry();

    refreshScrollBarRange();

    viewPort()->update();
}

void CSpGraphicView::clear()
{
    clearAllChecked();

    refreshStructAndGeometry();
    refresh();
}

void CSpGraphicView::refreshStructAndGeometry()
{
    updateItemsStructureAndGeometry();

    viewPort()->update();
}

int CSpGraphicView::topItemSize()
{
    return topCount();
}

int CSpGraphicView::dataItemSize()
{
    return dataCount();
}

int CSpGraphicView::allItemSize()
{
    return topCount()+dataCount();
}

int CSpGraphicView::logicScrollWith()
{
    return virLogicScrollWidth();
}

int CSpGraphicView::logicScrollHeight()
{
    return virLogicScrollHeight();
}

QSize CSpGraphicView::logicScrollSize()
{
    return QSize(logicScrollWith(),logicScrollHeight());
}

QRect CSpGraphicView::rctOfThisTimeRepaint()
{
    return _thisUpdateRct;
}

bool CSpGraphicView::isAnimationView()
{
    return false;
}

void CSpGraphicView::paintItem(CSpGraphicViewItem *pItem,
                             QPainter *painter,
                             const QRectF &visibleRect,
                             const PaintReson reson)
{
    PaintReson resonReal = reson;
    if(reson == Normal_Paint)
    {
        if(pItem->isOperating())
        {
            switch (pItem->OperatingType()) {
            case CSpGraphicView::DragMove:
                resonReal = DragOrg_Paint;
                break;
            case CSpGraphicView::ResizeMove:
                resonReal = ResizeOrg_Paint;
                break;
            default:
                break;
            }
        }
    }

    QRectF rect;
    QRectF rectInsert;
    QPointF posOffset;
    if(resonReal < Global_Paint)
    {
        rect       = pItem->boundRect();

        if(pItem->isGlItem())
        {
            posOffset  = mapGlPosToPaintPos(rect.topLeft());
        }
        else
        {
            posOffset  = rect.topLeft();
        }
    }
    else
    {
        rect       = pItem->boundRectOpera();
        posOffset  = mapGlPosToPaintPos(rect.topLeft());
    }

    rectInsert = rect.intersected(visibleRect);
    rectInsert.translate(-rect.topLeft());

    if(rectInsert.isValid())
    {
        painter->save();
        painter->translate(posOffset);

        painter->save();
        CSpGraphicViewPaintEventInfo info;
        info.rectF       = rect.translated(-rect.topLeft());
        info.visiRectF   = rectInsert;
        info.pViewPort   = viewPort();
        info.reson       = resonReal;
        info.isEnable    = true;
        info.enableState = pItem->enableState();
        info.isChecked   = (pItem->isCheckable() && pItem->isChecked());
        info.bgColor     = pItem->backgroundColor();
        info.borderColor = pItem->borderColor();
        pItem->paint(painter,&info);
        painter->restore();

        painter->restore();
    }
}

void CSpGraphicView::refreshVScrollBarRange()
{
    int max = qMax(virLogicScrollHeight()- viewPort()->height(),0);
    _verSlider->setRange(0,max);
}

void CSpGraphicView::refreshHScrollBarRange()
{
    int max = qMax(virLogicScrollWidth() - viewPort()->width(),0);
    _horSlider->setRange(0,max);
}

void CSpGraphicView::refreshScrollBarRange()
{
    refreshVScrollBarRange();
    refreshHScrollBarRange();
}

void CSpGraphicView::updateItemsStructureAndGeometry(bool isResizeReson)
{
    Q_UNUSED(isResizeReson);
}

bool CSpGraphicView::eventFilter(QObject *watcher, QEvent *e)
{
    if(watcher == _viewport)
    {
        if(e->type() == QEvent::MouseButtonPress)
        {
            //qDebug()<<"----------------press ";
            QMouseEvent* event = static_cast<QMouseEvent*>(e);
            const QPointF curGlPos = mapPaintPosToGlPos(event->pos());
            CSpGraphicViewMouseEventInfo info;
            info.button           = event->button();
            info.buttons          = event->buttons();
            info.paintPos         = event->pos();
            info.viewGlPos        = curGlPos;
            info.localPos         = curGlPos;
            info.screenPos        = mapToGlobal(event->pos());
            info.mdifiers         = event->modifiers();
            info.pGraphViewWidget = this;

            mousePress(&info);

            _viewport->update();
        }
        else if(e->type() == QEvent::MouseButtonRelease)
        {
            //qDebug()<<"----------------release ";
            QMouseEvent* event = static_cast<QMouseEvent*>(e);

            const QPointF curGlPos = mapPaintPosToGlPos(event->pos());
            CSpGraphicViewMouseEventInfo info;
            info.button           = event->button();
            info.buttons          = event->buttons();
            info.paintPos         = event->pos();
            info.viewGlPos        = curGlPos;
            info.localPos         = curGlPos;
            info.screenPos        = mapToGlobal(event->pos());
            info.mdifiers         = event->modifiers();
            info.pGraphViewWidget = this;

            mouseRelease(&info);

            _viewport->update();
        }
        else if(e->type() == QEvent::MouseButtonDblClick)
        {
            // qDebug()<<"----------------press ";
            QMouseEvent* event = static_cast<QMouseEvent*>(e);

            const QPointF curGlPos = mapPaintPosToGlPos(event->pos());
            CSpGraphicViewMouseEventInfo info;
            info.button           = event->button();
            info.buttons          = event->buttons();
            info.paintPos         = event->pos();
            info.viewGlPos        = curGlPos;
            info.localPos         = curGlPos;
            info.screenPos        = mapToGlobal(event->pos());
            info.mdifiers         = event->modifiers();
            info.pGraphViewWidget = this;

            viewPort()->setUpdatesEnabled(false);
            CSpGraphicViewMouseEventInfo info0 = info;
            mousePress(&info0);
            viewPort()->setUpdatesEnabled(true);

            //qDebug()<<"----------------doubleCliked ";
//            QMouseEvent* event = static_cast<QMouseEvent*>(e);

//            const QPointF curGlPos = mapPaintPosToGlPos(event->pos());
//            CGraphicViewMouseEventInfo info;
//            info.button           = event->button();
//            info.buttons          = event->buttons();
//            info.viewGlPos        = curGlPos;
//            info.localPos         = curGlPos;
//            info.screenPos        = mapToGlobal(event->pos());
//            info.mdifiers         = event->modifiers();
//            info.pGraphViewWidget = this;


            mouseDoubleClicked(&info);

        }
        else if(e->type() == QEvent::MouseMove)
        {
            QMouseEvent* event = static_cast<QMouseEvent*>(e);

            const QPointF curGlPos = mapPaintPosToGlPos(event->pos());

            CSpGraphicViewMouseEventInfo info;
            info.button           = event->button();
            info.buttons          = event->buttons();
            info.paintPos         = event->pos();
            info.viewGlPos        = curGlPos;
            info.localPos         = curGlPos;
            info.screenPos        = mapToGlobal(event->pos());
            info.mdifiers         = event->modifiers();
            info.pGraphViewWidget = this;
            info.pressItem        = _mousePressItem;
            info.pressItems       = &_mousePressItems;

            mouseMove(&info);
        }
        else if(e->type() == QEvent::Enter)
        {
            CSpGraphicViewMouseEventInfo info;
            info.paintPos         = mapFromGlobal(QCursor::pos());
            info.viewGlPos        = this->mapPaintPosToGlPos(info.paintPos);
            info.localPos         = info.viewGlPos;
            info.screenPos        = QCursor::pos();
            info.pGraphViewWidget = this;

            this->enter(&info);

            _viewport->update();
        }
        else if(e->type() == QEvent::Leave)
        {
            this->leave();

            _viewport->update();
        }
        else if(e->type() == QEvent::Paint)
        {
            _thisUpdateRct = static_cast<QPaintEvent*>(e)->rect();

            drawAll();

            return true;
        }
        else if(e->type() == QEvent::Resize)
        {
            QResizeEvent* eventRz = static_cast<QResizeEvent*>(e);
            this->resizeContex();
            this->updateItemsStructureAndGeometry(true);
            this->refreshScrollBarRange();
            this->viewPortResize(eventRz->oldSize(),eventRz->size());

        }
        else if(e->type() == QEvent::Show)
        {
            return viewShow();
        }
        else if(e->type() == QEvent::ToolTip)
        {
            QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
            const QPointF curGlPos = mapPaintPosToGlPos(helpEvent->pos());
            CSpGraphicViewItem* pItem = itemAt(curGlPos);
            if(pItem != nullptr)
            {
                const QString toolTip = pItem->toolTip();
                if(!toolTip.isEmpty())
                {
                    QToolTip::showText(helpEvent->globalPos(), toolTip,this);
                }
                else
                {
                    QToolTip::hideText();
                    e->ignore();
                }
            }
            else
            {
                QToolTip::hideText();
                e->ignore();
            }
            return true;
        }
    }
    return QWidget::eventFilter(watcher,e);
}

void CSpGraphicView::setVerSliderVisType(CSpGraphicView::SliderVisabelType tp)
{
    _verTp = tp;
    onVerScrollBarRangeChanged_pri(_verSlider->min(),_verSlider->max());
}

CSpGraphicView::SliderVisabelType CSpGraphicView::verSliderVisType()
{
    return _verTp;
}

void CSpGraphicView::setHorSliderVisType(CSpGraphicView::SliderVisabelType tp)
{
    _horTp = tp;
    onHorScrollBarRangeChanged_pri(_horSlider->min(),_horSlider->max());
}

CSpGraphicView::SliderVisabelType CSpGraphicView::horSliderVisType()
{
    return _horTp;
}

void CSpGraphicView::setMouseSelectValid(bool b)
{
    _mouseSelectValid = b;
}

bool CSpGraphicView::mouseSelectValid()
{
    return _mouseSelectValid;
}

void CSpGraphicView::setWheelWork(bool b)
{
    _wheelWork = b;
}

bool CSpGraphicView::wheelWork()
{
    return _wheelWork;
}

qreal CSpGraphicView::horValue()
{
    return _horSlider->value();
}

qreal CSpGraphicView::verValue()
{
    return _verSlider->value();
}

QWidget *CSpGraphicView::viewPort()
{
    return _viewport;
}
qreal CSpGraphicView::mapGlXToPaintX(const qreal glX)
{
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        qreal offsetWidth = pMode->offsetWidth();
        if(pMode->model() == CSpGraphicStructModel::List_Model)
        {
            return  glX + offsetWidth - horValue();
        }
    }
    return  glX - horValue();
}

qreal CSpGraphicView::mapGlYToPaintY(const qreal glY)
{
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        qreal offsetWidth = pMode->offsetWidth();
        if(pMode->model() == CSpGraphicStructModel::Row_Model)
        {
            return  glY - verValue() + offsetWidth;
        }
    }
    return  glY - verValue();
}
QPointF CSpGraphicView::mapGlPosToPaintPos(const QPointF pos)
{
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        qreal offsetWidth = pMode->offsetWidth();
        if(pMode->model() == CSpGraphicStructModel::List_Model)
        {
            return QPointF(pos.x() + offsetWidth - horValue(),pos.y() - verValue());
        }
        else if(pMode->model() == CSpGraphicStructModel::Row_Model)
        {
            return QPointF(pos.x() - horValue(),pos.y() + offsetWidth - verValue());
        }
    }
    return  QPointF(pos.x() - horValue(),pos.y() - verValue());
}

qreal CSpGraphicView::mapPaintXToGlX(const qreal paintX)
{
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        qreal offsetWidth = pMode->offsetWidth();
        if(pMode->model() == CSpGraphicStructModel::List_Model)
        {
            return  paintX - offsetWidth  + horValue();
        }
    }
    return  paintX + horValue();
}

qreal CSpGraphicView::mapPaintYToGlY(const qreal paintY)
{
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        qreal offsetWidth = pMode->offsetWidth();
        if(pMode->model() == CSpGraphicStructModel::Row_Model)
        {
            return paintY - offsetWidth + verValue();
        }
    }
    return  paintY + verValue();
}

QPointF CSpGraphicView::mapPaintPosToGlPos(const QPointF pos)
{
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        qreal offsetWidth = pMode->offsetWidth();
        if(pMode->model() == CSpGraphicStructModel::List_Model)
        {
            return  QPointF(pos.x() - offsetWidth  + horValue(),pos.y()+ verValue());
        }
        else if(pMode->model() == CSpGraphicStructModel::Row_Model)
        {
            return QPointF(pos.x() + horValue(),pos.y() - offsetWidth + verValue());
        }
    }
    return  QPointF(pos.x() + horValue(),pos.y() + verValue());
}

bool CSpGraphicView::isPressed()
{
    return _pressed;
}

bool CSpGraphicView::isOprating(CSpGraphicView::MouseOperationTp *outTp)
{
    bool result = (_mouseMoveOpera != NoneOperation);
    if(outTp != nullptr)
        *outTp = _mouseMoveOpera;
    return result;
}

CSpGraphicViewItem *CSpGraphicView::isPressedItem()
{
    return _mousePressItem;
}

void CSpGraphicView::select(CSpGraphicViewItem *pItem, bool emitChanged)
{
    if(pItem->isCheckable()&& !pItem->isChecked())
    {
        pItem->setChecked(true);
        makeCheckedFlag(pItem);
        makeDragFlag(pItem);

        if(emitChanged)
        emit pItem->sig_SelectChanged(true);
    }
}

void CSpGraphicView::notSelect(CSpGraphicViewItem *pItem,bool emitChanged)
{
    if(pItem->isCheckable()&&pItem->isChecked())
    {
        pItem->setChecked(false);
        makeCheckedFlag(pItem);
        makeDragFlag(pItem);

        if(emitChanged)
        emit pItem->sig_SelectChanged(false);
    }
}

void CSpGraphicView::clearSelected(bool emitChanged)
{
    clearAllChecked(emitChanged);
    clearAllDrag();
}

void CSpGraphicView::clearSelectedExcept(CSpGraphicViewItem *pItem, bool emitChanged)
{
    for(auto it = _checkedItems.begin();it != _checkedItems.end();)
    {
        CSpGraphicViewItem* p = (*it);

        if(pItem == p)
        {
            ++it;
            continue;
        }

        if(p->isChecked())
        {
            p->setChecked(false);
            if(emitChanged)
            emit p->sig_SelectChanged(false);
            p->update();
        }
        it = _checkedItems.erase(it);
    }

    _draggedItems.clear();
    if(pItem->draggable())
    _draggedItems.insert(pItem);
}

void CSpGraphicView::resizeEvent(QResizeEvent *event)
{

    if(_viewport != nullptr)
    {
        _viewport->move(0,0);

    }


    return QWidget::resizeEvent(event);
}

void CSpGraphicView::wheelEvent(QWheelEvent *event)
{
    if(!_wheelWork)
    {
        return QWidget::wheelEvent(event);
    }
    int move = -event->delta();
    _verSlider->setValue(_verSlider->value() + move/4);
    return QWidget::wheelEvent(event);
}

void CSpGraphicView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    painter.setBrush(this->backgroundColor());

    painter.drawRect(rect());
}

void CSpGraphicView::viewPortResize(const QSize &oldSz, const QSize &newSz)
{
    Q_UNUSED(oldSz);
    Q_UNUSED(newSz);
}

void CSpGraphicView::mousePress(CSpGraphicViewMouseEventInfo *pInfo)
{
    seletBegin_pri();

    QList<CSpGraphicViewItem*> allPosItemsList = allItemAt(pInfo->viewGlPos);
    CSpGraphicViewItem* pItem = nullptr;
    QPointF posLocal        = pInfo->localPos;

    if(!allPosItemsList.isEmpty())
    {
        pItem            = allPosItemsList.first();
        pInfo->localPos  = pItem->mapFromGl(pInfo->viewGlPos);
        posLocal         = pInfo->localPos;
        pInfo->pressItem = pItem;
        pInfo->pressItems= &allPosItemsList;

        for(int i = 0;i<allPosItemsList.size();++i)
        {
            CSpGraphicViewItem* p = allPosItemsList[i];
            pInfo->localPos     = p->mapFromGl(pInfo->viewGlPos);
            mouseBtnPressItem(p,pInfo);
            p->mousePressEvent(pInfo);
            if(pInfo->isFinished())
            {
                break;
            }
        }
    }

    _pressed                 = true;
    _mousePressItem          = pItem;
    _mousePressBeginPos      = pInfo->viewGlPos;
    _mousePressPos           = _mousePressBeginPos;
    _mousePressBeginLocalPos = posLocal;

    _mousePressPaintBeginPos = pInfo->paintPos;
    _mousePressPaintPos      = _mousePressPaintBeginPos;


    _mousePressItems = allPosItemsList;
}

void CSpGraphicView::mouseRelease(CSpGraphicViewMouseEventInfo *pInfo)
{
    pInfo->haveDraged       = _moveDrageding;
    pInfo->haveSelect       = _moveSelecting;
    pInfo->haveResize       = _moveResizing;
    pInfo->pressItem        = _mousePressItem;
    pInfo->pressItems       = &_mousePressItems;
    pInfo->pGraphViewWidget = this;

    if(handleOperatFirstWhenMouseRelease)
    {
        handleOperateEventWhenRelease(pInfo);
        handleNormalEventWhenRelease(pInfo);
    }
    else
    {
        handleNormalEventWhenRelease(pInfo);
        handleOperateEventWhenRelease(pInfo);
    }

    //Next:
    if((_mousePressItem == nullptr) && (_mouseMoveOpera == NoneOperation))
    {
        clearAllChecked();
        clearAllDrag();
    }

    _pressed        = false;
    _mousePressItem = nullptr;
    _mousePressItems.clear();
    _mousePosItems  = allItemAt(pInfo->viewGlPos);
    _mousePosItem   = (_mousePosItems.isEmpty() ? nullptr:_mousePosItems.first());
    _moveDrageding  = false;
    _moveSelecting  = false;
    _moveResizing   = false;
    _mouseMoveOpera = NoneOperation;
    _mouseMoveHaveMakenOpera = false;
    stopHorMove();
    stopVerMove();

    seletEnd_pri();

    setCursor(QCursor(Qt::ArrowCursor));

    //再执行一下鼠标的move保证鼠标下item应该呈现的hover态
    mouseMove(pInfo);
}

void CSpGraphicView::mouseDoubleClicked(CSpGraphicViewMouseEventInfo *pInfo)
{
    QList<CSpGraphicViewItem*> pItems = allItemAt(pInfo->viewGlPos);
    //CGraphicViewItem* pItem  = (pItems.isEmpty()?nullptr:pItems.first());

    for(int i = 0;i<pItems.size();++i)
    {
        CSpGraphicViewItem* p = pItems[i];
        pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
        mouseDoubleClickedItem(p,pInfo);
        if(pInfo->isFinished())
            break;
    }
}

void CSpGraphicView::mouseMove(CSpGraphicViewMouseEventInfo *pInfo)
{
    const QPointF curGlPos = pInfo->viewGlPos;

    if(pInfo->buttons == Qt::NoButton)
    {
        QList<CSpGraphicViewItem*> pItems = allItemAt(pInfo->viewGlPos);

        CSpGraphicViewItem* pItem  =  (pItems.isEmpty() ? nullptr:pItems.first());

        /*先判断新位置的Item和之前的item的关系*/
        if(_mousePosItem == nullptr)
        {
            /*_mouseMoveItem什么都不用做 直接处理鼠标移入pItem
              鼠标移入事件不用考虑CGraphicViewMouseEventInfo的finish标记 所有pItem的直接亲属 都要处理*/

            for(int i = 0;i<pItems.size();++i)
            {
                CSpGraphicViewItem* p = pItems[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseEnterItem(p,pInfo);
                p->enterEvent(pInfo);
            }
        }
        else if(pItem == nullptr)
        {
            /*_mouseMoveItem 不为nullptr 但当前pItem为nullptr 表示鼠标从_mouseMoveItem移动到了空白位置
              那么_mouseMoveItem要想赢鼠标移出的操作*/

            for(int i = 0;i<_mousePosItems.size();++i)
            {
                CSpGraphicViewItem* p = _mousePosItems[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseLeaveItem(p,pInfo);
                p->leaveEvent(pInfo);
            }
        }
        else if(_mousePosItem == pItem)
        {
            /* _mouseMoveItem 与 pItem 相同并且都不为nullptr
              那么就是响应鼠标在pItem上的移动事件了*/

            QList<CSpGraphicViewItem*> bothHave;
            QList<CSpGraphicViewItem*> onlyPItemsHave;
            QList<CSpGraphicViewItem*> onlyPosItemsHave = _mousePosItems;
            for(int i = 0;i<pItems.size();++i)
            {
                CSpGraphicViewItem* p = pItems[i];
                int index = onlyPosItemsHave.indexOf(p);
                if(index == -1)
                {
                    onlyPItemsHave.append(p);
                }
                else
                {
                    bothHave.append(p);
                    onlyPosItemsHave.removeAt(index);
                }
            }



            for(int i = 0;i<onlyPosItemsHave.size();++i)
            {
                CSpGraphicViewItem* p = onlyPosItemsHave[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseLeaveItem(p,pInfo);
                p->leaveEvent(pInfo);
            }

            for(int i = 0;i<onlyPItemsHave.size();++i)
            {
                CSpGraphicViewItem* p = onlyPItemsHave[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseEnterItem(p,pInfo);
                p->enterEvent(pInfo);
            }

            for(int i = 0;i<bothHave.size();++i)
            {
                CSpGraphicViewItem* p = bothHave[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseMoveItem(p,pInfo);
                p->mouseMoveEvent(pInfo);
                if(pInfo->isFinished())
                {
                    break;
                }
            }

        }
        else if(isAcontainB(pItem,_mousePosItem,_mousePosItems))
        {
            /*pItem是_mouseMoveItem的祖先 那么_mouseMoveItem与pItem之前的家庭成员要响应鼠标离开事件*/

            int indexBegin = _mousePosItems.indexOf(_mousePosItem);
            int indexEnd   = _mousePosItems.indexOf(pItem);
            for(int i = indexBegin;i<indexEnd;++i)
            {
                CSpGraphicViewItem* p = _mousePosItems[i];
                pInfo->localPos     = p->mapFromGl(pInfo->viewGlPos);
                mouseLeaveItem(p,pInfo);
                p->leaveEvent(pInfo);
            }
        }
        else if(isAcontainB(_mousePosItem,pItem,pItems))
        {
            /*_mouseMoveItem是pItem的祖先 那么pItem与_mouseMoveItem之前的家庭成员要响应鼠标进入事件*/
            int indexBegin = pItems.indexOf(pItem);
            int indexEnd   = pItems.indexOf(_mousePosItem);
            for(int i = indexBegin;i < indexEnd;++i)
            {
                CSpGraphicViewItem* p = pItems[i];
                pInfo->localPos     = p->mapFromGl(pInfo->viewGlPos);
                mouseEnterItem(p,pInfo);
                p->enterEvent(pInfo);
            }
        }
        else
        {
            CSpGraphicViewItem* pSameAnces = linkOfTwoFamily(pItems,_mousePosItems);
            if(pSameAnces == nullptr)
            {
                /*pItem与_mouseMoveItem没有关系
                  pItem及家族响应enterEvent _mouseMoveItem及家族响应leaveEvent*/
                for(int i = 0;i<pItems.size();++i)
                {
                    CSpGraphicViewItem* p = pItems[i];
                    pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                    mouseEnterItem(p,pInfo);
                    p->enterEvent( pInfo);
                }

                for(int i = 0;i<_mousePosItems.size();++i)
                {
                    CSpGraphicViewItem* p = _mousePosItems[i];
                    pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                    mouseLeaveItem(p,pInfo);
                    p->leaveEvent(pInfo);
                }

            }
            else
            {
                /*pItem与_mouseMoveItem有共同祖先pSameAnces
                  pItem及家族(到pSameAnces)响应enterEvent _mouseMoveItem及家族(到pSameAnces)响应leaveEvent  */
                int indexE = pItems.indexOf(pSameAnces);
                for(int i = 0;i<indexE;++i)
                {
                    CSpGraphicViewItem* p = pItems[i];
                    pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                    mouseEnterItem(p,pInfo);
                    p->enterEvent(pInfo);
                }

                indexE = _mousePosItems.indexOf(pSameAnces);
                for(int i = 0;i<indexE;++i)
                {
                    CSpGraphicViewItem* p = _mousePosItems[i];
                    pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                    mouseLeaveItem(p,pInfo);
                    p->leaveEvent(pInfo);
                }
            }
        }
        QPointF posLocal;
        _mousePosItems  = allItemAt(pInfo->viewGlPos);
        _mousePosItem   = (_mousePosItems.isEmpty() ? nullptr:_mousePosItems.first());
        if(_mousePosItem != nullptr)
        {
            posLocal = _mousePosItem->mapFromGl(pInfo->viewGlPos);
            CSpGraphicViewItem::ResizeType rTp = _mousePosItem->canResize(posLocal);
            if(rTp < CSpGraphicViewItem::Resize_Hor)
            {
                setCursor(QCursor(Qt::SizeHorCursor));
            }
            else if(rTp < CSpGraphicViewItem::Resize_Ver)
            {
                setCursor(QCursor(Qt::SizeVerCursor));
            }
            else
            {
                setCursor(QCursor(Qt::ArrowCursor));
            }
        }
        else
        {
            setCursor(QCursor(Qt::ArrowCursor));
        }
        //qDebug()<<"mouse move _mousePosItem = "<<_mousePosItem;
    }
    else
    {
        QPointF point = curGlPos - _mousePressBeginPos;
        if (point.manhattanLength() < 4.0)
        {
            return;
        }
        if(!_mouseMoveHaveMakenOpera)
        {
            _mouseMoveOpera = decideOperate(pInfo,_resizeTp);

            _mouseMoveHaveMakenOpera = true;
        }

        if(_mouseMoveOpera != NoneOperation)
        {
            if(_mouseMoveOpera != DragMapMove)
            //判断是否是在边缘如果是那么QSlider要进行增加或减少值
            moveBorderExec(pInfo);

            if(_mouseMoveOpera == DragMove)
            {
                if(_dragItemInfo == nullptr)
                {
                    _dragItemInfo = new CSpGraphViewDragItemInfo;
                }
                _dragItemInfo->accept            = true;
                _dragItemInfo->acceptPos         = true;
                _dragItemInfo->oldGlPos          = _mousePressPos;
                _dragItemInfo->newGlPos          = curGlPos;
                _dragItemInfo->pGraphViewWidget  = this;
                _dragItemInfo->pressedItem       = _mousePressItem;
                _dragItemInfo->pressedItemOldParent =  itemAt(_mousePressPos);
                _dragItemInfo->pressedItemNewParent =  itemAt(curGlPos);
                _dragItemInfo->pressedItemOldTopParent =  topItemAt(_mousePressPos);
                _dragItemInfo->pressedItemNewTopParent =  topItemAt(curGlPos);

                if(!_moveDrageding)
                {
                    _moveDrageding    = true;
                    _draggedItemsTemp = _draggedItems;
//                    for(auto it = _draggedItems.begin();it != _draggedItems.end();++it)
//                    {
//                        CGraphicViewItem* pItem = (*it);
//                        if(pItem->parentItem() == _mousePressItem->parentItem())
//                        {
//                            _draggedItemsTemp.insert(pItem);
//                        }
//                    }
                    mouseDrageBegin(_draggedItemsTemp/*_draggedItems*/,_dragItemInfo);
                }
                mouseDrageMove(_draggedItemsTemp/*_draggedItems*/,_dragItemInfo);
                if(_dragItemInfo->acceptPos)
                {
                    _mousePressPos = pInfo->viewGlPos;
                    _mousePressPaintPos = pInfo->paintPos;
                }
                return;
            }
            else if(_mouseMoveOpera == SelectMove)
            {
                if(!_moveSelecting)
                {
                    _moveSelecting = true;
                }

                QRectF oldRect = QRectF(QPointF(qMin(_mousePressBeginPos.x(),_mousePressPos.x()),qMin(_mousePressBeginPos.y(),_mousePressPos.y())),
                                        QPointF(qMax(_mousePressBeginPos.x(),_mousePressPos.x()),qMax(_mousePressBeginPos.y(),_mousePressPos.y())));

                viewPort()->update(QRectF(mapGlPosToPaintPos(oldRect.topLeft()) - QPoint(1,1),oldRect.size() + QSize(2,2)).toRect());

                QRectF newRect = QRectF(QPointF(qMin(_mousePressBeginPos.x(),curGlPos.x()),qMin(_mousePressBeginPos.y(),curGlPos.y())),
                                        QPointF(qMax(_mousePressBeginPos.x(),curGlPos.x()),qMax(_mousePressBeginPos.y(),curGlPos.y())));
                mouseSelect(QRectF(QPointF(qMin(_mousePressBeginPos.x(),curGlPos.x()),qMin(_mousePressBeginPos.y(),curGlPos.y())),
                                                QPointF(qMax(_mousePressBeginPos.x(),curGlPos.x()),qMax(_mousePressBeginPos.y(),curGlPos.y()))),
                                         _mousePressItem);

                viewPort()->update(QRectF(mapGlPosToPaintPos(newRect.topLeft()) - QPoint(1,1),newRect.size() + QSize(2,2)).toRect());
            }
            else if(_mouseMoveOpera == ResizeMove)
            {
//                if(!_moveResizing)
//                {
//                    //_mousePressItem->doResizeBegin(_resizeItemInfo);
//                    _mousePressItem->resizeBegin(static_cast<CGraphicViewItem::ResizeType>(_resizeTp));
//                    _mousePressItem->setBoundRectOpera(QRectF(_mousePressItem->mapToGl(QPointF(0.0,0.0)),_mousePressItem->boundRect().size()));
//                    _mousePressItem->_isOperating   = true;
//                }


                if(_resizeItemInfo == nullptr)
                {
                    _resizeItemInfo = new CSpGraphViewResizeItemInfo;
                }

                _resizeItemInfo->pressBeginGlPos = _mousePressBeginPos;
                _resizeItemInfo->oldGlPos = _mousePressPos;
                _resizeItemInfo->newGlPos = pInfo->viewGlPos;
                _resizeItemInfo->resizeTp = _resizeTp;

                if(!_moveResizing)
                {
                    _mousePressItem->doResizeBegin(_resizeItemInfo);
                    _mousePressItem->resizeBegin(static_cast<CSpGraphicViewItem::ResizeType>(_resizeTp));

                }

                _mousePressItem->update();

                _mousePressItem->doResizeMove(_resizeItemInfo);
                _mousePressItem->resizeMove(static_cast<CSpGraphicViewItem::ResizeType>(_resizeTp));

                _mousePressItem->update();

                _moveResizing = true;
                if(_resizeItemInfo->acceptPos)
                {
                    _mousePressPos = pInfo->viewGlPos;
                    _mousePressPaintPos = pInfo->paintPos;
                }
                return;
            }
            else if(_mouseMoveOpera == DragMapMove)
            {
                QPointF prePos = _mousePressPaintPos;
                QPointF nowPos = pInfo->paintPos;

                if(mouseDragMapMove(prePos,nowPos))
                {
                    _mousePressPaintPos = pInfo->paintPos;
                }
                return;
            }
        }
        else
        {
            if(_mousePressItem != nullptr)
            {
                pInfo->localPos  = _mousePressItem->mapFromGl(pInfo->viewGlPos);
                mouseMoveItem(_mousePressItem,pInfo);
               _mousePressItem->mouseMoveEvent(pInfo);
            }
        }
        _mousePressPos = pInfo->viewGlPos;
        _mousePressPaintPos = pInfo->paintPos;
    }
}

void CSpGraphicView::enter(CSpGraphicViewMouseEventInfo *pInfo)
{
    QList<CSpGraphicViewItem*> pItems = allItemAt(pInfo->viewGlPos);
    CSpGraphicViewItem* pItem  = (pItems.isEmpty()?nullptr:pItems.first());

    for(int i = 0;i<pItems.size();++i)
    {
        CSpGraphicViewItem* p = pItems[i];
        pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
        mouseEnterItem(p,pInfo);
        p->enterEvent(pInfo);
    }
    _mousePosItem  = pItem;
    _mousePosItems = pItems;
}

void CSpGraphicView::leave()
{
    if(_mousePosItem != nullptr)
    {
        CSpGraphicViewMouseEventInfo info;
        info.viewGlPos  = mapPaintPosToGlPos(viewPort()->mapFromGlobal(QCursor::pos()));
        info.localPos   = info.viewGlPos;
        info.screenPos  = QCursor::pos();
        info.pGraphViewWidget = this;

        for(int i = 0;i<_mousePosItems.size();++i)
        {
            CSpGraphicViewItem* p = _mousePosItems[i];
            info.localPos = p->mapFromGl(info.viewGlPos);
            mouseLeaveItem(p,&info);
            p->leaveEvent(&info);
        }

        // qDebug()<<"leaveEvent _mousePosItem = "<<_mousePosItem;
        _mousePosItem = nullptr;
        _mousePosItems.clear();
    }
}

void CSpGraphicView::childAddedEvent(CSpGraphicViewChildEventInfo *childEventInfo)
{
    Q_UNUSED(childEventInfo);
}

void CSpGraphicView::childRemovedEvent(CSpGraphicViewChildEventInfo *childEventInfo)
{
    Q_UNUSED(childEventInfo);
}

void CSpGraphicView::drawBackground(QPainter *painter, const QSize &viewSize)
{
    painter->setPen(QColor(0,0,0));
    painter->setBrush(_viewBgColor);
    painter->drawRect(QRect(QPoint(0,0),viewSize));
}

void CSpGraphicView::drawMore(QPainter *painter)
{
    Q_UNUSED(painter)
}

void CSpGraphicView::drawSelect(QPainter *painter, const QRectF &selectRctf)
{
    if(_mouseSelectValid)
    {
        painter->setPen(QColor(11,125,217));
        painter->setBrush(QColor(170,204,255,200));
        painter->drawRect(selectRctf);
    }
}

void CSpGraphicView::mouseSelect(const QRectF &selectRctf, CSpGraphicViewItem* pPressedItem)
{
    Q_UNUSED(pPressedItem);

    clearSelected();

    //选中逻辑
    CSpGraphicViewMouseMoveSelectEventInfo seletInfo;
    seletInfo.pGraphViewWidget = this;
    seletInfo.selectGlRect     = selectRctf;
    QSet<CSpGraphicViewItem*> pItems  = itemAt(seletInfo.selectGlRect);
    for(auto it = pItems.begin();it != pItems.end();++it)
    {
        CSpGraphicViewItem* p = (*it);
        while(p != nullptr)
        {
            p->doSelectMove(&seletInfo);
            p->update();
            if(seletInfo.finished)
            {
                break;
            }
            p = p->parentItem();
        }
    }
}

bool CSpGraphicView::mouseDragMapMove(const QPointF &prePos, const QPointF &newPos)
{
    QPointF move = newPos - prePos;

    qreal newHorValue = horSlider()->value() - move.x();
    qreal newVerValue = verSlider()->value() - move.y();

    horSlider()->setValue(qMin(qMax(newHorValue,0.0),horSlider()->max()));
    verSlider()->setValue(qMin(qMax(newVerValue,0.0),verSlider()->max()));

    return true;
}

void CSpGraphicView::drawResize(QPainter* painter,CSpGraphicViewItem *pPressedItem)
{
    if(_moveResizing)
    {
        paintItem(pPressedItem,painter,viewportRectF(),ResizeMov_Paint);
    }
}

void CSpGraphicView::drawDrag(QPainter *painter, const QSet<CSpGraphicViewItem *> &dragItems)
{
    for(auto it = dragItems.begin();it != dragItems.end();++it)
    {
        CSpGraphicViewItem* pItem = (*it);

        paintItem(pItem,painter,viewportRectF(),DragMov_Paint);
    }
}

void CSpGraphicView::mouseDrageBegin(const QSet<CSpGraphicViewItem *> &dragItems,
                                   CSpGraphViewDragItemInfo *pDragItemInfo)
{
    const QSet<CSpGraphicViewItem *>& tempDragItems = dragItems;
    for(auto it =  tempDragItems.begin();it !=  tempDragItems.end();++it)
    {
        CSpGraphicViewItem * p = (*it);
        p->doDragBegin(pDragItemInfo);
        p->_isOperating = true;
        p->dragBegin();
    }
}

void CSpGraphicView::mouseDrageMove(const QSet<CSpGraphicViewItem *> &dragItems,
                                  CSpGraphViewDragItemInfo *pDragItemInfo)
{
    /*可能在拖拽移动的过程中pDragItemInfo->pressedItem可能被删除了*/
    pDragItemInfo->allAcpt = true;

    if(pDragItemInfo->pressedItem != nullptr)
    pDragItemInfo->pressedItem->update();

    pDragItemInfo->pressedItemHaveDraged = false;

    if(pDragItemInfo->pressedItem != nullptr)
    pDragItemInfo->pressedItem->doDragMove(pDragItemInfo);

    pDragItemInfo->pressedItemHaveDraged = true;

    if(pDragItemInfo->pressedItem != nullptr)
    pDragItemInfo->pressedItem->update();

    if(!pDragItemInfo->accept)
    {
        pDragItemInfo->allAcpt =  false;
    }

    const QSet<CSpGraphicViewItem *>& tempDragItems = dragItems;
    for(auto it =  tempDragItems.begin();it !=  tempDragItems.end();++it)
    {
        CSpGraphicViewItem * p = (*it);

        if(p == pDragItemInfo->pressedItem)
            continue;

        p->update();

        p->doDragMove(pDragItemInfo);

        p->update();

        if(!pDragItemInfo->accept)
        {
            pDragItemInfo->allAcpt = false;
        }
    }
}

void CSpGraphicView::mouseDrageEnd(const QSet<CSpGraphicViewItem *> &dragItems,
                                 CSpGraphViewDragItemInfo *pDragItemInfo)
{
    //if(pDragItemInfo->allAccept())
    {
        const QSet<CSpGraphicViewItem *> tempDragItems = dragItems;
        for(auto it =  tempDragItems.begin();it !=  tempDragItems.end();++it)
        {
            CSpGraphicViewItem* pItem = (*it);
            pItem->doDragEnd(pDragItemInfo);
            pItem->_isOperating = false;
            pItem->dragEnd();
        }
    }
}

void CSpGraphicView::onHorScrollBarRangeChanged_pri(qreal min, qreal max)
{
    if(AsNeed == _horTp)
    {
        if(/*max - min == 0*/qFuzzyIsNull(max - min))
        {
            _horSlider->hide();
        }
        else
        {
            _horSlider->show();
        }
    }
    else if(Never == _horTp)
    {
        _horSlider->hide();
    }
    else
    {
        _horSlider->show();
    }
}

void CSpGraphicView::onVerScrollBarRangeChanged_pri(qreal min, qreal max)
{
    if(AsNeed == _verTp)
    {
        if(qFuzzyIsNull(max - min))
        {
            _verSlider->hide();
        }
        else
        {
            _verSlider->show();
        }
    }
    else if(Never == _verTp)
    {
        _verSlider->hide();
    }
    else
    {
        _verSlider->show();
    }
}

void CSpGraphicView::onHorScrollBarValueChanged_pri(qreal value)
{
    onHorScrollBarValueChanged(value);
    if(viewPort() != nullptr)
    viewPort()->update();
}

void CSpGraphicView::onVerScrollBarValueChanged_pri(qreal value)
{
    onVerScrollBarValueChanged(value);
    if(viewPort() != nullptr)
    viewPort()->update();
}

void CSpGraphicView::onHorScrollBarValueChanged(qreal value)
{
    Q_UNUSED(value);
}

void CSpGraphicView::onVerScrollBarValueChanged(qreal value)
{
    Q_UNUSED(value);
}

void CSpGraphicView::makeDragFlag(CSpGraphicViewItem *pItem)
{
    if(pItem != nullptr)
    {
        bool finished = false;
        if(pItem->draggable())
        {
            if(pItem->isChecked())
            {
                _draggedItems.insert(pItem);
                finished = true;
            }
        }
        if(!finished)
        {
            auto itf = _draggedItems.find(pItem);
            if(itf != _draggedItems.end())
            {
                _draggedItems.erase(itf);
            }

            {
                auto itf = _draggedItemsTemp.find(pItem);
                if(itf != _draggedItemsTemp.end())
                {
                    _draggedItemsTemp.erase(itf);
                }
            }
        }
    }
}

void CSpGraphicView::makeCheckedFlag(CSpGraphicViewItem *pItem)
{
    if(pItem != nullptr)
    {
        if(pItem->isChecked())
        {
            _checkedItems.insert(pItem);
        }
        else
        {
            auto itf = _checkedItems.find(pItem);
            if(itf != _checkedItems.end())
            {
                _checkedItems.erase(itf);
            }
        }
    }

}

void CSpGraphicView::clearAllDrag()
{
    _draggedItems.clear();
}

void CSpGraphicView::clearAllChecked(bool emitChanged)
{
    for(auto it = _checkedItems.begin();it != _checkedItems.end();++it)
    {
        CSpGraphicViewItem* pItem = (*it);
        if(pItem->isChecked())
        {
            pItem->setChecked(false);
            if(emitChanged)
            emit pItem->sig_SelectChanged(false);
            pItem->update();
        }

    }
    _checkedItems.clear();
}

void CSpGraphicView::clearAllMarkOfItem(CSpGraphicViewItem *pItem, bool willDelet)
{
    if(_mousePosItem == pItem)
    {
        _mousePosItem = nullptr;
    }
    if(_mousePressItem == pItem)
    {
        _mousePressItem = nullptr;
    }

    seletBegin_pri();
    //同时还要清楚掉被选中的pItem的孩子
    CSpGraphicViewItem* pChild = pItem->firstChild();
    while(pChild != nullptr)
    {
        notSelect(pChild,willDelet);
        pChild = pItem->nextChild(pChild);
    }
    notSelect(pItem,willDelet);
    seletEnd_pri();

    if(!willDelet)
    {
        addToWaitToDeleteClub(pItem);
    }
}

void CSpGraphicView::setEnableState(CSpGraphicViewItem *pItem, int enableState)
{
    pItem->_enableState = static_cast<CSpGraphicViewItem::ItemEnableState>(enableState);
}

bool CSpGraphicView::isAcontainB(CSpGraphicViewItem *A, CSpGraphicViewItem *B, const QList<CSpGraphicViewItem *> &Bfamily)
{
    int indexA = Bfamily.indexOf(A);
    int indexB = Bfamily.indexOf(B);
    if(indexA != -1)
    {
        if(indexA > indexB)
        {
            return true;
        }
    }
    return false;
}

CSpGraphicViewItem * CSpGraphicView::linkOfTwoFamily(const QList<CSpGraphicViewItem *> &Afamily, const QList<CSpGraphicViewItem *> &Bfamily)
{
    //Afamily.indexOf()
    CSpGraphicViewItem * p = nullptr;
    int iA = Afamily.size() - 1;
    int iB = Bfamily.size() - 1;
    while((iA>=0 && iB>= 0) && (iA<Afamily.size() && iB<Bfamily.size()))
    {
        CSpGraphicViewItem * a = Afamily.at(iA);
        CSpGraphicViewItem * b = Bfamily.at(iB);
        if(a != b)
        {
            break;
        }
        else
        {
            p = a;
        }
        --iA;
        --iB;
    }
    return p;
}

CSpGraphicView::MouseOperationTp CSpGraphicView::decideOperate(CSpGraphicViewMouseEventInfo* pInfo,int& outResizeTp)
{
    if( pInfo->buttons != Qt::LeftButton)
    {
        return NoneOperation;
    }

    MouseOperationTp ret = NoneOperation;
    CSpGraphicViewItem::ResizeType rTp = CSpGraphicViewItem::Resize_None;

    if(_mousePressItem != nullptr)
    {
        if(_mousePressItem->resizeabel())
        {
            rTp = _mousePressItem->canResize(_mousePressBeginLocalPos);
        }
        if(rTp != CSpGraphicViewItem::Resize_None)
        {
            ret         = ResizeMove;
            outResizeTp = rTp;
        }
        else
        {
            if(_mousePressItem->draggable()&& _mousePressItem->isPosDragAble(_mousePressBeginLocalPos))
            {
                ret = DragMove;
            }
            else
            {
                /*如果不在偏移区域 并且是设置了可矩形框选中标记的才能执行矩形选中*/
                if( mouseSelectValid() && !offsetAreaInPaint().contains(_mousePressPaintBeginPos.toPoint()))
                {
                    ret = SelectMove;
                }
            }
        }
    }
    else
    {
        ret = SelectMove;
    }
    return ret;
}

bool CSpGraphicView::isScrollShouldHorMoveWhenOperateToBorder(CSpGraphicViewMouseEventInfo *pInfo, int &move)
{   
    const int border = 20;

    QRectF clipBecaOffsetArea = clipRectBecauseOffsetArea();

    move = 0;
    if(pInfo->paintPos.x() - clipBecaOffsetArea.left() < border)
    {
        //水平线减少
        move = -1;
    }
    else if(pInfo->paintPos.x() > viewPort()->width() - border)
    {
        //水平线增加
        move = 1;
    }
    return (move != 0);
}

bool CSpGraphicView::isScrollShouldVerMoveWhenOperateToBorder(CSpGraphicViewMouseEventInfo *pInfo,int& move)
{
    const int border = 20;

    QRectF clipBecaOffsetArea = clipRectBecauseOffsetArea();

    move = 0;
    if(pInfo->paintPos.y() - clipBecaOffsetArea.top() < border)
    {
        //水平线减少
        move = -1;
    }
    else if(pInfo->paintPos.y()> viewPort()->height() - border)
    {
        //水平线增加
        move = 1;
    }
    return (move!=0);
}

void CSpGraphicView::moveBorderExec(CSpGraphicViewMouseEventInfo *pInfo)
{
    int moveX = 0;
    if(isScrollShouldHorMoveWhenOperateToBorder(pInfo,moveX))
    {
        _horMoveValue      = moveX;
        _tempMoveMouseInfo.paintPos = pInfo->paintPos;
        if(_horMove == nullptr ||(_horMove != nullptr && _horMove->state() != QPropertyAnimation::Running))
        {
            _tempMoveMouseInfo = *pInfo;
        }
        horMoveExec();
    }
    else
    {
        stopHorMove();
    }

    int moveY = 0;
    if(isScrollShouldVerMoveWhenOperateToBorder(pInfo,moveY))
    {
        _verMoveValue  = moveY;
        _tempMoveMouseInfo.paintPos = pInfo->paintPos;
        if(_verMove == nullptr ||(_verMove != nullptr && _verMove->state() != QPropertyAnimation::Running))
        {
            _tempMoveMouseInfo = *pInfo;
        }
        verMoveExec();
    }
    else
    {
        stopVerMove();
    }
}

inline void CSpGraphicView::horMoveExec()
{
    if(_horMove == nullptr)
    {
        _horMove = new QVariantAnimation(_horSlider);
        connect(_horMove,&QVariantAnimation::valueChanged,this,[=](const QVariant &value)
        {
            if(!_horRunning)
                return;

            qreal intValue = value.toDouble();

            _horSlider->setValue(intValue);

            QPointF curGlPos = mapPaintPosToGlPos(_tempMoveMouseInfo.paintPos);

            _tempMoveMouseInfo.viewGlPos        = curGlPos;
            _tempMoveMouseInfo.localPos         = curGlPos;

            mouseMove(&_tempMoveMouseInfo);

        });
        connect(_horMove,&QVariantAnimation::currentLoopChanged,this,[=](int currentLoop)
        {
            Q_UNUSED(currentLoop);
            if(!_horRunning)
                return;

            qreal value = _horSlider->value() + _horMoveValue * 10;

            _horMove->setStartValue(value);
            _horMove->setEndValue(value + _horMoveValue * 10);

        });

    }
    if(!_horRunning)
    {
        _horRunning = true;
        _horMove->setStartValue(_horSlider->value());
        _horMove->setEndValue(_horSlider->value() + _horMoveValue*10);
        _horMove->setDuration(40);
        _horMove->setLoopCount(-1);
        _horMove->start();
    }
}

inline void CSpGraphicView::verMoveExec()
{
    if(_verMove == nullptr)
    {
        _verMove = new QVariantAnimation(_verSlider);
        connect(_verMove,&QVariantAnimation::valueChanged,this,[=](const QVariant &value)
        {
            if(!_verRunning)
                return;

            int intValue = value.toInt();
            _verSlider->setValue(intValue);

            QPointF curGlPos = mapPaintPosToGlPos(_tempMoveMouseInfo.paintPos);

            _tempMoveMouseInfo.viewGlPos        = curGlPos;
            _tempMoveMouseInfo.localPos         = curGlPos;

            mouseMove(&_tempMoveMouseInfo);

        });
        connect(_verMove,&QVariantAnimation::currentLoopChanged,this,[=](int currentLoop)
        {
            Q_UNUSED(currentLoop);
            if(!_verRunning)
                return;

            qreal value = _verSlider->value() + _verMoveValue * 10;

            _verMove->setStartValue(value);
            _verMove->setEndValue(value + _verMoveValue * 10);

        });

    }
    if(!_verRunning)
    {
        _verRunning = true;
        _verMove->setStartValue(_verSlider->value());
        _verMove->setEndValue(_verSlider->value() + _verMoveValue*10);
        _verMove->setDuration(40);
        _verMove->setLoopCount(-1);
        _verMove->start();
    }
}

inline void CSpGraphicView::stopHorMove()
{
    if(_horMove !=nullptr && (_horMove->state() == QVariantAnimation::Running))
    {
        _horMove->setLoopCount(0);
        _horMove->pause();
    }
    _horRunning = false;
}

inline void CSpGraphicView::stopVerMove()
{
    if(_verMove !=nullptr && (_verMove->state() == QVariantAnimation::Running))
    {
        _verMove->setLoopCount(0);
        _verMove->pause();
    }
    _verRunning = false;
}

void CSpGraphicView::drawAll()
{
    drawToContex();

    QPainter painter(_viewport);
    painter.drawPixmap(QPoint(0,0),_contex);

    drawToWidgetMore(&painter);
}

void CSpGraphicView::drawToContex()
{
    _contex.fill(QColor(0,0,0,0));
    QPainter painter(&_contex);

    painter.setClipping(true);
    painter.setFont(_viewport->font());


    painter.save();
    this->drawBackground(&painter,size());
    painter.restore();

    painter.save();
    this->drawItems(&painter);
    painter.restore();

    painter.save();
    drawOffsetAreaLine(&painter);
    painter.restore();

    CSpGraphicView::MouseOperationTp tp = this->_mouseMoveOpera;
    if(tp == CSpGraphicView::DragMove)
    {
        painter.save();
        this->drawDrag(&painter,this->_draggedItems);
        painter.restore();
    }
    else if(tp == CSpGraphicView::ResizeMove)
    {
        painter.save();
        this->drawResize(&painter,this->_mousePressItem);
        painter.restore();
    }
    else if(tp == CSpGraphicView::SelectMove)
    {
        painter.save();
        painter.setClipRect(clipRectBecauseOffsetArea());
        this->drawSelect(&painter,QRectF(this->mapGlPosToPaintPos(this->_mousePressBeginPos),                                       this->mapGlPosToPaintPos(this->_mousePressPos)));
        painter.restore();
    }

    painter.save();
    painter.setClipRect(clipRectBecauseOffsetArea());
    this->drawMore(&painter);
    painter.restore();
}

void CSpGraphicView::drawToWidgetMore(QPainter *painter)
{
    Q_UNUSED(painter);
}

void CSpGraphicView::resizeContex()
{
    qreal dpr = _viewport->devicePixelRatioF();
    _contex   = QPixmap(_viewport->size()*dpr);
    _contex.setDevicePixelRatio(dpr);
}

bool CSpGraphicView::viewShow()
{
    int offsetX = 0;
    int offsetY = 0;
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        if(pMode->model() == CSpGraphicStructModel::List_Model)
        {
            offsetX = qRound(pMode->offsetWidth());
        }
        else if(pMode->model() == CSpGraphicStructModel::Row_Model)
        {
            offsetY = qRound(pMode->offsetWidth());
        }
    }

    if(_verSliderLayOut != nullptr)
    _verSliderLayOut->setContentsMargins(0, offsetY, 0, 0);

    if(_horSliderLayOut != nullptr)
    _horSliderLayOut->setContentsMargins(offsetX, 0, 0, 0);

    return false;
}

void CSpGraphicView::drawOffsetAreaLine(QPainter *painter)
{
    int offsetWidth = 0;
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        offsetWidth = qRound(pMode->offsetWidth());;
        if(pMode->model() == CSpGraphicStructModel::List_Model)
        {
            if(offsetWidth > 0)
            {
                painter->setPen(QColor(0,0,0));
                painter->drawLine(QLineF(QPointF(offsetWidth/* - 2*/,0),
                                       QPointF(offsetWidth/* - 2*/,_viewport->height())));
            }
        }
        else if(pMode->model() == CSpGraphicStructModel::Row_Model)
        {
            if(offsetWidth > 0)
            {
                painter->setPen(QColor(0,0,0));
                painter->drawLine(QLineF(QPointF(0,offsetWidth/* - 2*/),
                                       QPointF(_viewport->width(),offsetWidth /*- 2*/)));
            }
        }
    }
}

QRect CSpGraphicView::clipRectBecauseOffsetArea()
{
    int offsetWidth = 0;
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        offsetWidth = qRound(pMode->offsetWidth());;
        if(pMode->model() == CSpGraphicStructModel::List_Model)
        {
            if(offsetWidth > 0)
            {
                return QRect(offsetWidth,0,_viewport->width() - offsetWidth,_viewport->height());
            }
        }
        else if(pMode->model() == CSpGraphicStructModel::Row_Model)
        {
            if(offsetWidth > 0)
            {
                return QRect(0,offsetWidth,_viewport->width(),_viewport->height() - offsetWidth);
            }
        }
    }
    return _viewport->rect();
}

QRectF CSpGraphicView::offsetAreaInPaint()
{
    qreal offsetWidth = 0;
    CSpGraphicStructModel* pMode = structModel();
    if(pMode != nullptr)
    {
        offsetWidth = pMode->offsetWidth();
        if(pMode->model() == CSpGraphicStructModel::List_Model)
        {
            if(offsetWidth > 0)
            {
                return QRectF(0,0,offsetWidth,_viewport->height());
            }
        }
        else if(pMode->model() == CSpGraphicStructModel::Row_Model)
        {
            if(offsetWidth > 0)
            {
                return QRectF(0,0,_viewport->width(),offsetWidth);
            }
        }
    }
    return QRectF(0,0,0,0);
}

CSpGraphicViewItem *CSpGraphicView::findFirstCheckableItemInList(QList<CSpGraphicViewItem *> *itemsList)
{
    for(int i = 0;i<itemsList->size();++i)
    {
        CSpGraphicViewItem* pItem = itemsList->at(i);
        if(pItem->isCheckable())
            return pItem;
    }
    return nullptr;
}

void CSpGraphicView::setItemListItemCheckableAncestors(QList<CSpGraphicViewItem *> *itemsList, CSpGraphicViewItem *pItem, bool checked)
{
    bool found = false;
    for(int i = 0;i<itemsList->size();++i)
    {
        CSpGraphicViewItem* p = itemsList->at(i);
        if(found)
        {
            if(p->isCheckable())
            {
                /*如果pItem只是mouseInfo->pressItem的兄弟要不要也选中？ 这里不选中*/
                if(!isBrother(p,pItem))
                checked?select(p,false):notSelect(p,false);
            }
        }
        else
        {
            if(p == pItem)
            {
                found = true;
            }
        }
    }
}

bool CSpGraphicView::isBrother(CSpGraphicViewItem *pItemA, CSpGraphicViewItem *pItemB)
{
    if(pItemA == pItemB)
        return false;
    return (pItemA->parentItem() == pItemB->parentItem());
}

void CSpGraphicView::mouseEnterItem(CSpGraphicViewItem *pItem, CSpGraphicViewMouseEventInfo *mouseInfo)
{
    Q_UNUSED(mouseInfo);
    pItem->_enableState = CSpGraphicViewItem::state_Hover;
    pItem->update();
}

void CSpGraphicView::mouseLeaveItem(CSpGraphicViewItem *pItem, CSpGraphicViewMouseEventInfo *mouseInfo)
{
    Q_UNUSED(mouseInfo);
    pItem->_enableState = CSpGraphicViewItem::state_Normal;
    pItem->update();
}

void CSpGraphicView::mouseMoveItem(CSpGraphicViewItem *pItem, CSpGraphicViewMouseEventInfo *mouseInfo)
{
    Q_UNUSED(mouseInfo);
    pItem->_enableState = CSpGraphicViewItem::state_Hover;

    //mouseInfo->makeFinished();
}

void CSpGraphicView::mouseDoubleClickedItem(CSpGraphicViewItem *pItem,
                                          CSpGraphicViewMouseEventInfo *mouseInfo)
{
    emit itemDoubleClicked(pItem);

    emit pItem->sig_DoubleClicked();

    pItem->mouseDoubleClickEvent(mouseInfo);

    //mouseInfo->makeFinished();
}

void CSpGraphicView::mouseBtnPressItem(CSpGraphicViewItem *pItem, CSpGraphicViewMouseEventInfo *mouseInfo)
{
    /*这里鼠标左键是选中问题的触发前提*/
    if(mouseInfo->button == Qt::LeftButton)
    {
        if(pItem->isCheckable())
        {
            /*这里只处理未点下ctrl时的选中问题(点下ctrl的是否选中问题交给鼠标释放(mouseBtnReleaseItem)时来做)*/
            if(!(mouseInfo->mdifiers & Qt::ControlModifier))
            {
                /*未按住ctrl时的鼠标左键点击分两种情况：
                 * 1.如果被点的顶层那个item是未选中状态的 那么在这里要先取消之前的选中。
                 * 2.如果被点的顶层那个item是选中状态的 那么可能的清除选中操作要放到release时去做(参见mouseBtnReleaseItem的B-a分支)
                 * 这里处理1情况:
                 * (一次点下应该仅响应一次 避免该次的下一个mouseBtnPressItem循环调用(的父item)进来时清掉孩子的选中状态
                 * 清理的时机是 通过pItem是在族关系的最上层的第一个可checkeable的item)*/
                if(!mouseInfo->pressItem->isChecked())
                {
                    if(findFirstCheckableItemInList(mouseInfo->pressItems) == pItem)
                    {
                        //clearSelected();
                        clearSelectedExcept(pItem);
                    }
                }

                /*如果pItem只是mouseInfo->pressItem的兄弟要不要也选中？ 这里不选中*/

                if(!isBrother(mouseInfo->pressItem,pItem))
                select(pItem);
            }
        }
    }
    pItem->_enableState = CSpGraphicViewItem::state_Pressed;

    pItem->update();

    //mouseInfo->makeFinished();
}

void CSpGraphicView::mouseBtnReleaseItem(CSpGraphicViewItem *pItem, CSpGraphicViewMouseEventInfo *mouseInfo)
{
    if(mouseInfo->button == Qt::LeftButton)
    {
        if(mouseInfo->mdifiers & Qt::ControlModifier)
        {
            /*点下了ctrl的鼠标左键释放 这种情况的选中逻辑应该是:
             * 在整个鼠标下的item列表的第一个可选中的item要设置成和它当前相反的选中状态，
             * 然后其父item链(也就是列表的其他可选中item)也应该设置成和它一样的选中状态
             * 实现方式:1.找到第一个可选中的第一个item然后设置成和它当前相反的选中状态，
                       2.然后把链表内的所有可选中的其他item设置成相反的选中状态(注意不要发送信号到外界了哈 因为这种实现方式是取巧的嘛) 下次进来时就会设置成一样的状态了(因为B分支)*/

            if(pItem->isCheckable())
            {
                if(findFirstCheckableItemInList(mouseInfo->pressItems) == pItem)
                {
                    pItem->isChecked()?notSelect(pItem):select(pItem);

                    bool checked = pItem->isChecked();

                    setItemListItemCheckableAncestors(mouseInfo->pressItems,pItem,!checked);
                }
                else
                {
                    //B分支
                    if(!isBrother(pItem,mouseInfo->pressItem))
                    pItem->isChecked()?notSelect(pItem):select(pItem);
                }
            }
        }
        else
        {
            if(pItem->isCheckable())
            {
                /*B-a分支   如果选中的item被未按住ctrl单击在鼠标释放时 要清理当前的不包括自身及自身父族的选中items
                            实现方式:清理除了自身的items 然后再去选中自身父族items(要不要判断一下当前鼠标有没有移除到item外了呢？)*/
                bool haveOperated =  (mouseInfo->haveDraged || mouseInfo->haveSelect);
                if(findFirstCheckableItemInList(mouseInfo->pressItems) == pItem  && !haveOperated)
                {
                    //clearSelected();
                    clearSelectedExcept(pItem);
                }
                /*判断一下当前鼠标有没有移除到item外？这里判断了*/
                if(pItem->contains(mouseInfo->localPos,true)/*pItem->selfBoundRect().contains(mouseInfo->localPos)*/)
                {
                    /*如果pItem是mouseInfo->pressItem的兄弟要不要也选中？ 这里不选中*/
                    if(!isBrother(mouseInfo->pressItem,pItem))
                    {
                        select(pItem);
                    }
                }
            }
        }

        if(pItem->contains(mouseInfo->localPos,true)/*pItem->selfBoundRect().contains(mouseInfo->localPos)*/)
        {
            pItem->_enableState = CSpGraphicViewItem::state_Hover;

            emit itemClicked(pItem);
            emit pItem->sig_Clicked();

            pItem->onClicked();
        }
        else
        {
            pItem->_enableState = CSpGraphicViewItem::state_Normal;
        }
    }

    pItem->update();
    //mouseInfo->makeFinished();
}

void CSpGraphicView::handleOperateEventWhenRelease(CSpGraphicViewMouseEventInfo *pInfo)
{
    Q_UNUSED(pInfo);

    if(_mouseMoveOpera == DragMove)
    {
        mouseDrageEnd(_draggedItemsTemp/*_draggedItems*/,_dragItemInfo);
        _draggedItemsTemp.clear();
    }
    else if(_mouseMoveOpera == ResizeMove)
    {
        CSpGraphicViewItem* pItem = _mousePressItem;
        pItem->doResizeEnd(_resizeItemInfo);
        pItem->_isOperating = false;
        pItem->resizeEnd(static_cast<CSpGraphicViewItem::ResizeType>(_resizeTp));
    }
}

void CSpGraphicView::handleNormalEventWhenRelease(CSpGraphicViewMouseEventInfo *pInfo)
{
    /*进行位置判断*/
    QList<CSpGraphicViewItem*> pItems = allItemAt(pInfo->viewGlPos);
    CSpGraphicViewItem* pItem  =  (pItems.isEmpty()?nullptr:pItems.first());


    /*先判断点击press位置的Item(_mousePressItem) 和释放时Item(pItem) 的关系*/
    if(_mousePressItem == nullptr)
    {
        /*_mousePressItem 什么都不用做 直接处理鼠标移入pItem
          鼠标移入事件不用考虑CGraphicViewMouseEventInfo的finish标记 所有pItem的直接亲属 都要处理*/

        for(int i = 0;i<pItems.size();++i)
        {
            CSpGraphicViewItem* p = pItems[i];
            pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
            mouseEnterItem(p,pInfo);
            p->enterEvent(pInfo);
        }
    }
    else if(pItem == nullptr)
    {
        /*_mousePressItem 不为nullptr 但当前pItem为nullptr 表示鼠标从_mousePressItem移动到了空白位置
          那么_mousePressItem要响应 鼠标按钮释放 和 鼠标移出的操作*/

        for(int i = 0;i<_mousePressItems.size();++i)
        {
            CSpGraphicViewItem* p = _mousePressItems[i];
            pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
            mouseBtnReleaseItem(p,pInfo);
            p->mouseReleaseEvent(pInfo);
            if( pInfo->isFinished())
            {
                break;
            }
        }
        for(int i = 0;i<_mousePressItems.size();++i)
        {
            CSpGraphicViewItem* p = _mousePressItems[i];
            pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
            mouseLeaveItem(p,pInfo);
            p->leaveEvent(pInfo);
        }
    }
    else if(_mousePressItem == pItem)
    {
        /* _mousePressItem 与 pItem 相同并且都不为nullptr
          那么就是响应鼠标在pItem上的按钮释放事件了*/

        for(int i = 0;i<pItems.size();++i)
        {
            CSpGraphicViewItem* p = pItems[i];
            pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
            mouseBtnReleaseItem(p,pInfo);
            p->mouseReleaseEvent(pInfo);
            if(pInfo->isFinished())
            {
                break;
            }
        }
    }
    else if(isAcontainB(pItem,_mousePressItem,_mousePressItems))
    {
        /* pItem 是 _mousePressItem的祖先 那么_mousePressItem与pItem之前的家庭成员要响应 鼠标按钮释放 和 鼠标离开事件*/
        for(int i = 0;i<_mousePressItems.size();++i)
        {
            CSpGraphicViewItem* p = _mousePressItems[i];
            pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
            mouseBtnReleaseItem(p,pInfo);
            p->mouseReleaseEvent(pInfo);
            if( pInfo->isFinished())
            {
                break;
            }
        }
        for(int i = 0;i<_mousePressItems.size();++i)
        {
            CSpGraphicViewItem* p = _mousePressItems[i];
            pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
            mouseLeaveItem(p,pInfo);
            p->leaveEvent(pInfo);
        }
    }
    else if(/*pItem->isAncestors(_mousePressItem)*/isAcontainB(_mousePressItem,pItem,pItems))
    {
        /*_mousePressItem是pItem的祖先 那么
         * _mousePressItem要响应鼠标释放事件
         * pItem与_mousePressItem之前的家庭成员要响应鼠标进入事件*/
        for(int i = 0;i<_mousePressItems.size();++i)
        {
            CSpGraphicViewItem* p = _mousePressItems[i];
            pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
            mouseBtnReleaseItem(p,pInfo);
            p->mouseReleaseEvent(pInfo);
            if( pInfo->isFinished())
            {
                break;
            }
        }
        int indexE = pItems.indexOf(_mousePressItem);
        for(int i = 0;i<indexE;++i)
        {
            CSpGraphicViewItem* p = pItems[i];
            pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
            mouseEnterItem(p,pInfo);
            p->enterEvent(pInfo);
        }
    }
    else
    {
        CSpGraphicViewItem* pSameAnces = /*pItem->getSameAncestorsWith(_mousePressItem)*/linkOfTwoFamily(pItems,_mousePressItems);
        if(pSameAnces == nullptr)
        {
            /*pItem与_mousePressItem没有关系
              _mousePressItem及家族响应releaseEvent和leaveEvent
              pItem及家族响应enterEvent */

            for(int i = 0;i<_mousePressItems.size();++i)
            {
                CSpGraphicViewItem* p = _mousePressItems[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseBtnReleaseItem(p,pInfo);
                p->mouseReleaseEvent(pInfo);
                if( pInfo->isFinished())
                {
                    break;
                }
            }

            for(int i = 0;i<_mousePressItems.size();++i)
            {
                CSpGraphicViewItem* p = _mousePressItems[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseLeaveItem(p,pInfo);
                p->leaveEvent(pInfo);
            }

            for(int i = 0;i<pItems.size();++i)
            {
                CSpGraphicViewItem* p = pItems[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseEnterItem(p,pInfo);
                p->enterEvent(pInfo);
            }
        }
        else
        {
            /*pItem与_mouseMoveItem有共同祖先pSameAnces
              _mouseMoveItem及家族(到pSameAnces)响应leaveEvent
              _mouseMoveItem及家族响应releaseEvent
              pItem及家族(到pSameAnces)响应enterEvent */

            int indexE = _mousePressItems.indexOf(pSameAnces);
            for(int i = 0;i<indexE;++i)
            {
                CSpGraphicViewItem* p = _mousePressItems[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseBtnReleaseItem(p,pInfo);
                p->mouseReleaseEvent(pInfo);
                if( pInfo->isFinished())
                {
                    break;
                }
            }

            for(int i = 0;i<indexE;++i)
            {
                CSpGraphicViewItem* p = _mousePressItems[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseLeaveItem(p,pInfo);
                p->leaveEvent(pInfo);
            }

            indexE = pItems.indexOf(pSameAnces);
            for(int i = 0;i<indexE;++i)
            {
                CSpGraphicViewItem* p = pItems[i];
                pInfo->localPos = p->mapFromGl(pInfo->viewGlPos);
                mouseEnterItem(p,pInfo);
                p->enterEvent(pInfo);
            }
        }
    }
}

