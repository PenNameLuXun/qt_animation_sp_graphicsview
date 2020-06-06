#ifndef CGRAPHICVIEWANIMATIONWIDGET_H
#define CGRAPHICVIEWANIMATIONWIDGET_H
#include "cspgraphicviewanimation.h"
class GRAPHICVIEW_EXPORT CSpGraphicViewAnimationWidget:public CSpGraphicViewAnimation
{
    Q_OBJECT
public:
    CSpGraphicViewAnimationWidget(QWidget* parent = nullptr);

    void setBgColor(const QColor& color);

    void addItem(CSpGraphicViewItem* pItem,
                   bool refresh = true);

    void removeItem(CSpGraphicViewItem* pTopItem,
                      bool dele = true,
                      bool refresh = true);

protected:
    /*添加一个顶层项目(如果设计了自己的保存顶层项目的数据结构 那么需重载该函数并实现添加)*/
    virtual bool addTopItem_virtual(CSpGraphicViewItem* pTopItem);

    /*删除一个顶层项目(如果设计了自己的保存顶层项目的数据结构 那么需重载该函数并实现删除)*/
    virtual bool removeTopItem_virtual(CSpGraphicViewItem* pTopItem);

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
    virtual void drawBackground(QPainter* painter,const QSize& viewSize);

protected:
    virtual void updateItemsStructureAndGeometry(bool isResize = false);
protected:
    typedef  QMultiMap<qreal, CSpGraphicViewItem*> TopItemZMap;

    TopItemZMap                       _topItems;

    qreal   _maxTopBottom = 0;
    qreal   _maxTopRight  = 0;

    QColor  _bgColor = QColor(0,0,0);
};

#endif // CGRAPHICVIEWANIMATIONWIDGET_H
