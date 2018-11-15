#ifndef SPX42DEFS_HPP
#define SPX42DEFS_HPP

#include <QObject>
#include <QtGlobal>
#include <utility>

namespace spx
{
  //
  // Aufzählungen, einige sind eigentlich nur für die bessere Lesbarkeitdes Codes
  //
  enum class SPX42FirmwareVersions : qint8
  {
    FIRMWARE_UNKNOWN,
    FIRMWARE_2_6x,
    FIRMWARE_2_7x,
    FIRMWARE_2_7_V_R83x,
    FIRMWARE_2_7_Hx,
    FIRMWARE_2_7_H_r83
  };

  enum class ApplicationTab : int
  {
    CONNECT_TAB,
    CONFIG_TAB,
    GAS_TAB,
    LOG_TAB,
    CHART_TAB,
    COUNT_OF_TABS
  };
  enum class ApplicationStat : int
  {
    STAT_OFFLINE,
    STAT_ONLINE,
    STAT_ERROR
  };
  enum class LicenseType : qint8
  {
    LIC_NITROX,
    LIC_NORMOXIX,
    LIC_FULLTMX,
    LIC_MIL
  };
  enum class IndividualLicense : qint8
  {
    LIC_NONE,
    LIC_INDIVIDUAL
  };
  enum class DiluentType : qint8
  {
    DIL_NONE,
    DIL_01,
    DIL_02
  };
  enum class DecompressionPreset : qint8
  {
    DECO_KEY_V_CONSERVATIVE,
    DECO_KEY_CONSERVATIVE,
    DECO_KEY_MODERATE,
    DECO_KEY_AGRESSIVE,
    DECO_KEY_V_AGRESSIVE,
    DECO_KEY_CUSTOM
  };
  enum class DecompressionDynamicGradient : qint8
  {
    DYNAMIC_GRADIENT_ON,
    DYNAMIC_GRADIENT_OFF
  };
  enum class DecompressionDeepstops : qint8
  {
    DEEPSTOPS_ENABLED,
    DEEPSTOPS_DISABLED
  };
  enum class DisplayBrightness : qint8
  {
    BRIGHT_20,
    BRIGHT_40,
    BRIGHT_60,
    BRIGHT_80,
    BRIGHT_100
  };
  enum class DisplayOrientation : qint8
  {
    LANDSCAPE,
    LANDSCAPE_180
  };
  enum class DeviceTemperaturUnit : qint8
  {
    CELSIUS,
    FAHRENHEID
  };
  enum class DeviceLenghtUnit : qint8
  {
    METRIC,
    IMPERIAL
  };
  enum class DeviceWaterType : qint8
  {
    FRESHWATER,
    SALTWATER
  };
  enum class DeviceSetpointAuto : qint8
  {
    AUTO_06,
    AUTO_10,
    AUTO_15,
    AUTO_20,
    AUTO_OFF
  };
  enum class DeviceSetpointValue : qint8
  {
    SETPOINT_10,
    SETPOINT_11,
    SETPOINT_12,
    SETPOINT_13,
    SETPOINT_14
  };
  enum class DeviceIndividualSensors : qint8
  {
    SENSORS_ON,
    SENSORS_OFF
  };
  enum class DeviceIndividualPSCR : qint8
  {
    PSCR_ON,
    PSCR_OFF
  };
  enum class DeviceIndividualSensorCount : qint8
  {
    SENSOR_COUNT_01,
    SENSOR_COUNT_02,
    SENSOR_COUNT_03
  };
  enum class DeviceIndividualAcoustic : qint8
  {
    ACOUSTIC_OFF,
    ACOUSTIC_ON
  };
  enum class DeviceIndividualLogInterval : qint8
  {
    INTERVAL_20,
    INTERVAL_30,
    INTERVAL_60
  };
  enum class DeviceIndividualTempstick : qint8
  {
    TEMPSTICK01,
    TEMPSTICK02,
    TEMPSTICK03
  };

  // Declariere Typen für Gradienten
  using DecoGradient = std::pair< qint8, qint8 >;
  using DecoGradientHash = QHash< int, DecoGradient >;

  class SPX42License : public QObject
  {
    private:
    Q_OBJECT
    LicenseType licType;       //! Lizenztyp merken
    IndividualLicense licInd;  //! Lizenzerweiterung Individualeinstsellungen

    public:
    SPX42License( void );                              //! Konstruktor
    SPX42License( SPX42License &lic );                 //! Kopierkonstruktor
    bool operator==( const SPX42License &lic );        //! Vergleichsoperator
    bool operator!=( const SPX42License &lic );        //! Vergleichsoperator
    bool operator<( const SPX42License &lic );         //! Vergleichsoperator
    bool operator>( const SPX42License &lic );         //! Vergleichsoperator
    LicenseType getLicType() const;                    //! Lizenztyp erfragen
    void setLicType( const LicenseType &value );       //! Lizenztyp setzten
    IndividualLicense getLicInd() const;               //! erfrage Individual Lizenz
    void setLicInd( const IndividualLicense &value );  //! setzte Individual Lizenz

    signals:
    // void licenseChangedPrivateSig( const SPX42License lic );  //! Signal das sich das geändert hat
  };
}  // namespace spx
#endif  // SPX42DEFS_HPP
