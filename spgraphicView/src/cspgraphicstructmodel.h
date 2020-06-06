#ifndef CGRAPHICSTRUCTMODEL_H
#define CGRAPHICSTRUCTMODEL_H
#include <QSize>
#include <QObject>
#include "spgraphicview_global.h"


/*像一个loayout？？？*/
class GRAPHICVIEW_EXPORT CSpGraphicStructModel:public QObject
{
    Q_OBJECT
public:
    enum ListModel{List_Model,Icon_Model,Row_Model};     //Row_Model 肯定是相同的大小

    CSpGraphicStructModel(QObject* parent = nullptr);
    ~CSpGraphicStructModel();

    void      setModel(ListModel ml);
    inline ListModel model(){return _model;}

    void      setItemSameSize(bool b);
    inline bool itemSameSize(){return _itemsSameSize;}

    void      setIconItemSize(const QSize& sz);
    inline QSize     iconItemSz(){return _iconItemSz;}

    void      setIconSpaceXStatic(bool bStatic);
    inline bool iconSpaceXStatic(){return _iconSpaceXIsStatic;}

    void      setIconModelSameDySpace(bool b);
    inline bool iconModelSameDySpace();

    void      setSpaceX(int sp);
    void      setSpaceY(int sp);

    inline int  spaceX(){return _spX;}
    inline int  spaceY(){return _spY;}


    qreal  offsetWidth();
    void   setOffsetWidth(qreal width);

    void getIndex(const QRectF &rctf,
                  const int width,const int count,int &indexB, int &indexE);

    void getAllRowAndAllCloun(const qreal width, const int count, int &numOfCloun, int &numOfRow);




    /*Icon mode下有效 优化的函数 ----------  begin*/

    int    realHeight_IconModel(const qreal width, const int count);
    QRectF boundRectF_IconModel(const qreal width, int index,const int count);

    /*Icon mode下水平方向上child item之间的动态间隙(_iconSpaceXIsStatic为false时有效)
      width 父亲的宽度;
      count 总孩子个数(_allIconModelSameDySpace为true时无意义 从而保证每一个mode icon的计算间隙值一样);
    */
    qreal  dySpaceX_IconModel(const qreal width, const int count, int* pOutNumOfCloun = nullptr);

    /*Icon mode下有效 优化的函数 ----------  end*/


signals:
    void modelChanged();

private:
    /* 当为icon model模式时有效
     * 标记是否水平方向间隙是固定不变的*/
    bool       _iconSpaceXIsStatic          = false;

    /* 当为icon model模式时且_iconSpaceXIsStatic为false时有效
     * 如果true将模拟有无限个数据 (dySpaceX_IconModel()的count将无意义)
     * 从而使所有该变量为true的iconModel的动态间隙相等*/
    bool       _allIconModelSameDySpace     = false;

    ListModel  _model              = Icon_Model;

    int        _spX = 6;
    int        _spY = 6;

    QSize      _iconItemSz         = QSize(80,80);

    qreal      _offsetWidth        = 0;

    bool       _itemsSameSize      = true;
};

#endif // CGRAPHICSTRUCTMODEL_H
