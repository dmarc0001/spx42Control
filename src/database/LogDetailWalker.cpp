#include "LogDetailWalker.hpp"

namespace spx
{
  LogDetailWalker::LogDetailWalker( QObject *parent,
                                    std::shared_ptr< Logger > logger,
                                    std::shared_ptr< SPX42Database > _database,
                                    std::shared_ptr< SPX42Config > spxCfg,
                                    const QString &devMac )
      : QThread( parent )
      , lg( logger )
      , database( _database )
      , spx42Config( spxCfg )
      , deviceMac( devMac )
      , threadShouldRun( true )
      , processed( 0 )
  {
  }

  void LogDetailWalker::run()
  {
    threadShouldRun = true;
    *lg << LINFO << "LogDetailWriter::writeLogDataToDatabase -> thread started" << Qt::endl;
    //
    // Vorprüfungen
    //
    if ( deviceMac.isNull() || deviceMac.isEmpty() )
    {
      //
      // Thread beenden, keine Mac-Addr
      //
      *lg << LCRIT << "LogDetailWalker::run -> no device addr given, thread will stop!" << Qt::endl;
      return;
    }
    //
    // Für immer, bis es reicht...
    //
    while ( threadShouldRun )
    {
      // ######################################################################
      // Hauptschleife, solange die Queue noch Einträge hat
      // ######################################################################
      while ( threadShouldRun && !logDetailQueue.empty() )
      {
        //
        // ein neuer Tauchgangslog
        //
        queueMutex.lock();
        auto divelogSet = logDetailQueue.dequeue();
        queueMutex.unlock();
        //
        // den ersten Eintrag untersuchen
        //
        auto firstEntry = divelogSet.first();
        int diveNum = firstEntry->getDiveNum();
        int detail_id = 0;
        //
        // gibt es den schon (update oder insert)
        //
        if ( database->existDiveLogInBase( deviceMac, diveNum ) )
        {
          *lg << LDEBUG << "LogDetailWalker::run -> update, drop old values..." << Qt::endl;
          //
          // existiert, daten löschen...
          // also ein "update", eigentlich natürlich löschen und neu machen
          //
          if ( !database->delDiveLogFromBase( deviceMac, diveNum ) )
          {
            emit onWriteCritSig( LOGWRITEERR::LOGWR_ERR_CANT_DELETE_DIVE );
            //
            // nächsten Tauchgang versuchen
            //
            *lg << LCRIT << "LogDetailWalker::run -> can't delete data for dive <" << diveNum << ">from database..." << Qt::endl;
            continue;
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
            *lg << LDEBUG
                << QString( "LogDetailWalker::run -> start time for dive #%1: %2" )
                       .arg( diveNum, 3, 10, QChar( '0' ) )
                       .arg( entry.getDateTimeStr() )
                << Qt::endl;
            // Schleife abbrechen, wenn ich die nummer gefunden habe
            break;
          }
        }
        //
        // Daten komplett, Tauchgang in der DB anlegen
        // falls ich keinen Zeitstempel gefunden habe ist das dann 0
        //
        if ( !database->insertDiveLogInBase( deviceMac, diveNum, timestamp ) )
        {
          //
          // Da ging dann was schief
          //
          emit onWriteCritSig( LOGWRITEERR::LOGWR_ERR_CANT_INSERT_DIVE );
          *lg << LCRIT << "LogDetailWalker::run -> can't insert new dataset for dive <" << diveNum << ">from database..." << Qt::endl;
          continue;
        }
        //
        // die neue detail_id muss ich noch erfragen
        //
        detail_id = database->getDetailId( deviceMac, diveNum );
        if ( detail_id == -1 )
        {
          emit onWriteCritSig( LOGWRITEERR::LOGWR_ERR_CANT_READ_NEW_DIVEID );
          *lg << LCRIT << "LogDetailWalker::run -> can't insert new dataset for dive <" << diveNum << ">from database..." << Qt::endl;
          continue;
        }
        //
        // jetzt den Tauchgang schreiben
        //
        writeOneDataset( divelogSet, diveNum, detail_id );
      }  // while ( threadShouldRun && !logDetailQueue.empty() )
      //
      // sollte eigentlich nur einmal gerufen werden, da der Thread dann schlafen geht
      //
      emit onWriteFinishedSig( processed );
      //
      // jetzt ist entweder die Queue leer oder der Thread soll enden
      //
      if ( threadShouldRun )
      {
        //
        // der Thread soll noch laufen aber es ist nichts zu tun
        // darum lege den Thread vis zum wake oder wakeAll schlafen
        //
        conditionMutex.lock();
        *lg << LDEBUG << "LogDetailWalker::run -> thread is sleeping..." << Qt::endl;
        isSleeping = true;
        queueCondiotion.wait( &conditionMutex );
        isSleeping = false;
        *lg << LDEBUG << "LogDetailWalker::run -> thread is waked up..." << Qt::endl;
        conditionMutex.unlock();
      }
    }  // while ( threadShouldRun )
    *lg << LDEBUG << "LogDetailWalker::run -> thread will end..." << Qt::endl;
  }

