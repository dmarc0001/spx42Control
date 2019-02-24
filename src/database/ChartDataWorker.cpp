#include "ChartDataWorker.hpp"

namespace spx
{
  const QString ChartDataWorker::depthSeriesName{"depthSeries"};
  const QString ChartDataWorker::depthAreaSeriesName{"depthAreaSeries"};
  const QString ChartDataWorker::tempSeriesName{"tempSeries"};
  const QString ChartDataWorker::ppo2SeriesName{"ppo2Series"};
  const QString ChartDataWorker::setpointSeriesName{"setpointSeries"};
  //
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

  void ChartDataWorker::prepareMiniChart( QChart *chart, bool isLightTheme )
  {
    lg->debug( "DiveMiniChart::prepareChart..." );
    chart->legend()->hide();  // Keine Legende in der Minivorschau
    // Chart Titel aufhübschen
    QFont font;
    font.setPixelSize( 9 );
    chart->setTitleFont( font );
    chart->setTitleBrush( QBrush( Qt::darkBlue ) );
    chart->setTitle( tr( "PREVIEW" ) );
    if ( isLightTheme )
      chart->setTheme( QChart::ChartTheme::ChartThemeLight );
    else
      chart->setTheme( QChart::ChartTheme::ChartThemeDark );
    /*
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
*/
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

  bool ChartDataWorker::prepareDiveCharts( QChart *bigchart, QChart *ppo2chart, bool isLightTheme )
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
      if ( isLightTheme )
        ch->setTheme( QChart::ChartTheme::ChartThemeLight );
      // ch->setTheme( QChart::ChartTheme::ChartThemeQt );
      else
        ch->setTheme( QChart::ChartTheme::ChartThemeDark );
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
    qreal milisecounds = 0.0;
    DiveDataSetsPtr dataSet;
    QAreaSeries *depthAreaSeries;
    QLineSeries *depthSeries;
    QLineSeries *nullDepthSeries;
    QLineSeries *tempSeries;
    QLineSeries *ppo2Series;
    QLineSeries *setpointSeries;
    QValueAxis *depthAxis;
    QValueAxis *ppo2Axis;
    QValueAxis *setpointAxis;
    QValueAxis *tempAxis;
    QDateTimeAxis *bigChartTimeAxis;
    QDateTimeAxis *littleChartTimeAxis;
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
    depthSeries->setName( ChartDataWorker::depthSeriesName );
    // Null Tiefenlinie
    nullDepthSeries = new QLineSeries();
    // Temperatur Serie
    tempSeries = new QLineSeries();
    tempSeries->setName( ChartDataWorker::tempSeriesName );
    // PO2 Serie
    ppo2Series = new QLineSeries();
    ppo2Series->setName( ChartDataWorker::ppo2SeriesName );
    // Setpoint Serie
    setpointSeries = new QLineSeries();
    setpointSeries->setName( ChartDataWorker::setpointSeriesName );
    //
    // berülle Daten in die Serien
    //
    milisecounds = 0.0;
    for ( auto singleSet : *dataSet.get() )
    {
      depthSeries->append( milisecounds, singleSet.depth );
      tempSeries->append( milisecounds, singleSet.temperature );
      ppo2Series->append( milisecounds, singleSet.ppo2 );
      setpointSeries->append( milisecounds, singleSet.setpoint );
      nullDepthSeries->append( milisecounds, 0 );
      milisecounds = milisecounds + ( static_cast< qreal >( singleSet.nextStep ) * 1000.0 );
    }
    //
    // Tiefen-Flächenserie machen
    //
    depthAreaSeries = new QAreaSeries( nullDepthSeries, depthSeries );
    depthAreaSeries->setName( ChartDataWorker::depthAreaSeriesName );
    //
    // Farben aufhübschen, Design festlegen
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
    bigchart->addSeries( depthSeries );
    bigchart->addSeries( depthAreaSeries );
    bigchart->addSeries( tempSeries );
    ppo2chart->addSeries( ppo2Series );
    ppo2chart->addSeries( setpointSeries );
    //
    // Achse für die Tiefe machen
    //
    lg->debug( "ChartDataWorker::makeChartData -> depth axis..." );
    depthAxis = new QValueAxis();
    depthAxis->setGridLineColor( QColor( "#FF202020" ) );
    depthAxis->setMax( 0.0 );
    depthAxis->setTitleText( tr( "DEPTH [m]" ) );
    depthAxis->setLabelFormat( "%.1f m" );
    depthAxis->setLabelsColor( QColor( "#FF202020" ) );
    bigchart->addAxis( depthAxis, Qt::AlignLeft );
    //
    // Area für die Tiefe machen
    //
    depthAreaSeries->attachAxis( depthAxis );
    //
    // Achse für die Temperatur machen
    //
    lg->debug( "ChartDataWorker::makeChartData -> temperature axis..." );
    tempAxis = new QValueAxis();
    tempAxis->setGridLineColor( tempSeries->color() );
    tempAxis->setTitleText( tr( "TEMP [&ordm;C]" ) );
    tempAxis->setLabelFormat( "%d&ordm;" );
    tempAxis->setLabelsColor( tempSeries->color() );
    QBrush br = tempAxis->titleBrush();
    br.setColor( tempSeries->color() );
    tempAxis->setTitleBrush( br );
    QPair< int, int > tempRange = getTempBorders( dataSet );
    tempAxis->setRange( 0 /*tempRange.first - 1*/, tempRange.second * 2 );
    bigchart->addAxis( tempAxis, Qt::AlignRight );
    tempSeries->attachAxis( tempAxis );
    //
    // Achse für die Zeit machen
    //
    lg->debug( "ChartDataWorker::makeChartData -> create time axis..." );
    // Zeitachse
    bigChartTimeAxis = new QDateTimeAxis();
    // bigChartTimeAxis->setFormat( "mm:ss' min'" );
    bigChartTimeAxis->setFormat( "H:mm:ss" );
    bigChartTimeAxis->setTitleText( tr( "DIVE TIME" ) );
    bigChartTimeAxis->setTickCount( 10 );
    bigchart->addAxis( bigChartTimeAxis, Qt::AlignBottom );
    depthSeries->attachAxis( bigChartTimeAxis );

    lg->debug( "ChartDataWorker::makeChartData -> ppo2 axis..." );
    // achse für den Partialdruck machen
    ppo2Axis = new QValueAxis();
    ppo2Axis->setLinePenColor( ppo2Series->color() );
    ppo2Axis->setLabelsColor( ppo2Series->color() );
    ppo2Axis->setTitleText( tr( "PPO2" ) );
    ppo2Axis->setLabelFormat( "%.1f bar" );
    ppo2Axis->setTickCount( 4 );
    ppo2Axis->setRange( 0.0, 3.0 );
    br = ppo2Axis->titleBrush();
    br.setColor( ppo2Series->color() );
    ppo2Axis->setTitleBrush( br );
    ppo2chart->addAxis( ppo2Axis, Qt::AlignLeft );
    ppo2Series->attachAxis( ppo2Axis );

    lg->debug( "ChartDataWorker::makeChartData -> setpoint axis..." );
    // achse für den setpoint
    setpointAxis = new QValueAxis();
    setpointAxis->setLinePenColor( setpointSeries->color() );
    setpointAxis->setLabelsColor( setpointSeries->color() );
    setpointAxis->setTitleText( tr( "SETPOINT" ) );
    setpointAxis->setLabelFormat( "%.1f bar" );
    setpointAxis->setTickCount( 4 );
    setpointAxis->setRange( 0.0, 3.0 );
    br = setpointAxis->titleBrush();
    br.setColor( setpointSeries->color() );
    setpointAxis->setTitleBrush( br );
    ppo2chart->addAxis( setpointAxis, Qt::AlignRight );
    setpointSeries->attachAxis( setpointAxis );

    lg->debug( "ChartDataWorker::makeChartData -> create 2`nd time axis..." );
    // Zeitachse
    littleChartTimeAxis = new QDateTimeAxis();
    littleChartTimeAxis->setTickCount( 10 );
    littleChartTimeAxis->setFormat( "H:mm:ss" );
    littleChartTimeAxis->setTitleText( "DIVETIME" );
    littleChartTimeAxis->setTitleText( tr( "DIVE TIME" ) );
    littleChartTimeAxis->setTickCount( 10 );
    ppo2chart->addAxis( littleChartTimeAxis, Qt::AlignBottom );
    ppo2Series->attachAxis( littleChartTimeAxis );
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
