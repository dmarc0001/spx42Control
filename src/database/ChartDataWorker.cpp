#include "ChartDataWorker.hpp"

namespace spx
{
  using namespace QtCharts;

  ChartDataWorker::ChartDataWorker( std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Database > _database, QObject *parent )
      : QObject( parent ), lg( logger ), database( _database ), shouldWriterRunning( true )
  {
  }

  void ChartDataWorker::reset()
  {
    shouldWriterRunning = false;
  }

  void ChartDataWorker::nowait( bool _shouldNoWait )
  {
    if ( _shouldNoWait )
      maxTimeoutVal = 0;
    else
      maxTimeoutVal = waitTimeout;
  }

  void ChartDataWorker::prepareMiniChart( QChart *chart )
  {
    lg->debug( "DiveMiniChart::prepareChart..." );
    chart->legend()->hide();  // Keine Legende in der Minivorschau
    // Chart Titel aufhübschen
    QFont font;
    font.setPixelSize( 8 );
    chart->setTitleFont( font );
    chart->setTitleBrush( QBrush( Qt::darkBlue ) );
    chart->setTitle( tr( "PREVIEW" ) );
    // Hintergrund aufhübschen
    QBrush backgroundBrush( Qt::NoBrush );
    chart->setBackgroundBrush( backgroundBrush );
    // Malhintergrund auch noch
    QLinearGradient plotAreaGradient;
    plotAreaGradient.setStart( QPointF( 0, 1 ) );
    plotAreaGradient.setFinalStop( QPointF( 1, 0 ) );
    plotAreaGradient.setColorAt( 0.0, QRgb( 0x202040 ) );
    plotAreaGradient.setColorAt( 1.0, QRgb( 0x2020f0 ) );
    plotAreaGradient.setCoordinateMode( QGradient::ObjectBoundingMode );
    chart->setPlotAreaBackgroundBrush( plotAreaGradient );
    chart->setPlotAreaBackgroundVisible( true );
    lg->debug( "DiveMiniChart::prepareChart...OK" );
  }

  bool ChartDataWorker::makeChartDataMini( QChart *chart, const QString &deviceMac, int diveNum )
  {
    QString tableName;
    DiveDataSetsPtr dataSet;
    QLineSeries *depthSeries;
    QValueAxis *axisYDepth;
    QLineSeries *ppo2Series;
    QValueAxis *axisYPPO2;
    QValueAxis *axisX;

    lg->debug( QString( "ChartDataWorker::makeChartDataMini for <%1>, num <%2>..." ).arg( deviceMac ).arg( diveNum ) );
    //
    // jetzt die Daten abholen
    //
    dataSet = database->getDiveDataSets( deviceMac, diveNum );
    // hat es sich gelohnt
    if ( dataSet->isEmpty() )
      return ( false );
    //
    // jetzt die Daten für das Chart machen
    //
    lg->debug( "ChartDataWorker::makeChartDataMini -> create depth serie..." );
    // tiefe Datenserie
    depthSeries = new QLineSeries();
    // berülle Daten
    for ( auto singleSet : *dataSet.get() )
    {
      depthSeries->append( singleSet.lfdnr, singleSet.depth );
    }
    chart->addSeries( depthSeries );
    axisYDepth = new QValueAxis();
    axisYDepth->setLinePenColor( depthSeries->pen().color() );
    axisYDepth->setLabelFormat( "%.1f m" );

    chart->addAxis( axisYDepth, Qt::AlignLeft );
    depthSeries->attachAxis( axisYDepth );
    axisYDepth->setMax( 0.0 );

    lg->debug( "ChartDataWorker::makeChartDataMini -> create ppo2 serie..." );
    // ppo2 Serie
    ppo2Series = new QLineSeries();
    for ( auto singleSet : *dataSet.get() )
    {
      ppo2Series->append( singleSet.lfdnr, singleSet.ppo2 );
    }
    chart->addSeries( ppo2Series );
    axisYPPO2 = new QValueAxis();
    axisYPPO2->setLinePenColor( depthSeries->pen().color() );
    axisYPPO2->setLabelFormat( "%.2f bar" );
    axisYPPO2->setTickCount( 4 );
    axisYPPO2->setRange( 0.0, 3.0 );

    chart->addAxis( axisYPPO2, Qt::AlignRight );
    ppo2Series->attachAxis( axisYPPO2 );

    lg->debug( "ChartDataWorker::makeChartDataMini -> create time axis..." );
    // Zeitachse, dimension aus DB lesen
    axisX = new QValueAxis();
    axisX->setTickCount( dataSet->count() );
    chart->addAxis( axisX, Qt::AlignBottom );
    return ( true );
  }

  bool ChartDataWorker::prepareDiveChart( QChart *chart )
  {
    QFont font;
    QBrush backgroundBrush( Qt::NoBrush );
    QLinearGradient plotAreaGradient;
    //
    lg->debug( "ChartDataWorker::makeDiveChart ->prepareChart..." );
    // Chart Titel aufhübschen
    font.setPointSize( 11 );
    chart->setTitleFont( font );
    chart->setTitleBrush( QBrush( Qt::darkBlue ) );
    chart->setTitle( tr( "DIVE VIEW" ) );
    // chart->setTheme( QChart::ChartTheme::ChartThemeDark );
    chart->setTheme( QChart::ChartTheme::ChartThemeQt );
    // chart->setTheme( QChart::ChartTheme::ChartThemeBrownSand );
    // chart->setTheme( QChart::ChartTheme::ChartThemeLight );
    chart->setAnimationOptions( QChart::SeriesAnimations );
    chart->legend()->setAlignment( Qt::AlignBottom );
    lg->debug( "ChartDataWorker::makeDiveChart ->prepareChart...OK" );
    return ( true );
  }

