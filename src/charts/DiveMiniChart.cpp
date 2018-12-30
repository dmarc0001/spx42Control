#include "DiveMiniChart.hpp"

namespace spx
{
  // vereinfache mal das geschreibsel
  using namespace QtCharts;

  DiveMiniChart::DiveMiniChart( std::shared_ptr< Logger > logger,
                                std::shared_ptr< SPX42Database > db,
                                QGraphicsItem *parent,
                                Qt::WindowFlags wFlags )
      : QtCharts::QChart( parent, wFlags )
      , lg( logger )
      , database( db )
      , chartWorker( std::unique_ptr< ChartDataWorker >( new ChartDataWorker( logger, db ) ) )
  {
    prepareChart();
  }

  void DiveMiniChart::prepareChart()
  {
    lg->debug( "DiveMiniChart::prepareChart..." );
    legend()->hide();  // Keine Legende in der Minivorschau
    // Chart Titel aufhübschen
    QFont font;
    font.setPixelSize( 8 );
    setTitleFont( font );
    setTitleBrush( QBrush( Qt::darkBlue ) );
    setTitle( tr( "PREVIEW" ) );
    // Hintergrund aufhübschen
    QBrush backgroundBrush( Qt::NoBrush );
    setBackgroundBrush( backgroundBrush );
    // Malhintergrund auch noch
    QLinearGradient plotAreaGradient;
    plotAreaGradient.setStart( QPointF( 0, 1 ) );
    plotAreaGradient.setFinalStop( QPointF( 1, 0 ) );
    plotAreaGradient.setColorAt( 0.0, QRgb( 0x202040 ) );
    plotAreaGradient.setColorAt( 1.0, QRgb( 0x2020f0 ) );
    plotAreaGradient.setCoordinateMode( QGradient::ObjectBoundingMode );
    setPlotAreaBackgroundBrush( plotAreaGradient );
    setPlotAreaBackgroundVisible( true );
    lg->debug( "DiveMiniChart::prepareChart...OK" );
  }

  void DiveMiniChart::showDiveDataInMiniGraph( const QString &remDevice, int diveNum )
  {
    lg->debug( "DiveMiniChart::showDiveDataForGraph..." );
    //
    // aufräumen
    //
    removeAllSeries();
    for ( auto ax : axes( Qt::Horizontal ) )
    {
      removeAxis( ax );
    }
    for ( auto ax : axes( Qt::Vertical ) )
    {
      removeAxis( ax );
    }
    lg->debug( "DiveMiniChart::showDiveDataForGraph -> cleaned series and axis..." );
    //
    // Überschrift ist immer gut
    //
    setTitle( tr( "DIVE NR %1" ).arg( diveNum, 3, 10, QChar( '0' ) ) );
    remoteDevice = remDevice;

    //
    // die Daten aufbereiten
    // TODO: nebenläufig als Future machen
    //
    if ( !dbgetDataFuture.isFinished() )
    {
      dbgetDataFuture.cancel();
      QThread::msleep( 100 );
    }
    if ( dbgetDataFuture.isFinished() )
    {
      dbgetDataFuture = QtConcurrent::run( this->chartWorker.get(), &ChartDataWorker::makeChartDataMini, this, remDevice, diveNum );
    }
    // chartWorker->makeChartDataMini( this, remDevice, diveNum );

    lg->debug( "DiveMiniChart::showDiveDataForGraph...OK" );
  }
}  // namespace spx
