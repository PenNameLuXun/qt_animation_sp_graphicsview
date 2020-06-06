#ifndef CGRAPHICVIEWITEM_H
#define CGRAPHICVIEWITEM_H
#include <QObject>
#include <QRectF>
#include <QMap>
#include <QSet>
#include <QStack>
#include "cspgraphicviewmouseeventinfo.h"
#include "cspgraphicview.h"
#include <QDebug>
#include <QPainter>

typedef CSpGraphicView::MouseOperationTp OperatingTp;
class CSpGraphicStructModel;
class QPainter;
class GRAPHICVIEW_EXPORT CSpGraphicViewItem:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRectF boundRect READ boundRect WRITE setBoundRect)
public:
    CSpGraphicViewItem(CSpGraphicViewItem*   parentItem = nullptr);

    ~CSpGraphicViewItem();

    /* 在父对象(item或graphview卷轴)中的位置及大小(纯虚函数需重载) */
    virtual QRectF boundRect() = 0;

    /* 设置在父对象(item或graphview卷轴)中的位置及大小(纯虚函数需重载) */
    virtual void   setBoundRect(const QRectF& rctF) = 0;

    /* 是否是动画item类型 */
    virtual bool   isAnimationItem(){return false;}

    /* 额外的数据组织结构模式(比如list模式 row模式 icon模式等设置) */
    virtual CSpGraphicStructModel* structModel(){return nullptr;}

    /* 相对自身的位置大小信息(位置肯定是QPointF(0.0,0.0)) */
    QRectF  selfBoundRect();

    /* 相对整个卷轴的位置大小信息 */
    QRectF  glBoundRect();


    /* 添加一个子item refreshStruct 表示是否刷新所有子item的组织结构 */
    void  addItem(CSpGraphicViewItem* pItem,
                  bool refreshStruct = true);


    /* 从view中删除一个item(肯定是顶层item)
     * dele:         是否清理该item的内存
     * refreshStruct 是否刷新所有孩子items的数据结构和位置(即调用refresh()函数) 如果传递了false 记得稍后调用refresh()函数
     * clearAllMark: 是否清理可能存在的当前被选中的信息.
     *               (在remove之前如果item自身或其子孙有被选中的那么要先清理不然会有野指针。
     *                这里只有一种情况clearAllMark不会传递false那就是改变了父对象(这个父对象也是在该view中的,可参见CGraphicViewItem::setParentItem)) */
    void  removeItem(CSpGraphicViewItem* pItem,
                     bool dele = false,
                     bool refreshStruct = true,
                     bool clearAllMark = true);

    /* 将相对于自身的坐标信息映射到父对象(item或graphview卷轴)中 */
    QPointF mapToParent(const QPointF pos);

    /* 从卷轴的坐标信息映射到自身的坐标系 */
    QPointF mapFromGl(const QPointF pos);

    /* 将相对于自身的坐标信息映射到卷轴中 */
    QPointF mapToGl(const QPointF pos);

    /* pAncestors是否是this的祖先 */
    bool    isAncestors(CSpGraphicViewItem* pAncestors);

    /* this是否是顶层item */
    bool    isTopItem();

    /* this是否是全局item(游离的或顶层的) */
    bool    isGlItem();

    /* 父item(如果是isGlItem那么返回nullptr) */
    CSpGraphicViewItem* parentItem();

    /* 设置父item */
    void                setParentItem(CSpGraphicViewItem* parent);

    /* 获取this和pItem的向上遍历的第一个共同先祖 */
    CSpGraphicViewItem* getSameAncestorsWith(CSpGraphicViewItem* pItem);

    /* 顶层父item(向上遍历直到获取到第一个顶层item) */
    CSpGraphicViewItem* topParentItem();

    /* 所属的CGraphicView指针 如果为nullptr证明this为游离的item */
    CSpGraphicView*     graphicView();

    enum   DragType{InParent,InGl};
    enum   ResizeType{Resize_Left,Resize_Right,Resize_Hor,Resize_Top,Resize_Bottom,Resize_Ver,Resize_None};

    /* 是否可以被拖拽 */
    bool     draggable();

    /* 设置是否可以被拖拽 */
    void     setDragAble(bool b);

    /* 被拖拽触发的点(鼠标点下然后移动(拖拽开始)会调用这个函数如果返回false证明这个点不能拖拽 反之则可以) */
    virtual bool isPosDragAble(const QPointF& pos);

    /* 被拖拽的方式(DragType分为InParent和InGl 前者表示只能在父对象中拖拽移动 后者表示是在整个卷轴中移动) */
    DragType dragType();

    /* 设置拖拽的方式 */
    void     setDragType(DragType tp);

    /* 是否可以resize */
    bool       resizeabel();

    /* 设置是否可以resize */
    void       setResizeAble(bool b);

    /* 允许resize的位置(只有上、下、左、右、水平(左右)、垂直(上下)) */
    ResizeType resizeType();

    /* 设置resize被允许的位置 */
    void       setResizeType(ResizeType tp);

    /* 自定义位置可以进行resize(如果重载那么自己判断边界等) */
    virtual ResizeType canResize(const QPointF &pos);

    /* 是否允许被选中 */
    bool    isCheckable();

    /* 设置是否允许被选中 */
    void    setCheckable(bool b);

    /* 是否被选中 */
    virtual bool    isChecked();

    /* 设置是否被选中 */
    virtual void    setChecked(bool b);


    /* 背景色 */
    QColor    backgroundColor();

    /* 设置背景色 */
    void      setBackgroundColor(const QColor& c);

    /* 边线颜色 */
    QColor    borderColor();

    /* 设置边线颜色 */
    void      setBorderColor(const QColor& c);


    /* 是否正在被鼠标操作控制中(drag、resize中) */
    bool    isOperating();

    /* 正在被鼠标操作控制的类型(drag或resize) */
    OperatingTp OperatingType();

    /* z坐标(暂时未使用有bug) */
    qreal   z();

    /* 设置z坐标(暂时未使用有bug) */
    void    setZ(qreal zTemp);

    /* 移动dx dy偏移 */
    void    moveBy(qreal dx,qreal dy);

    /* 移动dPos偏移 */
    void    moveBy(const QPointF& dPos);

    /* 通知刷新重绘 */
    void    update();

    /* 通知刷新重绘自身的某一部分 */
    void    update(const QRectF& rctf);

    /* 通知刷新组织结构并按照一定的组织方式设置孩子items的boundRect */
    void    refresh();

    /* 设置tooltip */
    void setToolTip(const QString &toopTip);

    /* 当前tooltip(设置为可重载方便子类动态修改) */
    virtual QString toolTip() ;

    /* 设置tooltip时长 */
    void setToolTipDuration(int msec);

    /* tooltip时长 */
    int toolTipDuration() const;