  bool ChartDataWorker::makeDiveChart( QtCharts::QChart *chart, const QString &deviceMac, int diveNum )
  {
    qint64 milisecounds = 0;
    DiveDataSetsPtr dataSet;
    QAreaSeries *depthAreaSeries;
    QLineSeries *depthSeries;
    QLineSeries *nullDepthSeries;
    QLineSeries *ppo2Series;
    QValueAxis *axisY_depth;
    QValueAxis *axisY_ppo2;
    QDateTimeAxis *axisX_time;
    QCategoryAxis *axisY_category_ppo2;
    //
    // jetzt Daten holen
    //
    //
    // aufräumen
    //
    chart->removeAllSeries();
    for ( auto ax : chart->axes( Qt::Horizontal ) )
    {
      chart->removeAxis( ax );
    }
    for ( auto ax : chart->axes( Qt::Vertical ) )
    {
      chart->removeAxis( ax );
    }
    lg->debug( "ChartDataWorker::makeDiveChart -> cleaned series and axis..." );
    chart->createDefaultAxes();
    //
    // jetzt die Daten abholen
    //
    dataSet = database->getDiveDataSets( deviceMac, diveNum );
    // hat es sich gelohnt
    if ( dataSet->isEmpty() )
      return ( false );
    //
    // ########################################################################
    // jetzt die Daten für das Chart machen
    // ########################################################################
    //
    lg->debug( "ChartDataWorker::makeChartData -> create depth serie..." );
    //
    // Tauchtiefe Datenserie
    //
    depthSeries = new QLineSeries();
    nullDepthSeries = new QLineSeries();
    // berülle Daten
    milisecounds = 0;
    for ( auto singleSet : *dataSet.get() )
    {
      depthSeries->append( milisecounds, singleSet.depth );
      nullDepthSeries->append( milisecounds, 0 );
      milisecounds += singleSet.nextStep * 1000;
    }
    // Achse für die Tiefe machen
    axisY_depth = new QValueAxis();
    axisY_depth->setGridLineColor( QColor( "#FF000050" ) );
    chart->addAxis( axisY_depth, Qt::AlignLeft );
    axisY_depth->setMax( 0.0 );
    axisY_depth->setTitleText( tr( "DEPTH [m]" ) );
    axisY_depth->setLabelFormat( "%.1f m" );
    axisY_depth->setLabelsColor( QColor( "#FF000050" ) );

    // Area für die Tiefe machen
    depthAreaSeries = new QAreaSeries( nullDepthSeries, depthSeries );
    depthAreaSeries->setColor( QColor( "#B0000060" ) );
    depthAreaSeries->setBorderColor( Qt::darkBlue );
    chart->addSeries( depthAreaSeries );
    depthAreaSeries->attachAxis( axisY_depth );
    //
    // Serie für O2 Partialdruck machen
    //
    lg->debug( "ChartDataWorker::makeChartData -> create ppo2 serie..." );
    // ppo2 Serie
    milisecounds = 0;
    ppo2Series = new QLineSeries();
    for ( auto singleSet : *dataSet.get() )
    {
      ppo2Series->append( milisecounds, singleSet.ppo2 );
      milisecounds += singleSet.nextStep * 1000;
    }
    chart->addSeries( ppo2Series );
    ppo2Series->setColor( QColor( "#B0f080ff" ) );
    // achse für den Partialdruck machen
    axisY_ppo2 = new QValueAxis();
    axisY_ppo2->setLinePenColor( QColor( "#FFF0B0ff" ) );
    axisY_ppo2->setLabelsColor( QColor( "#FFF0B0ff" ) );
    chart->addAxis( axisY_ppo2, Qt::AlignRight );
    ppo2Series->attachAxis( axisY_ppo2 );
    axisY_ppo2->setTitleText( tr( "PPO2 [bar]" ) );
    axisY_ppo2->setLabelFormat( "%.2f" );
    axisY_ppo2->setTickCount( 12 );
    //
    // Achse für die Zeit machen
    //
    lg->debug( "ChartDataWorker::makeChartDataMini -> create time axis..." );
    // Zeitachse
    axisX_time = new QDateTimeAxis();
    axisX_time->setTickCount( 10 );
    axisX_time->setFormat( "mm:ss' min'" );
    axisX_time->setTitleText( "DIVETIME" );
    axisX_time->setTitleText( tr( "DIVE TIME [min]" ) );
    chart->addAxis( axisX_time, Qt::AlignBottom );
    depthAreaSeries->attachAxis( axisX_time );
    // depthSeries->attachAxis( axisX_time );

    // Kategorie für ppo2
    axisY_category_ppo2 = new QCategoryAxis();
    axisY_category_ppo2->append( "Low", .21 );
    axisY_category_ppo2->append( "Medium", 1.0 );
    axisY_category_ppo2->append( "High", 3.0 );
    axisY_category_ppo2->setRange( 0.0, 3.0 );
    axisY_category_ppo2->setLinePenColor( ppo2Series->pen().color() );
    axisY_category_ppo2->setGridLinePen( ( ppo2Series->pen() ) );
    chart->addAxis( axisY_category_ppo2, Qt::AlignRight );
    ppo2Series->attachAxis( axisY_category_ppo2 );

    chart->legend()->show();
    emit onChartReadySig();
    return ( true );
  }

}  // namespace spx
