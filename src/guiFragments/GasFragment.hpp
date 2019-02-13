#ifndef GASFORM_HPP
#define GASFORM_HPP

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QRadioButton>
#include <QRegExp>
#include <QSpinBox>
#include <QWidget>
#include <array>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "config/AppConfigClass.hpp"
#include "config/ProjectConst.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Config.hpp"
#include "spx42/SPX42Defs.hpp"

namespace Ui
{
  class GasForm;
}

namespace spx
{
#define QSpinboxIntValueChanged static_cast< void ( QSpinBox::* )( int ) >( &QSpinBox::valueChanged )

  class GasFragment : public QWidget, IFragmentInterface
  {
    private:
    Q_OBJECT
    Q_INTERFACES( spx::IFragmentInterface )
    //! Zeiger auf GUI-Objekte
    std::unique_ptr< Ui::GasForm > ui;
    //! Ich merke mir, ob die Slots verbunden sind
    bool areSlotsConnected;
    //! gasliste
    QString gasSummaryTemplate;
    //! Titel der aktuellen bearbeitung
    QString gasCurrentBoxTitleTemplate;
    //! aktuelles Gas (1 bis 8)
    int currentGasNum;
    //! ignoriere, wenn ich das gas via programm setzte
    bool ignoreGasGuiEvents;
    //! rechnen wir mit Süßwasser?
    DeviceWaterType waterCompute;
    //! Gas Radiobuttons
    QList< QRadioButton * > gasRadios;

    public:
    //! Konstruktor
    explicit GasFragment( QWidget *parent,
                          std::shared_ptr< Logger > logger,
                          std::shared_ptr< SPX42Database > spx42Database,
                          std::shared_ptr< SPX42Config > spxCfg,
                          std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                          AppConfigClass *appCfg );
    //! der Zerstörer
    ~GasFragment() override;

    private:
    //! Initialisiere die GUI mit Werten aus der Config
    void initGuiWithConfig( void );
    //! verbunden oder nicht
    void setGuiConnected( bool connected );
    //! verbinde Slots mit Signalen
    void connectSlots( void );
    //! trenne Slots von Signalen
    void disconnectSlots( void );
    //! GUI events machen
    void connectGasSlots( void );
    //! gui nach aktuellem Gas aktualisieren
    void updateCurrGasGUI( int gasNum, bool withCurrent = true );
    void gasSelect( int gasNum, bool isSelected );

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
    //! wenn es Fehler in der BT Verbindung gibt
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;
    //! Wenn sich die Lizenz ändert
    virtual void onConfLicChangedSlot( void ) override;
    //! wenn die Datenbank geschlosen wird
    virtual void onCloseDatabaseSlot( void ) override;
    //! wenn ein Datentelegramm empfangen wurde
    virtual void onCommandRecivedSlot( void ) override;
    //! Konfiguration abrufen
    void onGasConfigUpdateSlot( void );
    //! O2 Wert eines Gases hat sich verändert
    void onSpinO2ValueChangedSlot( int o2Val );
    //! HE Wert eines Gases hat sich verändert
    void onSpinHeValueChangedSlot( int heVal );
    //! wenn sich das Diluent ändert
    void onDiluentUseChangeSlot( int state, DiluentType which );
    //! wenn sich das Bailout ändert
    void onBailoutCheckChangeSlot( int state );
    //! wenn sich der wassertyp zum berechnen ändert
    void onWaterTypeChanged( int state );
  };
}  // namespace spx
#endif  // GASFORM_HPP
