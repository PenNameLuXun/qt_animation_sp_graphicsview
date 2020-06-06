#ifndef CGRAPHICVIEWANIMATIONINDEXITEM_H
#define CGRAPHICVIEWANIMATIONINDEXITEM_H
#include "cspgraphicviewanimationitem.h"

class GRAPHICVIEW_EXPORT CSpGraphicViewAnimationIndexItem:public CSpGraphicViewAnimationItem
{
    Q_OBJECT
public:
    CSpGraphicViewAnimationIndexItem(CSpGraphicViewItem* pFatherItem = nullptr);
    ~CSpGraphicViewAnimationIndexItem();

    int    index();
    virtual void setBoundRectOpera(const QRectF& rct);

protected:
    inline int key(){return static_cast<int>(boundRect().bottom());}

    int      _index = -1;
public:
    static void setIndexItemIndex(CSpGraphicViewAnimationIndexItem* pItem,int index);
};

#endif // CGRAPHICVIEWANIMATIONINDEXITEM_H
