#ifndef IFRAGMENTINTERFACE_HPP
#define IFRAGMENTINTERFACE_HPP

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "../utils/SPX42Defs.hpp"

namespace spx42
{
  class IFragmentInterface
  {
    protected:
      LicenseType currLic;
      Logger *lg;                                               //! Zeiger auf das Log-Objekt
      SPX42Config *spxConfig;                                   //! Zeiger auf das SPX42 Config Objekt

    public:
      explicit IFragmentInterface(Logger *logger, SPX42Config *spxCfg); //! Standartkonstruktor
      virtual ~IFragmentInterface(void);                        //! Destruktor
  };
}

#endif // IFRAGMENTINTERFACE_HPP
