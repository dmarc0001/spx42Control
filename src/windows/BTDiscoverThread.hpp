#ifndef WINDISCOVERTHREAD_HPP
#define WINDISCOVERTHREAD_HPP

#include <winsock2.h>
#include <windows.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#include <stdio.h>

#include <QThread>
#include "../config/ProjectConst.hpp"
#include "../logging/Logger.hpp"
#include "../bluetooth/BtDeviceDescriber.hpp"

namespace spx
{
  class BTDiscoverThread: public QThread
  {
    private:
    Q_OBJECT
      // Member Variable
      Logger *lg;
      BluetoothDeviceDescriber *currDevice;
      PWSAQUERYSETW pWSAquerySet;
      PWSAQUERYSETW pWSAquerySetDevice;
      PWSAQUERYSETW pWSAquerySetResult;
      PWSAQUERYSETW pWSAquerySetResult2;
      HANDLE hDefaultProcessHeap;
      static const ulong ulDeviceSearchFlags;                   //! Flags zum Suchen von Devices
      static const ulong ulServiceBeginFlags;                   //! Flags zum Suchen von Services
      static const ulong ulServiceNextFlags;                    //! Flags für next Service

      // Member Funktionen
    public:
      BTDiscoverThread( QObject *parent, Logger *logger );
      virtual ~BTDiscoverThread();

    protected:
      void run() Q_DECL_OVERRIDE;

    private:

    private:
      bool allocHeap( void );
      void freeHeap( void );
      bool searchServices( BluetoothDeviceDescriber *currDevice );

    signals:
      void btDiscoveringSig(void);
      //* Signal wird ausgelöst, wenn das Discovering beendet ist
      void newDeviceFoundSig( BluetoothDeviceDescriber *device );
      void btDiscoverEndSig( void );
      void btConnectErrorSig( int errnr );                        //! Signal, wenn es Fehler beim Verbinden gab
  };
}
#endif // WINDISCOVERTHREAD_HPP
