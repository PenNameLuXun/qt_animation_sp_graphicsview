#ifndef CGRAPHICVIEWLISTWIDGET_H
#define CGRAPHICVIEWLISTWIDGET_H
#include "cspgraphicview.h"
#include "cspgraphicstructmodel.h"

class CSpGraphViewIndexItem;

class GRAPHICVIEW_EXPORT CSpGraphicViewListWidget:public CSpGraphicView
{
    Q_OBJECT
public:
    CSpGraphicViewListWidget(QWidget* parent = nullptr);

    ~CSpGraphicViewListWidget();

    CSpGraphicStructModel* structModel();

    void clear();

    void insertItem(CSpGraphViewIndexItem* pItem,
                    int index = -1,
                    bool updateItemsStructureAndGeometry_helper = true);

    void removeItem(CSpGraphViewIndexItem* pItem,
                      bool dele = true,
                      bool refresh = true);

    void removeItem(int index,
                      bool dele = true,
                      bool refresh = true);

    CSpGraphViewIndexItem* item(int index);

protected:
    /*添加一个顶层项目(如果设计了自己的保存顶层项目的数据结构 那么需重载该函数并实现添加)*/
    bool addTopItem_virtual(CSpGraphicViewItem* pTopItem);

    /*删除一个顶层项目(如果设计了自己的保存顶层项目的数据结构 那么需重载该函数并实现删除)*/
    bool removeTopItem_virtual(CSpGraphicViewItem* pTopItem);

    /*顶层项目个数(如果设计了自己的数据结构 那么需重载该函数并实现)*/
    int  topCount();

    /*数据个数(如果设计了自己的数据结构 那么需重载该函数并实现)*/
    int  dataCount();


    /*顶层项目的总高度*/
    int  virLogicScrollHeight();

    int  virLogicScrollWidth();


    bool posInBoundRectF(const QPointF &glPos, const QRectF &boundRctF);

    /*根据几何位置图形获得所处的项目(如果自己设计了数据结构那么要重构实现)*/
    virtual CSpGraphicViewItem*        itemAt(const QPointF &glPos,QPointF* out = nullptr);
    virtual CSpGraphicViewItem*        topItemAt(const QPointF &glPos,QPointF* out = nullptr);
    virtual QSet<CSpGraphicViewItem*>  itemAt(const QRectF& glRctF);
    virtual QList<CSpGraphicViewItem*> allItemAt(const QPointF &glPos);


    virtual void drawItems(QPainter* painter);

protected:
    void updateItemsStructureAndGeometry(bool isResize = false);

private:
    void updateItemsStructureAndGeometry_helper(int indexB = 0,int indexE = -1);

    int  _ref_index_b = 0;
    int  _ref_index_e = -1;

protected:
    QList<CSpGraphViewIndexItem*> _topItemList;

    CSpGraphicStructModel         _model;

    friend class CSpGraphViewIndexItem;
};

#endif // CGRAPHICVIEWLISTWIDGET_H
