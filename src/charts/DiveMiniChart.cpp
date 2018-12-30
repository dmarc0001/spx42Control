#include "DiveMiniChart.hpp"

namespace spx
{
  // vereinfache mal das geschreibsel
  using namespace QtCharts;

  DiveMiniChart::DiveMiniChart( std::shared_ptr< Logger > logger,
                                std::shared_ptr< SPX42Database > db,
                                QGraphicsItem *parent,
                                Qt::WindowFlags wFlags )
      : QtCharts::QChart( parent, wFlags ), lg( logger ), database( db )
  {
    prepareChart();
  }

  void DiveMiniChart::prepareChart()
  {
    lg->debug( "DiveMiniChart::prepareChart..." );
    legend()->hide();  // Keine Legende in der Minivorschau
    // Chart Titel aufhübschen
    QFont font;
    font.setPixelSize( 10 );
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

  void DiveMiniChart::showDiveDataForGraph( const QString &remDevice, int diveNum )
  {
    lg->debug( "DiveMiniChart::showDiveDataForGraph..." );
    return;
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
    // Zeitachdse machen
    //
    lg->debug( "DiveMiniChart::showDiveDataForGraph -> make horizontal axis..." );
    QValueAxis *axisX = new QValueAxis();
    axisX->setTickCount( 10 );
    addAxis( axisX, Qt::AlignBottom );
    lg->debug( "DiveMiniChart::showDiveDataForGraph -> make horizontal axis...OK" );
    //
    // erste Serie
    //
    lg->debug( "DiveMiniChart::showDiveDataForGraph -> make first vertical axis..." );
    QSplineSeries *series = new QSplineSeries();
    *series << QPointF( 1, 5 ) << QPointF( 3.5, 18 ) << QPointF( 4.8, 7.5 ) << QPointF( 10, 2.5 );
    addSeries( series );
    // y-achse
    QValueAxis *axisY = new QValueAxis;
    axisY->setLinePenColor( series->pen().color() );
    //
    addAxis( axisY, Qt::AlignLeft );
    series->attachAxis( axisX );
    series->attachAxis( axisY );
    lg->debug( "DiveMiniChart::showDiveDataForGraph -> make first vertical axis...OK" );
    //
    // noch eine Serie
    //
    lg->debug( "DiveMiniChart::showDiveDataForGraph -> make second vertical axis..." );
    series = new QSplineSeries;
    *series << QPointF( 1, 0.5 ) << QPointF( 1.5, 4.5 ) << QPointF( 2.4, 2.5 ) << QPointF( 4.3, 12.5 ) << QPointF( 5.2, 3.5 )
            << QPointF( 7.4, 16.5 ) << QPointF( 8.3, 7.5 ) << QPointF( 10, 17 );
    addSeries( series );
    //
    QCategoryAxis *axisY3 = new QCategoryAxis;
    axisY3->append( "Low", 5 );
    axisY3->append( "Medium", 12 );
    axisY3->append( "High", 17 );
    axisY3->setLinePenColor( series->pen().color() );
    axisY3->setGridLinePen( ( series->pen() ) );
    //
    addAxis( axisY3, Qt::AlignRight );
    lg->debug( "DiveMiniChart::showDiveDataForGraph -> make second vertical axis..." );
    lg->debug( "DiveMiniChart::showDiveDataForGraph -> attach axis to series axis..." );
    series->attachAxis( axisX );
    series->attachAxis( axisY3 );
    lg->debug( "DiveMiniChart::showDiveDataForGraph...OK" );
  }
}
