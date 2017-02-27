#include "DebugDataSeriesGenerator.hpp"

using namespace QtCharts;
namespace spx42
{
  DebugDataSeriesGenerator::DebugDataSeriesGenerator( std::shared_ptr<Logger> logger, std::shared_ptr<SPX42Config> spxCfg) :
    IDataSeriesGenerator( logger, spxCfg ),
    gen(rd())
  {
  }

  DebugDataSeriesGenerator::~DebugDataSeriesGenerator()
  {

  }

  void DebugDataSeriesGenerator::setDeviceId( int deviceId )
  {
    this->deviceId = deviceId;
    // Verteilung der Werte für Tiefe auswählen
    std::uniform_real_distribution<> depth(5, 90);
    // Jetz hab ich die Verteilung für die Tiefe
    maxdepth =  static_cast<float>( std::floor( depth( gen ) ) );
    // Länge des TG zwischen 30 und 90 Minuten, samples a 20 Sekunden
    std::uniform_real_distribution<> len(600, 1800);
    samples = static_cast<int>(std::floor( len( gen ) ) );
    dataAvaivible = true;
  }

  QLineSeries* DebugDataSeriesGenerator::makeDepthSerie( int diveNum )
  {
    QLineSeries* ser = new QLineSeries();
    float ind = 0;
    float depth = 0;
    if( !dataAvaivible ) return( ser );
    //
    // Alle Samples machen
    // TODO: am Ende flacher werden lassen
    //
    for( int i = 0; i < samples; i ++ )
    {
      std::uniform_real_distribution<> dist( std::max( 0.0f, depth - 1.0f ), std::min( depth + 1.8f, maxdepth ) );
      ind = static_cast<float>( i );
      depth = static_cast<float>( dist( gen ) );
      *ser << QPointF( ind, 0.0 - depth );
    }
    return( ser );
  }

  QLineSeries* DebugDataSeriesGenerator::makeTempSerie( int diveNum )
  {
    QLineSeries* ser = new QLineSeries();
    return( ser );
  }

  QLineSeries* DebugDataSeriesGenerator::makePPOSerie( int diveNum )
  {
    QLineSeries* ser = new QLineSeries();
    return( ser );
  }

  QLineSeries* DebugDataSeriesGenerator::makeSensor1Serie( int diveNum )
  {
    QLineSeries* ser = new QLineSeries();
    return( ser );
  }

  QLineSeries* DebugDataSeriesGenerator::makeSensor2Serie( int diveNum )
  {
    QLineSeries* ser = new QLineSeries();
    return( ser );
  }

  QLineSeries* DebugDataSeriesGenerator::makeSensor3Serie( int diveNum )
  {
    QLineSeries* ser = new QLineSeries();
    return( ser );
  }

}
