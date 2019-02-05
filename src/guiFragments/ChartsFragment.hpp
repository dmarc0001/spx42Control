#ifndef CHARTSFRAGMENT_HPP
#define CHARTSFRAGMENT_HPP

#include <QWidget>
#include <QtCharts>
#include <QtConcurrent>
#include <database/ChartDataWorker.hpp>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Config.hpp"

namespace Ui
{
  class ChartsFragment;
}

namespace spx
{
  class ChartsFragment : public QWidget, IFragmentInterface
  {
    private:
    Q_OBJECT
    Q_INTERFACES( spx::IFragmentInterface )
    std::unique_ptr< Ui::ChartsFragment > ui;  //! Zeiger auf die GUI Objekte
    //! Zeiger auf das eigene Chart
    QtCharts::QChart *bigDiveChart;
    //! Zeiger auf das eigene Chart PPO2
    QtCharts::QChart *ppo2DiveChart;
    //! Zeiger auf DUMMY gross
    QtCharts::QChart *bigDummyChart;
    //! Zeiger auf DUMMY ppo2
    QtCharts::QChart *ppo2DummyChart;
    //! Zeiger auf das ChartView für Hauptdaten
    std::unique_ptr< QtCharts::QChartView > bigChartView;
    //! Zeiger auf das ChartView für PPO2
    std::unique_ptr< QtCharts::QChartView > ppo2ChartView;
    //! Prozess zum abarbeiten
    std::unique_ptr< ChartDataWorker > chartWorker;
    //! Nebenläufiges future Objekt
    QFuture< bool > dbgetDataFuture;
    //! Liste mit Devices aus der Datenbank
    DeviceAliasHash spxDevicesAliasHash;
    //! mac adresse des ausgewählten gerätes
    QString deviceAddr;
    //! das ganze offline
    QString fragmentTitleOfflinePattern;

    public:
    explicit ChartsFragment( QWidget *parent,
                             std::shared_ptr< Logger > logger,
                             std::shared_ptr< SPX42Database > spx42Database,
                             std::shared_ptr< SPX42Config > spxCfg,
                             std::shared_ptr< SPX42RemotBtDevice > remSPX42 );  //! Konstruktor
    ~ChartsFragment() override;                                                 //! Destruktor, muss GUI säubern

    private:
    void initDeviceSelection( void );

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
    void onDeviceComboChangedSlot( int index );
    void onDiveComboChangedSlot( int index );
    void onChartReadySlot( void );
  };
}  // namespace spx
#endif  // CHARTSFRAGMENT_HPP