signals:
    void sig_Clicked();
    void sig_DoubleClicked();
    void sig_SelectChanged(bool selected);
public:
    /* 被点击时会被调用(可重载实现) */
    virtual void onClicked();

    /* 删除自身 */
    virtual void deleteSelf();

    /* BEGIN 数据结构重载---------------- */
public:
    virtual int  childrenCount();

    virtual CSpGraphicViewItem* firstChild();
    virtual CSpGraphicViewItem* lastChild();
    virtual CSpGraphicViewItem* nextChild(CSpGraphicViewItem* pItem);
    virtual CSpGraphicViewItem* prevChild(CSpGraphicViewItem* pItem);

    virtual void removeAllChidren(bool deleThem = true);
    virtual int  itemsBoundingHeight();
    virtual int  itemsBoundingWidth();

    /* 关于坐标位置的判定的A函数: posIsInLocal 为false 表示pos是在父对象的坐标系 为true表示在自身的坐标系 */
    virtual bool                     contains(const QPointF& pos,bool posIsInLocal,QPointF *out = nullptr);

    /* 如果没有添加新的数据结构 那么一般的不用重载这两个函数 额外的位置判定直接重载A函数就可以了 */
    virtual CSpGraphicViewItem*        ItemAt(const QPointF pos,QPointF* out = nullptr);
    virtual QList<CSpGraphicViewItem*> allItemAt(const QPointF pos);

    /* 也是一个关于位置的判定 但参数是一个矩形范围 现在只用于鼠标的矩形选中 */
    virtual QSet<CSpGraphicViewItem*>  ItemAt(const QRectF& rctF);

protected:
    virtual bool addItem_virtual(CSpGraphicViewItem* pItem);
    virtual bool removeItem_virtual(CSpGraphicViewItem* pItem);
    virtual void setChildrenGraphView(CSpGraphicView* pGv);
    virtual void paintChildren(QPainter *painter, const QRectF &rctVisible);

    virtual void updateItemsStructureAndGeometry();
    /* END   数据结构重载------------ */

public:

    static void s_clearItemChildren(CSpGraphicViewItem* pItem,bool deleThem);
    static void s_clearItemMarks(CSpGraphicViewItem* pItem,bool willDelete = false);

    static void s_setItemFather(CSpGraphicViewItem* pItem,CSpGraphicViewItem* pFather);
    static void s_setItemGraphView(CSpGraphicViewItem* pItem,CSpGraphicView* pGv);

    static void s_paintItem(CSpGraphicViewItem* pItem,
                            QPainter* painter,
                            const QRectF &rctVisible);

    QPixmap selfie(qreal Opacity = 1.0);

protected:
    void drawRect(QPainter *painter,const QRectF &rctf);
protected:
    virtual void paint(QPainter* painter, const CSpGraphicViewPaintEventInfo* paintInfo);
    virtual void paintMore(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo);
    void paintTopIcon(QPainter* painter, const CSpGraphicViewPaintEventInfo *paintInfo);
