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
    std::unique_ptr< Ui::ChartsFragment > ui;        //! Zeiger auf die GUI Objekte
    QtCharts::QChart *bigDiveChart;                  //! Zeiger auf das eigene Chart
    QtCharts::QChart *ppo2DiveChart;                 //! Zeiger auf das eigene Chart PPO2
    QtCharts::QChart *bigDummyChart;                 //! Zeiger auf DUMMY gross
    QtCharts::QChart *ppo2DummyChart;                //! Zeiger auf DUMMY ppo2
    std::unique_ptr< SPXChartView > bigChartView;    //! Zeiger auf das ChartView für Hauptdaten
    std::unique_ptr< SPXChartView > ppo2ChartView;   //! Zeiger auf das ChartView für PPO2
    std::unique_ptr< ChartDataWorker > chartWorker;  //! Prozess zum abarbeiten
    QFuture< DiveDataSetsPtr > dbgetDataFuture;      //! Nebenläufiges future Objekt
    DeviceAliasHash spxDevicesAliasHash;             //! Liste mit Devices aus der Datenbank
    QString deviceAddr;                              //! mac adresse des ausgewählten gerätes
    int diveNum;                                     //! Nummer des ausgewählten Tauchganges
    QString fragmentTitleOfflinePattern;             //! das ganze offline

    public:
    //! Konstruktor
    explicit ChartsFragment( QWidget *parent,
                             std::shared_ptr< Logger > logger,
                             std::shared_ptr< SPX42Database > spx42Database,
                             std::shared_ptr< SPX42Config > spxCfg,
                             std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                             AppConfigClass *appCfg );
    ChartsFragment( const ChartsFragment & );  //! Kopierkonstruktor
    ~ChartsFragment() override;                //! Destruktor, muss GUI säubern

    protected:
    // void keyPressEvent( QKeyEvent *event ) override;

    private:
    void configObject( void );
    void initDeviceSelection( void );

    signals:
    void onWarningMessageSig( const QString &msg,
                              bool asPopup = false ) override;                     //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onAkkuValueChangedSig( double aValue ) override;                          //! signalisiert, dass der Akku eine Spanniung hat

    private slots:
    virtual void onCommandRecivedSlot( void ) override;                              //! wenn ein Datentelegramm empfangen wurde
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! wenn bei einer Verbindung ein Fehler auftritt
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird
    void onDiveDataWaitFor( int index );                                             //! wenn auf das future gewartet wird
    void onDeviceComboChangedSlot( int index );
    void onDiveComboChangedSlot( int index );
    void onNotesLineEditFinishedSlot( void );
  };
}  // namespace spx
#endif  // CHARTSFRAGMENT_HPP
