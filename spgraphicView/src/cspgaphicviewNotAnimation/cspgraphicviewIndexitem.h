#ifndef CGRAPHICVIEWINDEXITEM_H
#define CGRAPHICVIEWINDEXITEM_H
#include "cspgraphicviewitem.h"
#include "cspgraphicviewlistitem.h"

class CSpGraphicViewListWidget;
class GRAPHICVIEW_EXPORT CSpGraphViewIndexItem:public CSpGraphicViewItem
{
    Q_OBJECT
public:
    CSpGraphViewIndexItem(CSpGraphicViewItem* parentItem = nullptr);

    ~CSpGraphViewIndexItem();

    int    index();

    QRectF boundRect();

    void   setBoundRect(const QRectF& rctF);
protected:
    void   paintSelf(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo);

    virtual void setBoundRectOpera(const QRectF& rct);
protected:
    inline int key(){return static_cast<int>(_rct.bottom());}

    int      _index = -1;

    QRectF   _rct;

    friend class CSpGraphicViewListWidget;
    friend class CSpGraphicViewMapListWidget;

    friend class CSpGraphicViewListItem;
    friend class CSpGraphViewIndexListItem;

public:
    static void setIndexItemIndex(CSpGraphViewIndexItem* pItem,int index);
};

#endif // CGRAPHICVIEWINDEXITEM_H
