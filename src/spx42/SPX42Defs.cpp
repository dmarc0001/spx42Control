#include "SPX42Defs.hpp"

namespace spx
{
  SPX42License::SPX42License() : licType( LicenseType::LIC_NITROX ), licInd( IndividualLicense::LIC_NONE )
  {
  }

  SPX42License::SPX42License( SPX42License &lic )
  {
    licType = lic.licType;
    licInd = lic.licInd;
  }

  bool SPX42License::operator==( const SPX42License &lic )
  {
    //
    // wenn was nicht gleich ist, dann sind sie ungleich!
    //
    if ( licType != lic.licType )
      return ( false );
    if ( licInd != lic.licInd )
      return ( false );
    return ( true );
  }

  bool SPX42License::operator!=( const SPX42License &lic )
  {
    //
    // wenn was nicht gleich ist, dann sind sie ungleich!
    //
    if ( licType != lic.licType )
      return ( true );
    if ( licInd != lic.licInd )
      return ( true );
    return ( false );
  }

  bool SPX42License::operator<( const SPX42License &lic )
  {
    //
    // ist meine Lizenz geringerwertig?
    //
    return ( static_cast< int >( licType ) < static_cast< int >( lic.licType ) );
  }

  bool SPX42License::operator>( const SPX42License &lic )
  {
    //
    // ist meine Lizenz höherwertig
    //
    return ( static_cast< int >( licType ) > static_cast< int >( lic.licType ) );
  }

  LicenseType SPX42License::getLicType() const
  {
    return licType;
  }

  void SPX42License::setLicType( const LicenseType &value )
  {
    licType = value;
    // emit licenseChangedPrivateSig( this );
  }

  IndividualLicense SPX42License::getLicInd() const
  {
    return licInd;
  }

  void SPX42License::setLicInd( const IndividualLicense &value )
  {
    licInd = value;
    // emit licenseChangedPrivateSig( this );
  }
}