  bool LogDetailWalker::writeOneDataset( LogDetailSetQueue divelogSet, int diveNum, int dive_id )
  {
    int _processed = 0;
    //
    // Schleife solange in diesem Log Details sind
    //
    onWriteDiveStartSig( diveNum );
    while ( threadShouldRun && !divelogSet.empty() )
    {
      //
      // ein Detail aus dem Log holen
      //
      auto logentry = divelogSet.dequeue();
      ++processed;
      ++_processed;
      *lg << LDEBUG
          << QString( "LogDetailWalker::writeOneDataset -> write set %1 dive number %2, over all processed: %3" )
                 .arg( _processed, 3, 10, QChar( '0' ) )
                 .arg( diveNum, 3, 10, QChar( '0' ) )
                 .arg( processed, 5, 10, QChar( '0' ) )
          << Qt::endl;
      //
      // in die Datenbank schreiben
      // (wird via mutex serialisiert mit anderen threads)
      //
      if ( !database->insertLogentry( dive_id, logentry ) )
      {
        emit onWriteCritSig( LOGWRITEERR::LOGWR_ERR_CANT_INSERT_LOGDETAIL );
        *lg << LCRIT << "LogDetailWalker::writeOneDataset -> can't insert new detail dataset for dive <" << diveNum
            << ">from database..." << Qt::endl;
        if ( !database->delDiveLogFromBase( deviceMac, diveNum ) )
        {
          emit onWriteCritSig( LOGWRITEERR::LOGWR_ERR_CANT_DELETE_DIVE );
          //
          // nächsten Tauchgang versuchen
          //
          *lg << LCRIT << "LogDetailWalker::writeOneDataset -> can't delete data for dive <" << diveNum << ">from database..."
              << Qt::endl;
        }
        return false;
      }
      //
      // OKAY, Datensatz gesichert
      //
    }  // while ( threadShouldRun && !divelogSet.empty() )
    //
    // hier sollten alle Datensätze gesichert sein
    // nun Statistik berechnen
    //
    if ( !database->computeStatistic( dive_id ) )
    {
      emit onWriteCritSig( LOGWRITEERR::LOGWR_ERR_CANT_INSERT_LOGDETAIL );
      *lg << LWARN << "LogDetailWalker::writeOneDataset -> can't not compute statistic for dive <" << diveNum << ">..." << Qt::endl;
      if ( !database->delDiveLogFromBase( deviceMac, diveNum ) )
      {
        emit onWriteCritSig( LOGWRITEERR::LOGWR_ERR_CANT_DELETE_DIVE );
        //
        // nächsten Tauchgang versuchen
        //
        *lg << LCRIT << "LogDetailWalker::writeOneDataset -> can't delete data for dive <" << diveNum << ">from database..."
            << Qt::endl;
      }
      return false;
    }
    emit onWriteDiveDoneSig( _processed );
    return true;
  }

  bool LogDetailWalker::setThreadEnd( bool _shouldEnd )
  {
    if ( threadShouldRun == _shouldEnd )
    {
      threadShouldRun = false;
      //
      // Thread aufwecken, wenn er schläft
      //
      queueCondiotion.wakeAll();
      return true;
    }
    return false;
  }

  void LogDetailWalker::setDeviceName( const QString &devMac )
  {
    deviceMac = devMac;
  }

  int LogDetailWalker::addLogQueue( LogDetailSetQueue logSet )
  {
    queueMutex.lock();
    logDetailQueue.append( logSet );
    queueMutex.unlock();

    //
    // Thread aufwecken, wenn er schläft
    //
    queueCondiotion.wakeAll();
    return logSet.size();
  }

  bool LogDetailWalker::isThreadSleeping()
  {
    return isSleeping;
  }

}  // namespace spx
