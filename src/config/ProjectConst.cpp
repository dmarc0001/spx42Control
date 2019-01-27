#include "ProjectConst.hpp"

namespace spx
{
  //
  // Der Programmtitel
  //
  const QString ProjectConst::MAIN_TITLE{"SPX42 BT-Controller"};
  const QString ProjectConst::APP_VERSION = QString( "%1.%2.%3" ).arg( VMAJOR ).arg( VMINOR ).arg( VPATCH );
  //
  // export Festlegungen
  //
  const QString ProjectConst::MANUFACTURER_APP{"spx42Control"};
  const QString ProjectConst::MANUFACTURER_APP_NAME{"Dirk Marciniak"};
  const QString ProjectConst::MANUFACTURER_EMAIL{"dirk@submatix.com"};
  const QString ProjectConst::MANUFACTURER_WWW{"https://www.submatix.com"};
  const QString ProjectConst::REBREATHER_SENSOR_TYPE{"PSR 11-39-MDSX-1"};

  // für Buggy Firmware, Temperatur-Lesen, Gradienten-Bug unterscheiden
  const QRegExp ProjectConst::FIRMWARE_2_6x{"V2\\.6.*"};
  const QRegExp ProjectConst::FIRMWARE_2_7x{"V2\\.7_V.*"};
  const QRegExp ProjectConst::FIRMWARE_2_7_V_R83x{"V2\\.7_V r83.*"};
  const QRegExp ProjectConst::FIRMWARE_2_7_Hx{"V2\\.7_?H.*"};
  const QRegExp ProjectConst::FIRMWARE_2_7_H_R83x{"V2\\.7_?H r83.*"};

  //
  // Einfache Konstanten
  //
  const QString ProjectConst::RFCommUUID{"00001101-0000-1000-8000-00805f9b34fb"};
  const QBluetoothUuid ProjectConst::SPX42ServiceUuid( ProjectConst::RFCommUUID );
  const QString ProjectConst::searchedServiceRegex{"SPX42|SPP"};

  //
  // Defaults für DEKOMPRESSIONS Einstellungen
  //
  const DecoGradient ProjectConst::DECO_VAL_V_CONSERVATIVE{35, 70};
  const DecoGradient ProjectConst::DECO_VAL_CONSERVATIVE{30, 85};
  const DecoGradient ProjectConst::DECO_VAL_MODERATE{25, 85};
  const DecoGradient ProjectConst::DECO_VAL_AGRESSIVE{15, 90};
  const DecoGradient ProjectConst::DECO_VAL_V_AGRESSIVE{10, 100};
  const DecoGradient ProjectConst::DECO_VAL_CUSTOM{DECO_VAL_MODERATE};

  //
  // Zuweisung der Konstanten
  //
  const QColor ProjectConst::COLOR_OFFLINE{128, 128, 128, 255};
  const QColor ProjectConst::COLOR_ONLINE{0, 220, 0, 255};
  const QColor ProjectConst::COLOR_CONNECTING{242, 135, 68, 255};
  const QColor ProjectConst::COLOR_BUSY{180, 20, 20, 128};
  const QColor ProjectConst::COLOR_ERROR{255, 20, 20, 180};
}  // namespace spx
