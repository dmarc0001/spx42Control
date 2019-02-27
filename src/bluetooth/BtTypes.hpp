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
  //! device MAC, device Name
  using SPXDeviceDescr = QPair< QString, QString >;
  //! device Addr, device Info, Service Info
  using SPXDeviceList = QHash< QString, SPXDeviceDescr >;
  //! Queue zum scannen der Devices nach Services
  using ToScannedDevicesQueue = QQueue< QBluetoothAddress >;
}  // namespace spx
#endif  // BTTYPES_HPP
