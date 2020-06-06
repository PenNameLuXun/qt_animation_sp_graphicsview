#include "cspgraphicstructmodel.h"
#include <QRectF>

CSpGraphicStructModel::CSpGraphicStructModel(QObject *parent):
    QObject (parent)
{

}

CSpGraphicStructModel::~CSpGraphicStructModel()
{

}

void CSpGraphicStructModel::setModel(CSpGraphicStructModel::ListModel ml)
{
    _model = ml;

    emit modelChanged();
}

void CSpGraphicStructModel::setItemSameSize(bool b)
{
    _itemsSameSize = b;

    emit modelChanged();
}

void CSpGraphicStructModel::setIconItemSize(const QSize &sz)
{
    _iconItemSz = sz;

    emit modelChanged();
}

void CSpGraphicStructModel::setIconSpaceXStatic(bool bStatic)
{
    _iconSpaceXIsStatic = bStatic;

    emit modelChanged();
}

void CSpGraphicStructModel::setIconModelSameDySpace(bool b)
{
    _allIconModelSameDySpace = b;
}

bool CSpGraphicStructModel::iconModelSameDySpace()
{
    return _allIconModelSameDySpace;
}

void CSpGraphicStructModel::setSpaceX(int sp)
{
    _spX = sp;

    emit modelChanged();
}

void CSpGraphicStructModel::setSpaceY(int sp)
{
    _spY = sp;

    emit modelChanged();
}

qreal CSpGraphicStructModel::offsetWidth()
{
    return _offsetWidth;
}

void CSpGraphicStructModel::setOffsetWidth(qreal width)
{
    _offsetWidth = width;

    emit modelChanged();
}

void CSpGraphicStructModel::getIndex(const QRectF &rctf,
                                   const int width,
                                   const int count,
                                   int &indexB,
                                   int &indexE)
{
    if(_model == List_Model)
    {
        indexB = 0;
        indexE = count - 1;
    }
    else if(_model == Icon_Model)
    {
//        int w      = iconItemSz().width();

//        qreal dNum  = width / static_cast<qreal>(w);
//        int   iNum  = static_cast<int>(dNum);

//        {
//            //补丁
//            qreal leftW = static_cast<qreal>(width) - static_cast<qreal>(iNum*w);
//            qreal space = leftW/static_cast<qreal>(iNum + 1);

//            if(space < spaceX())
//            {
//                //如果小于了最小的间隙那么要重新计算
//                space = spaceX();
//                iNum  = static_cast<int>((static_cast<qreal>(width) - space) / (static_cast<qreal>(w) + space));
//            }
//        }

//        int w       = iconItemSz().width();
//        qreal space = _iconSpaceXIsStatic?spaceX(): dySpaceX_IconModel(width,count);
//        int   iNum  = static_cast<int>((static_cast<qreal>(width) - space) / (static_cast<qreal>(w) + space));

        int w    = iconItemSz().width();
        int iNum = static_cast<int>((static_cast<qreal>(width) - spaceX()) / (static_cast<qreal>(w) + spaceX()));
        if(!_iconSpaceXIsStatic)
        {
            dySpaceX_IconModel(width,count,&iNum);
        }

        int  hOfOneLine = iconItemSz().height() + spaceY();

        int rowB   = static_cast<int>(rctf.top() / static_cast<qreal>(hOfOneLine));

        indexB = qMax(0,rowB*iNum);

        int rowE   = static_cast<int>(rctf.bottom() / static_cast<qreal>(hOfOneLine));

        indexE = qMin((rowE + 1)*iNum,count - 1);
    }
    else if(_model == Row_Model)
    {
        indexB = 0;
        indexE = count - 1;
    }
}

