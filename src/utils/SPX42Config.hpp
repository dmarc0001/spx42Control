#ifndef SPX42CONFIG_HPP
#define SPX42CONFIG_HPP

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QHash>

#include "../config/SPX42Defs.hpp"
#include "SPX42Gas.hpp"
#include "../config/ProjectConst.hpp"

namespace spx42
{
  class SPX42Config : public QObject
  {
    private:
      Q_OBJECT
      volatile bool sendSignals;                                               //! sollen beim stzen von Eigenschaften Signale gesendet werden?
      bool isValid;                                                            //! Ist das Objekt gültig?
      SPX42License spxLicense;                                                 //! SPX42 Lizenz
      QString serialNumber;                                                    //! Seriennummer des aktuellen SPX42
      SPX42Gas gasList[8];                                                     //! Gasliste des aktuellen SPX42
      // DEKOMPRESSIONSEINSTELLUNGEN
      DecompressionPreset currentPreset;                                       //! Aktueller Typ der Dekompressionsgradienten
      DecoGradientHash decoPresets;                                            //! Hashliste der DECO-Presets (incl. CUSTOM == variabel)
      bool decoDynamicGradient;                                                //! dynamische Gradienten erlaubt
      bool decoDeepstopsEnabled;                                               //! sind deepstops erlaubt/an
      // DISPLAYEINSTELLUNGEN
      DisplayBrightness displayBrightness;                                     //! Display Helligkeit
      DisplayOrientation displayOrientation;                                   //! Ausrichtung des Displays
      // EINHEITEN
      DeviceTemperaturUnit unitTemperature;                                    //! Einheit der Tempteratur
      DeviceLenghtUnit unitLength;                                             //! Einheit der Länge (metrisch/imperial)
      DeviceWaterType unitWaterType;                                           //! Typ des Wassers des SPX42
      // SETPOINT
      DeviceSetpointAuto setpointAuto;                                         //! in welcher Tiefe soll der Setpoint eingestellt werden
      DeviceSetpointValue setpointValue;                                       //! welcher PPO2 soll eingestellt sein

    public:
      SPX42Config();                                                           //! Der Konstruktor
      SPX42License& getLicense(void);                                          //! Lizenz des aktuellen SPX42
      void setLicense(const SPX42License value);                               //! Lizenz des aktuellen SPX42 merken
      QString getLicName(void) const;                                          //! Textliche Darstellung der Lizenz
      SPX42Gas& getGasAt( int num );                                           //! Gib ein Gas mit der Nummer num vom SPX42 zurück
      void reset(void);                                                        //! Resetiere das Objekt
      QString getSerialNumber(void) const;                                     //! Seriennummer des aktuellen SPX42 zurückgeben
      void setSerialNumber(const QString& serial);                             //! Seriennumemr des aktuellen SPX42 speichern
      // DEKOMPRESSIONSEINSTELLUNGEN
      void setCurrentPreset( DecompressionPreset presetType, qint8 low=0, qint8 high=0 ); //! Aktuelle Gradienteneinstellungen merken
      DecompressionPreset getCurrentDecoGradientPresetType();                  //! Welcher Typ Gradient ist gesetzt?
      DecoGradient getCurrentDecoGradientValue();                              //! Gib die aktuelle Gradienteneinstellung zurück
      DecoGradient getPresetValues( DecompressionPreset presetType ) const;    //! Gib die Werte füe ein Preset zurück
      DecompressionPreset getPresetForGradient( qint8 low, qint8 high );       //! Gib den Preset für gegebene Werte zurück
      bool getIsDecoDynamicGradients( void );                                  //! gib den aktuellen Wert zurück
      void setIsDecoDynamicGradients( bool isEnabled );                        //! setze dynamische Gradienten an/aus
      bool getIstDeepstopsEnabled( void );                                     //! gibt die Einstellung für deep stops zurück
      void setIsDeepstopsEnabled( bool isEnabled );                            //! setze deppstop enabled an/aus
      // DISPLAYEINSTELLUNGEN
      DisplayBrightness getDisplayBrightness( void );                          //! Wie hell ist das Display?
      void setDisplayBrightness( DisplayBrightness brightness );               //! Setze die Displayhelligkeit
      DisplayOrientation getDisplayOrientation( void );                        //! wie ist die Display Ausrichtung?
      void setDisplayOrientation( DisplayOrientation orientation );            //! Setzte die Displayausrichtung
      // EINHEITEN
      DeviceTemperaturUnit getUnitsTemperatur( void );                         //! Welche Temperatureinheit dat das Gerät
      void setUnitsTemperatur( DeviceTemperaturUnit tUnit );                   //! Setzte die Temperatureinheit des SPX42
      DeviceLenghtUnit getUnitsLength( void );                                 //! Welche Längeneinheit nutzt das Gerät
      void setUnitsLength( DeviceLenghtUnit lUnit );                           //! Setzte die Temperatureinheit
      DeviceWaterType getUnitsWaterType( void );                               //! Welchen Wassertyp hat das Gerät eingestellt
      void setUnitsWaterType( DeviceWaterType wUnit );                         //! Setzte den Wassertyp
      // SETPOINT
      DeviceSetpointAuto getSetpointAuto( void );                              //! Welchen Autosetpoint nutz das Gerät (Tiefe)
      void setSetpointAuto( DeviceSetpointAuto aSetpoint );                    //! setzte den Autosetpoint
      DeviceSetpointValue getSetpointValue( void );                            //! Welchen PPO2 benutzt der SPX42
      void setSetpointValue( DeviceSetpointValue ppo2 );                       //! setzte den Setpoint

    private slots:
      void licenseChangedPrivateSlot(void);

    signals:
      void licenseChangedSig( SPX42License& lic );                             //! Signal wenn die lizenz verändert wird
      void serialNumberChangedSig( QString serialNumber );                     //! Signal wenn die Seriennummer neu gesetzt wird
      // DEKOMPRESSIONSEINSTELLUNGEN
      void decoGradientChangedSig( DecoGradient preset );                      //! Signal wird gesendet wenn Gradienten verändert sind ( nutze: Qt::QueuedConnection )
      void decoDynamicGradientStateChangedSig( bool isDynamic );               //! Signal wenn "dynamische Gradienten" verändert wird
      void decoDeepStopsEnabledSig( bool isEnabled );                          //! Signal wenn "deep stops" geändert wird
      // DISPLAYEINSTELLUNGEN
      void displayBrightnessChangedSig( DisplayBrightness brightness );        //! Signal wenn sich die Einstellung für Helligkeit ändert
      void displayOrientationChangedSig( DisplayOrientation orientation );     //! Signal wenn die Diesplayausrichtung verändert wird
      // EINHEITEN
      void unitsTemperaturChangedSig( DeviceTemperaturUnit tUnit );            //! Signal wenn die Temperatureinheit geändert wurde
      void unitsLengtChangedSig( DeviceLenghtUnit lUnit );                     //! Signal wenn Längeneinheit geänder wird
      void untisWaterTypeChangedSig( DeviceWaterType wUnit );                  //! Signal wenn Wassertyp geändert wird
      // SETPOINT
      void setpointAutoChangeSig( DeviceSetpointAuto aSetpoint );              //! Signal wenn autosetpoint geändert wird
      void setpointValueChangedSig( DeviceSetpointValue ppo2 );                //! Signal wenn setpoint geändert wurde
  };
}
#endif // SPX42CONFIG_HPP
