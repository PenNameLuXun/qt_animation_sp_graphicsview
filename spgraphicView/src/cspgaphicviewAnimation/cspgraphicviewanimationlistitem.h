﻿#ifndef CGRAPHICVIEWANIMATIONLISTITEM_H
#define CGRAPHICVIEWANIMATIONLISTITEM_H

#include "cspgraphicviewanimationitem.h"

class CSpGraphicViewAnimationIndexItem;
class GRAPHICVIEW_EXPORT CSpGraphicViewAnimationListItem:public CSpGraphicViewAnimationItem
{
    Q_OBJECT
public:
    CSpGraphicViewAnimationListItem(CSpGraphicViewItem* pFatherItem = nullptr);

    CSpGraphicStructModel* structModel();

    void insertItem(CSpGraphicViewAnimationIndexItem* pItem,
                    int index,
                    bool refresh = true);

    void removeItem(CSpGraphicViewAnimationIndexItem* pItem,
                    bool dele = false,
                    bool refresh = true);

    void removeItem(int index,
                    bool dele = false,
                    bool refresh = true);

public slots:
    virtual void onModelChanged();

    /*BEGIN 数据结构重载----------------*/
public:
    virtual int  childrenCount();
    virtual CSpGraphicViewItem* firstChild();
    virtual CSpGraphicViewItem* lastChild();
    virtual CSpGraphicViewItem* nextChild(CSpGraphicViewItem* pItem);
    virtual CSpGraphicViewItem* prevChild(CSpGraphicViewItem* pItem);
    virtual void removeAllChidren(bool dele = true);
    virtual CSpGraphicViewItem*        ItemAt(const QPointF pos,QPointF* out = nullptr);
    virtual QSet<CSpGraphicViewItem*>  ItemAt(const QRectF& rctF);
    virtual QList<CSpGraphicViewItem*> allItemAt(const QPointF pos);
    virtual int  itemsBoundingHeight();
    virtual int  itemsBoundingWidth();
protected:
    virtual bool addItem_virtual(CSpGraphicViewItem* pItem);
    virtual bool removeItem_virtual(CSpGraphicViewItem* pItem);
    virtual void setChildrenGraphView(CSpGraphicView* pGv);
    virtual void paintChildren(QPainter* painter,const QRectF &rctVisible);

    virtual void updateItemsStructureAndGeometry();
    /*END   数据结构重载------------*/

    /*屏蔽从父类继承来的添加删除*/
private:
    void  addItem(CSpGraphicViewItem* pItem){CSpGraphicViewItem::addItem(pItem);}

protected:
    CSpGraphicStructModel*        _model;

    QList<CSpGraphicViewAnimationIndexItem*> _items;
};

#endif // CGRAPHICVIEWANIMATIONLISTITEM_H