void CSpGraphicStructModel::getAllRowAndAllCloun(const qreal width, const int count, int &numOfCloun, int &numOfRow)
{
    if(/*_srcList.isEmpty()*/count == 0 || qFuzzyIsNull(width))
    {
        numOfCloun   = 0;
        numOfRow     = 0;
        return;
    }


    if(_model == List_Model)
    {
        numOfCloun = 1;
        numOfRow = count;
    }
    else if(_model == Icon_Model)
    {
//        int w      = iconItemSz().width();


//        qreal dNum  = static_cast<qreal>(width) / static_cast<qreal>(w);
//        numOfCloun  = static_cast<int>(dNum);
//        {
//            qreal leftW = static_cast<qreal>(width) - static_cast<qreal>(numOfCloun*w);
//            qreal space = leftW/static_cast<qreal>(numOfCloun + 1);

//            if(space < spaceX())
//            {
//                //如果小于了最小的间隙那么要重新计算
//                space = spaceX();
//                numOfCloun  = static_cast<int>((static_cast<qreal>(width) - space) / (static_cast<qreal>(w) + space));
//            }
//        }
        int w          = iconItemSz().width();
        int numOfCloun = static_cast<int>((static_cast<qreal>(width) - spaceX()) / (static_cast<qreal>(w) + spaceX()));
        if(!_iconSpaceXIsStatic)
        {
            dySpaceX_IconModel(width,count,&numOfCloun);
        }

        int allCount = count;


        if(numOfCloun == 0)
        {
            numOfCloun   = 0;
            numOfRow     = 0;
            return;
        }
        numOfRow =  allCount / numOfCloun;

        if(allCount%numOfCloun != 0){++numOfRow;}
    }
    else if(_model == Row_Model)
    {
        numOfCloun   = count;
        numOfRow = 1;
    }
}
int CSpGraphicStructModel::realHeight_IconModel(const qreal width, const int count)
{
    int h            = 0;
    int countOfRow   = 0;
    int countOfCloun = 0;

    getAllRowAndAllCloun(width,count,countOfCloun,countOfRow);

    if(countOfRow != 0)
    {
        h = spaceY() * (countOfRow + 1) + iconItemSz().height() * countOfRow;
    }

    return h;
}
#include <QDebug>
QRectF CSpGraphicStructModel::boundRectF_IconModel(const qreal width , int index, const int count)
{
    int w = iconItemSz().width();
    int h = iconItemSz().height();


    int   iNum  = static_cast<int>((static_cast<qreal>(width) - spaceX()) / (static_cast<qreal>(w) + spaceX()));
    qreal space = _iconSpaceXIsStatic?spaceX(): dySpaceX_IconModel(width,count,&iNum);


    if(iNum == 0){return QRectF(QPointF(0,0),_iconItemSz);}

    int row = index / iNum;
    int col = index % iNum;

    qreal x = space*(col+1) + col*w;


    qreal sy = spaceY();
    qreal y  = sy*(row+1)  + row*h;

    return QRectF(QPointF(x,y),QSizeF(w,h));


}

qreal CSpGraphicStructModel::dySpaceX_IconModel(const qreal width, const int count,int* pOutNumOfCloun)
{
    int w = iconItemSz().width();

    qreal dNum  = static_cast<qreal>(width) / static_cast<qreal>(w);
    int   iNum  = static_cast<int>(dNum);
    qreal leftW = static_cast<qreal>(width) - static_cast<qreal>(iNum*w);
    qreal space = leftW/static_cast<qreal>(iNum + 1);

    //qDebug()<<"space = "<<space<<"spaceX() = "<<spaceX();
    if(space < spaceX())
    {
        //如果小于了最小的间隙那么要重新计算
        space = spaceX();
        iNum  = static_cast<int>((static_cast<qreal>(width) - space) / (static_cast<qreal>(w) + space));
        leftW = static_cast<qreal>(width) - static_cast<qreal>(iNum*w);
        space = leftW/static_cast<qreal>(iNum + 1);
    }

    if(!_allIconModelSameDySpace && (iNum >= count))
    {
        space = spaceX();
    }
    if(pOutNumOfCloun != nullptr)
    {
        *pOutNumOfCloun = iNum;
    }

    return space;
}
