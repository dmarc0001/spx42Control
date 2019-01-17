#include "LogDetailWalker.hpp"

namespace spx
{
  LogDetailWalker::LogDetailWalker( QObject *parent,
                                    std::shared_ptr< Logger > logger,
                                    std::shared_ptr< SPX42Database > _database,
                                    std::shared_ptr< SPX42Config > spxCfg )
      : QObject( parent )
      , lg( logger )
      , database( _database )
      , spx42Config( spxCfg )
      , shouldWriterRunning( true )
      , processed( 0 )
      , forThisDiveProcessed( 0 )
      , overAll( 0 )
  {
  }

  void LogDetailWalker::addDetail( spSingleCommand _detail )
  {
    overAll++;
    detailQueue.enqueue( _detail );
  }

  void LogDetailWalker::reset()
  {
    shouldWriterRunning = false;
  }

  void LogDetailWalker::nowait( bool _shouldNoWait )
  {
    if ( _shouldNoWait )
      maxTimeoutVal = 0;
    else
      maxTimeoutVal = waitTimeout;
  }

  int LogDetailWalker::getProcessed()
  {
    return ( processed );
  }

  int LogDetailWalker::getGlobal()
  {
    return ( overAll );
  }

  int LogDetailWalker::getQueueLen()
  {
    return ( detailQueue.count() );
  }

  int LogDetailWalker::writeLogDataToDatabase( const QString &deviceMac )
  {
    int diveNum = -1;
    int detail_id = -1;
    shouldWriterRunning = true;
    forThisDiveProcessed = 0;
    processed = 0;
    qint64 timeoutVal = 0;
    maxTimeoutVal = waitTimeout;
    //
    // solange es was zu schreiben gibt
    //
    while ( shouldWriterRunning )
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
          if ( diveNum != -1 && detail_id != -1 )
          {
            //
            // also gab es einene Tauchgang, den ich gerade gesichert habe?
            // dann erzeuge maximaltiefe und anzahl der einträge
            // in der Tabellenspalte in detaildir
            //
            if ( !database->computeStatistic( detail_id ) )
            {
              lg->warn( "LogDetailWriter::writeLogDataToDatabase -> can't not compute statistic for dive..." );
            }
          }
          //
          // Ok, anpassen
          //
          emit onNewDiveDoneSig( diveNum );
          diveNum = logentry->getDiveNum();
          lg->debug(
              QString( "LogDetailWriter::writeLogDataToDatabase -> new dive to store #%1" ).arg( diveNum, 3, 10, QChar( '0' ) ) );
          //
          // Nummer hat sich verändert
          // ist das ein update oder ein insert?
          //
          if ( database->existDiveLogInBase( deviceMac, diveNum ) )
          {
            lg->debug( "LogDetailWriter::writeLogDataToDatabase -> update, drop old values..." );
            //
            // existiert, daten löschen...
            // also ein "update", eigentlich natürlich löschen und neu machen
            //
            if ( !database->delDiveLogFromBase( deviceMac, diveNum ) )
            {
              return ( -1 );
            }
          }
          //
          // neu anlegen, Tauchgangszeit suchen
          //
          qint64 timestamp = 0;
          SPX42LogDirectoryEntryListPtr entrys = spx42Config->getLogDirectory();
          for ( auto &entry : *( entrys.get() ) )
          {
            if ( entry.number == diveNum )
            {
              // die gesuchte Tauchgangsnummer
              timestamp = entry.diveDateTime.toSecsSinceEpoch();
              lg->debug( QString( "LogDetailWriter::writeLogDataToDatabase -> start time for dive #%1: %2" )
                             .arg( diveNum, 3, 10, QChar( '0' ) )
                             .arg( entry.getDateTimeStr() ) );
              // Schleife abbrechen
              break;
            }
          }
          //
          // Daten komplett, Tauchgang in der DB anlegen
          //
          if ( !database->insertDiveLogInBase( deviceMac, diveNum, timestamp ) )
          {
            return ( -1 );
          }
          //
          // die neue detail_id muss ich noch erfragen
          //
          detail_id = database->getDetailId( deviceMac, diveNum );
          if ( detail_id == -1 )
          {
            return ( -1 );
          }
          emit onNewDiveStartSig( diveNum );
        }
        // zähle die Datensätze
        processed++;
        lg->debug( QString( "LogDetailWriter::writeLogDataToDatabase -> write set %1" ).arg( processed, 3, 10, QChar( '0' ) ) );
        //
        // in die Datenbank schreiben
        //
        database->insertLogentry( detail_id, logentry );
      }
      else
      {
        if ( shouldWriterRunning )
        {
          timeoutVal++;
          if ( timeoutVal > maxTimeoutVal )
            shouldWriterRunning = false;
          else
            QThread::msleep( waitUnits );
        }
      }
    }
    if ( diveNum != -1 && detail_id != -1 )
    {
      //
      // also gab es einen Tauchgang, den ich gerade gesichert habe?
      // dann erzeuge maximaltiefe und anzahl der einträge
      // in der Tabellenspalte in detaildir
      //
      if ( !database->computeStatistic( detail_id ) )
      {
        lg->warn( "LogDetailWriter::writeLogDataToDatabase -> can't not compute statistic für dive..." );
      }
    }
    emit onWriteDoneSig( processed );
    return ( processed );
  }

  bool LogDetailWalker::deleteLogDataFromDatabase( const QString &deviceMac, std::shared_ptr< QVector< int > > list )
  {
    bool shouldDeleteRunning = true;
    //
    // zuerst: ist die Tabelle da/wurde angelegt?
    //
    for ( int diveNum : *list )
    {
      if ( shouldDeleteRunning )
      {
        if ( !database->delDiveLogFromBase( deviceMac, diveNum ) )
        {
          shouldDeleteRunning = false;
        }
        else
        {
          // gib Bescheid, der ist Geschichte...
          emit onDeleteDoneSig( diveNum );
        }
      }
    }
    QThread::msleep( 1200 );
    emit onDeleteDoneSig( -1 );
    return ( shouldDeleteRunning );
  }
}  // namespace spx
