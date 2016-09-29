#ifndef SPX42CONFIG_HPP
#define SPX42CONFIG_HPP

#include <QtGlobal>
#include <QObject>
#include <QString>

#include "SPX42Defs.hpp"
#include "SPX42Gas.hpp"


namespace spx42
{
  class SPX42Config : public QObject
  {
    private:
      Q_OBJECT
      LicenseType licType;
      QString serialNumber;
      SPX42Gas gasList[8];
      bool isValid;

    public:
      SPX42Config();
      LicenseType getLicType() const;
      void setLicType(const LicenseType &value);
      QString getLicName() const;
      SPX42Gas& getGasAt( int num );
      void reset(void);
      QString getSerialNumber() const;
      void setSerialNumber(const QString &serial);

    signals:
      void licenseChanged( LicenseType lic );
  };
}
#endif // SPX42CONFIG_HPP
