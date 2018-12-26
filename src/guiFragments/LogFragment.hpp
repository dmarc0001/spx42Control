#ifndef LOGFORM_HPP
#define LOGFORM_HPP

#include <float.h>
#include <QAbstractItemView>
#include <QFuture>
#include <QMargins>
#include <QQueue>
#include <QStringList>
#include <QTableWidget>
#include <QTimer>
#include <QWidget>
#include <QtCharts>
#include <QtConcurrent>
#include <algorithm>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "config/ProjectConst.hpp"
#include "database/LogDetailWriter.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Config.hpp"
#include "spx42/SPX42Defs.hpp"
#include "spx42/SPX42LogDirectoryEntry.hpp"
#include "ui_LogFragment.h"
#include "utils/DebugDataSeriesGenerator.hpp"
#include "utils/DiveDataSeriesGenerator.hpp"
#include "utils/IDataSeriesGenerator.hpp"

namespace Ui
{
  class LogFragment;
}

namespace spx
{
  constexpr std::chrono::milliseconds TIMEOUTVAL = static_cast< std::chrono::milliseconds >( 2500 );

  class LogFragment : public QWidget, IFragmentInterface
  {
    private:
    Q_OBJECT
    Q_INTERFACES( spx::IFragmentInterface )
    std::unique_ptr< Ui::LogFragment > ui;      //! Zeiger auf GUI-Objekte
    std::unique_ptr< QtCharts::QChart > chart;  //! Zeiger auf das Chart
    QtCharts::QChart *dummyChart;               //! Zeiger auf das weisse, leere chart
    QtCharts::QChartView *chartView;            //! Zeiger auf das ChartView
    QtCharts::QCategoryAxis *axisY;             //! Y-Achse für Chart
    QTimer transferTimeout;                     //! timer für timeout bei transfers
    LogDetailWriter logWriter;                  //! schreibt logdetails queue in die DB
    QFuture< int > dbWriterFuture;              //! nebenläufig daten in DB schreiben
    QQueue< int > logDetailRead;                //! Liste mit zu lesenden Logdetails
    const QIcon savedIcon;
    const QIcon nullIcon;
    QString fragmentTitlePattern;  //! das Muster (lokalisierungsfähig) für Fragmentüberschrift
    QString diveNumberStr;
    QString diveDateStr;
    QString diveDepthStr;
    QString dbWriteNumTemplate;
    QString dbWriteNumIDLE;

    public:
    explicit LogFragment( QWidget *parent,
                          std::shared_ptr< Logger > logger,
                          std::shared_ptr< SPX42Database > spx42Database,
                          std::shared_ptr< SPX42Config > spxCfg,
                          std::shared_ptr< SPX42RemotBtDevice > remSPX42 );  //! Konstruktor
    ~LogFragment() override;                                                 //! Destruktor
    virtual void deactivateTab( void ) override;                             //! deaktiviere eventuelle signale

    protected:
    void changeEvent( QEvent *e ) override;

    private:
    void prepareMiniChart( void );
    void showDiveDataForGraph( int deviceId, int diveNum );
    void setGuiConnected( bool isConnected );
    void processLogDetails( void );    //! schreibe alle Daten aus der Queue in die Datenbank
    void testForSavedDetails( void );  //! schaue nach ob die Details dazu bereits gesichert wurden
    float getMinYValue( const QLineSeries *series );
    float getMaxYValue( const QLineSeries *series );

    signals:
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;   //! eine Warnmeldung soll das Main darstellen
    void onAkkuValueChangedSig( double aValue ) override;                           //! signalisiert, dass der Akku eine Spanniung hat

    public slots:
    void onSendBufferStateChangedSlot( bool isBusy );

    private slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! wenn fehler in der BT VERbindung auftauchen
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird
    virtual void onCommandRecivedSlot( void ) override;                              //! wenn ein Datentelegramm empfangen wurde
    void onTransferTimeout( void );                                                  //! wenn der Transfer ausbleibt
    void onReadLogDirectorySlot( void );
    void onReadLogContentSlot( void );
    void onLogListClickedSlot( const QModelIndex &index );
    void onWriterDoneSlot( int _countProcessed );
    void onNewDiveStartSlot( int newDiveNum );

    public slots:
    void onAddLogdirEntrySlot( const QString &entry );
  };
}
#endif  // LOGFORM_HPP
