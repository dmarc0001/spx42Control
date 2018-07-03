﻿#include "ConsoleMainObject.hpp"
#include <QTimer>

namespace spx
{
  ConsoleMainObject::ConsoleMainObject( QObject *parent ) : QObject( parent ), myTimer( this )
  {
    lg = std::shared_ptr< Logger >( new Logger() );
    lg->startLogging( LG_DEBUG, "btsecond_console.log" );
    lg->debug( "ConsoleMainObject::ConsoleMainObject..." );
  }

  ConsoleMainObject::~ConsoleMainObject()
  {
    lg->debug( "ConsoleMainObject::~ConsoleMainObject..." );
  }

  void ConsoleMainObject::init( void )
  {
    lg->debug( "ConsoleMainObject::execute: start execute, init any things..." );
    //
    // ein SPX-Geräteobjekt anlegen (könnte auch im Konstruktor passieren
    //
    lg->debug( "ConsoleMainObject::execute: create local SPX42Devices..." );
    // das Objekt wird beim Destrutor standartmäßig mit entsorgt
    btDevices = std::unique_ptr< SPX42BtDevices >( new SPX42BtDevices( lg, this ) );
    //
    // die Signale des Geräteobjektes mit lokalen slots verbinden...
    //
    lg->debug( "ConsoleMainObject::execute: connect signals and slots..." );
    connect( btDevices.get(), &SPX42BtDevices::sigDeviceHostModeStateChanged, this,
             &ConsoleMainObject::slotDeviceHostModeStateChanged );
    connect( btDevices.get(), &SPX42BtDevices::sigDevicePairingDone, this, &ConsoleMainObject::slotDevicePairingDone );
    connect( btDevices.get(), &SPX42BtDevices::sigDiscoverScanFinished, this, &ConsoleMainObject::slotDiscoverScanFinished );
    connect( btDevices.get(), &SPX42BtDevices::sigDiscoveredDevice, this, &ConsoleMainObject::slotDiscoveredDevice );
    connect( btDevices.get(), &SPX42BtDevices::sigAllScansFinished, this, &ConsoleMainObject::slotAllScansFinished );
    //
    // start discovering
    //
    btDevices->startDiscover();

    lg->debug( "ConsoleMainObject::execute: DEBUGGING: start kill timer with 120 s..." );
    QTimer::singleShot( 120000, this, [=] {
      qInfo() << "timer TIMEOUT";
      emit this->sigQuit();
    } );

    //
    // DEBUG:events erzeugen um zu sehen ob alles läuft
    //
    myTimer.setInterval( 300 );
    connect( &myTimer, &QTimer::timeout, this, [=] { qDebug() << "TIMER tick..."; } );
    myTimer.start();
  }

  void ConsoleMainObject::slotDiscoveredDevice( const QBluetoothDeviceInfo &info )
  {
    lg->debug( QString( "ConsoleMainObject::slotDiscoveredDevice: %1" ).arg( info.address().toString() ) );
  }

  void ConsoleMainObject::slotDevicePairingDone( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing )
  {
    lg->debug( QString( "SPX42BtDevices::slotDevicePairingDone: device: %1" ).arg( address.toString() ) );
  }

  void ConsoleMainObject::slotDiscoverScanFinished( void )
  {
    lg->debug( QString( "ConsoleMainObject::slotDiscoverScanFinished: found %1 devices ..." ).arg( btDevices->getDevices().count() ) );
  }

  void ConsoleMainObject::slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode hostMode )
  {
    lg->debug( QString( "ConsoleMainObject::slotDeviceHostModeStateChanged to %1" ).arg( hostMode ) );
  }

  void ConsoleMainObject::slotDiscoveryServicesFinished( const QString name )
  {
    lg->debug( QString( "ConsoleMainObject::slotDiscoveryServicesFinished: %1..." ).arg( name ) );
  }

  void ConsoleMainObject::slotDiscoveredService( const QString &name, const QBluetoothServiceInfo &info )
  {
    lg->debug( QString( "ConsoleMainObject::slotDiscoveredService: %1..." ).arg( info.device().name() ) );
  }

  void ConsoleMainObject::slotAllScansFinished( void )
  {
    lg->info( "ConsoleMainObject::slotAllScansFinished..." );
    emit sigQuit();
  }

}  // namespace spx
