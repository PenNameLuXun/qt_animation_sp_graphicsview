#include "csptestitem.h"

CSpTestItem::CSpTestItem(CSpGraphicViewItem *pFatherItem):
CSpGraphicViewAnimationIndexItem(pFatherItem)
{

}

CSpTestItem::~CSpTestItem()
{

}

void CSpTestItem::paintSelf(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo)
{
    CSpGraphicViewAnimationIndexItem::paintSelf(painter,paintInfo);

    painter->drawText(paintInfo->rectF,QString("%1").arg(index()),QTextOption(Qt::AlignCenter));
}
