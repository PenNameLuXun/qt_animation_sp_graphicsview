#ifndef CGRAPHICVIEWANIMATIONLISTWIDGET_H
#define CGRAPHICVIEWANIMATIONLISTWIDGET_H
#include "cspgraphicviewanimation.h"
#include <QWidget>
#include <QSlider>
#include <QMap>
#include <QSet>
#include <QStack>
#include <QMultiMap>
#include "cspgraphicstructmodel.h"

class CSpGraphicViewAnimationIndexItem;
class GRAPHICVIEW_EXPORT CSpGraphicViewAnimationListWidget:public CSpGraphicViewAnimation
{
    Q_OBJECT
public:
    CSpGraphicViewAnimationListWidget(QWidget* parent = nullptr);

    ~CSpGraphicViewAnimationListWidget();

    /* view的额外model */
    CSpGraphicStructModel* structModel();

    /* 刷新整个list(遍历所有item并设置他们的坐标位置并重绘) */
    void refreshList();

    /* 清除所有item */
    void clear();

    /* 添加一个item */
    void insertItem(CSpGraphicViewAnimationIndexItem* pItem,
                    int index = -1,
                   bool refresh = true);

    /* 删除一个item(通过指针) */
    void removeItem(CSpGraphicViewAnimationIndexItem* pItem,
                      bool dele = true,
                      bool refresh = true);

    /* 删除一个item(通过索引值) */
    void removeItem(int index,
                      bool dele = true,
                      bool refresh = true);

    /* 将当前的索引为srcIndex的一个item移动到当前索引为desIndex的前面
     * （finished表示这是一个完整的操作 接下来没有其他的一系列的移动了
     * (如果有那么这里可以传false然后在结束点记得refreshStructAndGeometry())）*/
    void moveItem(int srcIndex,int desIndex,bool finished = true);

    CSpGraphicViewAnimationIndexItem* item(int index);

    /* 设置resize导致的动画是否要以动画执行(默认为true) */
    void  setResizeAnimation(bool b);

protected:
    /* 添加一个顶层项目(如果设计了自己的保存顶层项目的数据结构 那么需重载该函数并实现添加) */
    virtual bool addTopItem_virtual(CSpGraphicViewItem* pTopItem);

    /* 删除一个顶层项目(如果设计了自己的保存顶层项目的数据结构 那么需重载该函数并实现删除) */
    virtual bool removeTopItem_virtual(CSpGraphicViewItem* pTopItem);

    /* 顶层项目个数(如果设计了自己的数据结构 那么需重载该函数并实现) */
    virtual int  topCount();

    /* 数据个数(如果设计了自己的数据结构 那么需重载该函数并实现) */
    virtual int  dataCount();


    /* 顶层项目的总高度 */
    virtual int  virLogicScrollHeight();

    virtual int  virLogicScrollWidth();


    bool posInBoundRectF(const QPointF &glPos, const QRectF &boundRctF);

    /* 根据几何位置图形获得所处的项目(如果自己设计了数据结构那么要重构实现) */
    virtual CSpGraphicViewItem*        itemAt(const QPointF &glPos,QPointF* out = nullptr);
    virtual CSpGraphicViewItem*        topItemAt(const QPointF &glPos,QPointF* out = nullptr);
    virtual QSet<CSpGraphicViewItem*>  itemAt(const QRectF& glRctF);
    virtual QList<CSpGraphicViewItem*> allItemAt(const QPointF &glPos);


    virtual void drawItems(QPainter* painter);

protected:
    void updateItemsStructureAndGeometry(bool isResize = false);

    void onChildAnimationsFinished();

    void refresh(int indexB = 0,int indexE = -1,bool isResize = false);

    int  _ref_index_b = 0;
    int  _ref_index_e = -1;

    bool _animationWayDoResize = true;

protected:
    QList<CSpGraphicViewAnimationIndexItem*> _topItemList;

    CSpGraphicStructModel         _model;

    friend class CSpGraphicViewAnimationIndexItem;
};

#endif // CGRAPHICVIEWANIMATIONLISTWIDGET_H
