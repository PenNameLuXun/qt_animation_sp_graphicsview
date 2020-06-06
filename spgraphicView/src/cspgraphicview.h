#ifndef CGRAPHICVIEW_H
#define CGRAPHICVIEW_H
#include "spgraphicview_global.h"
#include <QWidget>
#include <QSlider>
#include <QMap>
#include <QSet>
#include <QStack>
#include <QMultiMap>
#include <QTextOption>
#include <QList>

#include "cspgraphicviewmouseeventinfo.h"

class CGraphicPrivetViewport;
class CSpGraphicViewItem;
class CSpGraphicStructModel;

class QPropertyAnimation;
class QVariantAnimation;
class QVBoxLayout;
class QHBoxLayout;
class QScrollBar;
class CSpRealSlider;

typedef  QMultiMap<int, CSpGraphicViewItem*> TopItemMap;


class GRAPHICVIEW_EXPORT CSpGraphicView:public QWidget
{
    Q_OBJECT
public:
    enum MouseOperationTp{DragMove,ResizeMove,SelectMove,DragMapMove,NoneOperation};

    CSpGraphicView(QWidget* parent = nullptr,const QColor& bgColor = QColor(38,38,38));

    /* 设置视口widget(视口widget用于绘制显示) */
    void setViewPort(QWidget* viewPort);

    /* 设置卷轴水平的slider */
    void setHorRealSliderBar(CSpRealSlider* slider);

    /* 设置卷轴垂直的slider */
    void setVerRealSliderBar(CSpRealSlider* slider);

    /* 设置背景颜色 */
    void setBackgroundColor(const QColor& c);

    /* 返回背景颜色 */
    QColor backgroundColor();

    /* 设置viewPort背景颜色 */
    void   setViewPortBackgroundColor(const QColor& c);

    /* 返回viewPort背景颜色 */
    QColor viewPortBackgroundColor();

    /* 视口widget */
    QWidget* viewPort();

    ~CSpGraphicView();

    /* 视口的大小(当前可见区域的范围) */
    QRectF viewportRectF();

    /* 添加一个item(肯定是作为顶层item)
     * refresh: 表示是否刷新所有顶层items的数据结构和位置(即调用refresh()函数) 如果传递了false 记得稍后调用refresh()函数 */
    void addItem(CSpGraphicViewItem* pItem,
                   bool refresh = true);

    /* 从view中删除一个item(肯定是顶层item)
     * dele:   是否清理该item的内存
     * refresh 是否刷新所有顶层items的数据结构和位置(即调用refresh()函数) 如果传递了false 记得稍后调用refresh()函数
     * clearAllMark: 是否清理可能存在的当前被选中的信息.
     *               (在remove之前如果item自身或其子孙有被选中的那么要先清理不然会有野指针。
     *                这里只有一种情况clearAllMark不会传递false那就是改变了父对象(这个父对象也是在该view中的,可参见CGraphicViewItem::setParentItem)) */
    void removeItem(CSpGraphicViewItem* pTopItem,
                      bool dele = true,
                      bool refresh = true,
                      bool clearAllMark = true);

    /*是否有item存在*/
    bool   isEmpty();

    /*顶层item是否是空*/
    bool   isTopEmpty();

    /*是否存在非顶层item(data)*/
    bool   isDataEmpty();

    /*顶层item的数量*/
    int    topItemSize();

    /*非顶层item的数量*/
    int    dataItemSize();

    /*所有item的数量*/
    int    allItemSize();

    /*卷轴的宽度*/
    int    logicScrollWith();

    /*卷轴的高度*/
    int    logicScrollHeight();

    /*卷轴的高宽size*/
    QSize  logicScrollSize();

    /*刷新整个顶层items(包括 数据组织及位置  和 卷轴的长度和高度)*/
    void   refresh();

    /*清除所有的孩子等*/
    virtual void clear();

    enum   SliderVisabelType{AsNeed,Never,Always};

