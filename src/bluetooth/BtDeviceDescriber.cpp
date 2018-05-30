
//#include <QBluetoothDeviceInfo>
#include "BtDeviceDescriber.hpp"
namespace spx
{
  BluetoothDeviceDescriber::BluetoothDeviceDescriber( void ) :
    pin("0000")
  {

  }

  BluetoothDeviceDescriber::BluetoothDeviceDescriber(const QString& addr ) :
    pin("0000")
  {
    this->addr = addr;
    this->addr.remove( QString( "(" ) );
    this->addr.remove( QString( ")" ) );
  }

  /**
   * Kopierkonstruktor
   */
  BluetoothDeviceDescriber::BluetoothDeviceDescriber( const BluetoothDeviceDescriber& btd )
  {
    devName = btd.devName;
    addr = btd.addr;
    serviceNames = btd.serviceNames;
    alias = btd.alias;
    pin = btd.pin;
  }

  BluetoothDeviceDescriber::~BluetoothDeviceDescriber( void )
  {
  }

  void BluetoothDeviceDescriber::clear(void)
  {
    devName.clear();
    addr.clear();
    serviceNames.clear();
    alias.clear();
    pin = "0000";
  }

  bool BluetoothDeviceDescriber::istDefined( void )
  {
    if( addr.isEmpty() )
    {
      return( false );
    }
    if( devName.isEmpty() )
    {
      return( false );
    }
    if( serviceNames.isEmpty() )
    {
      return( false );
    }
    return( true );
  }

  const QString& BluetoothDeviceDescriber::getAddr( void )
  {
    return( addr );
  }

  void BluetoothDeviceDescriber::setAddr( const QString& addr )
  {
    this->addr = addr;
    this->addr.remove( QString( "(" ) );
    this->addr.remove( QString( ")" ) );
  }

  const QVector<QString>& BluetoothDeviceDescriber::getServiceNames( void )
  {
    return( serviceNames );
  }

  void BluetoothDeviceDescriber::addServiceName( const QString& sName )
  {
    this->serviceNames.append( QString( sName ) );
  }

  void BluetoothDeviceDescriber::clearServices( void )
  {
    serviceNames.clear();
  }

  const QString& BluetoothDeviceDescriber::getDevName( void )
  {
    return( devName );
  }

  void BluetoothDeviceDescriber::setDevName( const QString& devName )
  {
    this->devName = devName;
  }

  const QString& BluetoothDeviceDescriber::getAlias( void )
  {
    return( alias );
  }

  void BluetoothDeviceDescriber::setAlias(const QString &alias )
  {
    this->alias = alias;
  }

  const QString& BluetoothDeviceDescriber::getPin(void)
  {
    return pin;
  }

  void BluetoothDeviceDescriber::setPin(const QString& value)
  {
    pin = value;
  }

}