protected:
    enum ItemEnableState{state_Normal,state_Hover,state_Pressed};
    ItemEnableState enableState();

    bool  operaAccept();

    virtual void paintSelfTopIcon(QPainter* painter,const CSpGraphicViewPaintEventInfo *paintInfo);
    virtual void paintSelf(QPainter* painter,const CSpGraphicViewPaintEventInfo *paintInfo);
    virtual void mousePressEvent(CSpGraphicViewMouseEventInfo* mouseInfo);
    virtual void mouseReleaseEvent(CSpGraphicViewMouseEventInfo* mouseInfo);
    virtual void mouseMoveEvent(CSpGraphicViewMouseEventInfo* mouseInfo);
    virtual void mouseDoubleClickEvent(CSpGraphicViewMouseEventInfo* mouseInfo);
    virtual void enterEvent(CSpGraphicViewMouseEventInfo* mouseInfo);
    virtual void leaveEvent(CSpGraphicViewMouseEventInfo* mouseInfo);
    virtual void moveEvent(CSpGraphicViewItemMoveEventInfo* moveInfo);
    virtual void resizeEvent(const QSizeF &oldSz, const QSizeF &newSz);


    virtual void childAddedEvent(CSpGraphicViewChildEventInfo* childEventInfo);
    virtual void childRemovedEvent(CSpGraphicViewChildEventInfo* childEventInfo);

    virtual void doSelectMove(CSpGraphicViewMouseMoveSelectEventInfo* selectInfo);

    virtual void doDragBegin(CSpGraphViewDragItemInfo* pInfo);
    virtual void doDragMove(CSpGraphViewDragItemInfo* drageMoveInfo);
    virtual void doDragEnd(CSpGraphViewDragItemInfo* pInfo);

    virtual void doResizeBegin(CSpGraphViewResizeItemInfo* selectInfo);
    virtual void doResizeMove(CSpGraphViewResizeItemInfo* selectInfo);
    virtual void doResizeEnd(CSpGraphViewResizeItemInfo* resizeItemInfo);

protected:
    QRectF       boundRectOpera();
    virtual void setBoundRectOpera(const QRectF& rct);

    virtual void dragBegin();
    virtual void dragMove();
    virtual void dragEnd();

    virtual void resizeBegin(ResizeType tp);
    virtual void resizeMove(ResizeType tp);
    virtual void resizeEnd(ResizeType tp);
private:
    void dragMoveBy(CSpGraphViewDragItemInfo* drageMoveInfo);
protected:
    /*将一个孩子节点移动到数据结构的末尾(应用场景:将选中的item移动到末尾以保持它的最后绘制)*/
    void moveChildItemToLast(CSpGraphicViewItem* pItem);

    typedef QMap<qreal,QSet<CSpGraphicViewItem*>> ItemsWithZMap;

    bool                    _checkable;
    bool                    _checked;
    bool                    _isTopItem    = false;
    bool                    _isGlItem     = false;
    bool                    _isOperating = false;
    bool                    _draggable;
    bool                    _resizeable;

    QColor                  _bgColor;
    QColor                  _borderColor;

    DragType                _dragType;
    ResizeType              _resizeType;
    ItemEnableState         _enableState;


    CSpGraphicViewItem*       _parentItem = nullptr;
    CSpGraphicView*           _graphView  = nullptr;
    qreal                     _z = 0.0;

//    ItemsWithZMap           _childrenItemsWithZ;
//    QSet<CGraphicViewItem*> _childrenItems;

    QList<CSpGraphicViewItem*>  _childrenItems;

    QRectF                  _rectOperating;


    QString                 _toolTips;
    int                     _tooltipDur = -1;  //停留600ms就显示tooltip

    class CPainterHelper
    {
    public:
        CPainterHelper(QPainter* painter):_painter(painter){_painter->save();}
        ~CPainterHelper(){_painter->restore();}
    private:
        QPainter* _painter = nullptr;
    };

    class CResizeHelper
    {
    public:
        CResizeHelper(CSpGraphicViewItem* pItem):_p(pItem){_oldSz = _p->boundRect().size();_oldPos = _p->boundRect().topLeft();}
        ~CResizeHelper()
        {
            QSizeF   newSz = _p->boundRect().size();
            QPointF newPos = _p->boundRect().topLeft();
            if(_oldSz != newSz)
            {
                _p->resizeEvent(_oldSz,newSz);
            }

            if(_oldPos != newPos)
            {
                CSpGraphicViewItemMoveEventInfo info;
                info.oldPos = _oldPos;info.newPos = newPos;
                _p->moveEvent(&info);
            }
        }
    private:
        CSpGraphicViewItem* _p;
        QPointF _oldPos;
        QSizeF  _oldSz;
    };

    friend class CSpGraphicView;
};

#endif // CGRAPHICVIEWITEM_H
