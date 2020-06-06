#ifndef CGRAPHICVIEWMAPLISTWIDGET_H
#define CGRAPHICVIEWMAPLISTWIDGET_H


#include "cspgraphicview.h"
#include "cspgraphicviewlistwidget.h"
#include <QWidget>
#include <QSlider>
#include <QMap>
#include <QSet>
#include <QStack>
#include <QMultiMap>

class CSpGraphicViewItem;

class CSpGraphViewIndexItem;

/*利用map容器优化后的list*/
class GRAPHICVIEW_EXPORT CSpGraphicViewMapListWidget:public CSpGraphicViewListWidget
{
    Q_OBJECT
public:
    CSpGraphicViewMapListWidget(QWidget* parent = nullptr);
    ~CSpGraphicViewMapListWidget();


protected:
    /*顶层项目个数(如果设计了自己的数据结构 那么需重载该函数并实现)*/
    virtual int  topCount();

    /*数据个数(如果设计了自己的数据结构 那么需重载该函数并实现)*/
    virtual int  dataCount();


    /*顶层项目的总高度*/
    virtual int  virLogicScrollHeight();

    virtual int  virLogicScrollWidth();

    /*根据几何位置图形获得所处的项目(如果自己设计了数据结构那么要重构实现)*/
    virtual CSpGraphicViewItem*        itemAt(const QPointF &glPos,QPointF* out = nullptr);
    virtual CSpGraphicViewItem*        topItemAt(const QPointF &glPos,QPointF* out = nullptr);
    virtual QSet<CSpGraphicViewItem*>  itemAt(const QRectF& glRctF);
    virtual QList<CSpGraphicViewItem*> allItemAt(const QPointF &glPos);


    virtual void drawItems(QPainter* painter);

protected:
    void  updateItemsStructureAndGeometry(bool isResize = false);
    void  onVerScrollBarValueChanged(int value);
    void  onHorScrollBarValueChanged(int value);

protected:
    typedef  QMultiMap<int, CSpGraphViewIndexItem*> TopItemMap;

    TopItemMap                       _topItemBottomMap;

    TopItemMap::iterator             _keyItUpdate = nullptr;

    TopItemMap::iterator             _it[2] = {nullptr,nullptr};
};

#endif // CGRAPHICVIEWMAPLISTWIDGET_H
