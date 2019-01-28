#ifndef DEVICEINFOFRAGMENT_HPP
#define DEVICEINFOFRAGMENT_HPP

#include <QWidget>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Config.hpp"

#include <QImage>

namespace Ui
{
  class DeviceInfoFragment;
}

namespace spx
{
  class DeviceInfoFragment : public QWidget, IFragmentInterface
  {
    private:
    Q_OBJECT
    Q_INTERFACES( spx::IFragmentInterface )
    std::unique_ptr< Ui::DeviceInfoFragment > ui;  //! Zeiger auf die GUI Objekte
    QImage spxPic;
    QString titleTemplate;
    QString deviceModelTemplate;
    QString deviceSerialNumberTemplate;
    QString deviceFirmwareVersionTemplate;
    QString unknownString;
    QString notString;
    QString fahrenheidBugTemplate;
    QString setDateTemplate;
    QString hasSixParamsTemplate;
    QString sixMetersAutoSetpointTemplate;
    QString firmwareSupportetTemplate;
    QString olderParamsSortTemplate;
    QString newerParamsSortTemplate;
    QString olderDisplayBrightnessTemplate;
    QString newerDisplayBrightnessTemplate;

    public:
    explicit DeviceInfoFragment( QWidget *parent,
                                 std::shared_ptr< Logger > logger,
                                 std::shared_ptr< SPX42Database > spx42Database,
                                 std::shared_ptr< SPX42Config > spxCfg,
                                 std::shared_ptr< SPX42RemotBtDevice > remSPX42 );  //! Konstruktor
    ~DeviceInfoFragment() override;                                                 //! Destruktor, muss GUI säubern

    private:
    void prepareGuiWithConfig( void );  //! GUI mit werten aus der Config machen

    signals:
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;   //! eine Warnmeldung soll das Main darstellen
    void onAkkuValueChangedSig( double aValue ) override;                           //! signalisiert, dass der Akku eine Spanniung hat

    private slots:
    virtual void onCommandRecivedSlot( void ) override;                              //! wenn ein Datentelegramm empfangen wurde
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! wenn bei einer Verbindung ein Fehler auftritt
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird
  };
}  // namespace spx
#endif  // DeviceInfoFragment_HPP