    /*设置卷轴垂直scrollbar的显示方式*/
    void              setVerSliderVisType(SliderVisabelType tp);

    /*卷轴垂直scrollbar的显示方式*/
    SliderVisabelType verSliderVisType();

    /*设置卷轴水平scrollbar的显示方式*/
    void              setHorSliderVisType(SliderVisabelType tp);

    /*卷轴水平scrollbar的显示方式*/
    SliderVisabelType horSliderVisType();

    /*设置是否可以通过点住鼠标然后移动去以一个范围的形式实现选中*/
    void setMouseSelectValid(bool b);

    /*是否可以通过点住鼠标然后移动去以一个范围的形式实现选中*/
    bool mouseSelectValid();

    /*设置wheel事件是否移动卷轴*/
    void setWheelWork(bool b);

    /*wheel事件是否移动卷轴*/
    bool wheelWork();

    /*卷轴水平方向当前显示区域的起点(左边界,即水平scrollBar的当前值)*/
    qreal horValue();

    /*卷轴垂直方向当前显示区域的起始点(上边界,即垂直scrollBar的当前值)*/
    qreal verValue();

    /*卷轴水平scrollbar*/
    CSpRealSlider* horSlider(){return _horSlider;}

    /*卷轴垂直scrollbar*/
    CSpRealSlider* verSlider(){return _verSlider;}

    /*将卷轴x坐标隐射到绘制的本地x坐标*/
    qreal   mapGlXToPaintX(const qreal glX);

    /*将卷轴y坐标隐射到绘制的本地x坐标*/
    qreal   mapGlYToPaintY(const qreal glY);

    /*将卷轴pos坐标隐射到绘制的本地pos坐标*/
    QPointF mapGlPosToPaintPos(const QPointF pos);

    /*将绘制的本地x坐标隐射到卷轴x坐标*/
    qreal   mapPaintXToGlX(const qreal paintX);

    /*将绘制的本地y坐标隐射到卷轴y坐标*/
    qreal   mapPaintYToGlY(const qreal paintY);

    /*将绘制的本地pos坐标隐射到卷轴pos坐标  */
    QPointF mapPaintPosToGlPos(const QPointF pos);

    /*是否被鼠标点击着 */
    bool isPressed();

    /* 是否正在进行操作 */
    bool isOprating(MouseOperationTp* outTp = nullptr);

    /*被鼠标点击着Item */
    CSpGraphicViewItem* isPressedItem();

    /*选中某个pItem 并决定是否发送pItem自身选中变化的信号(添加该参数可选择过滤某些pItem的选中或者过滤多次的信号发送 更加灵活) */
    Q_SLOT void select(CSpGraphicViewItem* pItem,bool emitChanged = true);

    /*不要选中某个pItem 并决定是否发送pItem自身选中变化的信号(添加该参数可选择过滤某些pItem的选中或者过滤多次的信号发送 更加灵活) */
    Q_SLOT void notSelect(CSpGraphicViewItem* pItem,bool emitChanged = true);

    /*清楚所有的选中 并决定是否发送每个item自身选中变化的信号 */
    Q_SLOT void clearSelected(bool emitChanged = true);

    /*清楚除了pItem的所有选中 并决定是否发送每个item自身选中变化的信号 */
    Q_SLOT void clearSelectedExcept(CSpGraphicViewItem* pItem,bool emitChanged = true);

    /*选中一堆或者一个之前 先调用该函数 在所有选中结束后 再调用seletEnd_pri 可实现发送一次selectChanged (设置成public方便外界实现自定义的选中)*/
    inline void seletBegin_pri(){_tempSelectOld = _checkedItems;}

    /*选中一堆或者一个之前 先调用seletBegin_pri 在所有选中结束后 再调用该函数 可实现发送一次selectChanged (设置成public方便外界实现自定义的选中)*/
    inline void seletEnd_pri(){if(_tempSelectOld != _checkedItems){emit selectChanged(_tempSelectOld,_checkedItems);}}

