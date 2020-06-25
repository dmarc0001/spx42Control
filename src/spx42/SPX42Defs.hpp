﻿#ifndef SPX42DEFS_HPP
#define SPX42DEFS_HPP

#include <QObject>
#include <QtGlobal>
#include <utility>

namespace spx
{
  //! definiert Klassen in der Konfiguration
  namespace SPX42ConfigClass
  {
    constexpr quint8 CF_CLASS_SPX{0x01};
    constexpr quint8 CF_CLASS_DECO{0x02};
    constexpr quint8 CF_CLASS_GASES{0x04};
    constexpr quint8 CF_CLASS_DISPLAY{0x08};
    constexpr quint8 CF_CLASS_UNITS{0x10};
    constexpr quint8 CF_CLASS_SETPOINT{0x20};
    constexpr quint8 CF_CLASS_INDIVIDUAL{0x40};
    constexpr quint8 CF_CLASS_LOG{0x80};
    constexpr quint8 CF_CLASS_ALL{0xff};
    //
    constexpr quint8 CF_GAS01{0x01};
    constexpr quint8 CF_GAS02{0x02};
    constexpr quint8 CF_GAS03{0x04};
    constexpr quint8 CF_GAS04{0x08};
    constexpr quint8 CF_GAS05{0x10};
    constexpr quint8 CF_GAS06{0x20};
    constexpr quint8 CF_GAS07{0x40};
    constexpr quint8 CF_GAS08{0x80};
    //
    constexpr double MAX_PPO2{1.60};
  }  // namespace SPX42ConfigClass

  //
  // Aufzählungen, einige sind eigentlich nur für die bessere Lesbarkeitdes Codes
  //
  //! unterstützte Firmware versionen
  enum class SPX42FirmwareVersions : qint8
  {
    FIRMWARE_UNKNOWN,
    FIRMWARE_2_6x,
    FIRMWARE_2_7_H_R83x,
    FIRMWARE_2_7_V_R83x,
    FIRMWARE_2_7_Hx,
    FIRMWARE_2_7x
  };

  //! Tabs er Anwendung
  enum class ApplicationTab : int
  {
    CONNECT_TAB,
    DEVICE_INFO_TAB,
    CONFIG_TAB,
    GAS_TAB,
    LOG_TAB,
    CHART_TAB,
    COUNT_OF_TABS
  };

  //! Stati der Applikation bezogen auf connectivität
  enum class ApplicationStat : int
  {
    STAT_OFFLINE,
    STAT_ONLINE,
    STAT_ERROR
  };

  //! Lizenzierung des SPX42
  enum class LicenseType : qint8
  {
    LIC_NITROX,
    LIC_NORMOXIX,
    LIC_FULLTMX,
    LIC_MIL
  };

  //! Die Lizenz individual
  enum class IndividualLicense : qint8
  {
    LIC_NONE,
    LIC_INDIVIDUAL
  };

  //! Diluent Type
  enum class DiluentType : qint8
  {
    DIL_NONE,
    DIL_01,
    DIL_02
  };

  //! Tyen des Presets für Dekompression
  enum class DecompressionPreset : qint8
  {
    DECO_KEY_V_CONSERVATIVE,
    DECO_KEY_CONSERVATIVE,
    DECO_KEY_MODERATE,
    DECO_KEY_AGRESSIVE,
    DECO_KEY_V_AGRESSIVE,
    DECO_KEY_CUSTOM
  };

  //! Ein oder Aus der Dynamischen Gradienten
  enum class DecompressionDynamicGradient : qint8
  {
    DYNAMIC_GRADIENT_ON,
    DYNAMIC_GRADIENT_OFF
  };

  //! tiefe Deko stopps ein/aus
  enum class DecompressionDeepstops : qint8
  {
    DEEPSTOPS_ENABLED,
    DEEPSTOPS_DISABLED
  };

  //! Tiefe des letzten Dekostopps
  enum class DecoLastStop : qint8
  {
    LAST_STOP_ON_3,
    LAST_STOP_ON_6
  };

