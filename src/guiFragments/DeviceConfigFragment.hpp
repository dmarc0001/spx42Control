#ifndef DEVICECONFIG_HPP
#define DEVICECONFIG_HPP

#include <QBluetoothSocket>
#include <QByteArray>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Config.hpp"

namespace Ui
{
  class DeviceConfig;
}

namespace spx
{
  //
  // Konstanten für dieses Modul um die connect/disconnect sachen
  // übersichtlicher und lesbarer zu machen
  // die Kostanten repräsentieren BITS
  //
  constexpr quint8 SIGNALS_DISPLAY = 0x01;
  constexpr quint8 SIGNALS_UNITS = 0x02;
  constexpr quint8 SIGNALS_SETPOINT = 0x04;
  constexpr quint8 SIGNALS_LICENSE = 0x08;
  constexpr quint8 SIGNALS_INDIVIDUAL = 0x10;
  constexpr quint8 SIGNALS_DECOMPRESSION = 0x20;
  constexpr quint8 SIGNALS_PROGRAM = 0x80;
  constexpr quint8 SIGNALS_ALL = 0xff;

  /**
   * @brief Klasse für das Management der Geräteeinstellungen
   */
  class DeviceConfigFragment : public QWidget, IFragmentInterface
  {
    private:
    Q_OBJECT
    Q_INTERFACES( spx::IFragmentInterface )
    std::unique_ptr< Ui::DeviceConfig > ui;  //! Smart-Zeiger für das UI Objekt (automatische Speicherverwaltung)
    bool volatile gradentSlotsIgnore;        //! Um Schleifen zu vermeiden kontrolliert Callbacks ignorieren
    QStringList oldAutoSetpoint;             //! Liste für Combobox mit älterer Firmware
    QStringList newAutoSetpoint;             //! Liste mit Einträgen für Combobox für neuere Firmware
    QStringList oldDisplayBrightness;        //! alte Helligkeitswerte
    QStringList newDisplayBrightness;        //! neue Helligkeitswerte
    QTimer configComputeHashTimer;           //! timer soll nach dem Empfang von Einstellunge das berechnen eines hash verzögern
    QTimer configWriteTimer;                 //! Nach Änderungen der Konfiguration zurück zum SPX schreiben (etwas verzögert)

    public:
    explicit DeviceConfigFragment( QWidget *parent,
                                   std::shared_ptr< Logger > logger,
                                   std::shared_ptr< SPX42Database > spx42Database,
                                   std::shared_ptr< SPX42Config > spxCfg,
                                   std::shared_ptr< SPX42RemotBtDevice > remSPX42 );  //! Konstruktor
    ~DeviceConfigFragment() override;                                                 //! Destruktor
    virtual void deactivateTab( void ) override;                                      //! deaktiviere eventuelle signale
    void initGuiWithConfig( void );  //! Initialisiere die GUI mit Werten aus der Config

    protected:
    void changeEvent( QEvent *e ) override;  //! Globele Veränderungen

    private:
    void setGuiForDecompression( void );                                  //! GUI nach spxConfig einstellen
    void setGuiForDisplay( void );                                        //! GUI nach spxConfig einstellen
    void setGuiForUnits( void );                                          //! GUI nach spxConfig einstellen
    void setGuiForSetpoint( void );                                       //! GUI nach spxConfig einstellen
    void setGuiForLicense( void );                                        //! GUI nach spxConfig einstellen
    void setGuiForIndividual( void );                                     //! GUI nach spxConfig einstellen
    void disconnectSlots( quint8 which_signals = SIGNALS_ALL );           //! Trenne die Verbindung von Signalen und Slots
    void connectSlots( quint8 which_signals = SIGNALS_ALL );              //! verbinden von Signalen und Slots
    void setGradientPresetWithoutCallback( DecompressionPreset preset );  //! Gradienten-Combobox setzten ohne Callback auszuführen
    void setGuiConnected( bool connected );                               //! GUI verbunden oder offline zeigen
    void updateGuiFirmwareSpecific( void );                               //! GUI Firmwarespezifisch korrigieren

    signals:
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;   //! eine Warnmeldung soll das Main darstellen
    void onAkkuValueChangedSlot( double aValue ) override;                          //! signalisiert, dass der Akku eine Spanniung hat

    private slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! ein Fehler bei der BT Verbindung
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird
    virtual void onCommandRecivedSlot( void ) override;                              //! wenn ein Datentelegramm empfangen wurde
    void onDecoComboChangedSlot( int index );                 //! ändert sich der Inhalt der Combobox für Dekompressionseinstellungen
    void onDecoGradientLowChangedSlot( int low );             //! wenn der Gradient LOW geändert wurde
    void onDecoGradientHighChangedSlot( int high );           //! wenn der Gradient HIGH geändert wurde
    void onDecoDynamicGradientStateChangedSlot( int state );  //! wenn sich die Einstellung "dynamische Gradienten" ändert
    void onDecoDeepStopsEnableChangedSlot( int state );       //! wenn sich die einstellung "deep stops enable" ändert
    void onDecoLastStopChangedSlot( int index );              //! wenn sich die "last decostop" combobox ändert
    void onDisplayBrightnessChangedSlot( int index );         //! wenn sich die Einstellung Helligkeit des Display verändert
    void onDisplayOrientationChangedSlot( int index );        //! wenn sich die Orientierung verändert
    void onUnitsTemperatureChangedSlot( int index );        //! wenn sich die Einstellung für die Einheit (Celsius/Fahrenheid) ändert
    void onUnitsLengthChangedSlot( int index );             //! wenn sich die Einstellung für die Einheit (Fuß/Meter) ändert
    void onUnitsWatertypeChangedSlot( int index );          //! wenn sich die Einstellung für den Wassertyp ändert
    void onSetpointAutoChangedSlot( int index );            //! wenn sich die Einstellung für Autosetpoiunt ändert
    void onSetpointValueChangedSlot( int index );           //! wenn sich der Wert des Autosetpoints verändert
    void onIndividualSensorsOnChangedSlot( int state );     //! wenn sich die Einstellung Sensoren on/off ändert
    void onInIndividualPscrModeChangedSlot( int state );    //! wenn sich die Einstellung für den PSCR Mode ändert
    void onIndividualSensorsCountChangedSlot( int index );  //! wenn sich die Einstellung für die Anzahl der Sensoren ändert
    void onIndividualAcousticChangedSlot( int state );      //! wenn sich ide Einstellung der Akustischen Warnung ändert
    void onIndividualLogIntervalChangedSlot( int state );   //! wenn sich die Einstellung des Logintervals ändert
    void onIndividualTempstickChangedSlot( int state );     //! wenn sich die Einstellung für den TempStick ändert
    void onConfigUpdateSlot( void );  //! ereignis wenn der Timer abgelaufen ist für das Update der Einstellungen (vom spx lesen)
  };
}
#endif  // DEVICECONFIG_HPP