    /*刷新整个顶层items数据组织及位置*/
    Q_SLOT void refreshStructAndGeometry();

    /*是否是动画类型的CGraphicView子类(CGraphicViewAnimation或其子类)*/
    virtual bool isAnimationView();

    /*额外的数据组织结构模式(比如list模式 row模式 icon模式 及 偏移值等设置)*/
    virtual CSpGraphicStructModel* structModel(){return nullptr;}

    enum GraphViewType{NormalView,ListView};

    /*CGraphicView 的类型 (已经被弃用(structModel()替代了))*/
    inline GraphViewType graphType(){return _type;}

    /*返回两个矩形的相交矩形*/
    static QRectF intersection(const QRectF& rctfA,const QRectF& rctfB);

signals:
    void itemClicked(CSpGraphicViewItem* pItem);
    void itemDoubleClicked(CSpGraphicViewItem* pItem);
    void itemSelectChanged(CSpGraphicViewItem* pItem,bool checked);
    void selectChanged(const QSet<CSpGraphicViewItem*>& oldSelected,
                       const QSet<CSpGraphicViewItem*>& newSelected);

protected:
    /*添加一个顶层项目(需设计自己的保存顶层项目的数据结构 重载该函数并实现添加)*/
    virtual bool addTopItem_virtual(CSpGraphicViewItem* pTopItem) = 0;

    /*删除一个顶层项目(需设计自己的保存顶层项目的数据结构 重载该函数并实现删除)*/
    virtual bool removeTopItem_virtual(CSpGraphicViewItem* pTopItem) = 0;

    /*顶层项目个数(需设计自己的数据结构 重载该函数并实现)*/
    virtual int  topCount() = 0;

    /*数据个数(即非顶层item的个数。需设计自己的数据结构 重载该函数并实现)*/
    virtual int  dataCount() = 0;

    /*卷轴的总高度(需设计自己的数据结构 重载该函数并实现)*/
    virtual int  virLogicScrollHeight() = 0;

    /*卷轴的总宽度(需设计自己的数据结构 重载该函数并实现)*/
    virtual int  virLogicScrollWidth() = 0;

    /*根据数据结构重载该函数可调用paintItem实现绘制items (需设计自己的数据结构 重载该函数并实现)*/
    virtual void  drawItems(QPainter* painter) = 0;

public:
    /*根据几何位置图形获得所处的项目(需设计自己的数据结构 要重构实现)*/
    virtual CSpGraphicViewItem*        itemAt(const QPointF &glPos,QPointF* out = nullptr) = 0;
    virtual CSpGraphicViewItem*        topItemAt(const QPointF &glPos,QPointF* out = nullptr) = 0;
    virtual QSet<CSpGraphicViewItem*>  itemAt(const QRectF& glRctF) = 0;
    virtual QList<CSpGraphicViewItem*> allItemAt(const QPointF &glPos) = 0;

    /*刷新卷轴slider的取值范围(调用refreshVScrollBarRange()和refreshhSliderRange())*/
    virtual void  refreshScrollBarRange();

    /*刷新卷轴垂直方向ScrollBar的取值范围(virLogicScrollHeight() - viewPort().height())*/
    virtual void  refreshVScrollBarRange();

