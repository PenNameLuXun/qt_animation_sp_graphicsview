#include <QApplication>
#include "cspgraphicviewanimationlistwidget.h"
#include "cspgraphicviewanimationindexitem.h"

#include "csptestitem.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    CSpGraphicViewAnimationListWidget w;
    w.resize(1000,500);
    w.show();

    w.structModel()->setModel(CSpGraphicStructModel::List_Model);
    w.structModel()->setSpaceY(6);
    for(int i = 0;i<10000;++i)
    {
        CSpTestItem* pItem = new CSpTestItem(nullptr);
        pItem->setBoundRect(QRectF(0,0,80,80));
        w.insertItem(pItem,-1,false);
    }
    w.refreshList();

    return a.exec();
}
