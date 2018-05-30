#ifndef WINDISCONNECTTHREAD_HPP
#define WINDISCONNECTTHREAD_HPP

/**
  ACHTUNG: Reihenfolge der Includes ist WICHTIG!
*/
#include <winsock2.h>
#include <windows.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>

#include <QThread>

#include "../config/ProjectConst.hpp"
#include "../logging/Logger.hpp"

namespace spx
{

  class BTDisconnectThread : public QThread
  {
    private:
      Q_OBJECT
      //###########################################################################
      //#### Member Variable                                                   ####
      //###########################################################################
      Logger *lg;
      SOCKET btSocket;

    public:
      BTDisconnectThread( QObject *parent, Logger *log, SOCKET bts);
      ~BTDisconnectThread(void);
      void run();

    signals:
      void btDisconnectSig( void );                               //! Signal, wenn eine Verbindung beendet/unterbrochen wurde
      void btConnectErrorSig( int errnr );                        //! Signal, wenn es Fehler beim Verbinden gab
  };
}
#endif // WINDISCONNECTTHREAD_HPP
