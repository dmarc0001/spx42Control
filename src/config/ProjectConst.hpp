#ifndef PROJECTCONST_HPP
#define PROJECTCONST_HPP

#include <QBluetoothUuid>
#include <QColor>
#include <QRegExp>
#include <iostream>
#include <utility>

#include "spx42/SPX42Defs.hpp"

//
// Definitionen
//

namespace spx
{
  struct ProjectConst
  {
    enum class IndicatorStati;

    //###########################################################################
    //#### Stringkonstanten ohne Lokalisierung
    //###########################################################################
    //! der Fenstertitel
    static const QString MAIN_TITLE;
    static const QString APP_VERSION;
    static const QString MANUFACTURER_APP;
    static const QString MANUFACTURER_APP_NAME;
    static const QString MANUFACTURER_EMAIL;
    static const QString MANUFACTURER_WWW;
    static const QString REBREATHER_SENSOR_TYPE;
    // für Buggy Firmware, Temperatur-Lesen, Gradienten-Bug unterscheiden
    static const QRegExp FIRMWARE_2_6x;
    static const QRegExp FIRMWARE_2_7x;
    static const QRegExp FIRMWARE_2_7_V_R83x;
    static const QRegExp FIRMWARE_2_7_Hx;
    static const QRegExp FIRMWARE_2_7_H_R83x;

    //###########################################################################
    //#### Einfache Konstanten                                               ####
    //###########################################################################
    //! Anzahl der Felder der Logdaten (Konstant)
    static constexpr int LOG_FIELD_COUNT{36};
    //! Differenz Celsius zu Kelvin
    static constexpr double KELVINDIFF{273.15};
    //! Empfangspufferlänge
    static constexpr int BUFFER_LEN{2400};
    //! waretn bis Konfiguration zum SPX geschrieben wird
    static constexpr int CONFIG_WRITE_DELAY{3500};
    //! Minimaler Sauerstoffanteil
    static constexpr int SPX_MINIMUM_O2{2};
    //! RFComm UUID bei den SPX-Services
    static const QString RFCommUUID;
    //! die SPX42 Service UUID
    static const QBluetoothUuid SPX42ServiceUuid;
    //! Suchausdruck für SPX42 Service
    static const QString searchedServiceRegex;

    //###########################################################################
    // Defaults für DEKOMPRESSIONS Einstellungen                             ####
    //###########################################################################
    static const DecoGradient DECO_VAL_V_CONSERVATIVE;
    static const DecoGradient DECO_VAL_CONSERVATIVE;
    static const DecoGradient DECO_VAL_MODERATE;
    static const DecoGradient DECO_VAL_AGRESSIVE;
    static const DecoGradient DECO_VAL_V_AGRESSIVE;
    static const DecoGradient DECO_KEY_CUSTOM;
    static const DecoGradient DECO_VAL_CUSTOM;

    //###########################################################################
    //#### member-Variable/Konstante                                         ####
    //###########################################################################
    static const QColor COLOR_OFFLINE;
    static const QColor COLOR_ONLINE;
    static const QColor COLOR_CONNECTING;
    static const QColor COLOR_BUSY;
    static const QColor COLOR_ERROR;
  };

  //
  // Definition der Stati
  //
  enum class IndicatorStati : quint8
  {
    OFFLINE,
    DISCOVERING,
    CONNECTING,
    ONLINE,
    TRANSFER,
    FW_PROGRAMMING,
    AERROR
  };
}  // namespace spx
#endif
