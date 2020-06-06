#ifndef CSPTESTITEM_H
#define CSPTESTITEM_H
#include "cspgraphicviewanimationindexitem.h"

class CSpTestItem:public CSpGraphicViewAnimationIndexItem
{
    Q_OBJECT
public:
    CSpTestItem(CSpGraphicViewItem* pFatherItem = nullptr);
    ~CSpTestItem();

protected:
    void paintSelf(QPainter* painter,const CSpGraphicViewPaintEventInfo *paintInfo);
};

#endif // CSPTESTITEM_H