    /*刷新卷轴水平方向ScrollBar的取值范围(virLogicScrollWidth() - viewPort().width())*/
    virtual void  refreshHScrollBarRange();

public:
    /*绘制item (通常是item通过调用该函数来绘制自身 这里设置成虚函数以方便之后重载修改(估计要重载的情况比较少))
      pItem       表示需要绘制的item指针; painter qt绘制设备的绘制工具指针;
      visibleRect 当前可见的范围;        reson    绘制该pItem的原因*/
    virtual void  paintItem(CSpGraphicViewItem* pItem,
                            QPainter* painter,
                            const QRectF &visibleRect,
                            const PaintReson reson = Normal_Paint);

protected:
    /*刷新并设置item的结构并设置他们的位置(一般是list列表会重载实现该函数)
      isResizeReson 表示是否是viewPort() resize 时导致的*/
    virtual void  updateItemsStructureAndGeometry(bool isResizeReson = false);

protected:
    bool eventFilter(QObject* watcher,QEvent* event);
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);
    void paintEvent(QPaintEvent* event);

    virtual void resizeContex();
    virtual bool viewShow();
    virtual void viewPortResize(const QSize& oldSz,const QSize& newSz);
    virtual void mousePress(CSpGraphicViewMouseEventInfo* pInfo);
    virtual void mouseRelease(CSpGraphicViewMouseEventInfo* pInfo);
    virtual void mouseDoubleClicked(CSpGraphicViewMouseEventInfo* pInfo);
    virtual void mouseMove(CSpGraphicViewMouseEventInfo* pInfo);
    virtual void enter(CSpGraphicViewMouseEventInfo* pInfo);
    virtual void leave();
    virtual void childAddedEvent(CSpGraphicViewChildEventInfo* childEventInfo);
    virtual void childRemovedEvent(CSpGraphicViewChildEventInfo* childEventInfo);

    /*将缓存contex(实际是一个qpixmap)和drawToWidgetMore的内容绘制到viewWidget(视口)上*/
    virtual void drawAll();

    /*绘制到缓存contex(实际是一个qpixmap)*/
    virtual void drawToContex();

    /*将背景色绘制到缓存contex*/
    virtual void drawBackground(QPainter* painter,const QSize& viewSize);

    /*将可能存在的偏移线条绘制到缓存contex*/
    virtual void drawOffsetAreaLine(QPainter* painter);

    /*将额外的内容绘制到缓存contex*/
    virtual void drawMore(QPainter* painter);

    /*将额外的内容绘制到视口上*/
    virtual void drawToWidgetMore(QPainter* painter);

    /*将可能存在的选择矩形绘制到缓存contex*/
    virtual void drawSelect(QPainter* painter,const QRectF& selectRctf);

    /*将可能正在进行的resize内容绘制到缓存contex*/
    virtual void drawResize(QPainter* painter,CSpGraphicViewItem* pPressedItem);

    /*将可能正在进行的drag内容绘制到缓存contex*/
    virtual void drawDrag(QPainter* painter,const QSet<CSpGraphicViewItem*>& dragItems);

    /*可定制 对多个或一个item进行批量移动 (不确定在干什么的时候不要重载) */
    virtual void mouseDrageBegin(const QSet<CSpGraphicViewItem*>& dragItems,CSpGraphViewDragItemInfo* pDragItemInfo);
    virtual void mouseDrageMove(const QSet<CSpGraphicViewItem*>& dragItems,CSpGraphViewDragItemInfo* pDragItemInfo);
    virtual void mouseDrageEnd(const QSet<CSpGraphicViewItem*>& dragItems,CSpGraphViewDragItemInfo* pDragItemInfo);

    /*可定制 选中 (不确定在干什么的时候不要重载) */
    virtual void mouseSelect(const QRectF& selectRctf, CSpGraphicViewItem* pPressedItem);

    /*可定制 选中 (不确定在干什么的时候不要重载) */
    virtual bool mouseDragMapMove(const QPointF& prePos,const QPointF& newPos);

    /* 卷轴的水平bar值发生了改变 */
    virtual void onHorScrollBarValueChanged(qreal value);

    /* 卷轴的垂直bar值发生了改变 */
    virtual void onVerScrollBarValueChanged(qreal value);

    /* 被删除的pItem可定向他们的流向*/
    virtual void addToWaitToDeleteClub(CSpGraphicViewItem* pItem){Q_UNUSED(pItem);}

