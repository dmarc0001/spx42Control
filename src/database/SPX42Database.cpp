#include "SPX42Database.hpp"

namespace spx
{
  const QString SPX42Database::versionTableName{"version"};
  const QString SPX42Database::deviceTableName{"devices"};

  const QString SPX42Database::sqlDriver{"QSQLITE"};
  const qint16 SPX42Database::databaseVersion{2};

  //! template für insert in die Logdaten
  const QString SPX42Database::loglineInsertTemplate{
      "insert into '%1' \n"
      "(divenum,lfdNr,pressure,depth,temperature,acku,ppo2,ppo2_1,ppo2_2,ppo2_3,setpoint,n2,he,zeroTime,nextStep)\n"
      "values\n"
      "(%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16)"};

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

  DiveLogEntry::DiveLogEntry(){};
  DiveLogEntry::DiveLogEntry( int dn,
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
      : diveNum( dn )
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
      : diveNum( en.diveNum )
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
    if ( db.open() )
      db.commit();
    QSqlDatabase::database( currentConnectionName, false ).close();
    //
    // Neue db ==> das alte db objekt damit der Vernichtung anheimgeben
    //
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase( currentConnectionName );
    currentConnectionName.clear();
    lg->debug( "SPX42Database::closeDatabase...OK" );
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
      QSqlQuery query( QString( "select mac,name,alias,last from '%1'" ).arg( SPX42Database::deviceTableName ), db );
      while ( query.next() )
      {
        QString mac( query.value( 0 ).toString() );
        // &_mac, &_name, &_alias, _lastConnect
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
      sql = QString( "select mac from '%1' where mac='%2'" ).arg( SPX42Database::deviceTableName ).arg( mac );
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
        sql = QString( "insert into '%1' (mac, name, alias, last) values ('%2', '%3', '%4', %5)" )
                  .arg( SPX42Database::deviceTableName )
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
      sql = QString( "update '%1' set name='%2', alias='%3', last=%4 where mac='%5'" )
                .arg( SPX42Database::deviceTableName )
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
      QSqlQuery query(
          QString( "update '%1' set alias='%2' where mac='%3'" ).arg( SPX42Database::deviceTableName ).arg( alias ).arg( mac ), db );
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
      QSqlQuery query(
          QString( "update '%1' set alias='%2' where name='%3'" ).arg( SPX42Database::deviceTableName ).arg( alias ).arg( name ), db );
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
      sql = QString( "update %1 set last=0" ).arg( SPX42Database::deviceTableName );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::setLastConnected -> <%1>..." ).arg( err.text() ) );
        return ( false );
      }
      sql = QString( "update '%1' set last=%2 where mac='%3'" ).arg( SPX42Database::deviceTableName ).arg( 1 ).arg( mac );
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

  QString SPX42Database::getLogTableName( const QString &mac )
  {
    QString sql;
    QSqlQuery query( db );
    lg->debug( QString( "SPX42Database::getLogTableName for <%1>..." ).arg( mac ) );
    if ( db.isValid() && db.isOpen() )
    {
      //
      // existiert der Eintrag?
      //
      QSqlQuery query( QString( "select logtable from '%1' where mac='%2'" ).arg( SPX42Database::deviceTableName ).arg( mac ), db );
      if ( !query.next() )
      {
        lg->warn( QString( "SPX42Database::getLogTableName -> requested mac <%1> not found in database..." ).arg( mac ) );
        // Leeren String zurück
        return ( QString() );
      }
      else
      {
        if ( query.value( 0 ).isNull() )
        {
          //
          // keine Tabelle?
          //
          QString tableName( mac );
          tableName = tableName.remove( ':' );
          if ( createDeviceLogTable( tableName ) )
          {
            query.clear();
            sql = QString( "update '%1' set logtable='%2' where mac='%3'" )
                      .arg( SPX42Database::deviceTableName )
                      .arg( tableName )
                      .arg( mac );
            if ( query.exec( sql ) )
            {
              //
              // Tabelle erzeugt und eingetragen
              //
              lg->debug( "SPX42Database::getLogTableName...OK" );
              return ( tableName );
            }
          }
          return ( QString() );
        }
        else
        {
          return ( query.value( 0 ).toString() );
        }
      }
    }
    return ( QString() );
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
      QSqlQuery query( QString( "select mac from '%1' where last=1" ).arg( SPX42Database::deviceTableName ), db );
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

  // ##########################################################################
  // PRIVATE Funktionen
  // ##########################################################################

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
      QSqlQuery query( QString( "select max(version) from '%1'" ).arg( SPX42Database::versionTableName ), db );
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
      sql = QString( "drop table if exists '%1'" ).arg( SPX42Database::versionTableName );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createVersionTable -> failed drop table if exist <%1>" ).arg( err.text() ) );
        return ( false );
      }
      sql = QString( "create table '%1' ( version INTEGER default 0 )" ).arg( SPX42Database::versionTableName );
      query.clear();
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createVersionTable -> failed create table <%1>" ).arg( err.text() ) );
        return ( false );
      }
      query.clear();
      lg->debug( "SPX42Database::createVersionTable -> insert version number..." );
      sql = QString( "insert into '%1' (version) values (%2)" )
                .arg( SPX42Database::versionTableName )
                .arg( SPX42Database::databaseVersion );
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
    if ( !existTable( SPX42Database::versionTableName ) )
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
    if ( !existTable( SPX42Database::deviceTableName ) )
    {
      if ( !createAliasTable() )
      {
        lg->crit( "SPX42Database::checkOrCreateTables -> can't not find or create alias table!" );
        return ( false );
      }
    }
    else
    {
      //
      // Tabelle exixistiert
      //
      lg->debug( QString( "SPX42Database::checkOrCreateTables -> database version: %1 programm database version: %2..." )
                     .arg( currentDatabaseVersionNumber )
                     .arg( databaseVersion ) );
      if ( currentDatabaseVersionNumber < databaseVersion )
      {
        //
        // ALARM, da muss ich was machen
        //
        lg->info( "SPX42Database::checkOrCreateTables -> version is different..." );
        if ( databaseVersion == 2 )
        {
          //
          // von Version 1 zu 2
          //
          if ( alterAliasTableFrom1To2() )
          {
            // War ok...
            haveToCreateNewVersionTable = true;
          }
          else
          {
            return ( false );
          }
        }
      }
    }

