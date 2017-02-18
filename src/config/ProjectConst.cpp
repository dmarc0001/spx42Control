#include "ProjectConst.hpp"

namespace spx42
{
  //
  // Defaults für DEKOMPRESSIONS Einstellungen
  //
  const DecoGradient ProjectConst::DECO_VAL_V_CONSERVATIVE( 35, 70 );
  const DecoGradient ProjectConst::DECO_VAL_CONSERVATIVE( 30, 85 );
  const DecoGradient ProjectConst::DECO_VAL_MODERATE( 25, 85 );
  const DecoGradient ProjectConst::DECO_VAL_AGRESSIVE( 15, 90 );
  const DecoGradient ProjectConst::DECO_VAL_V_AGRESSIVE( 10, 100 );
  const DecoGradient ProjectConst::DECO_VAL_CUSTOM( DECO_VAL_MODERATE );

  //
  // Zuweisung der Konstanten
  //
  const QColor ProjectConst::offlineColor( 128, 128, 128 );
  const QColor ProjectConst::onlineColor( 0, 254, 0, 35 );
  const QColor ProjectConst::connectingColor( 200, 140, 50, 35 );
  const QColor ProjectConst::workingColor( 254, 254, 0, 35 );
  const QColor ProjectConst::errorColor( 255, 20, 20, 100 );
}
