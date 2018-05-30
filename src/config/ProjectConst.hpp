#ifndef PROJECTCONST_HPP
#define PROJECTCONST_HPP

#include <iostream>
#include <utility>
#include <QColor>
#include <QRegExp>

#include "SPX42Defs.hpp"


//
// Definitionen
//

namespace spx
{
  struct ProjectConst
  {
    enum class IndicatorStati;


    //###########################################################################
    //#### Einfache Konstanten                                               ####
    //###########################################################################
    static constexpr int BUFFER_LEN = 1200;              //! Empfangspufferlänge
    static constexpr int CXN_SUCCESS = 0;                //! Winsock OK Value
    static constexpr int CXN_ERROR = 1;                  //! Winsock ERROR Value
    static constexpr int SOCK_BUFFLEN = 128;             //! WINSOCK Pufferlänge (fuer BT)
    static const QString RFCommUUID;                     //! RFComm UUID bei den SPX-Services
    static const QRegExp searchedServiceRegex;           //! Suchausdruck für SPX42 Service

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
    static const QColor offlineColor;
    static const QColor onlineColor;
    static const QColor connectingColor;
    static const QColor workingColor;
    static const QColor errorColor;
  };

  //
  // Definition der Stati
  //
  enum class IndicatorStati
  {
    OFFLINE, DISCOVERING, CONNECTING, ONLINE, TRANSFER, FW_PROGRAMMING, AERROR
  };

}
#endif
