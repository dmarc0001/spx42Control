#ifndef SPX42CONFIG_HPP
#define SPX42CONFIG_HPP

#include <QtGlobal>
#include <QString>

#include "SPX42Defs.hpp"
#include "SPX42Gas.hpp"


namespace spx42
{
  class SPX42Config
  {
    private:
      LicenseType licType;
      SPX42Gas gasList[8];
      bool isValid;

    public:
      SPX42Config();
      LicenseType getLicType() const;
      void setLicType(const LicenseType &value);
      SPX42Gas& getGasAt( int num );
      void reset(void);
  };
}
#endif // SPX42CONFIG_HPP
