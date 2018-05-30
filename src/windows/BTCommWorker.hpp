#ifndef WINCOMMWORKER_HPP
#define WINCOMMWORKER_HPP

#include <winsock2.h>
#include <windows.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#include <stdio.h>

#include <QThread>
#include <QByteArray>
#include <QMutex>

#include "../config/ProjectConst.hpp"
#include "../logging/Logger.hpp"

namespace spx
{
  class BTCommWorker : public QThread
  {
    private:
      Q_OBJECT
      Logger *lg;
      SOCKET btSocket;
      volatile bool isWorkerRunning;
      QMutex mutex;
      char recBuff[1200];

    public:
      explicit BTCommWorker( QObject *parent, Logger *log, SOCKET bts);
      virtual void run(void);
      int write( const char *str, int len );                      //! schreibe zum Gerät (wenn verbunden)
      int write( const QByteArray& cmd );                         //! schreibe zum Gerät (wenn verbunden)
      void cancel(void);                                          //! brich den Thread ab

    signals:
      void btDisconnectSig( void );                               //! Signal, wenn eine Verbindung beendet/unterbrochen wurde
      void btConnectErrorSig( int errnr );                        //! Signal, wenn es Fehler beim Verbinden gab
      void btDataRecivedSig( const char * data, int len );        //! Signal, wenn Daten unstrukturiert kamen
  };
}
#endif // WINCOMMWORKER_HPP
