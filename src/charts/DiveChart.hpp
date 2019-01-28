#ifndef DIVECHART_HPP
#define DIVECHART_HPP

#include <QCategoryAxis>
#include <QChart>
#include <QFuture>
#include <QObject>
#include <QSplineSeries>
#include <QValueAxis>
#include <QtConcurrent>
#include "database/ChartDataWorker.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  class DiveChart : public QtCharts::QChart
  {
    private:
    QString remoteDevice;
    std::shared_ptr< Logger > lg;
    std::shared_ptr< SPX42Database > database;
    std::unique_ptr< ChartDataWorker > chartWorker;
    QFuture< bool > dbgetDataFuture;

    public:
    explicit DiveChart( std::shared_ptr< Logger > logger,
                        std::shared_ptr< SPX42Database > db,
                        QGraphicsItem *parent = nullptr,
                        Qt::WindowFlags wFlags = Qt::Widget );
    void showDiveDataInGraph( const QString &deviceAddr, int diveNum );

    private:
    void prepareChart( void );
  };
}  // namespace spx
#endif  // DIVECHART_HPP
