#ifndef WINBTCONNECTTHREAD_HPP
#define WINBTCONNECTTHREAD_HPP

/**
  ACHTUNG: Reihenfolge der Includes ist WICHTIG!
*/
#include <winsock2.h>
#include <windows.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#include <stdio.h>

#include <QThread>
#include <QByteArray>

#include "../config/ProjectConst.hpp"
#include "../logging/Logger.hpp"

namespace spx
{

  class BTConnectThread : public QThread
  {
    private:
      Q_OBJECT
      //###########################################################################
      //#### Member Variable                                                   ####
      //###########################################################################
      Logger *lg;
      QByteArray deviceName;
    protected:
      QByteArray pin;
      volatile bool authError;
    private:
      SOCKET btSocket;
      SOCKADDR_BTH SockAddrBthServer;
      BLUETOOTH_DEVICE_INFO  btdi;
      HBLUETOOTH_AUTHENTICATION_REGISTRATION hRegHandle;

    public:
      BTConnectThread( QObject *parent, Logger *log, const QByteArray& dName, const QByteArray& pi, SOCKET bts);
      virtual ~BTConnectThread();
      void run();
      void cancel();

    private:
      static bool WINAPI BluetoothAuthCallback(LPVOID pvParam, PBLUETOOTH_AUTHENTICATION_CALLBACK_PARAMS pAuthCallbackParams );
      static int str2ba( const char *straddr, BTH_ADDR *btaddr );

    signals :
      void btConnectingSig( void );                               //! Signal, wenn eine Verbindung aufgebaut wird
      void btConnectedSig( const QByteArray& dName );             //! Signal, wenn eine Verbindung zustande gekommen ist
      void btDisconnectSig( void );                               //! Signal, wenn eine Verbindung beendet/unterbrochen wurde
      void btConnectErrorSig( int errnr );                        //! Signal, wenn es Fehler beim Verbinden gab
      void btPairingPinRequestSig( void );                        //! Signal, wenn Pairing gefordert wird, aber keine PIN da ist oder PIN falsch

    private slots:
      void isFinishedSlot();
  };
}
#endif // WINBTCONNECTTHREAD_HPP
