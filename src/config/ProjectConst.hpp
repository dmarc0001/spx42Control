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
    static const QString MAIN_TITLE;  //! der Fenstertitel
    // für Buggy Firmware, Temperatur-Lesen, Gradienten-Bug unterscheiden
    static const QRegExp FIRMWARE_2_6x;
    static const QRegExp FIRMWARE_2_7x;
    static const QRegExp FIRMWARE_2_7_V_R83x;
    static const QRegExp FIRMWARE_2_7_Hx;
    static const QRegExp FIRMWARE_2_7_H_R83x;

    //###########################################################################
    //#### Einfache Konstanten                                               ####
    //###########################################################################
    static constexpr int BUFFER_LEN{1200};          //! Empfangspufferlänge
    static constexpr int CXN_SUCCESS{0};            //! Winsock OK Value
    static constexpr int CXN_ERROR{1};              //! Winsock ERROR Value
    static constexpr int SOCK_BUFFLEN{128};         //! WINSOCK Pufferlänge (fuer BT)
    static constexpr int CONFIG_WRITE_DELAY{3500};  //! waretn bis Konfiguration zum SPX geschrieben wird
    static const QString RFCommUUID;                //! RFComm UUID bei den SPX-Services
    static const QBluetoothUuid SPX42ServiceUuid;   //! die SPX42 Service UUID
    static const QString searchedServiceRegex;      //! Suchausdruck für SPX42 Service

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
}
#endif
