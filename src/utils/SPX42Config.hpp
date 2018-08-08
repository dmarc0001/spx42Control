﻿#ifndef SPX42CONFIG_HPP
#define SPX42CONFIG_HPP

#include <QHash>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "../config/ProjectConst.hpp"
#include "../config/SPX42Defs.hpp"
#include "SPX42Gas.hpp"

namespace spx
{
  class SPX42Config : public QObject
  {
    private:
    Q_OBJECT
    volatile bool sendSignals;  //! sollen beim stzen von Eigenschaften Signale gesendet werden?
    bool isValid;               //! Ist das Objekt gültig?
    SPX42License spxLicense;    //! SPX42 Lizenz
    QString serialNumber;       //! Seriennummer des aktuellen SPX42
    SPX42Gas gasList[ 8 ];      //! Gasliste des aktuellen SPX42
    // DEKOMPRESSIONSEINSTELLUNGEN
    DecompressionPreset currentPreset;                 //! Aktueller Typ der Dekompressionsgradienten
    DecoGradientHash decoPresets;                      //! Hashliste der DECO-Presets (incl. CUSTOM == variabel)
    DecompressionDynamicGradient decoDynamicGradient;  //! dynamische Gradienten erlaubt
    DecompressionDeepstops decoDeepstopsEnabled;       //! sind deepstops erlaubt/an
    // DISPLAYEINSTELLUNGEN
    DisplayBrightness displayBrightness;    //! Display Helligkeit
    DisplayOrientation displayOrientation;  //! Ausrichtung des Displays
    // EINHEITEN
    DeviceTemperaturUnit unitTemperature;  //! Einheit der Tempteratur
    DeviceLenghtUnit unitLength;           //! Einheit der Länge (metrisch/imperial)
    DeviceWaterType unitWaterType;         //! Typ des Wassers des SPX42
    // SETPOINT
    DeviceSetpointAuto setpointAuto;    //! in welcher Tiefe soll der Setpoint eingestellt werden
    DeviceSetpointValue setpointValue;  //! welcher PPO2 soll eingestellt sein
    // INDIVIDUAL
    DeviceIndividualSensors individualSensorsOn;        //! sind die Sensoren generell AN/AUS
    DeviceIndividualPSCR individualPSCROn;              //! Ist der SPX im PSCR Mode?
    DeviceIndividualSensorCount individualSensorCount;  //! wie viele Sensoren im SPX42 aktiv
    DeviceIndividualAcoustic individualAcustic;         //! sollen akustische Warnunge ausggeben werden
    DeviceIndividualLogInterval individualLogInterval;  //! welches Interval zum loggen
    DeviceIndividualTempstick individualTempStick;      //! welcher Tempstick wird genutzt?

    public:
    SPX42Config();                                     //! Der Konstruktor
    SPX42License &getLicense( void );                  //! Lizenz des aktuellen SPX42
    void setLicense( const LicenseType value );        //! Lizenz des aktuellen SPX42 merken
    void setLicense( const IndividualLicense value );  //! Lizenz des aktuellen SPX42 merken
    QString getLicName( void ) const;                  //! Textliche Darstellung der Lizenz
    SPX42Gas &getGasAt( int num );                     //! Gib ein Gas mit der Nummer num vom SPX42 zurück
    void reset( void );                                //! Resetiere das Objekt
    QString getSerialNumber( void ) const;             //! Seriennummer des aktuellen SPX42 zurückgeben
    void setSerialNumber( const QString &serial );     //! Seriennumemr des aktuellen SPX42 speichern
    // DEKOMPRESSIONSEINSTELLUNGEN
    void setCurrentPreset( DecompressionPreset presetType,
                           qint8 low = 0,
                           qint8 high = 0 );  //! Aktuelle Gradienteneinstellungen merken
    void setCurrentPreset( DecompressionPreset presetType,
                           const DecoGradient &dGradient );                    //! Aktuelle Gradienteneinstellungen merken
    DecompressionPreset getCurrentDecoGradientPresetType();                    //! Welcher Typ Gradient ist gesetzt?
    DecoGradient getCurrentDecoGradientValue() const;                          //! Gib die aktuelle Gradienteneinstellung zurück
    DecoGradient getPresetValues( DecompressionPreset presetType ) const;      //! Gib die Werte füe ein Preset zurück
    DecompressionPreset getPresetForGradient( qint8 low, qint8 high );         //! Gib den Preset für gegebene Werte zurück
    DecompressionDynamicGradient getIsDecoDynamicGradients( void );            //! gib den aktuellen Wert zurück
    void setIsDecoDynamicGradients( DecompressionDynamicGradient isEnabled );  //! setze dynamische Gradienten an/aus
    DecompressionDeepstops getIstDeepstopsEnabled( void );                     //! gibt die Einstellung für deep stops zurück
    void setIsDeepstopsEnabled( DecompressionDeepstops isEnabled );            //! setze deppstop enabled an/aus
    // DISPLAYEINSTELLUNGEN
    DisplayBrightness getDisplayBrightness( void );                //! Wie hell ist das Display?
    void setDisplayBrightness( DisplayBrightness brightness );     //! Setze die Displayhelligkeit
    DisplayOrientation getDisplayOrientation( void );              //! wie ist die Display Ausrichtung?
    void setDisplayOrientation( DisplayOrientation orientation );  //! Setzte die Displayausrichtung
    // EINHEITEN
    DeviceTemperaturUnit getUnitsTemperatur( void );        //! Welche Temperatureinheit dat das Gerät
    void setUnitsTemperatur( DeviceTemperaturUnit tUnit );  //! Setzte die Temperatureinheit des SPX42
    DeviceLenghtUnit getUnitsLength( void );                //! Welche Längeneinheit nutzt das Gerät
    void setUnitsLength( DeviceLenghtUnit lUnit );          //! Setzte die Temperatureinheit
    DeviceWaterType getUnitsWaterType( void );              //! Welchen Wassertyp hat das Gerät eingestellt
    void setUnitsWaterType( DeviceWaterType wUnit );        //! Setzte den Wassertyp
    // SETPOINT
    DeviceSetpointAuto getSetpointAuto( void );            //! Welchen Autosetpoint nutz das Gerät (Tiefe)
    void setSetpointAuto( DeviceSetpointAuto aSetpoint );  //! setzte den Autosetpoint
    DeviceSetpointValue getSetpointValue( void );          //! Welchen PPO2 benutzt der SPX42
    void setSetpointValue( DeviceSetpointValue ppo2 );     //! setzte den Setpoint
    // INDIVIDUAL
    DeviceIndividualSensors getIndividualSensorsOn( void );                    //! Sind die Sensoren an?
    void setIndividualSensorsOn( DeviceIndividualSensors onOff );              //! Setze Sensoren an/aus
    DeviceIndividualPSCR getIndividualPscrMode( void );                        //! ist der SPX im PSCR Modus
    void setIndividualPscrMode( DeviceIndividualPSCR pscrMode );               //! Setzte PSCR-Mode
    DeviceIndividualSensorCount getIndividualSensorsCount( void );             //! Wie viele Sensoren sind aktiv
    void setIndividualSensorsCount( DeviceIndividualSensorCount sCount );      //! Setzte Anzahl aktiver Sensoren
    DeviceIndividualAcoustic getIndividualAcoustic( void );                    //! wie ist der Status der Akustischen Meldungen
    void setIndividualAcoustic( DeviceIndividualAcoustic acoustic );           //! setzte Akustik an oder aus
    DeviceIndividualLogInterval getIndividualLogInterval( void );              //! wie ist das Log interval
    void setIndividualLogInterval( DeviceIndividualLogInterval logInterval );  //! setze das Log interval
    DeviceIndividualTempstick getIndividualTempStick( void );                  //! welcher Temperatur Stick ist eingebaut?
    void setIndividualTempStick( DeviceIndividualTempstick tStick );           //! setze den eingebauten TemperaturStick
    private slots:
    // void licenseChangedPrivateSlot( SPX42License& lic );

