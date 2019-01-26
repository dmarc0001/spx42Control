#include "spx42uddfexport.hpp"
#include <QDateTime>
#include <QFile>

namespace spx
{
  SPX42UDDFExport::SPX42UDDFExport( std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Database > _database, QObject *parent )
      : QObject( parent ), lg( logger ), database( _database )
  {
    lg->debug( "SPX42UDDFExport::SPX42UDDFExport(...)" );
  }

  SPX42UDDFExport::~SPX42UDDFExport()
  {
    lg->debug( "SPX42UDDFExport::~SPX42UDDFExport" );
  }

  void SPX42UDDFExport::setXmlFileBaseName( const QString &_fileName )
  {
    XMLFileNameTemplate = _fileName + "%1.uddf";
    lg->debug( QString( "SPX42UDDFExport::setXmlFileBaseName -> file base name template: %1" ).arg( XMLFileNameTemplate ) );
  }

  void SPX42UDDFExport::setExportDives( const QString &mac, const QVector< int > &numbers )
  {
    lg->debug( QString( "SPX42UDDFExport::setExportDives -> device: %1" ).arg( mac ) );
    device_mac = mac;
    diveNums.clear();
    diveNums.append( numbers );
  }

  int SPX42UDDFExport::createExportXml()
  {
    int exported = 0;
    lg->debug( "SPX42UDDFExport::createExportXml" );
    //
    // erzeuge Datei
    //
    QString exportFile( XMLFileNameTemplate.arg( QDateTime::currentDateTime().toString( "yyyyMMddHHmmss" ) ) );
    QFile file( exportFile );
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
      lg->crit( "SPX42UDDFExport::createExportXml -> can't open export file! ABORT" );
      return ( -1 );
    }
    QXmlStreamWriter stream( &file );
    stream.setAutoFormatting( true );
    stream.setAutoFormattingIndent( 2 );
    stream.setCodec( "UTF-8" );
    stream.writeStartDocument();
    //
    // jetzt beginne ich mit dem Wurzelelement
    //
    stream.writeStartElement( "uddf" );
    stream.writeAttribute( "version", "2.2.0" );
    writeGenerator( stream );

    stream.writeEndElement();
    stream.writeEndDocument();
    file.close();
    lg->info( QString( "SPX42UDDFExport::createExportXml -> successful exportet to file <%1>" ).arg( exportFile ) );
    return ( exported );
  }

  //###########################################################################
  //#### private routinen                                                  ####
  //###########################################################################

  bool SPX42UDDFExport::writeGenerator( QXmlStreamWriter &stream )
  {
    //
    // wurzeltag für diese Ebene
    //
    stream.writeStartElement( "generator" );

    // Name als textelement
    stream.writeTextElement( "name", ProjectConst::MANUFACTURER_APP );

    // app Version als Textelement
    stream.writeTextElement( "version", ProjectConst::APP_VERSION );
    // Hersteller-Block
    stream.writeStartElement( "manufacturer" );

    // Textelement Hersteller name
    stream.writeTextElement( "name", ProjectConst::MANUFACTURER_APP_NAME );

    // Kontakt-Block
    stream.writeStartElement( "contact" );

    // Text-Element email
    stream.writeTextElement( "email", ProjectConst::MANUFACTURER_EMAIL );
    stream.writeTextElement( "homepage", ProjectConst::MANUFACTURER_WWW );

    // Kontakt-Block ENDE
    stream.writeEndElement();

    // Hersteller-Block ENDE
    stream.writeEndElement();

    // Datumsblock
    stream.writeStartElement( "Date" );
    stream.writeTextElement( "year", QDate::currentDate().toString( "yyyy" ) );
    stream.writeTextElement( "month", QDate::currentDate().toString( "MM" ) );
    stream.writeTextElement( "day", QDate::currentDate().toString( "dd" ) );

    // Datumsblock ENDE
    stream.writeEndElement();

    //
    // Wurzel schliessen
    //
    stream.writeEndElement();
    return ( true );
  }

}  // namespace spx
