#include "SPX42Database.hpp"

namespace spx
{
  const QString SPX42Database::sqlDriver{"QSQLITE"};
  const qint16 SPX42Database::databaseVersion{4};

  SPX42DeviceAlias::SPX42DeviceAlias() : mac(), name(), alias(), lastConnected( false )
  {
  }

  SPX42DeviceAlias::SPX42DeviceAlias( const QString &_mac, const QString &_name, const QString &_alias, bool _lastConnect )
      : mac( _mac ), name( _name ), alias( _alias ), lastConnected( _lastConnect )
  {
  }

  SPX42DeviceAlias::SPX42DeviceAlias( const SPX42DeviceAlias &di )
      : mac( di.mac ), name( di.name ), alias( di.alias ), lastConnected( di.lastConnected )
  {
  }

  void DiveChartDataset::clear( void )
  {
    lfdnr = 0;
    depth = 0.0;
    temperature = 0;
    ppo2 = 0.0;
    ppo2_1 = 0.0;
    ppo2_2 = 0.0;
    ppo2_3 = 0.0;
    nextStep = 0;
  }

  DiveLogEntry::DiveLogEntry(){};
  DiveLogEntry::DiveLogEntry( int di,
                              int lf,
                              int pr,
                              int de,
                              int te,
                              double ac,
                              double p2,
                              double p21,
                              double p22,
                              double p23,
                              int sp,
                              int n,
                              int h,
                              int z,
                              int nx )
      : detailId( di )
      , lfdNr( lf )
      , pressure( pr )
      , depth( de )
      , temperature( te )
      , acku( ac )
      , ppo2( p2 )
      , ppo2_1( p21 )
      , ppo2_2( p22 )
      , ppo2_3( p23 )
      , setpoint( sp )
      , n2( n )
      , he( h )
      , zeroTime( z )
      , nextStep( nx )
  {
  }

  DiveLogEntry::DiveLogEntry( const DiveLogEntry &en )
      : detailId( en.detailId )
      , lfdNr( en.lfdNr )
      , pressure( en.pressure )
      , depth( en.depth )
      , temperature( en.temperature )
      , acku( en.acku )
      , ppo2( en.ppo2 )
      , ppo2_1( en.ppo2_1 )
      , ppo2_2( en.ppo2_1 )
      , ppo2_3( en.ppo2_3 )
      , setpoint( en.setpoint )
      , n2( en.n2 )
      , he( en.he )
      , zeroTime( en.zeroTime )
      , nextStep( en.nextStep )
  {
  }

  /**
   * @brief SPX42Database::SPX42Database
   * @param logger
   * @param databaseName
   * @param parent
   */
  SPX42Database::SPX42Database( std::shared_ptr< Logger > logger, const QString &databaseName, QObject *parent )
      : QObject( parent ), lg( logger ), dbName( databaseName )
  {
    // dbName default: spx42Database.db
    lg->debug( "SPX42Database::SPX42Database..." );
  }

  /**
   * @brief SPX42Database::~SPX42Database
   */
  SPX42Database::~SPX42Database()
  {
    lg->debug( "SPX42Database::~SPX42Database..." );
    if ( db.isValid() && db.isOpen() )
      closeDatabase();
  }

  /**
   * @brief SPX42Database::openDatabase
   * @param createPath
   * &@return
   */
  QSqlError SPX42Database::openDatabase( bool createPath )
  {
    // static: wie viele Verbindungen gibt es hier
    static int lfdNum = 0;
    lg->debug( "SPX42Database::openDatabase..." );
    QSqlError err;
    //
    // Falls vorhanden, Datenbank schliessen
    //
    currentConnectionName = QString( "sqliteConn_%1" ).arg( lfdNum, 2, 10, QChar( '0' ) );
    lfdNum++;
    QSqlDatabase::database( currentConnectionName, false ).close();
    QSqlDatabase::removeDatabase( currentConnectionName );
    //
    // Datenbank zufügen
    //
    lg->debug( "SPX42Database::openDatabase -> add db connection" );
    db = QSqlDatabase::addDatabase( SPX42Database::sqlDriver, currentConnectionName );
    lg->debug( QString( "SPX42Database::openDatabase -> add db name: " ).append( dbName ) );
    db.setDatabaseName( dbName );
    //
    // Pfad prüfen und ggf anlegen ?
    //
    if ( createPath )
    {
      QFileInfo fInfo( dbName );
      QString filePath( fInfo.path() );
      lg->debug( QString( "SPX42Database::openDatabase -> db path: " ).append( filePath ) );
      if ( !QDir( filePath ).exists() )
      {
        lg->debug( "SPX42Database::openDatabase -> db path not exist. create it!" );
        if ( QDir().mkpath( filePath ) )
        {
          lg->debug( "SPX42Database::openDatabase -> path succsesful created!" );
        }
        else
        {
          lg->crit( "SPX42Database::openDatabase -> path NOT created!" );
        }
      }
    }
    //
    // öffnen
    //
    if ( !db.open() )
    {
      err = db.lastError();
      db = QSqlDatabase();
      QSqlDatabase::removeDatabase( currentConnectionName );
      lg->warn( QString( "SPX42Database::openDatabase -> an error while open the database: " ).append( err.text() ) );
    }
    else
    {
      if ( db.isOpen() )
        if ( !checkOrCreateTables() )
        {
          lg->warn( "SPX42Database::openDatabase -> NOT OK" );
          // TODO: Fehler in err hinterlassen!
        }
        else
        {
          lg->debug( "SPX42Database::openDatabase -> OK" );
        }
      else
      {
        lg->warn( "SPX42Database::openDatabase -> NOT OK" );
        // TODO: Fehler in err hinterlassen!
      }
    }
    return ( err );
  }

