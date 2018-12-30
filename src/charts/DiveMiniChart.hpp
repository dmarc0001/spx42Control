#ifndef DIVEMINICHART_HPP
#define DIVEMINICHART_HPP

#include <QCategoryAxis>
#include <QChart>
#include <QObject>
#include <QSplineSeries>
#include <QValueAxis>
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  class DiveMiniChart : public QtCharts::QChart
  {
    private:
    QString remoteDevice;
    std::shared_ptr< Logger > lg;
    std::shared_ptr< SPX42Database > database;

    public:
    explicit DiveMiniChart( std::shared_ptr< Logger > logger,
                            std::shared_ptr< SPX42Database > db,
                            QGraphicsItem *parent = nullptr,
                            Qt::WindowFlags wFlags = Qt::Widget );
    void showDiveDataForGraph( const QString &remDevice, int diveNum );

    private:
    void prepareChart( void );
  };
}
#endif  // DIVEMINICHART_HPP
