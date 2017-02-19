#ifndef DEVICECONFIG_HPP
#define DEVICECONFIG_HPP

#include <memory>
#include <QWidget>

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "IFragmentInterface.hpp"



namespace Ui {
  class DeviceConfig;
}

namespace spx42
{
  /**
   * @brief Klasse für das Management der Geräteeinstellungen
   */
  class DeviceConfigFragment : public QWidget, IFragmentInterface
  {
    private:
      Q_OBJECT
      std::unique_ptr<Ui::DeviceConfig> ui;                     //! Smart-Zeiger für das UI Objekt (automatische Speicherverwaltung)
      bool areSlotsConnected;                                   //! Ich merke mir, ob die Slots verbunden sind
      bool volatile gradentSlotsIgnore;                         //! Um Schleifen zu vermeiden kontrolliert Callbacks ignorieren

    public:
      explicit DeviceConfigFragment(QWidget *parent, std::shared_ptr<Logger> logger , std::shared_ptr<SPX42Config> spxCfg); //! Konstruktor
      ~DeviceConfigFragment();                                  //! Destruktor
      void initGuiWithConfig( void );                           //! Initialisiere die GUI mit Werten aus der Config

    protected:
      void changeEvent(QEvent *e);                              //! Globele Veränderungen

    private:
      void connectSlots( void );                                //! verbinde signale mit Slots
      void disconnectSlots( void );                             //! Trenne die Verbindung von Signalen und Slots
      void setGradientPresetWithoutCallback( DecompressionPreset preset ); //! Gradienten-Combobox setzten ohne Callback auszuführen

    private slots:
      void licChangedSlot( SPX42License& lic );                 //! Wenn sich die Lizenz ändert
      // DEKOMPRESSIONSEINSTELLUNGEN
      void decoComboChangedSlot( int index );                   //! ändert sich der Inhalt der Combobox für Dekompressionseinstellungen
      void decoGradientLowChangedSlot( int low );               //! wenn der Gradient LOW geändert wurde
      void decoGradientHighChangedSlot( int high );             //! wenn der Gradient HIGH geändert wurde
      void decoDynamicGradientStateChangedSlot( int state );    //! wenn sich die Einstellung "dynamische Gradienten" ändert
      void decoDeepStopsEnableChangedSlot( int state );         //! wenn sich die einstellung "deep stops enable" ändert
      // DISPLAYEINSTELLUNGEN
      void displayBrightnessChangedSlot( int index );           //! wenn sich die Einstellung Helligkeit des Display verändert
      void displayOrientationChangedSlot( int index );          //! wenn sich die Orientierung verändert
      // EINHEITENSYSTEM
      void unitsTemperatureChangedSlot( int index );            //! wenn sich die Einstellung für die Einheit (Celsius/Fahrenheid) ändert
      void unitsLengthChangedSlot( int index );                 //! wenn sich die Einstellung für die Einheit (Fuß/Meter) ändert
      void unitsWatertypeChangedSlot( int index );              //! wenn sich die Einstellung für den Wassertyp ändert
      // SETPOINTS
      void setpointAutoChangedSlot( int index );                //! wenn sich die Einstellung für Autosetpoiunt ändert
      void setpointValueChangedSlot( int index );               //! wenn sich der Wert des Autosetpoints verändert
      // INDIVIDUAL
      void individualSensorsOnChangedSlot( int state );         //! wenn sich die Einstellung Sensoren on/off ändert
      void individualPscrModeChangedSlot( int state );          //! wenn sich die Einstellung für den PSCR Mode ändert
      void individualSensorsCountChangedSlot( int index );      //! wenn sich die Einstellung für die Anzahl der Sensoren ändert
      void individualAcousticChangedSlot( int state );          //! wenn sich ide Einstellung der Akustischen Warnung ändert
      void individualLogIntervalChangedSlot( int state );       //! wenn sich die Einstellung des Logintervals ändert
  };
}
#endif // DEVICECONFIG_HPP
