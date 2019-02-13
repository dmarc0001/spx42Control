#ifndef CHARTSFRAGMENT_HPP
#define CHARTSFRAGMENT_HPP

#include <QMouseEvent>
#include <QPointF>
#include <QRect>
#include <QWidget>
#include <QtCharts>
#include <QtConcurrent>
#include <database/ChartDataWorker.hpp>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "config/AppConfigClass.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Config.hpp"
#include "utils/SPXChartView.hpp"

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
    //! Zeiger auf die GUI Objekte
    std::unique_ptr< Ui::ChartsFragment > ui;
    //! Zeiger auf das eigene Chart
    QtCharts::QChart *bigDiveChart;
    //! Zeiger auf das eigene Chart PPO2
    QtCharts::QChart *ppo2DiveChart;
    //! Zeiger auf DUMMY gross
    QtCharts::QChart *bigDummyChart;
    //! Zeiger auf DUMMY ppo2
    QtCharts::QChart *ppo2DummyChart;
    //! Zeiger auf das ChartView für Hauptdaten
    std::unique_ptr< SPXChartView > bigChartView;
    //! Zeiger auf das ChartView für PPO2
    std::unique_ptr< SPXChartView > ppo2ChartView;
    //! Prozess zum abarbeiten
    std::unique_ptr< ChartDataWorker > chartWorker;
    //! Nebenläufiges future Objekt
    QFuture< bool > dbgetDataFuture;
    //! Liste mit Devices aus der Datenbank
    DeviceAliasHash spxDevicesAliasHash;
    //! mac adresse des ausgewählten gerätes
    QString deviceAddr;
    //! Nummer des ausgewählten Tauchganges
    int diveNum;
    //! das ganze offline
    QString fragmentTitleOfflinePattern;

    public:
    //! Konstruktor
    explicit ChartsFragment( QWidget *parent,
                             std::shared_ptr< Logger > logger,
                             std::shared_ptr< SPX42Database > spx42Database,
                             std::shared_ptr< SPX42Config > spxCfg,
                             std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                             AppConfigClass *appCfg );
    //! Destruktor, muss GUI säubern
    ~ChartsFragment() override;

    protected:
    // void keyPressEvent( QKeyEvent *event ) override;

    private:
    void initDeviceSelection( void );

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
    void onDeviceComboChangedSlot( int index );
    void onDiveComboChangedSlot( int index );
    void onChartReadySlot( void );
    void onNotesLineEditFinishedSlot( void );
  };
}  // namespace spx
#endif  // CHARTSFRAGMENT_HPP
