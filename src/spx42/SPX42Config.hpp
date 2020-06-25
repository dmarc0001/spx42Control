#ifndef SPX42CONFIG_HPP
#define SPX42CONFIG_HPP

#include <QCryptographicHash>
#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include "SPX42Gas.hpp"
#include "SPX42LogDirectoryEntry.hpp"
#include "config/ProjectConst.hpp"
#include "spx42/SPX42Defs.hpp"

namespace spx
{
  constexpr int GAS_HASHES{9};
  constexpr int GASES{8};

  class SPX42Config : public QObject
  {
    private:
    Q_OBJECT
    //! sollen beim stzen von Eigenschaften Signale gesendet werden?
    volatile bool sendSignals;
    //! Ist das Objekt gültig?
    bool isValid;
    //! welche Firmwareversion?
    SPX42FirmwareVersions spxFirmwareVersion;
    //! Firmwareversion im Original
    QString spxFirmwareVersionString;
    //! SPX42 Lizenz
    SPX42License spxLicense;
    //! Seriennummer des aktuellen SPX42
    QString spxSerialNumber;
    //! Gasliste des aktuellen SPX42
    SPX42Gas gasList[ GASES ];
    //! Aktueller Typ der Dekompressionsgradienten
    DecompressionPreset decoCurrentPreset;
    //! Hashliste der DECO-Presets (incl. CUSTOM == variabel)
    DecoGradientHash decoPresets;
    //! dynamische Gradienten erlaubt
    DecompressionDynamicGradient decoDynamicGradient;
    //! sind deepstops erlaubt/an
    DecompressionDeepstops decoDeepstopsEnabled;
    //! wo ist der letzte DECO Stop (3 oder 6 Meter)
    DecoLastStop decoLastStop;
    //! Display Helligkeit
    DisplayBrightness displayBrightness;
    //! Ausrichtung des Displays
    DisplayOrientation displayOrientation;
    //! Einheit der Tempteratur
    DeviceTemperaturUnit unitTemperature;
    //! Einheit der Länge (metrisch/imperial)
    DeviceLenghtUnit unitLength;
    //! Typ des Wassers des SPX42
    DeviceWaterType unitWaterType;
    //! in welcher Tiefe soll der Setpoint eingestellt werden
    DeviceSetpointAuto setpointAuto;
    //! welcher PPO2 soll eingestellt sein
    DeviceSetpointValue setpointValue;
    //! sind die Sensoren generell AN/AUS
    DeviceIndividualSensors individualSensorsOn;
    //! Ist der SPX im PSCR Mode?
    DeviceIndividualPSCR individualPSCROn;
    //! wie viele Sensoren im SPX42 aktiv
    DeviceIndividualSensorCount individualSensorCount;
    //! sollen akustische Warnunge ausggeben werden
    DeviceIndividualAcoustic individualAcustic;
    //! welches Interval zum loggen
    DeviceIndividualLogInterval individualLogInterval;
    //! welcher Tempstick wird genutzt?
    DeviceIndividualTempstick individualTempStick;
    //! Liste mit Einträgen im Verzeichnis
    SPX42LogDirectoryEntryListPtr logDirectory;
    //! bug bei der darstellung
    bool hasFahrenheidBug;
    //! kann diese Version das Datum setzten?
    bool canSetDate;
    //! beiindividual anzehl der Werte
    bool hasSixValuesIndividual;
    //! wird diese Firmware von der Software unterstützt
    bool isFirmwareSupported;
    //! alte Versionen haben andre Reihenfolge der Parameter
    bool isOldParamSorting;
    //! neue Version Helligkeit
    bool isNewerDisplayBrightness;
    //! Autosetpoint alt deact, 6, 10, 15, 20 NEU 6, 10, 15
    bool isSixMetersAutoSetpoint;
    //! hasobjekt erzeugen
    QCryptographicHash qhash;
    //! Hashes für gesetzte Gaase
    QString currentGasHashes[ GAS_HASHES ];
    //! Hash über Hauptwerte
    QString currentSpxHash;
    //! Hash über Dekompressionswerte
    QString currentDecoHash;
    //! Hashwert über Displaywerte
    QString currentDisplayHash;
    //! Hash über unit einstellungen
    QString currentUnitHash;
    //! Hash über setpoint Einstellungen
    QString currentSetpointHash;
    //! Hash über Inidividual einstellungen
    QString currentIndividualHash;
    //! Hashes für gesetzte Gaase
    QString savedGasHashes[ GAS_HASHES ];
    //! Hash über Hauptwerte
    QString savedSpxHash;
    //! Hash über Dekompressionswerte
    QString savedDecoHash;
    //! Hashwert über Displaywerte
    QString savedDisplayHash;
    //! Hash über unit einstellungen
    QString savedUnitHash;
    //! Hash über setpoint Einstellungen
    QString savedSetpointHash;
    //! Hash über Inidividual einstellungen
    QString savedIndividualHash;

