#ifndef CHARTDATAWORKER_HPP
#define CHARTDATAWORKER_HPP

#include <QCategoryAxis>
#include <QChart>
#include <QSplineSeries>
#include <QValueAxis>
#include <QtCore/QObject>
#include <cmath>
#include <memory>
#include "database/SPX42Database.hpp"
#include "spx42/SPX42SingleCommand.hpp"

namespace spx
{
  class ChartDataWorker : public QObject
  {
    Q_OBJECT
    static const qint64 waitTimeout{( 1000 / 50 ) * 20};
    static const qint64 waitUnits{50};
    std::shared_ptr< Logger > lg;               // der Logger
    std::shared_ptr< SPX42Database > database;  // Datenbankverbindung
    bool shouldWriterRunning;                   // thread soll laufen und auf daten warten
    qint64 maxTimeoutVal;                       // zeit bis zum timeout
    QString logDetailTableName;

    public:
    explicit ChartDataWorker( std::shared_ptr< Logger > logger,
                              std::shared_ptr< SPX42Database > _database,
                              QObject *parent = nullptr );
    bool makeChartDataMini( QtCharts::QChart *chart, const QString &deviceMac, int diveNum );
    void reset( void );
    void nowait( bool _shouldNoWait = true );  //! nicht mehr warten wenn die queue leer ist

    signals:

    public slots:
  };
}  // namespace spx
#endif  // CHARTDATAWORKER_HPP
