#ifndef DEVICECONFIG_HPP
#define DEVICECONFIG_HPP

#include <QWidget>
#include <memory>
#include "IFragmentInterface.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "utils/SPX42Config.hpp"

namespace Ui
{
  class DeviceConfig;
}

namespace spx
{
  /**
   * @brief Klasse für das Management der Geräteeinstellungen
   */
  class DeviceConfigFragment : public QWidget, IFragmentInterface
  {
    private:
    Q_OBJECT
    std::unique_ptr< Ui::DeviceConfig > ui;  //! Smart-Zeiger für das UI Objekt (automatische Speicherverwaltung)
    bool volatile gradentSlotsIgnore;        //! Um Schleifen zu vermeiden kontrolliert Callbacks ignorieren

    public:
    explicit DeviceConfigFragment( QWidget *parent,
                                   std::shared_ptr< Logger > logger,
                                   std::shared_ptr< SPX42Database > spx42Database,
                                   std::shared_ptr< SPX42Config > spxCfg );  //! Konstruktor
    ~DeviceConfigFragment() override;                                        //! Destruktor
    void initGuiWithConfig( void );                                          //! Initialisiere die GUI mit Werten aus der Config

    protected:
    void changeEvent( QEvent *e ) override;  //! Globele Veränderungen

    private:
    void setGuiForDecompression( void );                                  //! GUI nach spxConfig einstellen
    void setGuiForDisplay( void );                                        //! GUI nach spxConfig einstellen
    void setGuiForUnits( void );                                          //! GUI nach spxConfig einstellen
    void setGuiForSetpoint( void );                                       //! GUI nach spxConfig einstellen
    void setGuiForLicense( void );                                        //! GUI nach spxConfig einstellen
    void setGuiForIndividual( void );                                     //! GUI nach spxConfig einstellen
    void disconnectSlots( void );                                         //! Trenne die Verbindung von Signalen und Slots
    void setGradientPresetWithoutCallback( DecompressionPreset preset );  //! Gradienten-Combobox setzten ohne Callback auszuführen

    private slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;  //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onConfLicChangedSlot( void ) override;                //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                 //! wenn die Datenbank geschlosen wird
    // DEKOMPRESSIONSEINSTELLUNGEN
    void onDecoComboChangedSlot( int index );                 //! ändert sich der Inhalt der Combobox für Dekompressionseinstellungen
    void onDecoGradientLowChangedSlot( int low );             //! wenn der Gradient LOW geändert wurde
    void onDecoGradientHighChangedSlot( int high );           //! wenn der Gradient HIGH geändert wurde
    void onDecoDynamicGradientStateChangedSlot( int state );  //! wenn sich die Einstellung "dynamische Gradienten" ändert
    void onDecoDeepStopsEnableChangedSlot( int state );       //! wenn sich die einstellung "deep stops enable" ändert
    // DISPLAYEINSTELLUNGEN
    void onDisplayBrightnessChangedSlot( int index );   //! wenn sich die Einstellung Helligkeit des Display verändert
    void onDisplayOrientationChangedSlot( int index );  //! wenn sich die Orientierung verändert
    // EINHEITENSYSTEM
    void onUnitsTemperatureChangedSlot( int index );  //! wenn sich die Einstellung für die Einheit (Celsius/Fahrenheid) ändert
    void onUnitsLengthChangedSlot( int index );       //! wenn sich die Einstellung für die Einheit (Fuß/Meter) ändert
    void onUnitsWatertypeChangedSlot( int index );    //! wenn sich die Einstellung für den Wassertyp ändert
    // SETPOINTS
    void onSetpointAutoChangedSlot( int index );   //! wenn sich die Einstellung für Autosetpoiunt ändert
    void onSetpointValueChangedSlot( int index );  //! wenn sich der Wert des Autosetpoints verändert
    // INDIVIDUAL
    void onIndividualSensorsOnChangedSlot( int state );     //! wenn sich die Einstellung Sensoren on/off ändert
    void onInIndividualPscrModeChangedSlot( int state );    //! wenn sich die Einstellung für den PSCR Mode ändert
    void onIndividualSensorsCountChangedSlot( int index );  //! wenn sich die Einstellung für die Anzahl der Sensoren ändert
    void onIndividualAcousticChangedSlot( int state );      //! wenn sich ide Einstellung der Akustischen Warnung ändert
    void onIndividualLogIntervalChangedSlot( int state );   //! wenn sich die Einstellung des Logintervals ändert
  };
}
#endif  // DEVICECONFIG_HPP