    signals:
    void licenseChangedSig( const SPX42License &lic );           //! Signal wenn die lizenz verändert wird
    void serialNumberChangedSig( const QString &serialNumber );  //! Signal wenn die Seriennummer neu gesetzt wird
    // DEKOMPRESSIONSEINSTELLUNGEN
    void decoGradientChangedSig(
        const DecoGradient &preset );  //! Signal wird gesendet wenn Gradienten verändert sind ( nutze: Qt::QueuedConnection )
    void decoDynamicGradientStateChangedSig(
        const DecompressionDynamicGradient &isDynamic );                      //! Signal wenn "dynamische Gradienten" verändert wird
    void decoDeepStopsEnabledSig( const DecompressionDeepstops &isEnabled );  //! Signal wenn "deep stops" geändert wird
    // DISPLAYEINSTELLUNGEN
    void displayBrightnessChangedSig(
        const DisplayBrightness &brightness );  //! Signal wenn sich die Einstellung für Helligkeit ändert
    void displayOrientationChangedSig( const DisplayOrientation &orientation );  //! Signal wenn die Diesplayausrichtung verändert wird
    // EINHEITEN
    void unitsTemperaturChangedSig( const DeviceTemperaturUnit &tUnit );  //! Signal wenn die Temperatureinheit geändert wurde
    void unitsLengtChangedSig( const DeviceLenghtUnit &lUnit );           //! Signal wenn Längeneinheit geänder wird
    void untisWaterTypeChangedSig( const DeviceWaterType &wUnit );        //! Signal wenn Wassertyp geändert wird
    // SETPOINT
    void setpointAutoChangeSig( const DeviceSetpointAuto &aSetpoint );  //! Signal wenn autosetpoint geändert wird
    void setpointValueChangedSig( const DeviceSetpointValue &ppo2 );    //! Signal wenn setpoint geändert wurde
    // INDIVIDUAL
    void individualSensorsOnChangedSig( const DeviceIndividualSensors &sensorMode );     //! Signal wenn sensoren an/aus geschaltet
    void individualPscrModeChangedSig( const DeviceIndividualPSCR &pscrMode );           //! Signal wenn PSCR-Mode geändert wird
    void individualSensorsCountChangedSig( const DeviceIndividualSensorCount &sCount );  //! Signal wenn Anzahl der Sensoren geändert
    void individualAcousticChangedSig( const DeviceIndividualAcoustic &acoustic );  //! Signal wenn Akustik an oder aus geshaltet wird
    void individualLogIntervalChangedSig( const DeviceIndividualLogInterval &lInterval );  //! Signal wenn das interval geändert wurde
    void individualTempstickChangedSig( const DeviceIndividualTempstick &tStick );  //! Signal, wenn sich der Stick verändert hat
  };
}  // namespace spx
#endif  // SPX42CONFIG_HPP
