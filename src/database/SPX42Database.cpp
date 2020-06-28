#include "SPX42Database.hpp"
#include "config/ProjectConst.hpp"

namespace spx
{
  const QString SPX42Database::sqlDriver{"QSQLITE"};
  const qint16 SPX42Database::databaseVersion{5};
  const QString SPX42Database::statisticTemplate{
      "update  detaildir "
      "set "
      " max_depth=(select max(depth) from logdata where detail_id=%1), "
      " detail_count=(select count(*) from logdata where detail_id=%1) "
      "where detail_id=%1"};

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

  void DiveDataset::clear( void )
  {
    lfdnr = 0;
    depth = 0.0;
    temperature = 0;
    ppo2 = 0.0;
    ppo2_1 = 0.0;
    ppo2_2 = 0.0;
    ppo2_3 = 0.0;
    nextStep = 0;
    setpoint = 10;
    n2 = 79;
    he = 0;
    z_time = 999;
    abs_depth = 0.0;
  }

  DiveLogEntry::DiveLogEntry(){};
  DiveLogEntry::DiveLogEntry( int _di,
                              int _lf,
                              int _pr,
                              int _de,
                              int _te,
                              double _ac,
                              double _p2,
                              double _p21,
                              double _p22,
                              double _p23,
                              int _sp,
                              int _n,
                              int _h,
                              int _z,
                              int _nx )
      : detailId( _di )
      , lfdNr( _lf )
      , pressure( _pr )
      , depth( _de )
      , temperature( _te )
      , acku( _ac )
      , ppo2( _p2 )
      , ppo2_1( _p21 )
      , ppo2_2( _p22 )
      , ppo2_3( _p23 )
      , setpoint( _sp )
      , n2( _n )
      , he( _h )
      , zeroTime( _z )
      , nextStep( _nx )
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
    *lg << LDEBUG << "SPX42Database::SPX42Database..." << Qt::endl;
  }

  /**
   * @brief SPX42Database::~SPX42Database
   */
  SPX42Database::~SPX42Database()
  {
    *lg << LDEBUG << "SPX42Database::~SPX42Database..." << Qt::endl;
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
    *lg << LDEBUG << "SPX42Database::openDatabase..." << Qt::endl;
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
    *lg << LDEBUG << "SPX42Database::openDatabase -> add db connection" << Qt::endl;
    db = QSqlDatabase::addDatabase( SPX42Database::sqlDriver, currentConnectionName );
    *lg << LDEBUG << "SPX42Database::openDatabase -> add db name: " << dbName << Qt::endl;
    db.setDatabaseName( dbName );
    //
    // Pfad prüfen und ggf anlegen ?
    //
    if ( createPath )
    {
      QFileInfo fInfo( dbName );
      QString filePath( fInfo.path() );
      *lg << LDEBUG << "SPX42Database::openDatabase -> db path: " << filePath << Qt::endl;
      if ( !QDir( filePath ).exists() )
      {
        *lg << LDEBUG << "SPX42Database::openDatabase -> db path not exist. create it!" << Qt::endl;
        if ( QDir().mkpath( filePath ) )
        {
          *lg << LDEBUG << "SPX42Database::openDatabase -> path succsesful created!" << Qt::endl;
        }
        else
        {
          *lg << LCRIT << "SPX42Database::openDatabase -> path NOT created!" << Qt::endl;
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
      *lg << LWARN << "SPX42Database::openDatabase -> an error while open the database: " << err.text() << Qt::endl;
    }
    else
    {
      if ( db.isOpen() )
        if ( !checkOrCreateTables() )
        {
          *lg << LWARN << "SPX42Database::openDatabase -> NOT OK" << Qt::endl;
          // TODO: Fehler in err hinterlassen!
        }
        else
        {
          *lg << LDEBUG << "SPX42Database::openDatabase -> OK" << Qt::endl;
        }
      else
      {
        *lg << LWARN << "SPX42Database::openDatabase -> NOT OK" << Qt::endl;
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
    *lg << LDEBUG << "SPX42Database::closeDatabase..." << Qt::endl;
    QSqlDatabase::database( currentConnectionName, false ).close();
    //
    // Neue db ==> das alte db objekt damit der Vernichtung anheimgeben
    //
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase( currentConnectionName );
    currentConnectionName.clear();
    *lg << LDEBUG << "SPX42Database::closeDatabase...OK" << Qt::endl;
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
    *lg << LDEBUG << "SPX42Database::getDeviceAliasHash..." << Qt::endl;
    DeviceAliasHash aliase;
    //
    // Sperre DB solange
    //
    QMutexLocker locker( &dbMutex );
    //
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      QString sql = "select mac,name,alias,last from devices";
      query.setForwardOnly( true );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::getDeviceAliasHash -> problem while select from database: <" << err.text() << ">" << Qt::endl;
        return ( aliase );
      }
      while ( query.next() )
      {
        QString mac( query.value( 0 ).toString() );
        // &_mac, &_name, &_alias, _lastConnect, _device_id
        SPX42DeviceAlias devAlias( query.value( 0 ).toString(), query.value( 1 ).toString(), query.value( 2 ).toString(),
                                   static_cast< bool >( ( query.value( 3 ).toInt() & 0x01 ) ) );
        aliase.insert( mac, devAlias );
      }
      *lg << LDEBUG << "SPX42Database::getDeviceAliasHash -> <" << aliase.count() << "> items..." << Qt::endl;
    }
    else
    {
      *lg << LWARN << "SPX42Database::getDeviceAliasHash -> database not valid or not opened." << Qt::endl;
    }
    return ( aliase );
  }

  bool SPX42Database::addAlias( const QString &mac, const QString &name, const QString &alias, bool lastConnected )
  {
    // serialize schreibzugriff
    QMutexLocker locker( &dbMutex );
    //
    *lg << LDEBUG << "SPX42Database::addAlias..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      QSqlError err;
      QString sql;
      //
      // guck nach, ob der Eintrag vorhanden ist
      sql = QString( "select mac from devices where mac='%1'" ).arg( mac );
      *lg << LDEBUG << "SPX42Database::addAlias -> ASK (" << sql << ")..." << Qt::endl;
      if ( !query.exec( sql ) )
      {
        err = db.lastError();
        *lg << LWARN << "SPX42Database::addAlias -> problem while select from database: <" << err.text() << ">" << Qt::endl;
        return ( false );
      }

      *lg << LDEBUG << "SPX42Database::addAlias -> ASK ...OK" << Qt::endl;
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
        *lg << LDEBUG << "SPX42Database::addAlias -> INSERT (" << sql << ")..." << Qt::endl;
        if ( query.exec( sql ) )
        {
          *lg << LDEBUG << "SPX42Database::addAlias -> insert OK" << Qt::endl;
          return ( true );
        }
        err = db.lastError();
        *lg << LWARN << "SPX42Database::addAlias -> insert failed: <" << err.text() << ">" << Qt::endl;
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
      *lg << LDEBUG << "SPX42Database::addAlias -> UPDATE (" << sql << ")..." << Qt::endl;
      if ( !query.exec( sql ) )
      {
        err = db.lastError();
        *lg << LWARN << "SPX42Database::addAlias -> update failed: <" << err.text() << ">" << Qt::endl;
        return ( false );
      }
      *lg << LDEBUG << "SPX42Database::addAlias -> update OK" << Qt::endl;
      return ( true );
    }
    else
    {
      *lg << LDEBUG << "SPX42Database::addAlias -> database not valid or not opened." << Qt::endl;
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
    // serialize schreibzugriff
    QMutexLocker locker( &dbMutex );
    //
    *lg << LDEBUG << "SPX42Database::setAliasForMac..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( QString( "update devices set alias='%1' where mac='%2'" ).arg( alias ).arg( mac ), db );
      if ( query.exec() )
      {
        //
        // Abfrage korrekt bearbeitet
        //
        *lg << LDEBUG << "SPX42Database::setAliasForMac...OK" << Qt::endl;
        return ( true );
      }
      else
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::setAliasForMac -> <" << err.text() << ">..." << Qt::endl;
      }
    }
    else
    {
      *lg << LWARN << "SPX42Database::setAliasForMac -> db is not valid or not opened." << Qt::endl;
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
    // serialize schreibzugriff
    QMutexLocker locker( &dbMutex );
    //
    *lg << LDEBUG << "SPX42Database::setAliasForName..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( QString( "update devices set alias='%1' where name='%2'" ).arg( alias ).arg( name ), db );
      if ( query.exec() )
      {
        //
        // Abfrage korrekt bearbeitet
        //
        *lg << LDEBUG << "SPX42Database::setAliasForName...OK" << Qt::endl;
        return ( true );
      }
      else
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::setAliasForName -> <" << err.text() << ">..." << Qt::endl;
      }
    }
    else
    {
      *lg << LWARN << "SPX42Database::setAliasForName -> db is not valid or not opened." << Qt::endl;
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
    *lg << LDEBUG << "SPX42Database::getAliasForMac..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      QString sql = QString( "select alias from devices where mac='%1'" ).arg( mac );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::getAliasForMac -> <" << err.text() << ">..." << Qt::endl;
      }
      if ( query.next() )
      {
        if ( !query.value( 0 ).isNull() )
          result = query.value( 0 ).toString();
      }
      return ( result );
    }
    *lg << LWARN << "SPX42Database::getAliasForMac -> db is not valid or not opened." << Qt::endl;
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
    *lg << LDEBUG << "SPX42Database::getAliasForName..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      QString sql = QString( "select alias from devices where name='%1'" ).arg( name );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LDEBUG << "SPX42Database::getAliasForName -> <" << err.text() << ">..." << Qt::endl;
      }
      if ( query.next() )
      {
        if ( !query.value( 0 ).isNull() )
          result = query.value( 0 ).toString();
      }
      return ( result );
    }
    *lg << LWARN << "SPX42Database::getAliasForName -> db is not valid or not opened." << Qt::endl;
    return ( result );
  }

  /**
   * @brief SPX42Database::setLastConnected
   * @param mac
   * @return
   */
  bool SPX42Database::setLastConnected( const QString &mac )
  {
    // serialize schreibzugriff
    QMutexLocker locker( &dbMutex );
    //
    QString sql;
    QSqlQuery query( db );
    *lg << LDEBUG << "SPX42Database::setLastConnected..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      //
      // alle flags löschen
      //
      sql = QString( "update devices set last=0" );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::setLastConnected -> <" << err.text() << ">..." << Qt::endl;
        return ( false );
      }
      sql = QString( "update devices set last=%1 where mac='%2'" ).arg( 1 ).arg( mac );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::setLastConnected -> <" << err.text() << ">..." << Qt::endl;
        return ( false );
      }
      //
      // Abfrage korrekt bearbeitet
      //
      *lg << LDEBUG << "SPX42Database::setLastConnected...OK" << Qt::endl;
      return ( true );
    }
    else
    {
      *lg << LWARN << "SPX42Database::setLastConnected -> db is not valid or not opened." << Qt::endl;
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
    *lg << LDEBUG << "SPX42Database::getDevicveId for <" << mac << ">..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      //
      // existiert der Eintrag?
      //
      QSqlQuery query( QString( "select device_id from devices where mac='%1'" ).arg( mac ), db );
      if ( !query.next() )
      {
        *lg << LWARN << "SPX42Database::getDevicveId -> requested mac <" << mac << "> not found in database..." << Qt::endl;
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
        *lg << LWARN << "SPX42Database::getDetailId -> <" << err.text() << ">..." << Qt::endl;
        return ( false );
      }
      if ( !query.next() )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::getDetailId -> <" << err.text() << ">..." << Qt::endl;
        return ( detailId );
      }
      else
        detailId = query.value( 0 ).toInt();
      query.clear();
      return ( detailId );
    }
    *lg << LWARN << "SPX42Database::getDetailId -> db not open or not exist!" << Qt::endl;
    return ( detailId );
  }

  /**
   * @brief SPX42Database::getLastConnected
   * @return
   */
  QString SPX42Database::getLastConnected( void )
  {
    *lg << LDEBUG << "SPX42Database::getLastConnected..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      //
      // alle flags löschen
      //
      QSqlQuery query( "select mac from devices where last=1", db );
      if ( !query.next() )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::getLastConnected -> <" << err.text() << ">..." << Qt::endl;
        return ( "" );
      }
      //
      // Abfrage korrekt bearbeitet
      //
      *lg << LDEBUG
          << QString( "SPX42Database::getLastConnected -> device: %1" )
                 .arg( query.value( 0 ).isNull() ? "NONE" : query.value( 1 ).toString() )
          << Qt::endl;
      return ( query.value( 0 ).toString() );
    }
    else
    {
      *lg << LWARN << "SPX42Database::setAliasForName -> db is not valid or not opened." << Qt::endl;
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
        *lg << LDEBUG << "SPX42Database::existTable -> tablename <" << foundTable << "> found..." << Qt::endl;
        //
        // ist das das von mir erwartete Ergebnis (eigentlich würde reichen "nicht leer")
        //
        if ( foundTable == tableName )
        {
          *lg << LDEBUG << "SPX42Database::existTable -> table <" << tableName << "> exists..." << Qt::endl;
          return ( true );
        }
      }
      *lg << LDEBUG << "SPX42Database::existTable -> table <" << tableName << "> NOT exists..." << Qt::endl;
      return ( false );
    }
    *lg << LWARN << "SPX42Database::existTable -> db is not open or exist!" << Qt::endl;
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
      *lg << LDEBUG << "SPX42Database::getDatabaseVersion -> <" << version << ">..." << Qt::endl;
    }
    else
    {
      *lg << LDEBUG << "SPX42Database::getDatabaseVersion -> db not open or not exist!" << Qt::endl;
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
    *lg << LDEBUG << "SPX42Database::createVersionTable..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      sql = "drop table if exists 'version'";
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createVersionTable -> failed drop table if exist <" << err.text() << ">" << Qt::endl;
        return ( false );
      }
      query.clear();
      if ( !query.exec( SPX42DatabaseConstants::createVersionTable ) )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createVersionTable -> failed create table <" << err.text() << ">" << Qt::endl;
        return ( false );
      }
      query.clear();
      *lg << LDEBUG << "SPX42Database::createVersionTable -> insert version number..." << Qt::endl;
      sql = QString( "insert into 'version' (version) values (%1)" ).arg( SPX42Database::databaseVersion );
      if ( query.exec( sql ) )
      {
        *lg << LDEBUG << "SPX42Database::createVersionTable -> insert version number: OK" << Qt::endl;
        return ( true );
      }
      QSqlError err = db.lastError();
      *lg << LCRIT << "SPX42Database::createVersionTable -> failed insert data <" << err.text() << ">" << Qt::endl;
    }
    else
    {
      *lg << LWARN << "SPX42Database::createVersionTable -> db nor open or not exist!" << Qt::endl;
    }
    return ( false );
  }

  /**
   * @brief SPX42Database::checkOrCreateTables
   * @return
   */
  bool SPX42Database::checkOrCreateTables()
  {
    *lg << LDEBUG << "SPX42Database::checkOrCreateTables..." << Qt::endl;
    bool haveToCreateNewVersionTable = false;
    //
    // Datenbank ist offen, finde zuerst die Versionsnummer
    //
    if ( !existTable( "version" ) )
    {
      *lg << LWARN << "SPX42Database::checkOrCreateTables -> version table not availible!" << Qt::endl;
      //
      // keine Versionstabelle-> dann erzeuge alle Tabellen neu
      //
      return createAllTables();
    }
    qint16 currentDatabaseVersionNumber = getDatabaseVersion();
    *lg << LDEBUG << "SPX42Database::checkOrCreateTables -> current database version " << currentDatabaseVersionNumber << "..."
        << Qt::endl;
    if ( currentDatabaseVersionNumber == -1 )
    {
      // Da ging was gewaltig schief, reagiere!
      *lg << LCRIT << "SPX42Database::checkOrCreateTables -> not an version number found!" << Qt::endl;
      // TODO: was unternehmen wenn keine Version gefunden wurde
      return ( false );
    }
    //
    if ( !existTable( "devices" ) )
    {
      if ( !createAliasTable() )
      {
        *lg << LCRIT << "SPX42Database::checkOrCreateTables -> can't not find or create alias table!" << Qt::endl;
        return ( false );
      }
    }
    //
    if ( !existTable( "detaildir" ) )
    {
      if ( !createLogDirTable() )
      {
        *lg << LCRIT << "SPX42Database::checkOrCreateTables -> can't not find or create log directory table!" << Qt::endl;
        return ( false );
      }
    }
    else
    {
      if ( currentDatabaseVersionNumber < databaseVersion )
      {
        //
        // von 4 zu 5 kommt das Feld NOTES dazu...
        // und wenn es klappt, Versiontabelle erzeugen
        //
        haveToCreateNewVersionTable = updateDetailDirTableFromFour();
      }
    }
    //
    if ( !existTable( "logdata" ) )
    {
      if ( !createLogDataTable() )
      {
        *lg << LCRIT << "SPX42Database::checkOrCreateTables -> can't not find or create log details table!" << Qt::endl;
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

  bool SPX42Database::updateDetailDirTableFromFour( void )
  {
    QString sql;
    *lg << LDEBUG << "SPX42Database::updateDetailDirTableFromFour..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      sql = "alter table detaildir add column notes text(120) default null";
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::updateDetailDirTableFromFour -> failed alter table  <" << err.text() << ">" << Qt::endl;
        return ( false );
      }
      *lg << LDEBUG << "SPX42Database::updateDetailDirTableFromFour...OK" << Qt::endl;
      return ( true );
    }
    return ( false );
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
    *lg << LDEBUG << "SPX42Database::createAliasTable..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      sql = "drop table if exists devices";
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createAliasTable -> failed drop if exist <" << err.text() << ">" << Qt::endl;
        return ( false );
      }
      //
      // das Statement
      //
      sql = SPX42DatabaseConstants::createDeviceTable;
      if ( query.exec( sql ) )
      {
        *lg << LDEBUG << "SPX42Database::createAliasTable...OK" << Qt::endl;
        return ( true );
      }
      QSqlError err = db.lastError();
      *lg << LCRIT << "SPX42Database::createAliasTable -> failed create table <" << err.text() << ">" << Qt::endl;
    }
    else
    {
      *lg << LWARN << "SPX42Database::createAliasTable -> db not open or not exist!" << Qt::endl;
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
    *lg << LDEBUG << "SPX42Database::createLogDirTable..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      if ( !db.transaction() )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createLogDirTable -> open transaction failed " << err.text() << Qt::endl;
      }
      QSqlQuery query( db );
      // alte Tabelle entfernen
      sql = "drop table if exists 'detaildir'";
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createLogDirTable -> failed drop if exist <" << err.text() << ">" << Qt::endl;
        db.rollback();
        return ( false );
      }
      // erzeuge neue Tabelle
      sql = SPX42DatabaseConstants::createDiveLogDirectoryTable;
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::createLogDirTable -> <" << err.text() << ">..." << Qt::endl;
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
        *lg << LWARN << "SPX42Database::createLogDirTable 1 -> <" << err.text() << ">..." << Qt::endl;
        db.rollback();
        return ( false );
      }
      query.clear();
      sql = SPX42DatabaseConstants::createDiveLogTableIndex02;
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::createLogDirTable 2 -> <" << err.text() << ">..." << Qt::endl;
        db.rollback();
        return ( false );
      }
      if ( !db.commit() )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createLogDirTable -> commit failed " << err.text() << Qt::endl;
        db.rollback();
        return ( false );
      }
      *lg << LDEBUG << "SPX42Database::createLogDirTable...OK" << Qt::endl;
      return ( true );
    }
    *lg << LWARN << "SPX42Database::createLogDirTable -> db not opened" << Qt::endl;
    return ( false );
  }

  /**
   * @brief SPX42Database::createLogDataTable
   * @return
   */
  bool SPX42Database::createLogDataTable()
  {
    QString sql;
    *lg << LDEBUG << "SPX42Database::createLogDataTable <logdata>..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      // Transaktion eröffnen
      if ( !db.transaction() )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createLogDataTable -> open transaction failed <" << err.text() << ">" << Qt::endl;
      }
      QSqlQuery query( db );
      sql = "drop table if exists logdata";
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createLogDataTable -> failed drop if exist <" << err.text() << ">" << Qt::endl;
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
        *lg << LDEBUG << "SPX42Database::createLogDataTable -> create table ok" << Qt::endl;
      }
      else
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createLogDataTable -> failed create <" << err.text() << ">" << Qt::endl;
        db.rollback();
        return ( false );
      }
      // index 1 machen
      query.clear();
      sql = SPX42DatabaseConstants::createDiveLogDetailsTableIndex01;
      if ( query.exec( sql ) )
      {
        *lg << LDEBUG << "SPX42Database::createLogDataTable -> create index ok" << Qt::endl;
      }
      else
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createLogDataTable -> failed create index  <" << err.text() << ">" << Qt::endl;
        db.rollback();
        return ( false );
      }
      // index 2 machen
      query.clear();
      sql = SPX42DatabaseConstants::createDiveLogDetailsTableIndex02;
      if ( query.exec( sql ) )
      {
        *lg << LDEBUG << "SPX42Database::createLogDataTable -> create index ok" << Qt::endl;
      }
      else
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createLogDataTable -> failed create index  <" << err.text() << ">" << Qt::endl;
        db.rollback();
        return ( false );
      }
      // commit
      if ( !db.commit() )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::createLogDataTable -> commit failed <" << err.text() << ">" << Qt::endl;
        db.rollback();
        return ( false );
      }
      return ( true );
    }
    *lg << LWARN << "SPX42Database::createLogDataTable -> db not opened" << Qt::endl;
    return ( false );
  }

  /**
   * @brief SPX42Database::validateNextStep es kann nur 10, 30 oder 60 geben
   * @param val
   * @param oldval
   * @return
   */
  int SPX42Database::validateNextStep( int val, int oldval )
  {
    if ( ( val == ProjectConst::LOG_INTERVAL_01 ) || ( val == ProjectConst::LOG_INTERVAL_02 ) ||
         ( val == ProjectConst::LOG_INTERVAL_03 ) )
      return ( val );
    //
    // da kam unsinn...
    //
    // Voreinstellung machen
    //
    int retval = ProjectConst::LOG_INTERVAL_02;
    //
    // vorhergehenden Wert versuchen
    //
    if ( ( oldval == ProjectConst::LOG_INTERVAL_01 ) || ( oldval == ProjectConst::LOG_INTERVAL_02 ) ||
         ( oldval == ProjectConst::LOG_INTERVAL_03 ) )
      retval = oldval;
    *lg << LWARN
        << QString( "SPX42Database::validateNextStep -> next step value <%1> not plausible, corrected to <%2>" )
               .arg( val )
               .arg( retval )
        << Qt::endl;
    return ( retval );
  }

  /**
   * @brief SPX42Database::insertLogentry
   * @param entr
   * @return
   */
  bool SPX42Database::insertLogentry( const DiveLogEntry &entr )
  {
    static int next_step = 0;
    // serialize schreibzugriff
    QMutexLocker locker( &dbMutex );
    //
    // Plausibilität testen
    //
    next_step = validateNextStep( entr.nextStep, next_step );

    //
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
                        .arg( next_step /*entr.nextStep*/ );
      QSqlQuery query( sql, db );
      if ( query.exec() )
      {
        //
        // Abfrage korrekt bearbeitet
        //
        return ( true );
      }
      else
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::insertLogentry -> <" << err.text() << ">..." << Qt::endl;
      }
    }
    *lg << LWARN << "SPX42Database::insertLogentry -> db is not valid or not opened." << Qt::endl;
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
    static int next_step = 0;
    // serialize schreibzugriff
    QMutexLocker locker( &dbMutex );
    //
    // Plausibilität testen
    //
    next_step = validateNextStep( static_cast< int >( cmd->getValueAt( SPXCmdParam::LOGDETAIL_NEXT_STEP ) ), next_step );
    //
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
                        .arg( static_cast< int >( next_step ) );
      if ( query.exec( sql ) )
      {
        //
        // Abfrage korrekt bearbeitet
        //
        return ( true );
      }
      else
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::insertLogentry -> <" << err.text() << ">..." << Qt::endl;
      }
    }
    *lg << LWARN << "SPX42Database::insertLogentry -> db is not valid or not opened." << Qt::endl;
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
    // serialize schreibzugriff
    QMutexLocker locker( &dbMutex );
    //

    *lg << LDEBUG << "SPX42Database::insertDiveLogInBase..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      int device_id = getDevicveId( mac );
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
        *lg << LWARN << "SPX42Database::insertDiveLogInBase -> select count exec <" << err.text() << ">..." << Qt::endl;
        return ( -1 );
      }
      if ( !query.next() )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::insertDiveLogInBase -> select count next <" << err.text() << ">..." << Qt::endl;
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
        *lg << LWARN << "SPX42Database::insertDiveLogInBase -> insert/update <" << err.text() << ">..." << Qt::endl;
        return ( -1 );
      }
      *lg << LWARN << "SPX42Database::insertDiveLogInBase...OK" << Qt::endl;
      return ( device_id );
    }
    *lg << LWARN << "SPX42Database::insertDiveLogInBase -> db not open or not exist!" << Qt::endl;
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
    QString sql;
    //
    if ( db.isValid() && db.isOpen() )
    {
      *lg << LDEBUG << "SPX42Database::existDiveLogInBase -> database open and valid..." << Qt::endl;
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
        int datasets = query.value( 0 ).toInt();
        if ( datasets > 0 )
        {
          exist = true;
        }
        *lg << LDEBUG
            << QString( "SPX42Database::existDiveLogInBase -> <%1> sets == %2..." ).arg( datasets ).arg( exist ? "YES" : "NO" )
            << Qt::endl;
        return ( exist );
      }
      else
      {
        *lg << LWARN << "SPX42Database::existDiveLogInBase -> no dataset..." << Qt::endl;
      }
      return ( exist );
    }
    *lg << LWARN << "SPX42Database::existDiveLogInBase -> db not open or not exist!" << Qt::endl;
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
    // serialize schreibzugriff
    QMutexLocker locker( &dbMutex );
    //
    *lg << LDEBUG << QString( "SPX42Database::delDiveLogFromBase -> <%1> num: <%2>..." ).arg( mac ).arg( diveNum ) << Qt::endl;

    if ( db.isValid() && db.isOpen() )
    {
      int detailId = getDetailId( mac, diveNum );
      //
      // Schritt 1 Detailid erfragen
      //
      if ( detailId < 0 )
        return ( false );
      QString sql;
      if ( !db.transaction() )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::delDiveLogFromBase -> open transaction failed <" << err.text() << ">" << Qt::endl;
      }
      QSqlQuery query( db );
      //
      // Schritt 2: das Verzeichnis löschen (wegen Foreign Key das zuerst....)
      //
      sql = QString( "delete from detaildir where detail_id=%1 and divenum=%2" ).arg( detailId ).arg( diveNum );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::delDiveLogFromBase -> <" << err.text() << ">..." << Qt::endl;
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
        *lg << LWARN << "SPX42Database::delDiveLogFromBase -> <" << err.text() << ">..." << Qt::endl;
        db.rollback();
        return ( false );
      }
      //
      // ging alles Glatt, commit
      //
      if ( !db.commit() )
      {
        QSqlError err = db.lastError();
        *lg << LCRIT << "SPX42Database::delDiveLogFromBase -> commit failed <" << err.text() << ">..." << Qt::endl;
        db.rollback();
        return ( false );
      }
      return ( true );
    }
    *lg << LWARN << "SPX42Database::delDiveLogFromBase -> db not open or not exist!" << Qt::endl;
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
    QString sql;
    *lg << LDEBUG << "SPX42Database::getMaxDepthFor..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      int maxDepth = -1;
      int detail_id = getDetailId( mac, diveNum );
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
        *lg << LWARN << "SPX42Database::getMaxDepthFor -> <" << err.text() << ">..." << Qt::endl;
        return ( -1 );
      }
      //
      // Abfrage korrekt bearbeitet
      //
      maxDepth = query.value( 0 ).toInt();
      *lg << LDEBUG << "SPX42Database::getMaxDepthFor -> max depth: <" << maxDepth << "m>" << Qt::endl;
      return ( maxDepth );
    }
    *lg << LWARN << "SPX42Database::getMaxDepthFor -> db is not valid or not opened." << Qt::endl;
    return ( -1 );
  }

  /**
   * @brief SPX42Database::getDiveLenFor
   * @param mac
   * @param diveNum
   * @return Länge in Sekunden
   */
  int SPX42Database::getDiveLenFor( const QString &mac, int diveNum )
  {
    QString sql;
    *lg << LDEBUG << "SPX42Database::getDiveLenFor..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      int diveLen = -1;
      int detail_id = getDetailId( mac, diveNum );
      if ( detail_id == -1 )
      {
        return ( diveLen );
      }
      //
      // versuche mal rauszubekommen wie tief das war
      //
      sql = QString( "select sum(next_step) from logdata where detail_id= %1" ).arg( detail_id );
      QSqlQuery query( sql, db );
      if ( !query.next() )
      {
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::getDiveLenFor -> <" << err.text() << ">..." << Qt::endl;
        return ( -1 );
      }
      //
      // Abfrage korrekt bearbeitet
      //
      diveLen = query.value( 0 ).toInt();
      *lg << LDEBUG << "SPX42Database::getDiveLenFor -> dive len: <" << diveLen << "sec>" << Qt::endl;
      return ( diveLen );
    }
    *lg << LWARN << "SPX42Database::getDiveLenFor -> db is not valid or not opened." << Qt::endl;
    return ( -1 );
  }

  /**
   * @brief SPX42Database::getChartSet
   * @param mac
   * @param diveNum
   * @return
   */
  DiveDataSetsPtr SPX42Database::getDiveDataSets( const QString &mac, int diveNum )
  {
    QString sql;
    DiveDataSetsPtr chartSet = DiveDataSetsPtr( new DiveDataSetsVector() );
    *lg << LDEBUG << "SPX42Database::getDiveDataSets..." << Qt::endl;
    if ( db.isValid() && db.isOpen() )
    {
      int detail_id = getDetailId( mac, diveNum );
      if ( detail_id == -1 )
        return ( chartSet );
      QSqlQuery query( db );
      query.setForwardOnly( true );
      //
      // suche die Daten aus der Datenbank zusammen
      //
      sql = QString(
                "select lfnr,depth,temperature,ppo2,ppo2_1,ppo2_2,ppo2_3,next_step,setpoint,n2,he,zerotime from logdata where "
                "detail_id=%1 order by lfnr" )
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
        *lg << LWARN << "SPX42Database::getDiveDataSets -> <" << err.text() << ">..." << Qt::endl;
        chartSet->clear();
        return ( chartSet );
      }
      //
      // alle Datensätze abholen
      //
      while ( query.next() )
      {
        DiveDataset currSet;
        currSet.lfdnr = query.value( 0 ).toInt();
        currSet.depth = 0.0 - ( query.value( 1 ).toDouble() / 10.0 );
        currSet.abs_depth = query.value( 1 ).toDouble() / 10.0;
        currSet.temperature = query.value( 2 ).toInt();
        currSet.ppo2 = query.value( 3 ).toDouble();
        currSet.ppo2_1 = query.value( 4 ).toDouble();
        currSet.ppo2_2 = query.value( 5 ).toDouble();
        currSet.ppo2_3 = query.value( 6 ).toDouble();
        currSet.nextStep = query.value( 7 ).toInt();
        currSet.setpoint = ( query.value( 8 ).toDouble() / 10.0 );
        currSet.n2 = query.value( 9 ).toInt();
        currSet.he = query.value( 10 ).toInt();
        currSet.z_time = query.value( 11 ).toInt();
        chartSet->append( currSet );
      }
      if ( chartSet->size() > 0 )
      {
        //
        // Kleiner Trick zum finden eines Punkt 0
        //
        chartSet->insert( 0, chartSet->first() );
      }
      *lg << LDEBUG << "SPX42Database::getDiveDataSets...OK" << Qt::endl;
      return ( chartSet );
    }
    *lg << LWARN << "SPX42Database::getDiveDataSets -> db is not valid or not opened." << Qt::endl;
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
    QString sql;
    //
    // mache einen zeiger auf die Liste
    //
    SPX42LogDirectoryEntryListPtr deviceLog = SPX42LogDirectoryEntryListPtr( new SPX42LogDirectoryEntryList );
    //
    if ( db.isValid() && db.isOpen() )
    {
      int device_id = getDevicveId( mac );
      if ( device_id == -1 )
        return ( deviceLog );
      //
      // suche die Daten aus der Datenbank zusammen
      //
      QSqlQuery query( db );
      query.setForwardOnly( true );
      sql = QString( "select divenum,ux_timestamp from detaildir where device_id=%1" ).arg( device_id );
      if ( !query.exec( sql ) )
      {
        //
        // es gab einen Fehler bei der Anfrage
        //
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::getLogentrysForDevice -> error: <" << err.text() << ">..." << Qt::endl;
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
    *lg << LWARN << "SPX42Database::getLogentrysForDevice -> db is not valid or not opened." << Qt::endl;
    return ( deviceLog );
  }

  bool SPX42Database::computeStatistic( int detail_id )
  {
    // serialize schreibzugriff
    QMutexLocker locker( &dbMutex );
    //
    if ( db.isValid() && db.isOpen() )
    {
      QString sql;
      //
      // SQL aus dem Template erstellen
      //
      sql = SPX42Database::statisticTemplate.arg( detail_id );
#ifdef DEBUG
      *lg << LDEBUG << "SPX42Database::computeStatistic -> STATISTIC SQL: <" << sql << ">" << Qt::endl;
#endif
      QSqlQuery query( sql, db );
      if ( !query.exec( sql ) )
      {
        //
        // es gab einen Fehler bei der Anfrage
        //
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::computeStatistic -> error: <" << err.text() << ">..." << Qt::endl;
        return ( false );
      }
      return ( true );
    }
    *lg << LWARN << "SPX42Database::computeStatistic -> db is not valid or not opened." << Qt::endl;
    return ( false );
  }

  UsedGasList SPX42Database::getGasList( const QString &mac, const QVector< int > *diveNums )
  {
    QString sql;
    UsedGasList gasList;
    QStringList diveNumStrList;
    if ( db.isValid() && db.isOpen() )
    {
      *lg << LDEBUG << "SPX42Database::getGasList..." << Qt::endl;
      int device_id = getDevicveId( mac );
      if ( device_id == -1 )
        return ( gasList );
      for ( auto num : *diveNums )
      {
        diveNumStrList.append( QString( "%1" ).arg( num ) );
      }
      QSqlQuery query( db );
      //
      // sammle alle gase der angeforderten Tauchgänge
      //
      sql = "select n2,he\n";
      sql += "from logdata\n";
      sql += "where detail_id in\n";
      sql += "  ( \n";
      sql += "    select detail_id \n";
      sql += "    from detaildir \n";
      sql += "    where \n";
      sql += "    device_id = 2 and \n";
      sql += QString( "    divenum in( %1 ) \n" ).arg( diveNumStrList.join( "," ) );
      sql += "  ) \n";
      sql += "group by \n";
      sql += "  n2, he";
      if ( !query.exec( sql ) )
      {
        //
        // es gab einen Fehler bei der Anfrage
        //
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::getGasList -> error: <" << err.text() << ">..." << Qt::endl;
        return ( gasList );
      }
      //
      // Daten einsammeln
      //
      while ( query.next() )
      {
        QPair< int, int > entry( query.value( 0 ).toInt(), query.value( 1 ).toInt() );
        gasList.append( entry );
      }
      *lg << LDEBUG << "SPX42Database::getGasList...OK" << Qt::endl;
      return ( gasList );
    }
    *lg << LWARN << "SPX42Database::computeStatistic -> db is not valid or not opened." << Qt::endl;
    return ( gasList );
  }

  qint64 SPX42Database::getTimestampForDive( const QString &mac, int diveNum )
  {
    QString sql;
    qint64 ux_timestamp = -1;
    if ( db.isValid() && db.isOpen() )
    {
      *lg << LDEBUG << "SPX42Database::getTimestampForDive..." << Qt::endl;
      int device_id = getDevicveId( mac );
      if ( device_id == -1 )
        return ( ux_timestamp );
      QSqlQuery query( db );
      sql = "select ux_timestamp \n";
      sql += "from detaildir \n";
      sql += QString( "where device_id=%1 and divenum=%2" ).arg( device_id ).arg( diveNum );
      if ( !query.exec( sql ) )
      {
        //
        // es gab einen Fehler bei der Anfrage
        //
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::getTimestampForDive -> error: <" << err.text() << ">..." << Qt::endl;
        return ( ux_timestamp );
      }
      //
      // bis hier ging es, jetzt Ergebnis abholen
      //
      if ( query.next() )
      {
        ux_timestamp = static_cast< qint64 >( query.value( 0 ).toLongLong() );
      }
      *lg << LDEBUG << "SPX42Database::getTimestampForDive...OK" << Qt::endl;
      return ( ux_timestamp );
    }
    *lg << LWARN << "SPX42Database::getTimestampForDive -> db is not valid or not opened." << Qt::endl;
    return ( ux_timestamp );
  }

  bool SPX42Database::writeNotesForDive( const QString &mac, int diveNum, const QString &notes )
  {
    QString sql;
    if ( db.isValid() && db.isOpen() )
    {
      *lg << LDEBUG << "SPX42Database::getNotesForDive..." << Qt::endl;
      int device_id = getDevicveId( mac );
      if ( device_id == -1 )
        return ( false );
      QSqlQuery query( db );
      sql = "update detaildir \n";
      sql += QString( "  set notes = '%1'\n" ).arg( notes.left( 120 ) );
      sql += QString( "where device_id=%1 and divenum=%2" ).arg( device_id ).arg( diveNum );
      if ( !query.exec( sql ) )
      {
        //
        // es gab einen Fehler bei der Anfrage
        //
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::getNotesForDive -> error: <" << err.text() << ">..." << Qt::endl;
        return ( false );
      }
      *lg << LDEBUG << "SPX42Database::getNotesForDive...OK" << Qt::endl;
      return ( true );
    }
    *lg << LWARN << "SPX42Database::getNotesForDive -> db is not valid or not opened." << Qt::endl;
    return ( false );
  }

  QString SPX42Database::getNotesForDive( const QString &mac, int diveNum )
  {
    QString sql;
    QString notes;
    if ( db.isValid() && db.isOpen() )
    {
      *lg << LDEBUG << "SPX42Database::getNotesForDive..." << Qt::endl;
      int device_id = getDevicveId( mac );
      if ( device_id == -1 )
        return ( notes );
      QSqlQuery query( db );
      sql = "select notes\n";
      sql += "from detaildir \n";
      sql += QString( "where device_id=%1 and divenum=%2" ).arg( device_id ).arg( diveNum );
      if ( !query.exec( sql ) )
      {
        //
        // es gab einen Fehler bei der Anfrage
        //
        QSqlError err = db.lastError();
        *lg << LWARN << "SPX42Database::getNotesForDive -> error: <" << err.text() << ">..." << Qt::endl;
        return ( notes );
      }
      //
      // bis hier ging es, jetzt Ergebnis abholen
      //
      if ( query.next() )
      {
        if ( !query.value( 0 ).isNull() )
          notes = query.value( 0 ).toString();
      }
      *lg << LDEBUG << "SPX42Database::getNotesForDive...OK" << Qt::endl;
      return ( notes );
    }
    *lg << LWARN << "SPX42Database::getNotesForDive -> db is not valid or not opened." << Qt::endl;
    return ( notes );
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
