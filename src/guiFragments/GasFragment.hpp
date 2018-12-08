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
    std::unique_ptr< Ui::GasForm > ui;   //! Zeiger auf GUI-Objekte
    bool areSlotsConnected;              //! Ich merke mir, ob die Slots verbunden sind
    QString gasSummaryTemplate;          //! gasliste
    QString gasCurrentBoxTitleTemplate;  //! Titel der aktuellen bearbeitung
    int currentGasNum;                   //! aktuelles Gas (1 bis 8)
    bool ignoreGasGuiEvents;             //! ignoriere, wenn ich das gas via programm setzte
    DeviceWaterType waterCompute;        //! rechnen wir mit Süßwasser?
    QList< QRadioButton * > gasRadios;   //! Gas Radiobuttons

    public:
    explicit GasFragment( QWidget *parent,
                          std::shared_ptr< Logger > logger,
                          std::shared_ptr< SPX42Database > spx42Database,
                          std::shared_ptr< SPX42Config > spxCfg,
                          std::shared_ptr< SPX42RemotBtDevice > remSPX42 );  //! Konstruktor
    ~GasFragment() override;                                                 //! der Zerstörer
    virtual void deactivateTab( void ) override;                             //! deaktiviere eventuelle signale

    private:
    void initGuiWithConfig( void );                                //! Initialisiere die GUI mit Werten aus der Config
    void setGuiConnected( bool connected );                        //! verbunden oder nicht
    void connectSlots( void );                                     //! verbinde Slots mit Signalen
    void disconnectSlots( void );                                  //! trenne Slots von Signalen
    void connectGasSlots( void );                                  //! GUI events machen
    void updateCurrGasGUI( int gasNum, bool withCurrent = true );  //! gui nach aktuellem Gas aktualisieren
    void gasSelect( int gasNum, bool isSelected );

    signals:
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;   //! eine Warnmeldung soll das Main darstellen
    void onAkkuValueChangedSig( double aValue ) override;                           //! signalisiert, dass der Akku eine Spanniung hat
    void onConfigWasChangedSig( void );  //! signalisieret das ich was in die Config geschrieben habe

    private slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! wenn es Fehler in der BT Verbindung gibt
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird
    virtual void onCommandRecivedSlot( void ) override;                              //! wenn ein Datentelegramm empfangen wurde
    void onGasConfigUpdateSlot( void );                                              //! Konfiguration abrufen
    void onSpinO2ValueChangedSlot( int o2Val );                                      //! O2 Wert eines Gases hat sich verändert
    void onSpinHeValueChangedSlot( int heVal );                                      //! HE Wert eines Gases hat sich verändert
    void onDiluentUseChangeSlot( int state, DiluentType which );                     //! wenn sich das Diluent ändert
    void onBailoutCheckChangeSlot( int state );                                      //! wenn sich das Bailout ändert
    void onWaterTypeChanged( int state );                                            //! wenn sich der wassertyp zum berechnen ändert
  };
}
#endif  // GASFORM_HPP
