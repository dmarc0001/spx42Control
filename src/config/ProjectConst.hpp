#ifndef PROJECTCONST_HPP
#define PROJECTCONST_HPP

#include <iostream>
#include <QColor>

//
// Definitionen
//

namespace spx42
{
  struct ProjectConst
  {
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