  //! Hellogkeit des Displays
  enum class DisplayBrightness : qint8
  {
    BRIGHT_20,
    BRIGHT_40,
    BRIGHT_60,
    BRIGHT_80,
    BRIGHT_100
  };

  //! Ausrichtung des Displays
  enum class DisplayOrientation : qint8
  {
    LANDSCAPE,
    LANDSCAPE_180
  };

  //! Einheit der Temperaturen
  enum class DeviceTemperaturUnit : qint8
  {
    CELSIUS,
    FAHRENHEID
  };

  //! Einheitensystem der Länge/Tiefe/Temperatur
  enum class DeviceLenghtUnit : qint8
  {
    METRIC,
    IMPERIAL
  };

  //! Wassertyp
  enum class DeviceWaterType : qint8
  {
    SALTWATER,
    FRESHWATER
  };

  //! Autosetpont des SPX aus /tiefe
  enum class DeviceSetpointAuto : qint8
  {
    AUTO_OFF,
    AUTO_06,
    AUTO_10,
    AUTO_15,
    AUTO_20
  };

  //! O2 Setpoint des SPX42
  enum class DeviceSetpointValue : qint8
  {
    SETPOINT_10,
    SETPOINT_11,
    SETPOINT_12,
    SETPOINT_13,
    SETPOINT_14
  };

  //! O2 Sensoren des SPX42 ein/aus
  enum class DeviceIndividualSensors : qint8
  {
    SENSORS_ON,
    SENSORS_OFF
  };

  //! Passiver SCR Mode
  enum class DeviceIndividualPSCR : qint8
  {
    PSCR_ON,
    PSCR_OFF
  };

  //! Anzahl der berücksichtigten Sensoren
  enum class DeviceIndividualSensorCount : qint8
  {
    SENSOR_COUNT_01,
    SENSOR_COUNT_02,
    SENSOR_COUNT_03
  };

  //! Akustische Warnungen an/aus
  enum class DeviceIndividualAcoustic : qint8
  {
    ACOUSTIC_OFF,
    ACOUSTIC_ON
  };

  //! Loginterval des SPX42
  enum class DeviceIndividualLogInterval : qint8
  {
    INTERVAL_20,
    INTERVAL_30,
    INTERVAL_60
  };

  //! Tempstick für den Kalk, drei Versionen
  enum class DeviceIndividualTempstick : qint8
  {
    TEMPSTICK01,
    TEMPSTICK02,
    TEMPSTICK03
  };

  //! Deklariere Typen für Gradienten LOW,HIGH
  using DecoGradient = std::pair< qint8, qint8 >;
  using DecoGradientHash = QHash< int, DecoGradient >;

  class SPX42License : public QObject
  {
    private:
    Q_OBJECT
    //! Lizenztyp merken
    LicenseType licType;
    //! Lizenzerweiterung Individualeinstsellungen
    IndividualLicense licInd;

    public:
    //! Konstruktor
    SPX42License( void );
    //! Kopierkonstruktor
    SPX42License( SPX42License &lic );
    //! Vergleichsoperator
    bool operator==( const SPX42License &lic );
    //! Vergleichsoperator
    bool operator!=( const SPX42License &lic );
    //! Vergleichsoperator
    bool operator<( const SPX42License &lic );
    //! Vergleichsoperator
    bool operator>( const SPX42License &lic );
    //! Lizenztyp erfragen
    LicenseType getLicType() const;
    //! Lizenztyp setzten
    void setLicType( const LicenseType &value );
    //! erfrage Individual Lizenz
    IndividualLicense getLicInd() const;
    //! setzte Individual Lizenz
    void setLicInd( const IndividualLicense &value );
    //! serialisiere die Lizenz
    QByteArray serialize( void );

    signals:
    // void licenseChangedPrivateSig( const SPX42License lic );  //! Signal das sich das geändert hat
  };
}  // namespace spx
#endif  // SPX42DEFS_HPP
