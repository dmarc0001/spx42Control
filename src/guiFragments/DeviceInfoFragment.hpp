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
    //! Zeiger auf die GUI Objekte
    std::unique_ptr< Ui::DeviceInfoFragment > ui;
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
    //! Konstruktor
    explicit DeviceInfoFragment( QWidget *parent,
                                 std::shared_ptr< Logger > logger,
                                 std::shared_ptr< SPX42Database > spx42Database,
                                 std::shared_ptr< SPX42Config > spxCfg,
                                 std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                                 AppConfigClass *appCfg );
    //! Destruktor, muss GUI säubern
    ~DeviceInfoFragment() override;

    private:
    //! GUI mit werten aus der Config machen
    void prepareGuiWithConfig( void );

    signals:
    //! eine Warnmeldung soll das Main darstellen
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;
    //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;
    //! signalisiert, dass der Akku eine Spanniung hat
    void onAkkuValueChangedSig( double aValue ) override;

    private slots:
    //! wenn ein Datentelegramm empfangen wurde
    virtual void onCommandRecivedSlot( void ) override;
    //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;
    //! wenn bei einer Verbindung ein Fehler auftritt
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;
    //! Wenn sich die Lizenz ändert
    virtual void onConfLicChangedSlot( void ) override;
    //! wenn die Datenbank geschlosen wird
    virtual void onCloseDatabaseSlot( void ) override;
  };
}  // namespace spx
#endif  // DeviceInfoFragment_HPP
