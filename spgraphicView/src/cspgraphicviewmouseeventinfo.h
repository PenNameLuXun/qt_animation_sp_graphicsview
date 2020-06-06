#ifndef CGRAPHICVIEWMOUSEEVENTINFO_H
#define CGRAPHICVIEWMOUSEEVENTINFO_H
#include <QPointF>
#include <QRectF>
#include <QColor>
#include "spgraphicview_global.h"
class QMouseEvent;
class CSpGraphicView;
class CSpGraphicViewItem;
//class CGraphicViewTopItem;
class GRAPHICVIEW_EXPORT CSpGraphicViewMouseEventInfo
{
public:
    CSpGraphicViewMouseEventInfo();

    CSpGraphicViewMouseEventInfo(QMouseEvent* event);
    CSpGraphicViewMouseEventInfo(QMouseEvent* event,const QPointF& pos);
    CSpGraphicViewMouseEventInfo(CSpGraphicViewMouseEventInfo& data,const QPointF& pos);
    CSpGraphicViewMouseEventInfo& operator=(const CSpGraphicViewMouseEventInfo& data);

    void cancelModifier(Qt::KeyboardModifiers f);
    void makeFinished();
    bool isFinished();
public:
    QPointF               paintPos;
    QPointF               localPos;
    QPointF               viewGlPos;
    QPointF               windowPos;
    QPointF               screenPos;
    Qt::MouseButton       button   = Qt::NoButton;
    Qt::MouseButtons      buttons  = Qt::NoButton;
    Qt::KeyboardModifiers mdifiers = Qt::NoModifier;
    Qt::MouseEventSource  source;
    bool                  finish = false;

    CSpGraphicView*         pGraphViewWidget         = nullptr;
    bool                  haveDraged               = false;      /*判断是否有拖拽过*/
    bool                  haveSelect               = false;      /*判断是否有选择过*/
    bool                  haveResize               = false;
    CSpGraphicViewItem*     pressItem                = nullptr;
    QList<CSpGraphicViewItem*>* pressItems           = nullptr;    /*点下时 所有处于该点下的items*/
};
#define LOCALPAINT 0
#define GLOBALPAINT 100
#define OTHERPAINT  200
enum PaintReson{Normal_Paint = LOCALPAINT,
                DragOrg_Paint,
                ResizeOrg_Paint,
                Selfie_Paint,

                Global_Paint     = GLOBALPAINT,
                DragMov_Paint,
                ResizeMov_Paint,


                Other_Paint  = OTHERPAINT
               };
class QWidget;
class GRAPHICVIEW_EXPORT CSpGraphicViewPaintEventInfo
{
public:
    QRect      updateRect;
public:
    QRectF     rectF;
    QRectF     visiRectF;
    bool       isEnable;
    bool       isChecked;
    int        enableState;
    QColor     borderColor;
    QColor     bgColor;

    PaintReson reson;

    QWidget* pViewPort;
};
class GRAPHICVIEW_EXPORT CSpGraphicViewItemMoveEventInfo
{
public:
    QPointF               oldPos;
    QPointF               newPos;
};

class GRAPHICVIEW_EXPORT CSpGraphViewDragItemInfo
{
public:
    QPointF               oldGlPos;
    QPointF               newGlPos;
    QPointF               realMove = QPoint(0,0);
    CSpGraphicViewItem*     pressedItem                      = nullptr;
    CSpGraphicViewItem*     pressedItemOldParent             = nullptr;
    CSpGraphicViewItem*     pressedItemNewParent             = nullptr;
    CSpGraphicViewItem*     pressedItemOldTopParent          = nullptr;
    CSpGraphicViewItem*     pressedItemNewTopParent          = nullptr;
    CSpGraphicView*         pGraphViewWidget                 = nullptr;
    bool                  pressedItemHaveDraged            = false;
    bool                  accept                           = true;
    bool                  acceptPos                        = true;

    bool                  allAccept()const{return allAcpt;}
private:
    bool                  allAcpt                        = true;

    friend class CSpGraphicView;
};
class GRAPHICVIEW_EXPORT CSpGraphViewResizeItemInfo
{
public:
    QPointF               pressBeginGlPos;
    QPointF               oldGlPos;
    QPointF               newGlPos;
    QPointF               realMove = QPoint(0,0);
    int                   resizeTp = 6;
    CSpGraphicView*   pGraphViewWidget                 = nullptr;
    bool                  accept                           = true;
    bool                  acceptPos                        = true;
};

class  GRAPHICVIEW_EXPORT CSpGraphicViewMouseMoveSelectEventInfo
{
public:
    QRectF                selectGlRect;
    CSpGraphicView*   pGraphViewWidget                 = nullptr;
    bool finished = false;
};

class  GRAPHICVIEW_EXPORT CSpGraphicViewChildEventInfo
{
public:
    enum ChildEventType{Added,Removed,Count};
    ChildEventType type;
    CSpGraphicViewItem* pItem          = nullptr;
    CSpGraphicView*   pGraphViewWidget = nullptr;
};
#endif // CGRAPHICVIEWMOUSEEVENTINFO_H
