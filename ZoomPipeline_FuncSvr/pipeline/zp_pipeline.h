#ifndef ZP_PIPELINE_H
#define ZP_PIPELINE_H

#include <QObject>
#include <QVector>
#include <QThread>
namespace ZPTaskEngine{
class zp_pipeline : public QObject
{
    Q_OBJECT
public:
    explicit zp_pipeline(QObject *parent = 0);
protected:

signals:

public slots:

};
}
#endif // ZP_PIPELINE_H
