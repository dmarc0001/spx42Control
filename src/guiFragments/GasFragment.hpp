#ifndef GASFORM_HPP
#define GASFORM_HPP

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QRegExp>
#include <QSpinBox>
#include <QWidget>
#include <array>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "config/ProjectConst.hpp"
#include "config/SPX42Defs.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "utils/SPX42Config.hpp"

namespace Ui
{
  class GasForm;
}

namespace spx
{
#define QSpinboxIntValueChanged static_cast< void ( QSpinBox::* )( int ) >( &QSpinBox::valueChanged )

  class GasFragmentGuiRef
  {
    public:
    QSpinBox *o2Spin;
    QSpinBox *heSpin;
    QLineEdit *n2Line;
    QLabel *gasName;
    QCheckBox *dil1CheckBox;
    QCheckBox *dil2CheckBox;
    QCheckBox *baCheckBox;
    explicit GasFragmentGuiRef( QSpinBox *o2SpinBox,
                                QSpinBox *heSpinBox,
                                QLineEdit *n2LineEdit,
                                QLabel *gasNameLabel,
                                QCheckBox *dil1,
                                QCheckBox *dil2,
                                QCheckBox *ba );
    ~GasFragmentGuiRef() = default;
  };

  class GasFragment : public QWidget, IFragmentInterface
  {
    private:
    Q_OBJECT
    Q_INTERFACES( spx::IFragmentInterface )
    std::unique_ptr< Ui::GasForm > ui;                           //! Zeiger auf GUI-Objekte
    bool areSlotsConnected;                                      //! Ich merke mir, ob die Slots verbunden sind
    std::array< std::unique_ptr< GasFragmentGuiRef >, 8 > gRef;  //! Referenzen für acht GUI-Objekte
    // std::unique_ptr< GasFragmentGuiRef > gRef[ 8 ];  //! Referenzen für acht GUI-Objekte TODO: std::array machen

    public:
    explicit GasFragment( QWidget *parent,
                          std::shared_ptr< Logger > logger,
                          std::shared_ptr< SPX42Database > spx42Database,
                          std::shared_ptr< SPX42Config > spxCfg,
                          std::shared_ptr< SPX42RemotBtDevice > remSPX42 );  //! Konstruktor
    ~GasFragment() override;                                                 //! der Zerstörer
    virtual void deactivateTab( void ) override;                             //! deaktiviere eventuelle signale

    private:
    void fillReferences( void );     //! fülle die indizies mit Referenzen
    void initGuiWithConfig( void );  //! Initialisiere die GUI mit Werten aus der Config
    void connectSlots( void );       //! verbinde Slots mit Signalen
    void disconnectSlots( void );    //! trenne Slots von Signalen
    void checkGases( void );         //! Alle Gase nach Lizenzwechsel testen

    signals:
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;   //! eine Warnmeldung soll das Main darstellen

    private slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! wenn es Fehler in der BT Verbindung gibt
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird
    void onSpinO2ValueChangedSlot( int index, int o2Val );                           //! O2 Wert eines Gases hat sich verändert
    void onSpinHeValueChangedSlot( int index, int heVal );                           //! HE Wert eines Gases hat sich verändert
    void onGasUseTypChangeSlot( int index, DiluentType which, int state );           //! wenn sich das Diluent ändert
    void onBaCheckChangeSlot( int index, int state );                                //! wenn sich das Bailout ändert
  };
}
#endif  // GASFORM_HPP
