#ifndef SPX42DEFS_HPP
#define SPX42DEFS_HPP

#include <utility>
#include <QObject>
#include <QtGlobal>

namespace spx42
{

  // Aufzählungen
  enum class LicenseType : qint8 { LIC_NITROX, LIC_NORMOXIX, LIC_FULLTMX, LIC_MIL };
  enum class IndividualLicense : qint8 { LIC_NONE, LIC_INDIVIDUAL };
  enum class DiluentType : qint8 { DIL_NONE, DIL_01, DIL_02 };
  enum class DecompressionPreset : qint8 { DECO_KEY_V_CONSERVATIVE,
                                           DECO_KEY_CONSERVATIVE,
                                           DECO_KEY_MODERATE,
                                           DECO_KEY_AGRESSIVE,
                                           DECO_KEY_V_AGRESSIVE,
                                           DECO_KEY_CUSTOM
                                         };
  enum class DisplayBrightness : qint8 { BRIGHT_20, BRIGHT_40, BRIGHT_60, BRIGHT_80, BRIGHT_100 };
  enum class DisplayOrientation : qint8 { LANDSCAPE, LANDSCAPE_180 };

  // Declariere Typen für Gradienten
  using DecoGradient = std::pair<qint8,qint8>;
  using DecoGradientHash =  QHash<int, DecoGradient>;

  class SPX42License : public QObject
  {
    private:
      Q_OBJECT
      LicenseType licType;                                      //! Lizenztyp merken
      IndividualLicense licInd;                                 //! Lizenzerweiterung Individualeinstsellungen

    public:
      SPX42License(void);                                       //! Konstruktor
      SPX42License( SPX42License& lic );                        //! Kopierkonstruktor
      bool operator == (const SPX42License& lic);               //! Vergleichsoperator
      bool operator != (const SPX42License& lic);               //! Vergleichsoperator
      bool operator < (const SPX42License& lic );               //! Vergleichsoperator
      bool operator > (const SPX42License& lic );               //! Vergleichsoperator
      LicenseType getLicType() const;                           //! Lizenztyp erfragen
      void setLicType(const LicenseType& value);                //! Lizenztyp setzten
      IndividualLicense getLicInd() const;                      //! erfrage Individual Lizenz
      void setLicInd(const IndividualLicense &value);           //! setzte Individual Lizenz

    signals:
      void licenseChangedPrivateSig( void );                    //! Signal das sich das geändert hat

  };
}
#endif // SPX42DEFS_HPP