    //
    // zu guter letzt:
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
      QSqlQuery query( QString( "drop table if exists '%1'" ).arg( SPX42Database::deviceTableName ), db );
      //
      // das Statement zusammenschrauben
      //
      sql = "create table '" + SPX42Database::deviceTableName + "'\n";
      sql += "(\n";
      sql += " mac TEXT not NULL,\n";          // MAC Adresse des Gerätes
      sql += " name TEXT not NULL,\n ";        // Name, gemeldet vom Gerät
      sql += " alias TEXT default NULL,\n";    // Alias, vom Benutzer gestgelegt
      sql += " last INTEGER default 0,\n";     // War es das letzte verbundene Gerät
      sql += " logtable TEXT default NULL\n";  // welche Tabelle?
      sql += ")";
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

  bool SPX42Database::alterAliasTableFrom1To2()
  {
    QString sql;
    lg->debug( "SPX42Database::alterAliasTableFrom1To2..." );
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      if ( !db.transaction() )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::alterAliasTableFrom1To2 -> open transaction failed %1" ).arg( err.text() ) );
      }
      sql = QString( "alter table %1 add logtable TEXT default NULL" ).arg( SPX42Database::deviceTableName );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::alterAliasTableFrom1To2 -> alter table add column failed <%1>" ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      if ( !db.commit() )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::alterAliasTableFrom1To2 -> commit failed %1" ).arg( err.text() ) );
        db.rollback();
        return ( false );
      }
      lg->debug( "SPX42Database::alterAliasTableFrom1To2...OK" );
      return ( true );
    }
    lg->warn( "SPX42Database::alterAliasTableFrom1To2 -> db not opened!" );
    return ( false );
  }

  bool SPX42Database::createDeviceLogTable( QString deviceLogTable )
  {
    QString sql;
    lg->debug( QString( "SPX42Database::createDeviceLogTable <%1>..." ).arg( deviceLogTable ) );
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      sql = QString( "drop table if exists '%1'" ).arg( deviceLogTable );
      if ( !query.exec( sql ) )
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createDeviceLogTable -> failed drop if exist <%1>" ).arg( err.text() ) );
        return ( false );
      }
      query.clear();
      //
      // das Statement zusammenschrauben
      //
      sql = QString( "create table '%1'\n" ).arg( deviceLogTable );
      sql += "(\n";                                // Tabellenneme ist MAC
      sql += " divenum INTEGER not NULL,\n";       // Tauchgangsnummer laufend
      sql += " lfdNr INTEGER not NULL,\n ";        // logeintrag nummer des Tauchganges
      sql += " pressure INTEGER default 0,\n";     // Umgebungsdruck == Tiefe
      sql += " depth INTEGER default 0,\n";        // Tiefe
      sql += " temperature INTEGER default 0,\n";  // Temperatur Wasser
      sql += " acku REAL default 0,\n";            // Akkuspannung
      sql += " ppo2 REAL default 0,\n";            // Sauerstoffpartioaldruck reslultierend
      sql += " ppo2_1 REAL default 0,\n";          // PPO2 Sensor 1
      sql += " ppo2_2 REAL default 0,\n";          // PPO2 Sensor 2
      sql += " ppo2_3 REAL default 0,\n";          // PPO2 Sensor 3
      sql += " setpoint INTEGER default 0,\n";     // Setpoint PPO2
      sql += " n2 INTEGER default 78,\n";          // Stickstoff in prozent
      sql += " he INTEGER default 0,\n";           // Helium in prozent
      sql += " zeroTime INTEGER default 0,\n";     // Nullzeit zu diesem Zeitpunkt
      sql += " nextStep INTEGER default 60\n";     // nächster Logeintrag in zeitlichem Abstand
      sql += ")";
      if ( query.exec( sql ) )
      {
        lg->debug( "SPX42Database::createDeviceLogTable -> create table ok" );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->crit( QString( "SPX42Database::createDeviceLogTable -> failed create <%1>" ).arg( err.text() ) );
        return ( false );
      }
      query.clear();
      sql = QString( "create unique index 'ixd_logId%1' on '%2' (divenum,lfdNr)" ).arg( deviceLogTable ).arg( deviceLogTable );
      if ( query.exec( sql ) )
      {
        lg->debug( "SPX42Database::createDeviceLogTable -> create index ok" );
        return ( true );
      }
      QSqlError err = db.lastError();
      lg->crit( QString( "SPX42Database::createDeviceLogTable -> failed create index  <%1>" ).arg( err.text() ) );
      return ( false );
    }
    lg->warn( "SPX42Database::createDeviceLogTable -> db not opened" );
    return ( false );
  }

  bool SPX42Database::insertLogentry( const QString &tableName, const DiveLogEntry &entr )
  {
    lg->debug( "SPX42Database::insertLogentry..." );
    if ( db.isValid() && db.isOpen() )
    {
      QString sql = loglineInsertTemplate.arg( tableName )
                        .arg( entr.diveNum )
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
    else
    {
      lg->warn( "SPX42Database::insertLogentry -> db is not valid or not opened." );
    }
    return ( false );
  }

  bool SPX42Database::insertLogentry( const QString &tableName, spSingleCommand cmd )
  {
    lg->debug( "SPX42Database::insertLogentry..." );
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( db );
      QString sql = loglineInsertTemplate.arg( tableName )
                        .arg( cmd->getDiveNum() )
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
    else
    {
      lg->warn( "SPX42Database::insertLogentry -> db is not valid or not opened." );
    }
    return ( false );
  }

  bool SPX42Database::existDiveLogInBase( const QString &tableName, int diveNum )
  {
    bool exist = false;
    //
    if ( db.isValid() && db.isOpen() )
    {
      QString sql = QString( "select count(*) from '%1' where divenum=%2" ).arg( tableName ).arg( diveNum );
      QSqlQuery query( sql, db );
      if ( query.next() )
      {
        if ( query.value( 0 ).toInt() > 0 )
        {
          exist = true;
        }
        lg->debug( QString( "SPX42Database::existDiveLogInBase -> <%1> sets == %2..." )
                       .arg( query.value( 0 ).toInt() )
                       .arg( exist ? "YES" : "NO" ) );
        return ( exist );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::existDiveLogInBase -> <%1>..." ).arg( err.text() ) );
      }
    }
    else
    {
      lg->warn( "SPX42Database::existDiveLogInBase -> db not open or not exist!" );
    }
    return ( exist );
  }

  bool SPX42Database::delDiveLogFromBase( const QString &tableName, int diveNum )
  {
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( QString( "delete from '%1' where divenum=%2" ).arg( tableName ).arg( diveNum ), db );
      if ( query.exec() )
      {
        return ( true );
      }
      else
      {
        QSqlError err = db.lastError();
        lg->warn( QString( "SPX42Database::delDiveLogFromBase -> <%1>..." ).arg( err.text() ) );
      }
    }
    else
    {
      lg->warn( "SPX42Database::delDiveLogFromBase -> db not open or not exist!" );
    }
    return ( false );
  }

  int SPX42Database::getMaxDepthFor( const QString &tableName, int diveNum )
  {
    int maxDepth = -1;
    lg->debug( "SPX42Database::getMaxDepthFor..." );
    if ( db.isValid() && db.isOpen() )
    {
      //
      // versuche mal rauszubekommen wie tief das war
      //
      QSqlQuery query( QString( "select max(depth) from '%1' where divenum=%2" ).arg( tableName ).arg( diveNum ), db );
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
    else
    {
      lg->warn( "SPX42Database::getMaxDepthFor -> db is not valid or not opened." );
      return ( -1 );
    }
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
}
