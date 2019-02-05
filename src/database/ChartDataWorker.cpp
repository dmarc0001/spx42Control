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
    QLineSeries *ppo2Series;
    QValueAxis *axisYDepth;
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
    lg->debug( "ChartDataWorker::makeChartDataMini -> create series..." );
    // tiefe Datenserie
    depthSeries = new QLineSeries();
    // ppo2 Serie
    ppo2Series = new QLineSeries();

    // berülle Daten
    for ( auto singleSet : *dataSet.get() )
    {
      depthSeries->append( singleSet.lfdnr, singleSet.depth );
      ppo2Series->append( singleSet.lfdnr, singleSet.ppo2 );
    }
    // Tiefe Datenserie
    chart->addSeries( depthSeries );
    axisYDepth = new QValueAxis();
    axisYDepth->setLinePenColor( depthSeries->pen().color() );
    axisYDepth->setLabelFormat( "%.1f m" );
    chart->addAxis( axisYDepth, Qt::AlignLeft );
    depthSeries->attachAxis( axisYDepth );
    axisYDepth->setMax( 0.0 );
    // ppo2 Serie
    chart->addSeries( ppo2Series );
    axisYPPO2 = new QValueAxis();
    axisYPPO2->setLinePenColor( depthSeries->pen().color() );
    axisYPPO2->setLabelFormat( "%.2f bar" );
    axisYPPO2->setTickCount( 4 );
    axisYPPO2->setRange( 0.0, 3.0 );
    chart->addAxis( axisYPPO2, Qt::AlignRight );
    ppo2Series->attachAxis( axisYPPO2 );
    //
    lg->debug( "ChartDataWorker::makeChartDataMini -> create time axis..." );
    // Zeitachse, dimension aus DB lesen
    axisX = new QValueAxis();
    axisX->setTickCount( dataSet->count() );
    chart->addAxis( axisX, Qt::AlignBottom );
    return ( true );
  }

  bool ChartDataWorker::prepareDiveCharts( QChart *bigchart, QChart *ppo2chart )
  {
    QFont font;
    QBrush backgroundBrush( Qt::NoBrush );
    QLinearGradient plotAreaGradient;
    //
    lg->debug( "ChartDataWorker::makeDiveChart ->prepareChart..." );
    // Chart Titel aufhübschen
    font.setPointSize( 13 );
    for ( auto ch : {bigchart, ppo2chart} )
    {
      ch->setTitleBrush( QBrush( Qt::darkBlue ) );
      ch->setTitleFont( font );
      ch->setTheme( QChart::ChartTheme::ChartThemeQt );
      // ch->setTheme( QChart::ChartTheme::ChartThemeDark );
      // ch->setTheme( QChart::ChartTheme::ChartThemeBrownSand );
      // ch->setTheme( QChart::ChartTheme::ChartThemeLight );
      ch->setAnimationOptions( QChart::SeriesAnimations );
      ch->legend()->setAlignment( Qt::AlignBottom );
      ch->legend()->hide();
      QMargins mar = ch->margins();
      mar.setTop( 0 );
      mar.setBottom( 0 );
      ch->setMargins( mar );
    }
    // bigchart->setTitle( tr( "DIVE VIEW" ) );
    bigchart->title().clear();
    ppo2chart->title().clear();
    lg->debug( "ChartDataWorker::makeDiveChart ->prepareChart...OK" );
    return ( true );
  }

  bool ChartDataWorker::makeDiveChart( QtCharts::QChart *bigchart, QChart *ppo2chart, const QString &deviceMac, int diveNum )
  {
    qint64 milisecounds = 0;
    DiveDataSetsPtr dataSet;
    QAreaSeries *depthAreaSeries;
    QLineSeries *depthSeries;
    QLineSeries *nullDepthSeries;
    QLineSeries *tempSeries;
    QLineSeries *ppo2Series;
    QLineSeries *setpointSeries;
    QValueAxis *axisY_depth;
    QValueAxis *axisY_ppo2;
    QValueAxis *axisY_setpoint;
    QValueAxis *axisY_temp;
    QDateTimeAxis *axisX_time;
    QDateTimeAxis *axisX_time2;
    // QCategoryAxis *axisY_category_ppo2;
    lg->debug( "ChartDataWorker::makeChartData..." );
    bigchart->setParent( nullptr );
    ppo2chart->setParent( nullptr );
    //
    // aufräumen
    //
    for ( auto ch : {bigchart, ppo2chart} )
    {
      ch->removeAllSeries();
      for ( auto ax : bigchart->axes( Qt::Horizontal ) )
      {
        ch->removeAxis( ax );
      }
      for ( auto ax : bigchart->axes( Qt::Vertical ) )
      {
        ch->removeAxis( ax );
      }
      lg->debug( "ChartDataWorker::makeDiveChart -> cleaned series and axis..." );
      ch->createDefaultAxes();
    }
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
    lg->debug( "ChartDataWorker::makeChartData -> create series..." );
    //
    // Datenserien machen
    //
    // Tiefe Serie
    depthSeries = new QLineSeries();
    // Null Tiefenlinie
    nullDepthSeries = new QLineSeries();
    // Temperatur Serie
    tempSeries = new QLineSeries();
    // PO2 Serie
    ppo2Series = new QLineSeries();
    // Setpoint Serie
    setpointSeries = new QLineSeries();
    //
    // berülle Daten in die Serien
    //
    milisecounds = 0;
    for ( auto singleSet : *dataSet.get() )
    {
      depthSeries->append( milisecounds, singleSet.depth );
      tempSeries->append( milisecounds, singleSet.temperature );
      ppo2Series->append( milisecounds, singleSet.ppo2 );
      setpointSeries->append( milisecounds, singleSet.setpoint );
      nullDepthSeries->append( milisecounds, 0 );
      milisecounds += singleSet.nextStep * 1000;
    }
    depthAreaSeries = new QAreaSeries( nullDepthSeries, depthSeries );
    //
    // Seiien aufhübschen, Design festlegen
    //
    lg->debug( "ChartDataWorker::makeChartData -> make designs..." );
    depthAreaSeries->setColor( QColor( "#30404040" ) );
    QPen myPen = depthAreaSeries->pen();
    myPen.setColor( Qt::darkBlue );
    myPen.setWidth( 4 );
    depthAreaSeries->setPen( myPen );
    //
    myPen = ppo2Series->pen();
    myPen.setColor( QColor( "#B0ff0000" ) );
    myPen.setWidth( 3 );
    ppo2Series->setPen( myPen );
    //
    myPen = setpointSeries->pen();
    myPen.setColor( QColor( "#B000ED77" ) );
    myPen.setWidth( 2 );
    setpointSeries->setPen( myPen );
    //
    myPen = tempSeries->pen();
    myPen.setColor( QColor( "#ff8A1CC9" ) );
    myPen.setWidth( 3 );
    tempSeries->setPen( myPen );
    //
    // Serien zum Chart zufügen
    //
    lg->debug( "ChartDataWorker::makeChartData -> add serieas to charts..." );
    bigchart->addSeries( depthAreaSeries );
    bigchart->addSeries( tempSeries );
    ppo2chart->addSeries( ppo2Series );
    ppo2chart->addSeries( setpointSeries );
    //
    // Achse für die Tiefe machen
    //
    lg->debug( "ChartDataWorker::makeChartData -> depth axis..." );
    axisY_depth = new QValueAxis();
    axisY_depth->setGridLineColor( QColor( "#FF202020" ) );
    axisY_depth->setMax( 0.0 );
    axisY_depth->setTitleText( tr( "DEPTH [m]" ) );
    axisY_depth->setLabelFormat( "%.1f m" );
    axisY_depth->setLabelsColor( QColor( "#FF202020" ) );
    bigchart->addAxis( axisY_depth, Qt::AlignLeft );
    //
    // Area für die Tiefe machen
    //
    depthAreaSeries->attachAxis( axisY_depth );
    //
    // Achse für die Temperatur machen
    //
    lg->debug( "ChartDataWorker::makeChartData -> temperature axis..." );
    axisY_temp = new QValueAxis();
    axisY_temp->setGridLineColor( tempSeries->color() );
    axisY_temp->setTitleText( tr( "TEMP [&ordm;C]" ) );
    axisY_temp->setLabelFormat( "%d&ordm;" );
    axisY_temp->setLabelsColor( tempSeries->color() );
    QBrush br = axisY_temp->titleBrush();
    br.setColor( tempSeries->color() );
    axisY_temp->setTitleBrush( br );
    QPair< int, int > tempRange = getTempBorders( dataSet );
    axisY_temp->setRange( 0 /*tempRange.first - 1*/, tempRange.second * 2 );
    bigchart->addAxis( axisY_temp, Qt::AlignRight );
    tempSeries->attachAxis( axisY_temp );
    //
    // Achse für die Zeit machen
    //
    lg->debug( "ChartDataWorker::makeChartData -> create time axis..." );
    // Zeitachse
    axisX_time = new QDateTimeAxis();
    axisX_time->setTickCount( 10 );
    axisX_time->setFormat( "mm:ss' min'" );
    axisX_time->setTitleText( "DIVETIME" );
    axisX_time->setTitleText( tr( "DIVE TIME [min]" ) );
    bigchart->addAxis( axisX_time, Qt::AlignBottom );
    depthAreaSeries->attachAxis( axisX_time );
    // ppo2chart->addAxis( axisX_time, Qt::AlignBottom );

    lg->debug( "ChartDataWorker::makeChartData -> ppo2 axis..." );
    // achse für den Partialdruck machen
    axisY_ppo2 = new QValueAxis();
    axisY_ppo2->setLinePenColor( ppo2Series->color() );
    axisY_ppo2->setLabelsColor( ppo2Series->color() );
    axisY_ppo2->setTitleText( tr( "PPO2" ) );
    axisY_ppo2->setLabelFormat( "%.1f bar" );
    axisY_ppo2->setTickCount( 4 );
    axisY_ppo2->setRange( 0.0, 3.0 );
    br = axisY_ppo2->titleBrush();
    br.setColor( ppo2Series->color() );
    axisY_ppo2->setTitleBrush( br );
    ppo2chart->addAxis( axisY_ppo2, Qt::AlignLeft );
    ppo2Series->attachAxis( axisY_ppo2 );

    lg->debug( "ChartDataWorker::makeChartData -> setpoint axis..." );
    // achse für den setpoint
    axisY_setpoint = new QValueAxis();
    axisY_setpoint->setLinePenColor( setpointSeries->color() );
    axisY_setpoint->setLabelsColor( setpointSeries->color() );
    axisY_setpoint->setTitleText( tr( "SETPOINT" ) );
    axisY_setpoint->setLabelFormat( "%.1f bar" );
    axisY_setpoint->setTickCount( 4 );
    axisY_setpoint->setRange( 0.0, 3.0 );
    br = axisY_setpoint->titleBrush();
    br.setColor( setpointSeries->color() );
    axisY_setpoint->setTitleBrush( br );
    ppo2chart->addAxis( axisY_setpoint, Qt::AlignRight );
    setpointSeries->attachAxis( axisY_setpoint );

    lg->debug( "ChartDataWorker::makeChartData -> create 2`nd time axis..." );
    // Zeitachse
    axisX_time2 = new QDateTimeAxis();
    axisX_time2->setTickCount( 10 );
    axisX_time2->setFormat( "mm:ss' min'" );
    axisX_time2->setTitleText( "DIVETIME" );
    axisX_time2->setTitleText( tr( "DIVE TIME [min]" ) );
    ppo2chart->addAxis( axisX_time2, Qt::AlignBottom );
    ppo2Series->attachAxis( axisX_time2 );
    // TODO: Kategorie für ppo2
    /*
    lg->debug( "ChartDataWorker::makeChartData -> create category..." );
    axisY_category_ppo2 = new QCategoryAxis();
    axisY_category_ppo2->append( "Low", .21 );
    axisY_category_ppo2->append( "Medium", 1.0 );
    axisY_category_ppo2->append( "High", 3.0 );
    axisY_category_ppo2->setRange( 0.0, 3.0 );
    axisY_category_ppo2->setLinePenColor( ppo2Series->pen().color() );
    axisY_category_ppo2->setGridLinePen( ( ppo2Series->pen() ) );
    lg->debug( "ChartDataWorker::makeChartData -> attach category..." );
    ppo2Series->attachAxis( axisY_category_ppo2 );
    ppo2chart->addAxis( axisY_category_ppo2, Qt::AlignRight );
   */
    //
    lg->debug( "ChartDataWorker::makeChartData...OK" );
    emit onChartReadySig();
    return ( true );
  }

  QPair< int, int > ChartDataWorker::getTempBorders( const DiveDataSetsPtr dataSet )
  {
    QPair< int, int > borders{100, -100};
    for ( auto dSet : *dataSet.get() )
    {
      if ( dSet.temperature < borders.first )
        borders.first = dSet.temperature;
      if ( dSet.temperature > borders.second )
        borders.second = dSet.temperature;
    }
    return ( borders );
  }
}  // namespace spx
