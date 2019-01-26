#include "spx42uddfexport.hpp"
#include <QDateTime>
#include <QFile>

namespace spx
{
  UddfGasDefinition::UddfGasDefinition() : gName( "GAS_2178000000" ), o2( 21.0 ), n2( 79.0 ), he( 0.0 ), ar( 0.0 ), h2( 0.0 )
  {
  }

  UddfGasDefinition::UddfGasDefinition( int _n2, int _he )
      : n2( static_cast< double >( _n2 ) / 100.0 ), he( static_cast< double >( _he ) / 100.0 ), ar( 0.0 ), h2( 0.0 )
  {
    int _o2 = 100 - ( _n2 + _he );
    o2 = 1.0 - ( n2 + he );
    gName = QString( "GAS_%1%2%3" )
                .arg( _o2, 2, 10, QChar( '0' ) )
                .arg( _n2, 2, 10, QChar( '0' ) )
                .arg( _he, 2, 10, QChar( '0' ) )
                .arg( "0000" );
  }

  UddfGasDefinition::UddfGasDefinition( double n2, double he ) : n2( n2 ), he( he ), ar( 0.0 ), h2( 0.0 )
  {
    o2 = 100.0 - ( n2 + he );
    // TODO: String für Namen erzeugen
  }

  UddfGasDefinition::UddfGasDefinition( double o2, double n2, double he, double ar, double h2 )
      : o2( o2 ), n2( n2 ), he( he ), ar( ar ), h2( h2 )
  {
    // TODO: String für Namen erzeugen
  }

  QString UddfGasDefinition::getName( void ) const
  {
    return ( gName );
  }

  QString UddfGasDefinition::getO2( void ) const
  {
    return ( getFloatStr( o2 ) );
  }

  QString UddfGasDefinition::getN2( void ) const
  {
    return ( getFloatStr( n2 ) );
  }

  QString UddfGasDefinition::getHe( void ) const
  {
    return ( getFloatStr( he ) );
  }

  QString UddfGasDefinition::getAr( void ) const
  {
    return ( getFloatStr( ar ) );
  }

  QString UddfGasDefinition::getH2( void ) const
  {
    return ( getFloatStr( h2 ) );
  }

  QString UddfGasDefinition::getFloatStr( double val ) const
  {
    QString retval = QString( "%1" ).arg( val, 0, 'f', 3, QChar( '0' ) );
    return ( retval );
  }

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
    // Gasliste löschen (für alle Fälle)
    //
    gasDefs.clear();
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
    //
    // Gase suchen und speichern
    //
    UsedGasList gList = database->getGasList( device_mac, &diveNums );
    for ( auto entry : gList )
    {
      UddfGasDefinition def( entry.first, entry.second );
      gasDefs.append( def );
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

    //
    // Informationen über den Hersteller schreiben
    //
    writeGenerator( stream );

    //
    // Gasdefinitionen finden und schreiben
    //
    writeGasDefinitions( stream );

    stream.writeEndElement();
    stream.writeEndDocument();
    file.close();
    lg->info( QString( "SPX42UDDFExport::createExportXml -> successful exportet to file <%1>" ).arg( exportFile ) );
    return ( exported );
  }

  //###########################################################################
  //#### private routinen                                                  ####
  //###########################################################################

  bool SPX42UDDFExport::writeGenerator( QXmlStreamWriter &st )
  {
    lg->debug( "SPX42UDDFExport::writeGenerator..." );
    //
    // wurzeltag für diese Ebene
    //
    st.writeStartElement( "generator" );

    // Name als textelement
    st.writeTextElement( "name", ProjectConst::MANUFACTURER_APP );

    // app Version als Textelement
    st.writeTextElement( "version", ProjectConst::APP_VERSION );
    // Hersteller-Block
    st.writeStartElement( "manufacturer" );

    // Textelement Hersteller name
    st.writeTextElement( "name", ProjectConst::MANUFACTURER_APP_NAME );

    // Kontakt-Block
    st.writeStartElement( "contact" );

    // Text-Element email
    st.writeTextElement( "email", ProjectConst::MANUFACTURER_EMAIL );
    st.writeTextElement( "homepage", ProjectConst::MANUFACTURER_WWW );

    // Kontakt-Block ENDE
    st.writeEndElement();

    // Hersteller-Block ENDE
    st.writeEndElement();

    // Datumsblock
    st.writeStartElement( "Date" );
    st.writeTextElement( "year", QDate::currentDate().toString( "yyyy" ) );
    st.writeTextElement( "month", QDate::currentDate().toString( "MM" ) );
    st.writeTextElement( "day", QDate::currentDate().toString( "dd" ) );

    // Datumsblock ENDE
    st.writeEndElement();

    //
    // Wurzel schliessen
    //
    st.writeEndElement();
    lg->debug( "SPX42UDDFExport::writeGenerator...OK" );
    return ( true );
  }

  bool SPX42UDDFExport::writeGasDefinitions( QXmlStreamWriter &st )
  {
    lg->debug( "SPX42UDDFExport::writeGasDefinitions..." );
    //
    // Gasdefinitionen zusammenstellen
    //

    // Block tag schreiben
    st.writeStartElement( "gasdefinitions" );
    //
    for ( auto entry : gasDefs )
    {
      // unterblock "mix"
      st.writeStartElement( "mix" );
      st.writeAttribute( "id", entry.getName() );
      // Textnode name => momentan wie id
      st.writeTextElement( "name", entry.getName() );
      // Textnode o2
      st.writeTextElement( "o2", entry.getO2() );
      // Textnode n2
      st.writeTextElement( "n2", entry.getN2() );
      // Textnode he
      st.writeTextElement( "he", entry.getHe() );
      // Textnode Argon
      st.writeTextElement( "ar", entry.getAr() );
      // Textnode Wasserstoff
      st.writeTextElement( "h2", entry.getH2() );
      // "mix" schliessen
      st.writeEndElement();
    }
    // Block schliessen
    st.writeEndElement();
    //
    lg->debug( "SPX42UDDFExport::writeGasDefinitions...OK" );
    return ( true );
  }

}  // namespace spx
