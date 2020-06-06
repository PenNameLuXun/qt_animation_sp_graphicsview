#ifndef CGRAPHVIEWINDEXLISTITEM_H
#define CGRAPHVIEWINDEXLISTITEM_H
#include "cspgraphicviewindexitem.h"

/*自身带索引 孩子item也是list结构组织的item*/
class GRAPHICVIEW_EXPORT CSpGraphViewIndexListItem:public CSpGraphViewIndexItem
{
    Q_OBJECT
public:
    CSpGraphViewIndexListItem(CSpGraphicViewItem* parentItem = nullptr);

    ~CSpGraphViewIndexListItem();

    CSpGraphicStructModel* structModel();

    void insertItem(CSpGraphViewIndexItem* pItem,
                    int index,
                    bool refresh = true);

    void removeItem(CSpGraphViewIndexItem* pItem,
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
    QList<CSpGraphViewIndexItem*> _items;
    CSpGraphicStructModel*        _model;
};

#endif // CGRAPHVIEWINDEXLISTITEM_H