protected:
    /* 根据pItem的当前drag(select)状态决定是否要将pItem加入到drag列表中*/
    void  makeDragFlag(CSpGraphicViewItem* pItem);

    /* 根据pItem的当前选中状态决定是否要将pItem加入到select列表中*/
    void  makeCheckedFlag(CSpGraphicViewItem* pItem);

    /* 清楚drag列表*/
    void  clearAllDrag();

    /* 清楚select列表(并清除他们的选中状态(emitChanged决定是否要发送选中状态改变的消息))*/
    void  clearAllChecked(bool emitChanged = true);

    /* 清楚pItem在drag列表和select列表的记录(如果willDelet为true那么还会发送他的状态变化信号并清空内存 否则 就不会再做什么)*/
    void  clearAllMarkOfItem(CSpGraphicViewItem* pItem,bool willDelet = false);

    /* 设置pItem的状态值 */
    void  setEnableState(CSpGraphicViewItem* pItem,int enableState);

    /* 该次重绘的重绘区域 */
    QRect rctOfThisTimeRepaint();
protected:
    GraphViewType                    _type      = NormalView;
    QWidget*                         _viewport  = nullptr;
    CSpRealSlider*                   _horSlider = nullptr;
    CSpRealSlider*                   _verSlider = nullptr;
    QVBoxLayout*                     _verSliderLayOut = nullptr;
    QHBoxLayout*                     _horSliderLayOut = nullptr;
    SliderVisabelType                _horTp     = AsNeed;
    SliderVisabelType                _verTp     = AsNeed;

    QSet<CSpGraphicViewItem*>          _draggedItems;
    QSet<CSpGraphicViewItem*>          _checkedItems;

    QSet<CSpGraphicViewItem*>          _draggedItemsTemp;  //每次drag开始被赋值(从_draggedItems拷贝过来) drag结束后清除

    bool                             _mouseSelectValid = true;
    bool                             _wheelWork = true;

    QSet<CSpGraphicViewItem*>          _tempSelectOld;

    CSpGraphicViewItem*   _mousePressItem  = nullptr;
    CSpGraphicViewItem*   _mousePosItem    = nullptr;

    QList<CSpGraphicViewItem*> _mousePressItems;
    QList<CSpGraphicViewItem*> _mousePosItems;

    bool isAcontainB(CSpGraphicViewItem* A,CSpGraphicViewItem* B,
                     const QList<CSpGraphicViewItem*>& Bfamily);

    CSpGraphicViewItem *linkOfTwoFamily(const QList<CSpGraphicViewItem*>& Afamily,
                                      const QList<CSpGraphicViewItem*>& Bfamily);

private slots:
    void onVerScrollBarRangeChanged_pri(qreal min, qreal max);
    void onHorScrollBarRangeChanged_pri(qreal min, qreal max);
    void onHorScrollBarValueChanged_pri(qreal value);
    void onVerScrollBarValueChanged_pri(qreal value);

protected:
    virtual MouseOperationTp decideOperate(CSpGraphicViewMouseEventInfo* pInfo,int& outResizeTp);

protected:
    MouseOperationTp     _mouseMoveOpera          = NoneOperation;
    int                  _resizeTp                = 6;
    bool                 _mouseMoveHaveMakenOpera = false;

    bool                 _moveDrageding = false;
    bool                 _moveSelecting = false;
    bool                 _moveResizing  = false;
    bool                 _pressed       = false;

    QPointF              _mousePressPaintPos;
    QPointF              _mousePressPaintBeginPos;

    QPointF              _mousePressPos;
    QPointF              _mousePressBeginPos;
    QPointF              _mousePressBeginLocalPos;


    CSpGraphViewDragItemInfo*   _dragItemInfo   = nullptr;
    CSpGraphViewResizeItemInfo* _resizeItemInfo = nullptr;

    QVariantAnimation* _horMove = nullptr;
    QVariantAnimation* _verMove = nullptr;
    int _horMoveValue = 0;
    int _verMoveValue = 0;
