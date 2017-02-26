#ifndef PROJECTCONST_HPP
#define PROJECTCONST_HPP

#include <iostream>
#include <utility>
#include <QColor>

#include "SPX42Defs.hpp"


//
// Definitionen
//

namespace spx42
{
  struct ProjectConst
  {
    //###########################################################################
    //#### Einfache Konstanten                                               ####
    //###########################################################################

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
}
#endif
