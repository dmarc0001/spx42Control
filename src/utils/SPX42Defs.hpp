#ifndef SPX42DEFS_HPP
#define SPX42DEFS_HPP

#include <QtGlobal>


namespace spx42
{
  enum class LicenseType : qint8 { LIC_NITROX, LIC_NORMOXIX, LIC_FULLTMX, LIC_MIL };
  enum class DiluentType : qint8 { DIL_NONE, DIL_01, DIL_02 };
}
#endif // SPX42DEFS_HPP
