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
#include "config/AppConfigClass.hpp"
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
    //! Smart-Zeiger für das UI Objekt (automatische Speicherverwaltung)
    std::unique_ptr< Ui::DeviceConfig > ui;
    //! Um Schleifen zu vermeiden kontrolliert Callbacks ignorieren
    bool volatile gradentSlotsIgnore;
    //! Liste für Combobox mit älterer Firmware
    QStringList oldAutoSetpoint;
    //! Liste mit Einträgen für Combobox für neuere Firmware
    QStringList newAutoSetpoint;
    //! alte Helligkeitswerte
    QStringList oldDisplayBrightness;
    //! neue Helligkeitswerte
    QStringList newDisplayBrightness;
    //! Template für die Überschrift
    QString configHeadlineTemplate;

    public:
    //! Konstruktor
    explicit DeviceConfigFragment( QWidget *parent,
                                   std::shared_ptr< Logger > logger,
                                   std::shared_ptr< SPX42Database > spx42Database,
                                   std::shared_ptr< SPX42Config > spxCfg,
                                   std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                                   AppConfigClass *appCfg );
    //! Destruktor
    ~DeviceConfigFragment() override;
    //! Initialisiere die GUI mit Werten aus der Config
    void initGuiWithConfig( void );

    protected:
    //! Globele Veränderungen
    void changeEvent( QEvent *e ) override;

    private:
    //! GUI nach spxConfig einstellen
    void setGuiForDecompression( void );
    //! GUI nach spxConfig einstellen
    void setGuiForDisplay( void );
    //! GUI nach spxConfig einstellen
    void setGuiForUnits( void );
    //! GUI nach spxConfig einstellen
    void setGuiForSetpoint( void );
    //! GUI nach spxConfig einstellen
    void setGuiForLicense( void );
    //! GUI nach spxConfig einstellen
    void setGuiForIndividual( void );
    //! Trenne die Verbindung von Signalen und Slots
    void disconnectSlots( quint8 which_signals = SIGNALS_ALL );
    //! verbinden von Signalen und Slots
    void connectSlots( quint8 which_signals = SIGNALS_ALL );
    //! Gradienten-Combobox setzten ohne Callback auszuführen
    void setGradientPresetWithoutCallback( DecompressionPreset preset );
    //! GUI verbunden oder offline zeigen
    void setGuiConnected( bool connected );
    //! GUI Firmwarespezifisch korrigieren
    void updateGuiFirmwareSpecific( void );

    signals:
    //! eine Warnmeldung soll das Main darstellen
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;
    //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;
    //! signalisiert, dass der Akku eine Spanniung hat
    void onAkkuValueChangedSig( double aValue ) override;
    //! signalisieret das ich was in die Config geschrieben habe
    void onConfigWasChangedSig( void );

    public slots:
    void onSendBufferStateChangedSlot( bool isBusy );

    private slots:
    //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;
    //! ein Fehler bei der BT Verbindung
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;
    //! Wenn sich die Lizenz ändert
    virtual void onConfLicChangedSlot( void ) override;
    //! wenn die Datenbank geschlosen wird
    virtual void onCloseDatabaseSlot( void ) override;
    //! wenn ein Datentelegramm empfangen wurde
    virtual void onCommandRecivedSlot( void ) override;
    //! ändert sich der Inhalt der Combobox für Dekompressionseinstellungen
    void onDecoComboChangedSlot( int index );
    //! wenn der Gradient LOW geändert wurde
    void onDecoGradientLowChangedSlot( int low );
    //! wenn der Gradient HIGH geändert wurde
    void onDecoGradientHighChangedSlot( int high );
    //! wenn sich die Einstellung "dynamische Gradienten" ändert
    void onDecoDynamicGradientStateChangedSlot( int state );
    //! wenn sich die einstellung "deep stops enable" ändert
    void onDecoDeepStopsEnableChangedSlot( int state );
    //! wenn sich die "last decostop" combobox ändert
    void onDecoLastStopChangedSlot( int index );
    //! wenn sich die Einstellung Helligkeit des Display verändert
    void onDisplayBrightnessChangedSlot( int index );
    //! wenn sich die Orientierung verändert
    void onDisplayOrientationChangedSlot( int index );
    //! wenn sich die Einstellung für die Einheit (Celsius/Fahrenheid) ändert
    void onUnitsTemperatureChangedSlot( int index );
    //! wenn sich die Einstellung für die Einheit (Fuß/Meter) ändert
    void onUnitsLengthChangedSlot( int index );
    //! wenn sich die Einstellung für den Wassertyp ändert
    void onUnitsWatertypeChangedSlot( int index );
    //! wenn sich die Einstellung für Autosetpoiunt ändert
    void onSetpointAutoChangedSlot( int index );
    //! wenn sich der Wert des Autosetpoints verändert
    void onSetpointValueChangedSlot( int index );
    //! wenn sich die Einstellung Sensoren on/off ändert
    void onIndividualSensorsOnChangedSlot( int state );
    //! wenn sich die Einstellung für den PSCR Mode ändert
    void onInIndividualPscrModeChangedSlot( int state );
    //! wenn sich die Einstellung für die Anzahl der Sensoren ändert
    void onIndividualSensorsCountChangedSlot( int index );
    //! wenn sich ide Einstellung der Akustischen Warnung ändert
    void onIndividualAcousticChangedSlot( int state );
    //! wenn sich die Einstellung des Logintervals ändert
    void onIndividualLogIntervalChangedSlot( int state );
    //! wenn sich die Einstellung für den TempStick ändert
    void onIndividualTempstickChangedSlot( int state );
    //! ereignis wenn der Timer abgelaufen ist für das Update der Einstellungen (vom spx lesen)
    void onConfigUpdateSlot( void );
  };
}  // namespace spx
#endif  // DEVICECONFIG_HPP