  /**
   * @brief SPX42Database::openDatabase
   * @param databaseName
   * @param createPath
   * @return
   */
  QSqlError SPX42Database::openDatabase( const QString &databaseName, bool createPath )
  {
    dbName = databaseName;
    return ( openDatabase( createPath ) );
  }

  /**
   * @brief SPX42Database::closeDatabase
   */
  void SPX42Database::closeDatabase()
  {
    lg->debug( "SPX42Database::closeDatabase..." );
    QSqlDatabase::database( currentConnectionName, false ).close();
    //
    // Neue db ==> das alte db objekt damit der Vernichtung anheimgeben
    //
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase( currentConnectionName );
    currentConnectionName.clear();
    lg->debug( "SPX42Database::closeDatabase...OK" );
  }

  QSqlDatabase *SPX42Database::getDatabase( void )
  {
    if ( db.isValid() && db.isOpen() )
      return ( &db );
    return ( nullptr );
  }

  /**
   * @brief SPX42Database::getDeviceAliasHash
   * @return
   */
  DeviceAliasHash SPX42Database::getDeviceAliasHash()
  {
    lg->debug( "SPX42Database::getDeviceAliasHash..." );
    DeviceAliasHash aliase;
    //
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( "select mac,name,alias,last from devices", db );
      while ( query.next() )
      {
        QString mac( query.value( 0 ).toString() );
        // &_mac, &_name, &_alias, _lastConnect, _device_id
        SPX42DeviceAlias devAlias( query.value( 0 ).toString(), query.value( 1 ).toString(), query.value( 2 ).toString(),
                                   static_cast< bool >( ( query.value( 3 ).toInt() & 0x01 ) ) );
        aliase.insert( mac, devAlias );
      }
      lg->debug( QString( "SPX42Database::getDeviceAliasHash -> <%1> items..." ).arg( aliase.count() ) );
    }
    else
    {
      lg->warn( "SPX42Database::getDeviceAliasHash -> database not valid or not opened." );
    }
    return ( aliase );
  }

  bool SPX42Database::addAlias( const QString &mac, const QString &name, const QString &alias, bool lastConnected )
  {
    QSqlQuery query( db );
    QSqlError err;
    QString sql;
    //
    lg->debug( "SPX42Database::addAlias..." );
    if ( db.isValid() && db.isOpen() )
    {
      //
      // guck nach, ob der Eintrag vorhanden ist
      sql = QString( "select mac from devices where mac='%1'" ).arg( mac );
      lg->debug( QString( "SPX42Database::addAlias -> ASK (%1)..." ).arg( sql ) );
      if ( !query.exec( sql ) )
      {
        err = db.lastError();
        lg->warn( QString( "SPX42Database::addAlias -> problem while select from database: <%1>" ).arg( err.text() ) );
        return ( false );
      }

      lg->debug( "SPX42Database::addAlias -> ASK ...OK" );
      query.first();
      if ( query.at() < 0 )
      {
        //
        // MAC ist noch nicht in der Datenbank, INSERT
        //
        sql = QString( "insert into devices (mac, name, alias, last) values ('%1', '%2', '%3', %4)" )
                  .arg( mac )
                  .arg( name )
                  .arg( alias )
                  .arg( lastConnected ? 1 : 0 );
        lg->debug( QString( "SPX42Database::addAlias -> INSERT (%1)..." ).arg( sql ) );
        if ( query.exec( sql ) )
        {
          lg->debug( "SPX42Database::addAlias -> insert OK" );
          return ( true );
        }
        err = db.lastError();
        lg->warn( QString( "SPX42Database::addAlias -> insert failed: <%1>" ).arg( err.text() ) );
        return ( false );
      }
      //
      // Mac war in der Datenbank, also update
      //
      sql = QString( "update devices set name='%1', alias='%2', last=%3 where mac='%4'" )
                .arg( name )
                .arg( alias )
                .arg( lastConnected ? 1 : 0 )
                .arg( mac );
      lg->debug( QString( "SPX42Database::addAlias -> UPDATE (%1)..." ).arg( sql ) );
      if ( !query.exec( sql ) )
      {
        err = db.lastError();
        lg->warn( QString( "SPX42Database::addAlias -> update failed: <%1>" ).arg( err.text() ) );
        return ( false );
      }
      lg->debug( "SPX42Database::addAlias -> update OK" );
      return ( true );
    }
    else
    {
      lg->warn( "SPX42Database::addAlias -> database not valid or not opened." );
    }
    return ( false );
  }

  /**
   * @brief SPX42Database::addAlias
   * @param devAlias
   * @return
   */
  bool SPX42Database::addAlias( const SPX42DeviceAlias &devAlias )
  {
    return ( addAlias( devAlias.mac, devAlias.name, devAlias.alias, devAlias.lastConnected ) );
  }

  /**
   * @brief SPX42Database::setAliasForMac
   * @param mac
   * @param alias
   * @return
   */
  bool SPX42Database::setAliasForMac( const QString &mac, const QString &alias )
  {
    lg->debug( "SPX42Database::setAliasForMac..." );
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( QString( "update devices set alias='%1' where mac='%2'" ).arg( alias ).arg( mac ), db );
      if ( query.exec() )
      {
        //
        // Abfrage korrekt bearbeitet
        //
        lg->debug( "SPX42Database::setAliasForMac...OK" );
        return ( true );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::setAliasForMac -> <%1>..." ).arg( err.text() ) );
      }
    }
    else
    {
      lg->warn( "SPX42Database::setAliasForMac -> db is not valid or not opened." );
    }
    return ( false );
  }

  /**
   * @brief SPX42Database::setAliasForName
   * @param name
   * @param alias
   * @return
   */
  bool SPX42Database::setAliasForName( const QString &name, const QString &alias )
  {
    lg->debug( "SPX42Database::setAliasForName..." );
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( QString( "update devices set alias='%1' where name='%2'" ).arg( alias ).arg( name ), db );
      if ( query.exec() )
      {
        //
        // Abfrage korrekt bearbeitet
        //
        lg->debug( "SPX42Database::setAliasForName...OK" );
        return ( true );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::setAliasForName -> <%1>..." ).arg( err.text() ) );
      }
    }
    else
    {
      lg->warn( "SPX42Database::setAliasForName -> db is not valid or not opened." );
    }
    return ( false );
  }

  /**
   * @brief SPX42Database::getAliasForMac
   * @param mac
   * @return
   */
  QString SPX42Database::getAliasForMac( const QString &mac )
  {
    QString result = "UNKNOWN";
    //
    lg->debug( "SPX42Database::getAliasForMac..." );
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      QString sql = QString( "select alias from devices where mac='%1'" ).arg( mac );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::getAliasForMac -> <%1>..." ).arg( err.text() ) );
      }
      if ( query.next() )
      {
        if ( !query.value( 0 ).isNull() )
          result = query.value( 0 ).toString();
      }
      return ( result );
    }
    lg->warn( "SPX42Database::getAliasForMac -> db is not valid or not opened." );
    return ( result );
  }

  /**
   * @brief SPX42Database::getAliasForName
   * @param name
   * @return
   */
  QString SPX42Database::getAliasForName( const QString &name )
  {
    QString result = "UNKNOWN";
    //
    lg->debug( "SPX42Database::getAliasForName..." );
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      QString sql = QString( "select alias from devices where name='%1'" ).arg( name );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::getAliasForName -> <%1>..." ).arg( err.text() ) );
      }
      if ( query.next() )
      {
        if ( !query.value( 0 ).isNull() )
          result = query.value( 0 ).toString();
      }
      return ( result );
    }
    lg->warn( "SPX42Database::getAliasForName -> db is not valid or not opened." );
    return ( result );
  }

  /**
   * @brief SPX42Database::setLastConnected
   * @param mac
   * @return
   */
  bool SPX42Database::setLastConnected( const QString &mac )
  {
    QString sql;
    QSqlQuery query( db );
    lg->debug( "SPX42Database::setLastConnected..." );
    if ( db.isValid() && db.isOpen() )
    {
      //
      // alle flags löschen
      //
      sql = QString( "update devices set last=0" );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::setLastConnected -> <%1>..." ).arg( err.text() ) );
        return ( false );
      }
      sql = QString( "update devices set last=%1 where mac='%2'" ).arg( 1 ).arg( mac );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::setLastConnected -> <%1>..." ).arg( err.text() ) );
        return ( false );
      }
      //
      // Abfrage korrekt bearbeitet
      //
      lg->debug( "SPX42Database::setLastConnected...OK" );
      return ( true );
    }
    else
    {
      lg->warn( "SPX42Database::setLastConnected -> db is not valid or not opened." );
    }
    return ( false );
  }

  /**
   * @brief SPX42Database::getDevicveId
   * @param mac
   * @return
   */
  int SPX42Database::getDevicveId( const QString &mac )
  {
    QString sql;
    QSqlQuery query( db );
    lg->debug( QString( "SPX42Database::getDevicveId for <%1>..." ).arg( mac ) );
    if ( db.isValid() && db.isOpen() )
    {
      //
      // existiert der Eintrag?
      //
      QSqlQuery query( QString( "select device_id from devices where mac='%1'" ).arg( mac ), db );
      if ( !query.next() )
      {
        lg->warn( QString( "SPX42Database::getDevicveId -> requested mac <%1> not found in database..." ).arg( mac ) );
        // Leeren String zurück
        return ( -1 );
      }
      else
      {
        return ( query.value( 0 ).toInt() );
      }
    }
    return ( -1 );
  }

  /**
   * @brief SPX42Database::getDetailId
   * @param mac
   * @param diveNum
   * @return
   */
  int SPX42Database::getDetailId( const QString &mac, int diveNum )
  {
    int detailId = -1;

    if ( db.isValid() && db.isOpen() )
    {
      QString sql;
      QSqlQuery query( db );
      //
      // Detail_id erfragen
      //
      sql = "select detaildir.detail_id \n";
      sql += " from detaildir,devices \n";
      sql += QString( " where detaildir.divenum=%1 and devices.mac='%2' and devices.device_id=detaildir.device_id" )
                 .arg( diveNum )
                 .arg( mac );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::getDetailId -> <%1>..." ).arg( err.text() ) );
        return ( false );
      }
      if ( !query.next() )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::getDetailId -> <%1>..." ).arg( err.text() ) );
        return ( detailId );
      }
      else
        detailId = query.value( 0 ).toInt();
      query.clear();
      return ( detailId );
    }
    lg->warn( "SPX42Database::getDetailId -> db not open or not exist!" );
    return ( detailId );
  }

  /**
   * @brief SPX42Database::getLastConnected
   * @return
   */
  QString SPX42Database::getLastConnected( void )
  {
    lg->debug( "SPX42Database::getLastConnected..." );
    if ( db.isValid() && db.isOpen() )
    {
      //
      // alle flags löschen
      //
      QSqlQuery query( "select mac from devices where last=1", db );
      if ( !query.next() )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::getLastConnected -> <%1>..." ).arg( err.text() ) );
        return ( "" );
      }
      //
      // Abfrage korrekt bearbeitet
      //
      lg->debug( QString( "SPX42Database::getLastConnected -> device: %1" )
                     .arg( query.value( 0 ).isNull() ? "NONE" : query.value( 1 ).toString() ) );
      return ( query.value( 0 ).toString() );
    }
    else
    {
      lg->warn( "SPX42Database::setAliasForName -> db is not valid or not opened." );
      return ( "" );
    }
  }

  /**
   * @brief SPX42Database::existTable
   * @param tableName
   * @return
   */
  bool SPX42Database::existTable( const QString &tableName )
  {
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( QString( "SELECT name FROM 'sqlite_master' WHERE type = 'table' AND name = '%1'" ).arg( tableName ), db );
      QString foundTable;
      if ( query.next() )
      {
        //
        // was war das Ergebnis, wenn die Anfrage erfolgreich war
        //
        foundTable = query.value( 0 ).toString().trimmed();
        lg->debug( QString( "SPX42Database::existTable -> tablename <%1> found..." ).arg( foundTable ) );
        //
        // ist das das von mir erwartete Ergebnis (eigentlich würde reichen "nicht leer")
        //
        if ( foundTable == tableName )
        {
          lg->debug( QString( "SPX42Database::existTable -> table <%1> exists..." ).arg( tableName ) );
          return ( true );
        }
      }
      lg->debug( QString( "SPX42Database::existTable -> table <%1> NOT exists..." ).arg( tableName ) );
      return ( false );
    }
    lg->warn( "SPX42Database::existTable -> db is not open or exist!" );
    return ( false );
  }

  /**
   * @brief SPX42Database::getDatabaseVersion
   * @return
   */
  qint16 SPX42Database::getDatabaseVersion()
  {
    qint16 version = -1;
    //
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( "select max(version) from 'version'", db );
      if ( query.next() )
      {
        version = static_cast< qint16 >( query.value( 0 ).toInt() );
      }
      lg->debug( QString( "SPX42Database::getDatabaseVersion -> <%1>..." ).arg( version ) );
    }
    else
    {
      lg->warn( "SPX42Database::getDatabaseVersion -> db not open or not exist!" );
    }
    return ( version );
  }

  /**
   * @brief SPX42Database::createVersionTable
   * @return
   */
  bool SPX42Database::createVersionTable()
  {
    QString sql;
    lg->debug( "SPX42Database::createVersionTable..." );
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      sql = "drop table if exists 'version'";
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createVersionTable -> failed drop table if exist <%1>" ).arg( err.text() ) );
        return ( false );
      }
      query.clear();
      if ( !query.exec( SPX42DatabaseConstants::createVersionTable ) )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createVersionTable -> failed create table <%1>" ).arg( err.text() ) );
        return ( false );
      }
      query.clear();
      lg->debug( "SPX42Database::createVersionTable -> insert version number..." );
      sql = QString( "insert into 'version' (version) values (%1)" ).arg( SPX42Database::databaseVersion );
      if ( query.exec( sql ) )
      {
        lg->debug( "SPX42Database::createVersionTable -> insert version number: OK" );
        return ( true );
      }
      QSqlError err = db.lastError();
      lg->crit( QString( "SPX42Database::createVersionTable -> failed insert data <%1>" ).arg( err.text() ) );
    }
    else
    {
      lg->warn( "SPX42Database::createVersionTable -> db nor open or not exist!" );
    }
    return ( false );
  }

  /**
   * @brief SPX42Database::checkOrCreateTables
   * @return
   */
  bool SPX42Database::checkOrCreateTables()
  {
    lg->debug( "SPX42Database::checkOrCreateTables..." );
    bool haveToCreateNewVersionTable = false;
    //
    // Datenbank ist offen, finde zuerst die Versionsnummer
    //
    if ( !existTable( "version" ) )
    {
      lg->warn( "SPX42Database::checkOrCreateTables -> version table not availible!" );
      //
      // keine Versionstabelle-> dann erzeuge alle Tabellen neu
      //
      return createAllTables();
    }
    qint16 currentDatabaseVersionNumber = getDatabaseVersion();
    lg->debug( QString( "SPX42Database::checkOrCreateTables -> current database version %1..." ).arg( currentDatabaseVersionNumber ) );
    if ( currentDatabaseVersionNumber == -1 )
    {
      // Da ging was gewaltig schief, reagiere!
      lg->crit( "SPX42Database::checkOrCreateTables -> not an version number found!" );
      // TODO: was unternehmen wenn keine Version gefunden wurde
      return ( false );
    }
    //
    if ( !existTable( "devices" ) )
    {
      if ( !createAliasTable() )
      {
        lg->crit( "SPX42Database::checkOrCreateTables -> can't not find or create alias table!" );
        return ( false );
      }
    }
    //
    if ( !existTable( "detaildir" ) )
    {
      if ( !createLogDirTable() )
      {
        lg->crit( "SPX42Database::checkOrCreateTables -> can't not find or create log directory table!" );
        return ( false );
      }
    }
    //
    if ( !existTable( "logdata" ) )
    {
      if ( !createLogDataTable() )
      {
        lg->crit( "SPX42Database::checkOrCreateTables -> can't not find or create log details table!" );
        return ( false );
      }
    }
    //
    // zu guter letzt: (bei weiteren Versionserhöhungen)
    //
    if ( haveToCreateNewVersionTable )
    {
      //
      // erzeuge eine neue Versionstabelle
      // mit aktuellem Inhalt
      createVersionTable();
    }
    return ( true );
  }

  /**
   * @brief SPX42Database::createAllTables
   * @return
   */
  bool SPX42Database::createAllTables()
  {
    if ( !createVersionTable() )
    {
      return ( false );
    }
    if ( !createAliasTable() )
    {
      return ( false );
    }
    if ( !createLogDirTable() )
    {
      return ( false );
    }
    if ( createLogDataTable() )
    {
      return ( false );
    }
    return ( true );
  }

  /**
   * @brief SPX42Database::createAliasTable
   * @return
   */
  bool SPX42Database::createAliasTable()
  {
    QString sql;
    lg->debug( "SPX42Database::createAliasTable..." );
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      sql = "drop table if exists devices";
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createAliasTable -> failed drop if exist <%1>" ).arg( err.text() ) );
        return ( false );
      }
      //
      // das Statement
      //
      sql = SPX42DatabaseConstants::createDeviceTable;
      if ( query.exec( sql ) )
      {
        lg->debug( "SPX42Database::createAliasTable...OK" );
        return ( true );
      }
      QSqlError err = db.lastError();
      lg->crit( QString( "SPX42Database::createAliasTable -> failed create table <%1>" ).arg( err.text() ) );
    }
    else
    {
      lg->warn( "SPX42Database::createAliasTable -> db not open or not exist!" );
    }
    return ( false );
  }

  /**
   * @brief SPX42Database::createLogDirTable
   * @return
   */
  bool SPX42Database::createLogDirTable()
  {
    QString sql;
    lg->debug( "SPX42Database::createLogDirTable..." );
    if ( db.isValid() && db.isOpen() )
    {
      if ( !db.transaction() )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createLogDirTable -> open transaction failed %1" ).arg( err.text() ) );
      }
      QSqlQuery query( db );
      // alte Tabelle entfernen
      sql = "drop table if exists 'detaildir'";
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createLogDirTable -> failed drop if exist <%1>" ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      // erzeuge neue Tabelle
      sql = SPX42DatabaseConstants::createDiveLogDirectoryTable;
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::createLogDirTable -> <%1>..." ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      //
      // Abfrage korrekt bearbeitet, Index erzeugen
      //
      query.clear();
      sql = SPX42DatabaseConstants::createDiveLogTableIndex01;
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::createLogDirTable 1 -> <%1>..." ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      query.clear();
      sql = SPX42DatabaseConstants::createDiveLogTableIndex02;
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::createLogDirTable 2 -> <%1>..." ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      if ( !db.commit() )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createLogDirTable -> commit failed %1" ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      lg->debug( "SPX42Database::createLogDirTable...OK" );
      return ( true );
    }
    lg->warn( "SPX42Database::createLogDirTable -> db not opened" );
    return ( false );
  }

  /**
   * @brief SPX42Database::createLogDataTable
   * @return
   */
  bool SPX42Database::createLogDataTable()
  {
    QString sql;
    lg->debug( "SPX42Database::createLogDataTable <logdata>..." );
    if ( db.isValid() && db.isOpen() )
    {
      // Transaktion eröffnen
      if ( !db.transaction() )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createLogDataTable -> open transaction failed %1" ).arg( err.text() ) );
      }
      QSqlQuery query( db );
      sql = "drop table if exists logdata";
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createLogDataTable -> failed drop if exist <%1>" ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      query.clear();
      //
      // das Statement Tabelle erzeugen
      //
      sql = SPX42DatabaseConstants::createDiveLogDetailsTable;
      if ( query.exec( sql ) )
      {
        lg->debug( "SPX42Database::createLogDataTable -> create table ok" );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createLogDataTable -> failed create <%1>" ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      // index 1 machen
      query.clear();
      sql = SPX42DatabaseConstants::createDiveLogDetailsTableIndex01;
      if ( query.exec( sql ) )
      {
        lg->debug( "SPX42Database::createLogDataTable -> create index ok" );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createLogDataTable -> failed create index  <%1>" ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      // index 2 machen
      query.clear();
      sql = SPX42DatabaseConstants::createDiveLogDetailsTableIndex02;
      if ( query.exec( sql ) )
      {
        lg->debug( "SPX42Database::createLogDataTable -> create index ok" );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createLogDataTable -> failed create index  <%1>" ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      // commit
      if ( !db.commit() )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createLogDataTable -> commit failed %1" ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      return ( true );
    }
    lg->warn( "SPX42Database::createLogDataTable -> db not opened" );
    return ( false );
  }

  /**
   * @brief SPX42Database::insertLogentry
   * @param entr
   * @return
   */
  bool SPX42Database::insertLogentry( const DiveLogEntry &entr )
  {
    lg->debug( "SPX42Database::insertLogentry..." );
    if ( db.isValid() && db.isOpen() )
    {
      QString sql = SPX42DatabaseConstants::loglineInsertTemplate.arg( entr.detailId )
                        .arg( entr.lfdNr )
                        .arg( entr.pressure )
                        .arg( entr.depth )
                        .arg( entr.temperature )
                        .arg( entr.acku )
                        .arg( entr.ppo2 )
                        .arg( entr.ppo2_1 )
                        .arg( entr.ppo2_2 )
                        .arg( entr.ppo2_3 )
                        .arg( entr.setpoint )
                        .arg( entr.n2 )
                        .arg( entr.he )
                        .arg( entr.zeroTime )
                        .arg( entr.nextStep );
      QSqlQuery query( sql, db );
      if ( query.exec() )
      {
        //
        // Abfrage korrekt bearbeitet
        //
        lg->debug( "SPX42Database::insertLogentry...OK" );
        return ( true );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::insertLogentry -> <%1>..." ).arg( err.text() ) );
      }
    }
    lg->warn( "SPX42Database::insertLogentry -> db is not valid or not opened." );
    return ( false );
  }

  /**
   * @brief SPX42Database::insertLogentry
   * @param detail_id
   * @param cmd
   * @return
   */
  bool SPX42Database::insertLogentry( int detail_id, spSingleCommand cmd )
  {
    lg->debug( "SPX42Database::insertLogentry..." );
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      QString sql = SPX42DatabaseConstants::loglineInsertTemplate.arg( detail_id )
                        .arg( cmd->getSequence() )
                        .arg( static_cast< int >( cmd->getValueAt( SPXCmdParam::LOGDETAIL_PRESSURE ) ) )
                        .arg( static_cast< int >( cmd->getValueAt( SPXCmdParam::LOGDETAIL_DEPTH ) ) )
                        .arg( static_cast< int >( cmd->getValueAt( SPXCmdParam::LOGDETAIL_TEMP ) ) )
                        .arg( cmd->getParamAt( SPXCmdParam::LOGDETAIL_ACKU ).replace( 'V', ' ' ).toDouble() )  // Volt entfernen
                        .arg( cmd->getDoubleValueAt( SPXCmdParam::LOGDETAIL_PPO2 ) )
                        .arg( cmd->getDoubleValueAt( SPXCmdParam::LOGDETAIL_PPO2_1 ) )
                        .arg( cmd->getDoubleValueAt( SPXCmdParam::LOGDETAIL_PPO2_2 ) )
                        .arg( cmd->getDoubleValueAt( SPXCmdParam::LOGDETAIL_PPO2_3 ) )
                        .arg( static_cast< int >( cmd->getValueAt( SPXCmdParam::LOGDETAIL_SETPOINT ) ) )
                        .arg( static_cast< int >( cmd->getValueAt( SPXCmdParam::LOGDETAIL_N2 ) ) )
                        .arg( static_cast< int >( cmd->getValueAt( SPXCmdParam::LOGDETAIL_HE ) ) )
                        .arg( static_cast< int >( cmd->getValueAt( SPXCmdParam::LOGDETAIL_ZEROTIME ) ) )
                        .arg( static_cast< int >( cmd->getValueAt( SPXCmdParam::LOGDETAIL_NEXT_STEP ) ) );
      if ( query.exec( sql ) )
      {
        //
        // Abfrage korrekt bearbeitet
        //
        lg->debug( "SPX42Database::insertLogentry...OK" );
        return ( true );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::insertLogentry -> <%1>..." ).arg( err.text() ) );
      }
    }
    lg->warn( "SPX42Database::insertLogentry -> db is not valid or not opened." );
    return ( false );
  }

  /**
   * @brief SPX42Database::insertDiveLogInBase
   * @param mac
   * @param diveNum
   * @param timestamp
   * @return
   */
  int SPX42Database::insertDiveLogInBase( const QString &mac, int diveNum, qint64 timestamp )
  {
    int device_id = -1;

    lg->debug( "SPX42Database::insertDiveLogInBase..." );
    if ( db.isValid() && db.isOpen() )
    {
      device_id = getDevicveId( mac );
      if ( device_id == -1 )
      {
        //
        // das ging schief
        //
        return ( device_id );
      }
      QString sql;
      QSqlQuery query( db );
      //
      // gibt es den Eintrag schon, dann nur update
      //
      sql = QString( "select count(*) from 'detaildir' where device_id=%1 and divenum=%2" ).arg( device_id ).arg( diveNum );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::insertDiveLogInBase -> select count exec <%1>..." ).arg( err.text() ) );
        return ( -1 );
      }
      if ( !query.next() )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::insertDiveLogInBase -> select count next <%1>..." ).arg( err.text() ) );
        return ( -1 );
      }
      //
      // dieFrage update oder insert
      //
      if ( query.value( 0 ).toInt() > 0 )
      {
        //
        // existiert, also update
        //
        sql = QString( "update 'detaildir' set ux_timestamp=%1 where device_id='%2' and divenum=%3" )
                  .arg( timestamp )
                  .arg( device_id )
                  .arg( diveNum );
      }
      else
      {
        //
        // neuer Datensatz, also insert
        //
        sql = QString( "insert into 'detaildir' (device_id, divenum, ux_timestamp ) values ('%1',%2, %3)" )
                  .arg( device_id )
                  .arg( diveNum )
                  .arg( timestamp );
      }
      //
      // egal was, aber ausführen!
      //
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::insertDiveLogInBase -> insert/update <%1>..." ).arg( err.text() ) );
        return ( -1 );
      }
      lg->warn( "SPX42Database::insertDiveLogInBase...OK" );
      return ( device_id );
    }
    lg->warn( "SPX42Database::insertDiveLogInBase -> db not open or not exist!" );
    return ( -1 );
  }

  /**
   * @brief SPX42Database::existDiveLogInBase
   * @param mac
   * @param diveNum
   * @return
   */
  bool SPX42Database::existDiveLogInBase( const QString &mac, int diveNum )
  {
    bool exist = false;
    int datasets = 0;
    QString sql;
    //
    if ( db.isValid() && db.isOpen() )
    {
      //
      // Suche im Inhaltsverzeichnis
      //
      sql = "select count(*.detaildir)\n";
      sql += " from detaildir, devices \n";
      sql += QString( " where devices.mac='%1' and detaildir.divenum=%2 and devices.device_id=detaildir.device_id " )
                 .arg( mac )
                 .arg( diveNum );
      QSqlQuery query( sql, db );
      if ( query.next() )
      {
        datasets = query.value( 0 ).toInt();
        if ( datasets > 0 )
        {
          exist = true;
        }
        lg->debug( QString( "SPX42Database::existDiveLogInBase -> <%1> sets == %2..." ).arg( datasets ).arg( exist ? "YES" : "NO" ) );
        return ( exist );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::existDiveLogInBase -> <%1>..." ).arg( err.text() ) );
      }
    }
    lg->warn( "SPX42Database::existDiveLogInBase -> db not open or not exist!" );
    return ( exist );
  }

  /**
   * @brief SPX42Database::delDiveLogFromBase
   * @param mac
   * @param diveNum
   * @return
   */
  bool SPX42Database::delDiveLogFromBase( const QString &mac, int diveNum )
  {
    int detailId = 0;
    lg->debug( QString( "SPX42Database::delDiveLogFromBase -> <%1> num: <%2>..." ).arg( mac ).arg( diveNum ) );

    if ( db.isValid() && db.isOpen() )
    {
      detailId = getDetailId( mac, diveNum );
      //
      // Schritt 1 Detailid erfragen
      //
      if ( detailId < 0 )
        return ( false );
      QString sql;
      if ( !db.transaction() )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::delDiveLogFromBase -> open transaction failed %1" ).arg( err.text() ) );
      }
      QSqlQuery query( db );
      //
      // Schritt 2: das Verzeichnis löschen (wegen Foreign Key das zuerst....)
      //
      sql = QString( "delete from detaildir where detail_id=%1 and divenum=%2" ).arg( detailId ).arg( diveNum );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::delDiveLogFromBase -> <%1>..." ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      query.clear();
      //
      // Schritt 2: details löschen
      //
      sql = QString( "delete from logdata where detail_id=%1" ).arg( detailId );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::delDiveLogFromBase -> <%1>..." ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      //
      // ging alles Glatt, commit
      //
      if ( !db.commit() )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::delDiveLogFromBase -> commit failed %1" ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      return ( true );
    }
    lg->warn( "SPX42Database::delDiveLogFromBase -> db not open or not exist!" );
    return ( false );
  }

  /**
   * @brief SPX42Database::getMaxDepthFor
   * @param mac
   * @param diveNum
   * @return
   */
  int SPX42Database::getMaxDepthFor( const QString &mac, int diveNum )
  {
    int maxDepth = -1;
    int detail_id = -1;
    QString sql;
    lg->debug( "SPX42Database::getMaxDepthFor..." );
    if ( db.isValid() && db.isOpen() )
    {
      detail_id = getDetailId( mac, diveNum );
      if ( detail_id == -1 )
      {
        return ( maxDepth );
      }
      //
      // versuche mal rauszubekommen wie tief das war
      //
      sql = QString( "select max_depth from detaildir where detail_id=%1 and divenum=%2" ).arg( detail_id ).arg( diveNum );
      QSqlQuery query( sql, db );
      if ( !query.next() )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::getMaxDepthFor -> <%1>..." ).arg( err.text() ) );
        return ( -1 );
      }
      //
      // Abfrage korrekt bearbeitet
      //
      maxDepth = query.value( 0 ).toInt();
      lg->debug( QString( "SPX42Database::getMaxDepthFor -> max depth: %1" ).arg( maxDepth ) );
      return ( maxDepth );
    }
    lg->warn( "SPX42Database::getMaxDepthFor -> db is not valid or not opened." );
    return ( -1 );
  }

  /**
   * @brief SPX42Database::getChartSet
   * @param mac
   * @param diveNum
   * @return
   */
  DiveChartSetPtr SPX42Database::getChartSet( const QString &mac, int diveNum )
  {
    int detail_id = -1;
    QString sql;
    DiveChartSetPtr chartSet = DiveChartSetPtr( new DiveChartSet() );
    lg->debug( "SPX42Database::getChartSet..." );
    if ( db.isValid() && db.isOpen() )
    {
      detail_id = getDetailId( mac, diveNum );
      if ( detail_id == -1 )
        return ( chartSet );
      QSqlQuery query( db );
      //
      // suche die Daten aus der Datenbank zusammen
      //
      sql =
          QString( "select lfnr,depth,temperature,ppo2,ppo2_1,ppo2_2,ppo2_3,next_step from logdata where detail_id=%1 order by lfnr" )
              .arg( detail_id );
      //
      // jetzt frage die Datenbank
      //
      if ( !query.exec( sql ) )
      {
        //
        // es gab einen Fehler bei der Anfrage
        //
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::getMaxDepthFor -> <%1>..." ).arg( err.text() ) );
        chartSet->clear();
        return ( chartSet );
      }
      //
      // alle Datensätze abholen
      //
      while ( query.next() )
      {
        DiveChartDataset currSet;
        currSet.lfdnr = query.value( 0 ).toInt();
        currSet.depth = 0.0 - ( query.value( 1 ).toDouble() / 10.0 );
        currSet.temperature = query.value( 2 ).toInt();
        currSet.ppo2 = query.value( 3 ).toDouble();
        currSet.ppo2_1 = query.value( 4 ).toDouble();
        currSet.ppo2_2 = query.value( 5 ).toDouble();
        currSet.ppo2_3 = query.value( 6 ).toDouble();
        currSet.nextStep = query.value( 7 ).toInt();
        chartSet->append( currSet );
      }
      lg->debug( "SPX42Database::getChartSet...OK" );
      return ( chartSet );
    }
    lg->warn( "SPX42Database::getMaxDepthFor -> db is not valid or not opened." );
    chartSet->clear();
    return ( chartSet );
  }

  /**
   * @brief SPX42Database::getLogentrysForDevice
   * @param mac
   * @return
   */
  SPX42LogDirectoryEntryListPtr SPX42Database::getLogentrysForDevice( const QString &mac )
  {
    int device_id = -1;
    QString sql;
    //
    // mache einen zeiger auf die Liste
    //
    SPX42LogDirectoryEntryListPtr deviceLog = SPX42LogDirectoryEntryListPtr( new SPX42LogDirectoryEntryList );
    //
    if ( db.isValid() && db.isOpen() )
    {
      device_id = getDevicveId( mac );
      if ( device_id == -1 )
        return ( deviceLog );
      //
      // suche die Daten aus der Datenbank zusammen
      //
      QSqlQuery query( db );
      sql = QString( "select divenum,ux_timestamp from detaildir where device_id=%1" ).arg( device_id );
      if ( !query.exec( sql ) )
      {
        //
        // es gab einen Fehler bei der Anfrage
        //
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::getLogentrysForDevice -> error: <%1>..." ).arg( err.text() ) );
        deviceLog->clear();
        return ( deviceLog );
      }
      //
      // Daten einsammeln
      //
      while ( query.next() )
      {
        QDateTime dt = QDateTime::fromSecsSinceEpoch( query.value( 1 ).toInt(), Qt::LocalTime, 0 );
        int diveNum = query.value( 0 ).toInt();
        SPX42LogDirectoryEntry _entry( diveNum, 0, dt, true );
        deviceLog->insert( diveNum, _entry );
      }
      return ( deviceLog );
    }
    lg->warn( "SPX42Database::getLogentrysForDevice -> db is not valid or not opened." );
    return ( deviceLog );
  }

  bool SPX42Database::computeStatistic( int detail_id )
  {
    if ( db.isValid() && db.isOpen() )
    {
      QString sql;
      sql = "update  detaildir\n";
      sql += "set \n";
      sql += "  max_depth=(select max(depth) from logdata where detail_id=%1),\n";
      sql += "  detail_count=(select count(*) from logdata where detail_id=%1)\n";
      sql += "where  detail_id=%1";
      QSqlQuery query( QString( sql.arg( detail_id ) ), db );
      if ( !query.exec( sql ) )
      {
        //
        // es gab einen Fehler bei der Anfrage
        //
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::computeStatistic -> error: <%1>..." ).arg( err.text() ) );
        return ( false );
      }
      return ( true );
    }
    lg->warn( "SPX42Database::computeStatistic -> db is not valid or not opened." );
    return ( false );
  }

  // ##########################################################################
  // ### GETTER UND SETTER
  // ##########################################################################

  bool SPX42Database::isDbOpen() const
  {
    return ( db.isValid() && db.isOpen() );
  }

  QString SPX42Database::getDbName() const
  {
    return dbName;
  }
}  // namespace spx
