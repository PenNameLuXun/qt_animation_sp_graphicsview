#include "cspgraphicviewmouseeventinfo.h"
#include <QMouseEvent>


CSpGraphicViewMouseEventInfo::CSpGraphicViewMouseEventInfo():
    localPos(QPoint(0,0)),
    screenPos(QPoint(0,0)),
    finish(false)
{

}

CSpGraphicViewMouseEventInfo::CSpGraphicViewMouseEventInfo(QMouseEvent* event):
    localPos(event->localPos()),
    screenPos(event->globalPos()),
    button(event->button()),
    buttons(event->buttons()),
    mdifiers(event->modifiers()),
    finish(false)
{
}

CSpGraphicViewMouseEventInfo::CSpGraphicViewMouseEventInfo(QMouseEvent* event,const QPointF &pos):
    localPos(pos),
    screenPos(event->globalPos()),
    button(event->button()),
    buttons(event->buttons()),
    mdifiers(event->modifiers()),
    finish(false)
{

}

CSpGraphicViewMouseEventInfo::CSpGraphicViewMouseEventInfo(CSpGraphicViewMouseEventInfo &data, const QPointF &pos):
    localPos(pos),
    screenPos(data.screenPos),
    button(data.button),
    buttons(data.buttons),
    mdifiers(data.mdifiers),
    finish(false)
{

}

CSpGraphicViewMouseEventInfo &CSpGraphicViewMouseEventInfo::operator=(const CSpGraphicViewMouseEventInfo &data)
{
    localPos  = data.localPos;
    windowPos = data.windowPos;
    viewGlPos = data.viewGlPos;
    screenPos = data.screenPos;
    button    = data.button;
    buttons   = data.buttons;
    mdifiers  = data.mdifiers;
    source    = data.source;
    finish    = data.finish;

    pGraphViewWidget = data.pGraphViewWidget;
    haveDraged = data.haveDraged;
    haveSelect = data.haveSelect;
    haveResize = data.haveResize;
    pressItem  = data.pressItem;
    pressItems = data.pressItems;
    return *this;
}

//const QPointF &CGraphicViewMouseEventInfo::pos()
//{
//    return localPos;
//}

//const QPointF &CGraphicViewMouseEventInfo::glPos()
//{
//    return screenPos;
//}

//const Qt::MouseButton &CGraphicViewMouseEventInfo::mouseBtn()
//{
//    return button;
//}

//const Qt::MouseButtons &CGraphicViewMouseEventInfo::mouseBtns()
//{
//    return buttons;
//}

//const Qt::KeyboardModifiers &CGraphicViewMouseEventInfo::modifiers()
//{
//    return mdifiers;
//}

void CSpGraphicViewMouseEventInfo::cancelModifier(Qt::KeyboardModifiers f)
{
    mdifiers = mdifiers&(~f);
}

void CSpGraphicViewMouseEventInfo::makeFinished()
{
    finish = true;
}

bool CSpGraphicViewMouseEventInfo::isFinished()
{
    return finish;
}
