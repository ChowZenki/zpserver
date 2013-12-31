#ifndef ZP_PLTASKBASE_H
#define ZP_PLTASKBASE_H

#include <QObject>
namespace ZPTaskEngine{
class zp_plTaskBase : public QObject
{
    Q_OBJECT
public:
    explicit zp_plTaskBase(QObject *parent = 0);

    virtual int run() = 0;

     int refCount;
signals:

public slots:

};
}
#endif // ZP_PLTASKBASE_H
