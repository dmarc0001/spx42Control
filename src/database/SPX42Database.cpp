#include "SPX42Database.hpp"

namespace spx
{
  const QString SPX42Database::sqlDriver{"QSQLITE"};

  SPX42Database::SPX42Database( std::shared_ptr< Logger > logger, const QString databaseName, QObject *parent )
      : QObject( parent ), lg( logger ), dbName( databaseName )
  {
    // dbName default: spx42Database.db
    lg->debug( "SPX42Database::SPX42Database..." );
  }

  SPX42Database::~SPX42Database()
  {
    lg->debug( "SPX42Database::~SPX42Database..." );
    if ( db.isValid() && db.isOpen() )
      closeDatabase();
  }

  QSqlError SPX42Database::openDatabase( void )
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
    lg->debug( "SPX42Database::openDatabase: add db connection" );
    db = QSqlDatabase::addDatabase( SPX42Database::sqlDriver, currentConnectionName );
    lg->debug( QString( "SPX42Database::openDatabase: add db name: " ).append( dbName ) );
    db.setDatabaseName( dbName );
    //
    // öffnen
    //
    if ( !db.open() )
    {
      err = db.lastError();
      db = QSqlDatabase();
      QSqlDatabase::removeDatabase( currentConnectionName );
      lg->warn( QString( "an error while open the database: " ).append( err.text() ) );
    }
    else
    {
      if ( db.isOpen() )
        lg->debug( "SPX42Database::openDatabase: OK" );
      else
        lg->warn( "SPX42Database::openDatabase: NOT OK" );
    }
    return ( err );
  }

  QSqlError SPX42Database::openDatabase( const QString databaseName )
  {
    dbName = databaseName;
    return ( openDatabase() );
  }

  void SPX42Database::closeDatabase( void )
  {
    lg->debug( "SPX42Database::closeDatabase..." );
    QSqlDatabase::database( currentConnectionName, false ).close();
    QSqlDatabase::removeDatabase( currentConnectionName );
    currentConnectionName.clear();
    db = QSqlDatabase();
  }

  DeviceAliasHash SPX42Database::getDeviceAliasHash( void )
  {
    lg->debug( "SPX42Database::getDeviceAliasHash..." );
    DeviceAliasHash aliase;
    if ( db.isValid() && db.isOpen() )
    {
      QSqlQuery query( "select mac,name,alias from devAliase", db );
      while ( query.next() )
      {
        QString mac( query.value( 0 ).toString() );
        QPair< QString, QString > nPair( query.value( 1 ).toString(), query.value( 2 ).toString() );
        aliase.insert( mac, nPair );
      }
      lg->debug( QString( "SPX42Database::getDeviceAliasHash: <%1> items..." ).arg( aliase.count() ) );
    }
    return ( aliase );
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
