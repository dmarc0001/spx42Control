#include "SPX42Database.hpp"

namespace spx
{
  const QString SPX42Database::versionTableName{"version"};
  const QString SPX42Database::deviceTableName{"devices"};

  const QString SPX42Database::sqlDriver{"QSQLITE"};
  const qint16 SPX42Database::databaseVersion{1};

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
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( QString( "select mac,name,alias from %1" ).arg( SPX42Database::deviceTableName ), db );
      while ( query.next() )
      {
        QString mac( query.value( 0 ).toString() );
        QPair< QString, QString > nPair( query.value( 1 ).toString(), query.value( 2 ).toString() );
        aliase.insert( mac, nPair );
      }
      lg->debug( QString( "SPX42Database::getDeviceAliasHash -> <%1> items..." ).arg( aliase.count() ) );
    }
    else
    {
      lg->warn( "SPX42Database::getDeviceAliasHash -> database not valid or not opened." );
    }
    return ( aliase );
  }

  bool SPX42Database::addAlias( const QString &mac, const QString &name, const QString &alias )
  {
    QSqlQuery query( QString( "select mac from %1" ).arg( SPX42Database::deviceTableName ), db );
    QSqlError err;
    QString sql;
    //
    lg->debug( "SPX42Database::addAlias..." );
    if ( db.isValid() && db.isOpen() )
    {
      if ( !query.exec() )
      {
        err = db.lastError();
        lg->warn( QString( "SPX42Database::addAlias -> problem while select from database: <%1>" ).arg( err.text() ) );
        return ( false );
      }
      if ( query.value( 0 ).isNull() )
      {
        //
        // MAC ist noch nicht in der Datenbank, INSERT
        //
        sql = QString( "insert into %1 (mac, name, alias) values ('%2', '%3', '%4')" )
                  .arg( SPX42Database::deviceTableName )
                  .arg( mac )
                  .arg( name )
                  .arg( alias );
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
      sql = QString( "update %1 set name='%2', alias='%3' where mac='%4'" )
                .arg( SPX42Database::deviceTableName )
                .arg( name )
                .arg( alias )
                .arg( mac );
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
          QString( "update %1 set alias='%2' where mac='%3'" ).arg( SPX42Database::deviceTableName ).arg( alias ).arg( mac ), db );
      if ( query.next() )
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
          QString( "update %1 set alias='%2' where name='%3'" ).arg( SPX42Database::deviceTableName ).arg( alias ).arg( name ), db );
      if ( query.next() )
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
    QSqlQuery query( QString( "SELECT name FROM sqlite_master WHERE type = 'table' AND name = '%1'" ).arg( tableName ), db );
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

  /**
   * @brief SPX42Database::getDatabaseVersion
   * @return
   */
  qint16 SPX42Database::getDatabaseVersion()
  {
    qint16 version = -1;
    //
    QSqlQuery query( QString( "select max(version) from %1" ).arg( SPX42Database::versionTableName ), db );
    if ( query.next() )
    {
      version = static_cast< qint16 >( query.value( 0 ).toInt() );
    }
    lg->debug( QString( "SPX42Database::getDatabaseVersion -> <%1>..." ).arg( version ) );
    return ( version );
  }

  /**
   * @brief SPX42Database::createVersionTable
   * @return
   */
  bool SPX42Database::createVersionTable()
  {
    lg->debug( "SPX42Database::createVersionTable..." );
    QSqlQuery query( QString( "drop table if exist %1" ).arg( SPX42Database::versionTableName ), db );
    query.exec();
    if ( !query.exec( QString( "create table %1 ( version INTEGER )" ).arg( SPX42Database::versionTableName ) ) )
    {
      QSqlError err = db.lastError();
      lg->crit( QString( "SPX42Database::createVersionTable -> %1" ).arg( err.text() ) );
      return ( false );
    }
    lg->debug( "SPX42Database::createVersionTable -> insert version number..." );
    if ( query.exec( QString( "insert into %1 (version) values (%2)" )
                         .arg( SPX42Database::versionTableName )
                         .arg( SPX42Database::databaseVersion ) ) )
    {
      lg->debug( "SPX42Database::createVersionTable -> insert version number: OK" );
      return ( true );
    }
    QSqlError err = db.lastError();
    lg->crit( QString( "SPX42Database::createVersionTable -> %1" ).arg( err.text() ) );
    return ( false );
  }

  /**
   * @brief SPX42Database::checkOrCreateTables
   * @return
   */
  bool SPX42Database::checkOrCreateTables()
  {
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
    qint16 currentVersionNumber = getDatabaseVersion();
    if ( currentVersionNumber == -1 )
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
    //
    // zu guter letzt:
    //
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
    QSqlQuery query( QString( "drop table if exist %1" ).arg( SPX42Database::deviceTableName ), db );
    //
    // das Statement zusammenschrauben
    //
    sql = "create table " + SPX42Database::deviceTableName + "\n(\n";
    sql += " mac TEXT not NULL,\n";
    sql += " name TEXT not NULL,\n ";
    sql += " alias TEXT default NULL\n";
    sql += ")";
    if ( query.exec( sql ) )
    {
      return ( true );
    }
    QSqlError err = db.lastError();
    lg->crit( QString( "SPX42Database::createAliasTable -> %1" ).arg( err.text() ) );
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
}
