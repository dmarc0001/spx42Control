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
    *lg << LDEBUG << "SPX42UDDFExport::SPX42UDDFExport(...)" << Qt::endl;
  }

  SPX42UDDFExport::~SPX42UDDFExport()
  {
    *lg << LDEBUG << "SPX42UDDFExport::~SPX42UDDFExport" << Qt::endl;
  }

  void SPX42UDDFExport::setXmlFileBaseName( const QString &_fileName )
  {
    XMLFileNameTemplate = _fileName + "%1-from-%2-to-%3-count-%4.uddf";
    *lg << LDEBUG << "SPX42UDDFExport::setXmlFileBaseName -> file base name template: " << XMLFileNameTemplate << Qt::endl;
  }

  void SPX42UDDFExport::setExportDives( const QString &mac, const QVector< int > &numbers )
  {
    *lg << LDEBUG << "SPX42UDDFExport::setExportDives -> device: %1" << mac << Qt::endl;
    device_mac = mac;
    diveNums.clear();
    diveNums.append( numbers );
    // sortiere nach Nummer, macht es übersichtlicher
    std::sort( diveNums.begin(), diveNums.end() );
  }

  bool SPX42UDDFExport::createExportXml()
  {
    int fromNum = diveNums.first();
    int toNum = diveNums.last();
    int diveCount = diveNums.count();
    *lg << LDEBUG << "SPX42UDDFExport::createExportXml" << Qt::endl;
    //
    // Gasliste löschen (für alle Fälle)
    //
    gasDefs.clear();
    //
    // erzeuge Datei
    //
    QString exportFile( XMLFileNameTemplate.arg( QDateTime::currentDateTime().toString( "yyyyMMddHHmmss" ) )
                            .arg( fromNum, 3, 10, QChar( '0' ) )
                            .arg( toNum, 3, 10, QChar( '0' ) )
                            .arg( diveCount, 3, 10, QChar( '0' ) ) );
    QFile file( exportFile );
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
      *lg << LCRIT << "SPX42UDDFExport::createExportXml -> can't open export file! ABORT" << Qt::endl;
      emit onEndSavedUddfFiileSig( false, exportFile );
      return ( false );
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
    stream.writeAttribute( "version", "3.2.2" );

    //
    // Informationen über den Hersteller schreiben
    //
    if ( !writeGenerator( stream ) )
    {
      file.close();
      file.remove();
      emit onEndSavedUddfFiileSig( false, exportFile );
      return ( false );
    }

    //
    // Gasdefinitionen finden und schreiben
    //
    if ( !writeGasDefinitions( stream ) )
    {
      file.close();
      file.remove();
      emit onEndSavedUddfFiileSig( false, exportFile );
      return ( false );
    }

    //
    // Definition für Rebreather im owner Abschnitt
    //
    if ( !writeDiverDefinitions( stream ) )
    {
      file.close();
      file.remove();
      emit onEndSavedUddfFiileSig( false, exportFile );
      return ( false );
    }

    //
    // Profiledata Sektion (null oder genau einmal)
    //
    if ( !writeProfileData( stream ) )
    {
      file.close();
      file.remove();
      emit onEndSavedUddfFiileSig( false, exportFile );
      return ( false );
    }

    stream.writeEndElement();
    stream.writeEndDocument();
    file.close();
    *lg << LINFO << "SPX42UDDFExport::createExportXml -> successful exportet to file <" << exportFile << ">" << Qt::endl;
    emit onEndSavedUddfFiileSig( true, exportFile );
    return ( true );
  }

  //###########################################################################
  //#### private routinen                                                  ####
  //###########################################################################

  bool SPX42UDDFExport::writeGenerator( QXmlStreamWriter &st )
  {
    *lg << LDEBUG << "SPX42UDDFExport::writeGenerator..." << Qt::endl;
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
    *lg << LDEBUG << "SPX42UDDFExport::writeGenerator...OK" << Qt::endl;
    return ( true );
  }

  bool SPX42UDDFExport::writeDiverDefinitions( QXmlStreamWriter &st )
  {
    *lg << LDEBUG << "SPX42UDDFExport::writeOwnerDefinitions..." << Qt::endl;
    // Block owner eröffnen
    st.writeStartElement( "diver" );

    //
    // ich brauche für die Sensoren einen owner (darf nur einmal und muss das erste subtag sein)
    //
    if ( !writeOwnerDefinitions( st ) )
      return ( false );

    //
    // Möglich nicht ein bis x buddy Elemente
    //

    // diver Block schliessen
    st.writeEndElement();
    //
    *lg << LDEBUG << "SPX42UDDFExport::writeOwnerDefinitions...OK" << Qt::endl;
    return ( true );
  }

  bool SPX42UDDFExport::writeOwnerDefinitions( QXmlStreamWriter &st )
  {
    *lg << LDEBUG << "SPX42UDDFExport::writeOwnerDefinitions..." << Qt::endl;
    // Block owner öffnen
    st.writeStartElement( "owner" );
    // TODO: Eigentümername berücksichtigen
    st.writeAttribute( "id", "yourself" );

    //
    // Ausrüstungsblock einfüfen, für Rebreather
    //
    if ( !writeOwnerEquipment( st ) )
      return ( false );
    //
    // TODO: Möglich sind hier noch Angaben zum Taucher selber
    // <address>, <contact>, <diveinsurances> <divepermissions>, <education>, <medical>, <notes>, <personal>
    //

    // Block owner wieder schliessen
    st.writeEndElement();
    //
    *lg << LDEBUG << "SPX42UDDFExport::writeOwnerDefinitions...OK" << Qt::endl;
    return ( true );
  }

  bool SPX42UDDFExport::writeOwnerEquipment( QXmlStreamWriter &st )
  {
    *lg << LDEBUG << "SPX42UDDFExport::writeOwnerEquipment..." << Qt::endl;
    // equipment block öffnen
    st.writeStartElement( "equipment" );

    //
    // an dieser Stele kommt der Rebreather
    //
    if ( !writeRebreatherDefinitions( st ) )
      return ( false );

    // equipment block schliessen
    st.writeEndElement();
    //
    *lg << LDEBUG << "SPX42UDDFExport::writeOwnerEquipment...OK" << Qt::endl;
    return ( true );
  }

  bool SPX42UDDFExport::writeRebreatherDefinitions( QXmlStreamWriter &st )
  {
    *lg << LDEBUG << "SPX42UDDFExport::writeRebreatherDefinitions..." << Qt::endl;
    // rebreather Block öffnen
    st.writeStartElement( "rebreather" );
    st.writeAttribute( "id", "submatix_rebreather" );

    //
    // Name des Gerätes
    //
    st.writeTextElement( "name", "REBRETHER ONE" );
    //
    // Block Hersteller
    //
    st.writeStartElement( "manufacturer" );
    // Name des Herstellers
    st.writeTextElement( "name", "SUBMATIX" );
    // Herstellerblock schliessen
    st.writeEndElement();

    // Modell
    st.writeTextElement( "model", "EXP or MINI" );

    // Sauerstoffsensoren
    st.writeStartElement( "o2sensor" );
    st.writeAttribute( "id", "o2_sensor_00" );
    st.writeTextElement( "model", "RESULT" );
    st.writeEndElement();
    //
    if ( !writeO2Sensor( 1, st ) )
      return ( false );
    if ( !writeO2Sensor( 2, st ) )
      return ( false );
    if ( !writeO2Sensor( 3, st ) )
      return ( false );

    // rebreather Block schliessen
    st.writeEndElement();
    *lg << LDEBUG << "SPX42UDDFExport::writeRebreatherDefinitions...OK" << Qt::endl;
    return ( true );
  }

  bool SPX42UDDFExport::writeO2Sensor( int number, QXmlStreamWriter &st )
  {
    *lg << LDEBUG << "SPX42UDDFExport::writeO2Sensor -> write def for sensor #" << number << "..." << Qt::endl;
    // Sensorblockeröffnen
    st.writeStartElement( "o2sensor" );
    st.writeAttribute( "id", QString( "o2_sensor_%1" ).arg( number, 2, 10, QChar( '0' ) ) );

    // Modell TODO: Konfigurierbar machen?
    st.writeTextElement( "model", ProjectConst::REBREATHER_SENSOR_TYPE );
    // TODO: Seriennummer/Kaufdatum

    // swensorblock schliessen
    st.writeEndElement();
    *lg << LDEBUG << "SPX42UDDFExport::writeO2Sensor...OK" << Qt::endl;
    return ( true );
  }

  bool SPX42UDDFExport::writeGasDefinitions( QXmlStreamWriter &st )
  {
    *lg << LDEBUG << "SPX42UDDFExport::writeGasDefinitions..." << Qt::endl;
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
    *lg << LDEBUG << "SPX42UDDFExport::writeGasDefinitions...OK" << Qt::endl;
    return ( true );
  }

  bool SPX42UDDFExport::writeProfileData( QXmlStreamWriter &st )
  {
    *lg << LDEBUG << "SPX42UDDFExport::writeProfileData..." << Qt::endl;
    // Block "profiledata"
    st.writeStartElement( "profiledata" );
    //
    // da ich nur Tauchgänge archiviere mache ich auch nur eine einzige repetitiongroup
    // dafür, deko-Berechnungen sind nachträglch etwas verspätet ;-)
    //
    // unterblock repetitiongroup
    st.writeStartElement( "repetitiongroup" );
    st.writeAttribute( "id", "1" );

    // jetzt füge Tauchgänge ein
    if ( !writeAllDives( st ) )
      return ( false );

    // repetitiongroup schliessen
    st.writeEndElement();

    // Block schliessen
    st.writeEndElement();
    //
    *lg << LDEBUG << "SPX42UDDFExport::writeProfileData...OK" << Qt::endl;
    return ( true );
  }

  bool SPX42UDDFExport::writeAllDives( QXmlStreamWriter &st )
  {
    bool resultOk = true;
    *lg << LDEBUG << "SPX42UDDFExport::writeAllDives..." << Qt::endl;
    //
    // jetzt alle Tauchgänge durch
    //
    for ( auto diveId : diveNums )
    {
      *lg << LDEBUG << "SPX42UDDFExport::writeAllDives -> compute dive num #" << diveId << " from device..." << Qt::endl;
      resultOk = writeOneDive( diveId, st );
      if ( !resultOk )
        break;
    }
    // alle Tauchgänge durch...
    *lg << LDEBUG << "SPX42UDDFExport::writeAllDives...OK" << Qt::endl;
    return ( resultOk );
  }

  bool SPX42UDDFExport::writeOneDive( int diveNum, QXmlStreamWriter &st )
  {
    bool resultOk = true;
    *lg << LDEBUG << "SPX42UDDFExport::writeOneDive -> dive #" << diveNum << "..." << Qt::endl;
    emit onStartSaveDiveSig( diveNum );
    //
    // Zeitpunkt des Tauchganges feststellen
    //
    qint64 ux_timestamp = database->getTimestampForDive( device_mac, diveNum );
    QDateTime diveDateTime = QDateTime::fromSecsSinceEpoch( ux_timestamp );
    DiveDataSetsPtr dataSerie = database->getDiveDataSets( device_mac, diveNum );
    // Bemerkungen aus Datenbank lesen
    QString diveNotes = database->getNotesForDive( device_mac, diveNum );
    // ich gehe mal davon aus, das der erste wert noch luft ist... (ist angenähert)
    double airtemperature = static_cast< double >( dataSerie->first().temperature ) + ProjectConst::KELVINDIFF;
    // niedrigste Temperatur finden
    double lowesttemperature = static_cast< double >( getLowestTemp( dataSerie ) ) + ProjectConst::KELVINDIFF;
    // grösste tiefe finden
    double greatestdepth = getGreatestDepth( dataSerie );
    //
    // Tauchgang Block öffnen
    //
    st.writeStartElement( "dive" );
    st.writeAttribute( "id", QString( "%1" ).arg( diveNum, 4, 10, QChar( '0' ) ) );

    // Block informationbeforedive oeffnen
    st.writeStartElement( "informationbeforedive" );
    // Dtetime schreiben
    st.writeTextElement( "datetime", diveDateTime.toString( "yyyy-MM-ddTHH:mm:ss" ) );
    // Tauchgangsnummer
    st.writeTextElement( "divenumber", QString( "%1" ).arg( diveNum, 4, 10, QChar( '0' ) ) );
    // Luft Temperatur
    st.writeTextElement( "airtemperature", QString( "%1" ).arg( airtemperature, 0, 'f', 1 ) );

    // informationbeforedive block schliessen
    st.writeEndElement();

    // die Serie schreiben
    if ( !writeDiveSamples( dataSerie, st ) )
    {
      emit onEndSaveDiveSig( diveNum );
      return ( false );
    }

    // Block informationafterdive öffnen
    st.writeStartElement( "informationafterdive" );
    // niedrigste Temperatur
    st.writeTextElement( "lowesttemperature", QString( "%1" ).arg( lowesttemperature, 0, 'f', 1 ) );
    // grösste Tiefe
    st.writeTextElement( "greatestdepth", QString( "%1" ).arg( greatestdepth, 0, 'f', 1 ) );
    // Bemerkungen Block
    st.writeStartElement( "notes" );
    st.writeTextElement( "text", diveNotes );
    st.writeEndElement();
    // Block informationafterdive schliessen
    st.writeEndElement();

    // Tauchgang Block schliessen
    st.writeEndElement();
    //
    *lg << LDEBUG << "SPX42UDDFExport::writeOneDive... OK" << Qt::endl;
    emit onEndSaveDiveSig( diveNum );
    return ( resultOk );
  }

  bool SPX42UDDFExport::writeDiveSamples( DiveDataSetsPtr data, QXmlStreamWriter &st )
  {
    //
    // möglich sind hier
    // <alarm>, <batterychargecondition>, <bodytemperature>,
    // <calculatedpo2>, <cns>, <decostop/>, <depth>, <divemode/>,
    // <divetime>, <gradientfactor>, <heading>, <measuredpo2>,
    // <nodecotime>, <otu>, <pulserate>, <remainingbottomtime>,
    // <remainingo2time>, <setmarker>, <setpo2>, <switchmix/>,
    // <tankpressure>, <temperature>
    //
    int he = 0;
    int n2 = 0;
    int setpoint = 0;
    double secounds = 0;
    bool isFirstWaypoint = true;
    *lg << LDEBUG << "SPX42UDDFExport::writeDiveSamples..." << Qt::endl;
    // Block samples oeffnen
    st.writeStartElement( "samples" );

    //
    // alle Samples in die Datei schreiben
    //
    for ( auto entry : *data.get() )
    {
      QString currentGas = "";
      // vergangene Zeit
      secounds += static_cast< double >( entry.nextStep );
      // waypoint block öffnen
      st.writeStartElement( "waypoint" );
      //
      // beim ersten wegpunkt Tachgangstyp einschreiben
      //
      if ( isFirstWaypoint )
      {
        //
        // da das Gerät ein emCCR ist gehe ich mal von CCR aus
        // TODO: konfigurierbar auch semiclosedcircuit
        //
        isFirstWaypoint = false;
        st.writeStartElement( "divemode" );
        st.writeAttribute( "type", "closedcircuit" );
        st.writeEndElement();
      }
      //
      // schaue nach ob sich das gas geändert hat
      //
      if ( he != entry.he || n2 != entry.n2 )
      {
        //
        // so, hier muss ein Gaswechsel eingetragen werden
        //
        n2 = entry.n2;
        he = entry.he;
        currentGas = getGasRef( n2, he );
        st.writeStartElement( "switchmix" );
        st.writeAttribute( "ref", currentGas );
        st.writeEndElement();
      }
      //
      // schaue nach ob sich der setpoint geändert hat
      //
      if ( setpoint != entry.setpoint )
      {
        //
        // ok, ein wechsel muss rein
        //
        setpoint = entry.setpoint;
        st.writeStartElement( "setpo2" );
        st.writeAttribute( "setby", "computer" );
        st.writeCharacters( QString( "%1" ).arg( setpoint / 10.0, 0, 'f', 1 ) );
        st.writeEndElement();
      }
      // Tiefe
      st.writeTextElement( "depth", QString( "%1" ).arg( entry.abs_depth, 0, 'f', 1 ) );
      // Tauchzeit
      st.writeTextElement( "divetime", QString( "%1" ).arg( secounds, 0, 'f', 1 ) );
      // Temperatur
      st.writeTextElement( "temperature",
                           QString( "%1" ).arg( static_cast< double >( entry.temperature ) + ProjectConst::KELVINDIFF, 0, 'f', 1 ) );
      //
      // PPO2 Werte
      //
      // zuerst effektivber Wert
      st.writeStartElement( "measuredpo2" );
      st.writeAttribute( "ref", "o2_sensor_00" );
      st.writeCharacters( QString( "%1" ).arg( entry.ppo2, 0, 'f', 2 ) );
      st.writeEndElement();
      // Sensor 1
      st.writeStartElement( "measuredpo2" );
      st.writeAttribute( "ref", "o2_sensor_01" );
      st.writeCharacters( QString( "%1" ).arg( entry.ppo2_1, 0, 'f', 2 ) );
      st.writeEndElement();
      // Sensor 2
      st.writeStartElement( "measuredpo2" );
      st.writeAttribute( "ref", "o2_sensor_02" );
      st.writeCharacters( QString( "%1" ).arg( entry.ppo2_2, 0, 'f', 2 ) );
      st.writeEndElement();
      // Sensor 3
      st.writeStartElement( "measuredpo2" );
      st.writeAttribute( "ref", "o2_sensor_03" );
      st.writeCharacters( QString( "%1" ).arg( entry.ppo2_3, 0, 'f', 2 ) );
      st.writeEndElement();
      //
      // waypoint schliessen
      //
      st.writeEndElement();
    }

    // Block samples schliessen
    st.writeEndElement();
    *lg << LDEBUG << "SPX42UDDFExport::writeDiveSamples...OK" << Qt::endl;
    return ( true );
  }

  QString SPX42UDDFExport::getGasRef( int n2, int he ) const
  {
    QString n2_str = QString( "0.%1%2" ).arg( n2, 2, 10, QChar( '0' ) ).arg( "0" );
    QString he_str = QString( "0.%1%2" ).arg( he, 2, 10, QChar( '0' ) ).arg( "0" );
    //
    for ( auto gasref : gasDefs )
    {
      *lg << LDEBUG << "SPX42UDDFExport::getGasRef -> current test with " << gasref.getN2() << " / " << gasref.getHe() << Qt::endl;
      //
      if ( n2_str == gasref.getN2() && he_str == gasref.getHe() )
        return ( gasref.gName );
    }
    return ( "0000000000" );
  }

  int SPX42UDDFExport::getLowestTemp( DiveDataSetsPtr data )
  {
    int lowest = 100;
    //
    for ( auto entry : *data.get() )
    {
      if ( entry.temperature < lowest )
        lowest = entry.temperature;
    }
    return ( lowest );
  }

  double SPX42UDDFExport::getGreatestDepth( DiveDataSetsPtr data )
  {
    double depth = 0;
    for ( auto entry : *data.get() )
    {
      if ( entry.abs_depth > depth )
        depth = entry.abs_depth;
    }
    return ( depth );
  }

}  // namespace spx
