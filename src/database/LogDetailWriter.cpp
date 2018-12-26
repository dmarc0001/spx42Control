#include "LogDetailWriter.hpp"

namespace spx
{
  LogDetailWriter::LogDetailWriter( QObject *parent, std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Database > _database )
      : QObject( parent )
      , lg( logger )
      , database( _database )
      , shouldRunning( true )
      , processed( 0 )
      , forThisDiveProcessed( 0 )
      , overAll( 0 )
  {
  }

  void LogDetailWriter::addDetail( spSingleCommand _detail )
  {
    overAll++;
    detailQueue.enqueue( _detail );
  }

  void LogDetailWriter::reset()
  {
    shouldRunning = false;
  }

  void LogDetailWriter::nowait( bool _shouldNoWait )
  {
    if ( _shouldNoWait )
      maxTimeoutVal = 0;
    else
      maxTimeoutVal = waitTimeout;
  }

  int LogDetailWriter::getProcessed()
  {
    return ( processed );
  }

  int LogDetailWriter::getGlobal()
  {
    return ( overAll );
  }

  int LogDetailWriter::getQueueLen()
  {
    return ( detailQueue.count() );
  }

  int LogDetailWriter::writeLogDataToDatabase( const QString &deviceMac )
  {
    int diveNum = -1;
    shouldRunning = true;
    forThisDiveProcessed = 0;
    processed = 0;
    qint64 timeoutVal = 0;
    maxTimeoutVal = waitTimeout;
    //
    // zuerst: ist die Tabelle da/wurde angelegt?
    //
    QString tableName = database->getLogTableName( deviceMac );
    if ( tableName.isNull() || tableName.isEmpty() )
    {
      lg->crit( "LogDetailWriter::writeLogDataToDatabase -> database error, not table für logdata exist..." );
      return ( -1 );
    }
    lg->debug( QString( "LogDetailWriter::writeLogDataToDatabase -> table %1 exist!" ).arg( tableName ) );
    //
    while ( shouldRunning )
    {
      if ( detailQueue.count() > 0 )
      {
        timeoutVal = 0;
        //
        // den Datensatz aus der Queue holen
        // (ist ja ein shared ptr, also sehr schnell)
        //
        auto logentry = detailQueue.dequeue();
        //
        // ist die diveNum immer noch dieselbe?
        //
        if ( diveNum != logentry->getDiveNum() )
        {
          //
          // Ok, anpassen
          //
          diveNum = logentry->getDiveNum();
          lg->debug(
              QString( "LogDetailWriter::writeLogDataToDatabase -> new dive to store %1" ).arg( diveNum, 3, 10, QChar( '0' ) ) );
          //
          // Nummer hat sich verändert
          // ist das ein update oder ein insert?
          //
          if ( database->existDiveLogInBase( tableName, diveNum ) )
          {
            lg->debug( "LogDetailWriter::writeLogDataToDatabase -> update, drop old values..." );
            //
            //  existiert, daten löschen...
            // also ein "update", eigentlich natürlich löschen und neu machen
            //
            if ( !database->delDiveLogFromBase( tableName, diveNum ) )
            {
              return ( -1 );
            }
          }
        }
        // zähle die Datensätze
        processed++;
        lg->debug( QString( "LogDetailWriter::writeLogDataToDatabase -> write set %1" ).arg( processed, 3, 10, QChar( '0' ) ) );
        // in die Warteschlange des writer Threads schicken
        database->insertLogentry( tableName, logentry );
      }
      else
      {
        if ( shouldRunning )
        {
          timeoutVal++;
          if ( timeoutVal > maxTimeoutVal )
            shouldRunning = false;
          else
            QThread::msleep( waitUnits );
        }
      }
    }
    emit onWriteDoneSig( processed );
    return ( processed );
  }
}
