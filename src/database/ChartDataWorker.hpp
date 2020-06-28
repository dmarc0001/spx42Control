#ifndef CHARTDATAWORKER_HPP
#define CHARTDATAWORKER_HPP

#include <QAreaSeries>
#include <QCategoryAxis>
#include <QChart>
#include <QDateTimeAxis>
#include <QPair>
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
    private:
    static const qint64 waitTimeout{( 1000 / 50 ) * 20};
    static const qint64 waitUnits{50};

    public:
    //! Name für die Tiefenserie
    static const QString depthSeriesName;
    //! Name für die Area TIEFE
    static const QString depthAreaSeriesName;
    //! Name für die Temperaturserie (zum wiederfinden mapToValue/mapToPosition)
    static const QString tempSeriesName;
    //! Name für die PPO2 Serie (zum wiederfinden mapToValue/mapToPosition)
    static const QString ppo2SeriesName;
    //! Name für die Setpointserie (zum wiederfinden mapToValue/mapToPosition)
    static const QString setpointSeriesName;

    private:
    //! der Logger
    std::shared_ptr< Logger > lg;
    //! Datenbankverbindung
    std::shared_ptr< SPX42Database > database;
    //! thread soll laufen und auf daten warten
    bool shouldWriterRunning;
    //! zeit bis zum timeout
    qint64 maxTimeoutVal;
    QString logDetailTableName;

    public:
    explicit ChartDataWorker( std::shared_ptr< Logger > logger,
                              std::shared_ptr< SPX42Database > _database,
                              QObject *parent = nullptr );
    void prepareMiniChart( QtCharts::QChart *chart, bool isLightTheme );
    bool makeChartDataMini( QtCharts::QChart *chart, const QString &deviceMac, int diveNum );
    bool prepareDiveCharts( QtCharts::QChart *bigchart, QtCharts::QChart *ppo2chart, bool isLightTheme );
    DiveDataSetsPtr getFutureDiveDataSet( const QString &deviceMac, int diveNum );
    bool makeDiveChart( QtCharts::QChart *bigchart, QtCharts::QChart *ppo2chart, DiveDataSetsPtr dataSet );
    void reset( void );
    void nowait( bool _shouldNoWait = true );  //! nicht mehr warten wenn die queue leer ist

    private:
    QPair< int, int > getTempBorders( const DiveDataSetsPtr dataSet );

    signals:

    public slots:
  };
}  // namespace spx
#endif  // CHARTDATAWORKER_HPP