protected:
    virtual bool isScrollShouldHorMoveWhenOperateToBorder(CSpGraphicViewMouseEventInfo* pInfo,int& move);
    virtual bool isScrollShouldVerMoveWhenOperateToBorder(CSpGraphicViewMouseEventInfo* pInfo,int& move);

    void moveBorderExec(CSpGraphicViewMouseEventInfo* pInfo);
    inline void horMoveExec();
    inline void verMoveExec();

    inline void stopHorMove();
    inline void stopVerMove();

    CSpGraphicViewMouseEventInfo _tempMoveMouseInfo;

    bool _horRunning = false;
    bool _verRunning = false;

public:
    QRect  clipRectBecauseOffsetArea();
protected:
    QRectF offsetAreaInPaint();
protected:
    QColor  _color;
    QColor  _viewBgColor;
    QRect   _thisUpdateRct;
    QPixmap _contex;

protected:
    /* 顾名思义(在列表中找到第一个checkabled item) */
    CSpGraphicViewItem* findFirstCheckableItemInList(QList<CSpGraphicViewItem*>* itemsList);

    /* 在列表中找到pItem的可选中祖先并将它们的选中状态设置为checked */
    void              setItemListItemCheckableAncestors(QList<CSpGraphicViewItem*>* itemsList,CSpGraphicViewItem* pItem,bool checked);

    /* 是否是兄弟 */
    bool    isBrother(CSpGraphicViewItem* pItemA,CSpGraphicViewItem* pItemB);

    /* 鼠标进入pItem必须要做的事情(设置成虚函数 方便子类修改 但修改请特别小心(一般不需要修改了)) */
    virtual void mouseEnterItem(CSpGraphicViewItem* pItem, CSpGraphicViewMouseEventInfo *mouseInfo);

    /* 鼠标离开pItem必须要做的事情(设置成虚函数 方便子类修改 但修改请特别小心(一般不需要修改了)) */
    virtual void mouseLeaveItem(CSpGraphicViewItem* pItem, CSpGraphicViewMouseEventInfo *mouseInfo);

    /* 鼠标在pItem内移动必须要做的事情(设置成虚函数 方便子类修改 但修改请特别小心(一般不需要修改了)) */
    virtual void mouseMoveItem(CSpGraphicViewItem* pItem, CSpGraphicViewMouseEventInfo *mouseInfo);

    /* 鼠标双击pItem必须要做的事情(设置成虚函数 方便子类修改 但修改请特别小心(一般不需要修改了)) */
    virtual void mouseDoubleClickedItem(CSpGraphicViewItem* pItem, CSpGraphicViewMouseEventInfo *mouseInfo);

    /* 鼠标单击pItem必须要做的事情(设置成虚函数 方便子类修改 但修改请特别小心(一般不需要修改了)) */
    virtual void mouseBtnPressItem(CSpGraphicViewItem* pItem, CSpGraphicViewMouseEventInfo *mouseInfo);

    /* 鼠标按钮释放pItem必须要做的事情(设置成虚函数 方便子类修改 但修改请特别小心(一般不需要修改了)) */
    virtual void mouseBtnReleaseItem(CSpGraphicViewItem* pItem, CSpGraphicViewMouseEventInfo *mouseInfo);

protected:

    /* 鼠标按钮释放时处理operate的善后函数 */
    void handleOperateEventWhenRelease(CSpGraphicViewMouseEventInfo* pInfo);

    /* 鼠标按钮释放时处理非operate的善后函数 */
    void handleNormalEventWhenRelease(CSpGraphicViewMouseEventInfo* pInfo);

    /* 鼠标释放时是优先处理点击事件还是存在的操作事件(默认优先处理存在的操作事件) */
    void setHandleOperatFirstWhenMouseRelease(bool b){handleOperatFirstWhenMouseRelease = b;}

    bool handleOperatFirstWhenMouseRelease = true;

    friend class CGraphicPrivetViewport;
    friend class CSpGraphicViewItem;
};


#endif // CGRAPHICVIEW_H
