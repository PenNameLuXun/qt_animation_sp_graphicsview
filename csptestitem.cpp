#include "csptestitem.h"

CSpTestItem::CSpTestItem(CSpGraphicViewItem *pFatherItem):
CSpGraphicViewAnimationIndexItem(pFatherItem)
{
    setDragAble(true);
    setCheckable(true);
}

CSpTestItem::~CSpTestItem()
{

}

void CSpTestItem::paintSelf(QPainter *painter, const CSpGraphicViewPaintEventInfo *paintInfo)
{
    CSpGraphicViewAnimationIndexItem::paintSelf(painter,paintInfo);

    QColor c(200,200,200);
    if(paintInfo->isChecked)
    {
        c = QColor(255,188,0);
    }

    if(paintInfo->enableState == state_Hover)
    {
        c = c.lighter();
    }
    else if(paintInfo->enableState == state_Pressed)
    {
        c = c.darker();
    }

    painter->setBrush(c);

    painter->drawRect(paintInfo->visiRectF);

    painter->drawText(paintInfo->rectF,QString("%1").arg(index()),QTextOption(Qt::AlignCenter));
}
