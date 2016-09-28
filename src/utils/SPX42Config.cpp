#include "SPX42Config.hpp"

namespace spx42
{
  SPX42Config::SPX42Config() :
    QObject(Q_NULLPTR)
  {
    reset();
  }

  LicenseType SPX42Config::getLicType() const
  {
    return licType;
  }

  void SPX42Config::setLicType(const LicenseType& value)
  {
    if( licType != value )
    {
      licType = value;
      emit licenseChanged( licType );
    }
  }

  QString SPX42Config::getLicName() const
  {
    switch( static_cast<int>(licType))
    {
      case static_cast<int>(LicenseType::LIC_NITROX):
        return( tr("NITROX") );
      case static_cast<int>(LicenseType::LIC_NORMOXIX):
        return( tr("NORMOXIC TMX") );
      case static_cast<int>(LicenseType::LIC_FULLTMX):
        return( tr("FULL TMX") );
      case static_cast<int>(LicenseType::LIC_MIL):
        return( tr("MILITARY") );
    }
    return( tr("UNKNOWN") );
  }

  SPX42Gas& SPX42Config::getGasAt( int num )
  {
    if( num < 0 || num > 7 )
    {
      return( gasList[0] );
    }
    return( gasList[num]);
  }

  void SPX42Config::reset(void)
  {
    isValid = false;
    licType = LicenseType::LIC_NITROX;
    serialNumber = "0000000000";
    for( int i=0; i<8; i++)
    {
      gasList[0].reset();
    }
    emit licenseChanged( licType );
  }

  QString SPX42Config::getSerialNumber() const
  {
    return( serialNumber );
  }

  void SPX42Config::setSerialNumber(const QString &serial)
  {
    serialNumber = serial;
  }

}
