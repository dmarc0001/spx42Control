#include "DebugDataSeriesGenerator.hpp"

using namespace QtCharts;
namespace spx
{
  DebugDataSeriesGenerator::DebugDataSeriesGenerator( std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Config > spxCfg )
      : IDataSeriesGenerator( logger, spxCfg ), gen( rd() )
  {
  }

  void DebugDataSeriesGenerator::setDeviceId( int deviceId )
  {
    this->deviceId = deviceId;
    // Verteilung der Werte für Tiefe auswählen
    std::uniform_real_distribution<> depth( 5, 90 );
    // Jetz hab ich die Verteilung für die Tiefe
    maxdepth = static_cast< double >( std::floor( depth( gen ) ) );
    // Länge des TG zwischen 30 und 90 Minuten, samples a 20 Sekunden
    std::uniform_real_distribution<> len( 600, 1800 );
    samples = static_cast< int >( std::floor( len( gen ) ) );
    dataAvaivible = true;
  }

  QLineSeries *DebugDataSeriesGenerator::makeDepthSerie( int )
  {
    auto *ser = new QLineSeries();
    double ind = 0;
    double depth = 0;
    if ( !dataAvaivible )
      return ( ser );
    //
    // Alle Samples machen
    // TODO: am Ende flacher werden lassen
    //
    for ( int i = 0; i < samples; i++ )
    {
      std::uniform_real_distribution<> dist( std::max( 0.0, depth - 1.0 ), std::min( depth + 1.8, maxdepth ) );
      ind = static_cast< double >( i );
      depth = static_cast< double >( dist( gen ) );
      *ser << QPointF( static_cast< qreal >( ind ), 0.0 - static_cast< qreal >( depth ) );
    }
    return ( ser );
  }

  QLineSeries *DebugDataSeriesGenerator::makeTempSerie( int )
  {
    auto *ser = new QLineSeries();
    return ( ser );
  }

  QLineSeries *DebugDataSeriesGenerator::makePPOSerie( int )
  {
    auto *ser = new QLineSeries();
    return ( ser );
  }

  QLineSeries *DebugDataSeriesGenerator::makeSensor1Serie( int )
  {
    auto *ser = new QLineSeries();
    return ( ser );
  }

  QLineSeries *DebugDataSeriesGenerator::makeSensor2Serie( int )
  {
    auto *ser = new QLineSeries();
    return ( ser );
  }

  QLineSeries *DebugDataSeriesGenerator::makeSensor3Serie( int )
  {
    auto *ser = new QLineSeries();
    return ( ser );
  }
}
