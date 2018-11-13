#ifndef BTTYPES_HPP
#define BTTYPES_HPP

#include <QBluetoothAddress>
#include <QHash>
#include <QPair>
#include <QQueue>
#include <QString>

namespace spx
{
  //
  // ein paar Typendefinitionen, um die Sache lesbar zu machen
  //
  using SPXDeviceDescr = QPair< QString, QString >;           //! device MAC, device Name
  using SPXDeviceList = QHash< QString, SPXDeviceDescr >;     //! device Addr, device Info, Service Info
  using ToScannedDevicesQueue = QQueue< QBluetoothAddress >;  //! Queue zum scannen der Devices nach Services
}
#endif  // BTTYPES_HPP
