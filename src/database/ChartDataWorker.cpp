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

<<<<<<< HEAD
  bool ChartDataWorker::makeChartDataMini( QChart *chart, const QString &remDevice, int diveNum )
  {
    QString tableName;
    DiveChartSetPtr dataSet;
    lg->debug( "ChartDataWorker::makeChartData..." );
    tableName = database->getLogTableName( remDevice );
    if ( tableName.isNull() || tableName.isEmpty() )
    {
      lg->warn( "ChartDataWorker::makeChartData -> can't read data tablename..." );
      return ( false );
    }
    //
    // jetzt die Daten abholen
    //
    dataSet = database->getChartSet( tableName, diveNum );
=======
  bool ChartDataWorker::makeChartDataMini( QChart *chart, const QString &deviceMac, int diveNum )
  {
    QString tableName;
    DiveChartSetPtr dataSet;
    lg->debug( QString( "ChartDataWorker::makeChartDataMini for <%1>, num <%2>..." ).arg( deviceMac ).arg( diveNum ) );
    //
    // jetzt die Daten abholen
    //
    dataSet = database->getChartSet( deviceMac, diveNum );
>>>>>>> dev/stabilizing
    // hat es sich gelohnt
    if ( dataSet->isEmpty() )
      return ( false );
    //
    // jetzt die Daten für das Chart machen
    //
<<<<<<< HEAD
    lg->debug( "ChartDataWorker::makeChartData -> create depth serie..." );
=======
    lg->debug( "ChartDataWorker::makeChartDataMini -> create depth serie..." );
>>>>>>> dev/stabilizing
    // tiefe Datenserie
    QSplineSeries *depthSeries = new QSplineSeries();
    // berülle Daten
    for ( auto singleSet : *dataSet.get() )
    {
      depthSeries->append( singleSet.lfdnr, singleSet.depth );
    }
    chart->addSeries( depthSeries );
    QValueAxis *axisYDepth = new QValueAxis;
    axisYDepth->setLinePenColor( depthSeries->pen().color() );
    chart->addAxis( axisYDepth, Qt::AlignLeft );
    depthSeries->attachAxis( axisYDepth );
    axisYDepth->setMax( 0.0 );

<<<<<<< HEAD
    lg->debug( "ChartDataWorker::makeChartData -> create ppo2 serie..." );
=======
    lg->debug( "ChartDataWorker::makeChartDataMini -> create ppo2 serie..." );
>>>>>>> dev/stabilizing
    // ppo2 Serie
    QSplineSeries *ppo2Series = new QSplineSeries();
    for ( auto singleSet : *dataSet.get() )
    {
      ppo2Series->append( singleSet.lfdnr, singleSet.ppo2 );
    }
    chart->addSeries( ppo2Series );
    QValueAxis *axisYPPO2 = new QValueAxis;
    axisYPPO2->setLinePenColor( depthSeries->pen().color() );
    chart->addAxis( axisYPPO2, Qt::AlignRight );
    ppo2Series->attachAxis( axisYPPO2 );

<<<<<<< HEAD
    lg->debug( "ChartDataWorker::makeChartData -> create time axis..." );
=======
    lg->debug( "ChartDataWorker::makeChartDataMini -> create time axis..." );
>>>>>>> dev/stabilizing
    // Zeitachse, dimension aus DB lesen
    QValueAxis *axisX = new QValueAxis();
    axisX->setTickCount( dataSet->count() );
    chart->addAxis( axisX, Qt::AlignBottom );

    /*
    // Kategorie für ppo2
    QCategoryAxis *axisY3 = new QCategoryAxis;
    axisY3->append( "Low", .21 );
    axisY3->append( "Medium", 1.0 );
    axisY3->append( "High", 1.6 );
    axisY3->setLinePenColor( ppo2Series->pen().color() );
    axisY3->setGridLinePen( ( ppo2Series->pen() ) );
    chart->addAxis( axisY3, Qt::AlignRight );
    ppo2Series->attachAxis( axisY3 );
    */
    return ( true );
  }
<<<<<<< HEAD
}
=======
}  // namespace spx
>>>>>>> dev/stabilizing
