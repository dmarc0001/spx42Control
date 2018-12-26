#include "LogDetailWriter.hpp"

namespace spx
{
  LogDetailWriter::LogDetailWriter( QObject *parent, std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Database > _database )
      : QObject( parent ), lg( logger ), database( _database ), shouldRunning( true ), processed( 0 ), overAll( 0 )
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

  void LogDetailWriter::clear()
  {
    detailQueue.clear();
    processed = 0;
    overAll = 0;
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

  int LogDetailWriter::writeLogDataToDatabase( const QString &deviceMac, int diveNum )
  {
    shouldRunning = true;
    processed = 0;
    qint64 timeoutVal = 0;
    qint64 maxTimeoutVal = waitFirstTimeout;
    //
    // zuerst: ist die Tabelle da/wurde angelegt?
    //
    QString tableName = database->getLogTableName( deviceMac );
    if ( tableName.isNull() || tableName.isEmpty() )
    {
      lg->crit( "LogDetailWriter::writeLogDataToDatabase -> database error, not table für logdata exist..." );
      return ( -1 );
    }
    //
    // ist das ein update oder ein insert?
    //
    if ( database->existDiveLogInBase( tableName, diveNum ) )
    {
      //
      //  existiert, daten löschen...
      // also ein "update", eigentlich natürlich löschen und neu machen
      //
      if ( !database->delDiveLogFromBase( tableName, diveNum ) )
      {
        return ( -1 );
      }
    }
    //
    while ( shouldRunning )
    {
      if ( detailQueue.count() > 0 )
      {
        //
        // so, ab jetzt ist das der "kurze" Timeout
        //
        maxTimeoutVal = timeoutVal;
        timeoutVal = 0;
        //
        // den Datensatz aus der Queue holen
        // (ist ja ein shared ptr, also sehr schnell)
        //
        auto logentry = detailQueue.dequeue();
        if ( logentry->getCommand() == SPX42CommandDef::SPX_GET_LOG_NUMBER_SE )
        {
          //
          // Zur Sicherheut könnte man noch testen ob
          // recCommand->getValueFromHexAt( SPXCmdParam::LOGDETAIL_START_END ) == 0
          // das ist der ENDE Marker für die Logdaten, gesendet von LogFragment::onCommandRecivedSlot
          // verlässt unmittelbar die while Schleife
          // und beendet den Thread
          //
          break;
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
          QThread::msleep( waitUnits );
          timeoutVal++;
          if ( timeoutVal > maxTimeoutVal )
            shouldRunning = false;
        }
      }
    }
    emit onWriteDoneSig( processed );
    return ( processed );
  }
}
