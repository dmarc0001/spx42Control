#ifndef MAINOBJECT_HPP
#define MAINOBJECT_HPP

#include <QBluetoothDeviceInfo>
#include <QObject>
#include <QVector>
#include <QtCore/QCoreApplication>
#include <bluetooth/BtLocalDevice.hpp>
#include <bluetooth/BtService.hpp>
#include <logging/Logger.hpp>

class MainObject : public QObject
{
  Q_OBJECT
  private:
  std::shared_ptr< Logger > lg;
  bool isRunnning;
  QCoreApplication *ca;
  QVector< QBluetoothDeviceInfo > btDevices;
  BtService *btServiceDiscover;
  QVector< QBluetoothDeviceInfo >::iterator itDevice;

  public:
  explicit MainObject( int argc, char *argv[] );
  ~MainObject();
  int exec( void );
  int searchServices( void );

  signals:

  private slots:
  void slotDeviceDiscovered( const QBluetoothDeviceInfo &info );
  void slotDeviceDiscoverFinished( void );
  //
  void slotServiceDiscoverStarted( void );
  void slotServiceDiscoverCanceled( void );
  void slotServiceDiscoverError( QBluetoothServiceDiscoveryAgent::Error error );
  void slotServiceDiscovered( const QBluetoothServiceInfo &info );
  void slotServiceDiscoverFinished( void );
};

#endif  // MAINOBJECT_HPP
