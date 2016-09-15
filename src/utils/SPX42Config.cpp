#include "SPX42Config.hpp"

namespace spx42
{
  SPX42Config::SPX42Config() :
    licType( LicenseType::LIC_NITROX),
    isValid( false )
  {

  }

  LicenseType SPX42Config::getLicType() const
  {
    return licType;
  }

  void SPX42Config::setLicType(const LicenseType &value)
  {
    licType = value;
  }

  SPX42Gas& SPX42Config::getGasAt( int num )
  {
    if( num < 0 || num > 7 )
      return( gasList[0] );
    return( gasList[num]);
  }

  void SPX42Config::reset(void)
  {
    for( int i=0; i<8; i++)
    {
      gasList[0].reset();
    }
  }
}