    public:
    //! Der Konstruktor
    SPX42Config();
    //! Lizenz des aktuellen SPX42
    SPX42License &getLicense( void );
    //! Lizenz des aktuellen SPX42 merken
    void setLicense( const LicenseType value );
    //! Lizenz des aktuellen SPX42 merken
    void setLicense( const IndividualLicense value );
    //! Lizenz aus dem Kommando vom SPX lesen
    void setLicense( const QByteArray &lic, const QByteArray &ind );
    //! Textliche Darstellung der Lizenz
    QString getLicName( void ) const;
    //! Gib ein Gas mit der Nummer num vom SPX42 zurück
    SPX42Gas &getGasAt( int num );
    //! setzte Gas mit der Nummer xxx in die Config
    void setGasAt( int num, const SPX42Gas &gas );
    //! Resetiere das Objekt
    void resetConfig( quint8 classes = SPX42ConfigClass::CF_CLASS_ALL );
    //! Seriennummer des aktuellen SPX42 zurückgeben
    QString getSerialNumber( void ) const;
    //! Seriennumemr des aktuellen SPX42 speichern
    void setSerialNumber( const QString &serial );
    //! Aktuelle Gradienteneinstellungen merken
    void setCurrentPreset( DecompressionPreset presetType, qint8 low = 0, qint8 high = 0 );
    //! Aktuelle Gradienteneinstellungen merken
    void setCurrentPreset( DecompressionPreset presetType, const DecoGradient &dGradient );
    //! Welcher Typ Gradient ist gesetzt?
    DecompressionPreset getCurrentDecoGradientPresetType();
    //! Gib die aktuelle Gradienteneinstellung zurück
    DecoGradient getCurrentDecoGradientValue() const;
    //! Gib die Werte füe ein Preset zurück
    DecoGradient getPresetValues( DecompressionPreset presetType ) const;
    //! Gib den Preset für gegebene Werte zurück
    DecompressionPreset getPresetForGradient( qint8 low, qint8 high );
    //! gib den aktuellen Wert zurück
    DecompressionDynamicGradient getIsDecoDynamicGradients( void );
    //! setze dynamische Gradienten an/aus
    void setIsDecoDynamicGradients( DecompressionDynamicGradient isEnabled );
    //! gibt die Einstellung für deep stops zurück
    DecompressionDeepstops getIstDeepstopsEnabled( void );
    //! wo ist der letzte DECO Stop (3 oder 6 Meter)
    DecoLastStop getLastDecoStop( void );
    //! setzte den letzten Deco Stop
    void setLastDecoStop( DecoLastStop lastStop );
    //! setze deppstop enabled an/aus
    void setIsDeepstopsEnabled( DecompressionDeepstops isEnabled );
    //! Wie hell ist das Display?
    DisplayBrightness getDisplayBrightness( void );
    //! Setze die Displayhelligkeit
    void setDisplayBrightness( DisplayBrightness brightness );
    //! wie ist die Display Ausrichtung?
    DisplayOrientation getDisplayOrientation( void );
    //! Setzte die Displayausrichtung
    void setDisplayOrientation( DisplayOrientation orientation );
    //! Welche Temperatureinheit dat das Gerät
    DeviceTemperaturUnit getUnitsTemperatur( void );
    //! Setzte die Temperatureinheit des SPX42
    void setUnitsTemperatur( DeviceTemperaturUnit tUnit );
    //! Welche Längeneinheit nutzt das Gerät
    DeviceLenghtUnit getUnitsLength( void );
    //! Setzte die Temperatureinheit
    void setUnitsLength( DeviceLenghtUnit lUnit );
    //! Welchen Wassertyp hat das Gerät eingestellt
    DeviceWaterType getUnitsWaterType( void );
    //! Setzte den Wassertyp
    void setUnitsWaterType( DeviceWaterType wUnit );
    //! Welchen Autosetpoint nutz das Gerät (Tiefe)
    DeviceSetpointAuto getSetpointAuto( void );
    //! setzte den Autosetpoint
    void setSetpointAuto( DeviceSetpointAuto aSetpoint );
    //! Welchen PPO2 benutzt der SPX42
    DeviceSetpointValue getSetpointValue( void );
    //! setzte den Setpoint
    void setSetpointValue( DeviceSetpointValue ppo2 );
    //! Sind die Sensoren an?
    DeviceIndividualSensors getIndividualSensorsOn( void );
    //! Setze Sensoren an/aus
    void setIndividualSensorsOn( DeviceIndividualSensors onOff );
    //! ist der SPX im PSCR Modus
    DeviceIndividualPSCR getIndividualPscrMode( void );
    //! Setzte PSCR-Mode
    void setIndividualPscrMode( DeviceIndividualPSCR pscrMode );
    //! Wie viele Sensoren sind aktiv
    DeviceIndividualSensorCount getIndividualSensorsCount( void );
    //! Setzte Anzahl aktiver Sensoren
    void setIndividualSensorsCount( DeviceIndividualSensorCount sCount );
    //! wie ist der Status der Akustischen Meldungen
    DeviceIndividualAcoustic getIndividualAcoustic( void );
    //! setzte Akustik an oder aus
    void setIndividualAcoustic( DeviceIndividualAcoustic acoustic );
    //! wie ist das Log interval
    DeviceIndividualLogInterval getIndividualLogInterval( void );
    //! setze das Log interval
    void setIndividualLogInterval( DeviceIndividualLogInterval logInterval );
    //! welcher Temperatur Stick ist eingebaut?
    DeviceIndividualTempstick getIndividualTempStick( void );
    //! setze den eingebauten TemperaturStick
    void setIndividualTempStick( DeviceIndividualTempstick tStick );
    //! gibt die Version der Firmware als enum zurück
    SPX42FirmwareVersions getSpxFirmwareVersion( void ) const;
    //! gibt die Version der Firmware als string zurück
    QString getSpxFirmwareVersionString( void );
    //! setzt die Firmwareversion
    void setSpxFirmwareVersion( SPX42FirmwareVersions value );
    //! setzt die Firmwareversion
    void setSpxFirmwareVersion( const QString &value );
    //! setzt die Firmwareversion
    void setSpxFirmwareVersion( const QByteArray &value );
    //! hat die Firmware den Fahrenheid Bug?
    bool getHasFahrenheidBug() const;
    //! kann die Firmware Datum setzten
    bool getCanSetDate() const;
    //! hat individual sex parameter
    bool getHasSixValuesIndividual() const;
    //! ist diese Firmware unterstützt
    bool getIsFirmwareSupported() const;
    //! alte Parameterordnung?
    bool getIsOldParamSorting() const;
    //! neuere Helligkeitsabstufungen
    bool getIsNewerDisplayBrightness() const;
    //! fünf oder sechs meter autosetpoint
    bool getIsSixMetersAutoSetpoint() const;
    //! setzte die aktuelle Konfiguration als "gesichert"
    void freezeConfigs( quint8 changed = SPX42ConfigClass::CF_CLASS_ALL );
    //! was ist geändert?
    quint8 getChangedConfig( void );
    //! welches Gas wurde geändert?
    quint8 getChangedGases( void );
    //! gib dieu Unit-Hashes zurück
    QString getUnitHashes( void );
    //! Verzeichis ausgeben
    SPX42LogDirectoryEntryListPtr getLogDirectory( void );
    //! Einen Eintrag zufügen
    int addDirectoryEntry( const SPX42LogDirectoryEntry &entry );

