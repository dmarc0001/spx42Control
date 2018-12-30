#ifndef CHARTDATAWORKER_HPP
#define CHARTDATAWORKER_HPP

#include <QtCore/QObject>

class ChartDataWorker : public QObject
{
    Q_OBJECT
  public:
    explicit ChartDataWorker(QObject *parent = nullptr);

  signals:

  public slots:
};

#endif // CHARTDATAWORKER_HPP