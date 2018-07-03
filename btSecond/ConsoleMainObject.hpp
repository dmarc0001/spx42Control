﻿#ifndef CONSOLEMAINOBJECT_HPP
#define CONSOLEMAINOBJECT_HPP

#include <QCoreApplication>
#include <QObject>
#include <QTimer>
#include "bluetooth/SPX42BtDevices.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  class ConsoleMainObject : public QObject
  {
    Q_OBJECT
    private:
    std::shared_ptr< Logger > lg;
    std::unique_ptr< SPX42BtDevices > btDevices;
    QTimer myTimer;

    public:
    explicit ConsoleMainObject( QObject *parent = nullptr );
    ~ConsoleMainObject();
    void init( void );

    signals:
    void sigQuit( void );

    private slots:
    void slotDiscoveredDevice( const QBluetoothDeviceInfo &info );
    void slotDevicePairingDone( const QBluetoothAddress &remoteAddr, QBluetoothLocalDevice::Pairing pairing );
    void slotDiscoverScanFinished( void );
    void slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode hostMode );
    void slotDiscoveryServicesFinished( const QString name );
    void slotDiscoveredService( const QString &name, const QBluetoothServiceInfo &info );
    void slotAllScansFinished( void );

    public slots:
  };
}
#endif  // CONSOLEMAINOBJECT_HPP