    private:
    //! Hash über die globalen Einstellungen
    QString makeSpxHash( void );
    //! Hash über Dekompressionseinstellungen
    QString makeDecoHash( void );
    //! Hash über displayvariable
    QString makeDisplayHash( void );
    //! Hash über einheiten Einstellungen
    QString makeUnitsHash( void );
    //! Hash über setpointeinstellungen
    QString makeSetpointHash( void );
    //! Hash über individual Einstellungen
    QString makeIndividualHash( void );

    private slots:
    // void licenseChangedPrivateSlot( SPX42License& lic );

    signals:
    //! Signal wenn die lizenz verändert wird
    void licenseChangedSig( const SPX42License &lic );
    //! Signal wenn die Seriennummer neu gesetzt wird
    void serialNumberChangedSig( const QString &spxSerialNumber );
    // DEKOMPRESSIONSEINSTELLUNGEN
    //! Signal wird gesendet wenn Gradienten verändert sind ( nutze: Qt::QueuedConnection )
    void decoGradientChangedSig( const DecoGradient &preset );
    //! Signal wenn "dynamische Gradienten" verändert wird
    void decoDynamicGradientStateChangedSig( const DecompressionDynamicGradient &isDynamic );
    //! Signal wenn "deep stops" geändert wird
    void decoDeepStopsEnabledSig( const DecompressionDeepstops &isEnabled );
    //! wenn der letzte Stop verändert wurde
    void decoLastStopSig( DecoLastStop lastStop );
    // DISPLAYEINSTELLUNGEN
    //! Signal wenn sich die Einstellung für Helligkeit ändert
    void displayBrightnessChangedSig( const DisplayBrightness &brightness );
    //! Signal wenn die Diesplayausrichtung verändert wird
    void displayOrientationChangedSig( const DisplayOrientation &orientation );
    // EINHEITEN
    //! Signal wenn die Temperatureinheit geändert wurde
    void unitsTemperaturChangedSig( const DeviceTemperaturUnit &tUnit );
    //! Signal wenn Längeneinheit geänder wird
    void unitsLengtChangedSig( const DeviceLenghtUnit &lUnit );
    //! Signal wenn Wassertyp geändert wird
    void untisWaterTypeChangedSig( const DeviceWaterType &wUnit );
    // SETPOINT
    //! Signal wenn autosetpoint geändert wird
    void setpointAutoChangeSig( const DeviceSetpointAuto &aSetpoint );
    //! Signal wenn setpoint geändert wurde
    void setpointValueChangedSig( const DeviceSetpointValue &ppo2 );
    // INDIVIDUAL
    //! Signal wenn sensoren an/aus geschaltet
    void individualSensorsOnChangedSig( const DeviceIndividualSensors &sensorMode );
    //! Signal wenn PSCR-Mode geändert wird
    void individualPscrModeChangedSig( const DeviceIndividualPSCR &pscrMode );
    //! Signal wenn Anzahl der Sensoren geändert
    void individualSensorsCountChangedSig( const DeviceIndividualSensorCount &sCount );
    //! Signal wenn Akustik an oder aus geshaltet wird
    void individualAcousticChangedSig( const DeviceIndividualAcoustic &acoustic );
    //! Signal wenn das interval geändert wurde
    void individualLogIntervalChangedSig( const DeviceIndividualLogInterval &lInterval );
    //! Signal, wenn sich der Stick verändert hat
    void individualTempstickChangedSig( const DeviceIndividualTempstick &tStick );
  };
}  // namespace spx
#endif  // SPX42CONFIG_HPP
