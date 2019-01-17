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
#include <QVector>
#include <QWidget>
#include <QtAlgorithms>
#include <QtCharts>
#include <QtConcurrent>
#include <algorithm>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "charts/DiveMiniChart.hpp"
#include "config/ProjectConst.hpp"
#include "database/LogDetailWalker.hpp"
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
    //! Zeiger auf GUI-Objekte
    std::unique_ptr< Ui::LogFragment > ui;
    //! Zeiger auf das eigene Minichart
    std::unique_ptr< DiveMiniChart > miniChart;
    //! Zeiger auf das weisse, leere chart
    QtCharts::QChart *dummyChart;
    //! Zeiger auf das ChartView
    std::unique_ptr< QtCharts::QChartView > chartView;
    //! timer für timeout bei transfers
    QTimer transferTimeout;
    //! Timer startet den nächsten Transfer von Logdetails
    QTimer startNextTransferTimer;
    //! schreibt logdetails queue in die DB
    LogDetailWalker logWriter;
    //! Liste mit Devices aus der Datenbank
    DeviceAliasHash spxDevicesAliasHash;
    //! nebenläufig daten in DB schreiben
    QFuture< int > dbWriterFuture;
    //! Liste mit zu lesenden Logdetails
    QQueue< int > logDetailRead;
    //! nebenläufig daten aus DB löschen
    QFuture< bool > dbDeleteFuture;
    //! icon fur anzeige log ist in db
    const QIcon savedIcon;
    //! icon null
    const QIcon nullIcon;
    //! wenn das gerät offline ist und in der geräte combobox ein gerät ausgewählt ist
    QString offlineDeviceAddr;
    //! das Muster (lokalisierungsfähig) für Fragmentüberschrift
    QString fragmentTitlePattern;
    //! das ganze offline
    QString fragmentTitleOfflinePattern;
    QString diveNumberStr;
    QString diveDateStr;
    QString diveDepthStr;
    QString dbWriteNumTemplate;
    QString dbWriteNumIDLE;
    QString dbDeleteNumTemplate;

    public:
    //! Konstruktor
    explicit LogFragment( QWidget *parent,
                          std::shared_ptr< Logger > logger,
                          std::shared_ptr< SPX42Database > spx42Database,
                          std::shared_ptr< SPX42Config > spxCfg,
                          std::shared_ptr< SPX42RemotBtDevice > remSPX42 );
    //! Destruktor
    ~LogFragment() override;
    //! deaktiviere eventuelle signale
    virtual void deactivateTab( void ) override;

    protected:
    void changeEvent( QEvent *e ) override;

    private:
    //! Einstellungen entsprechend des Onlinestatus machen
    void setGuiConnected( bool isConnected );
    //! schreibe alle Daten aus der Queue in die Datenbank
    void processLogDetails( void );
    //! schaue nach ob die Details dazu bereits gesichert wurden
    void testForSavedDetails( void );
    //! den Thread starten (restarten)
    void tryStartLogWriterThread( void );
    //! gib alle selektierten Einträge, die in der DB sind
    std::shared_ptr< QVector< int > > getSelectedInDb( void );

    signals:
    //! eine Warnmeldung soll das Main darstellen
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;
    //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;
    //! signalisiert, dass der Akku eine Spanniung hat
    void onAkkuValueChangedSig( double aValue ) override;

    public slots:
    //! signalisiert wenn sich der Kommando Sendepuffer Status ändert
    void onSendBufferStateChangedSlot( bool isBusy );

    private slots:
    //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;
    //! wenn fehler in der BT VERbindung auftauchen
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;
    //! Wenn sich die Lizenz ändert
    virtual void onConfLicChangedSlot( void ) override;
    //! wenn die Datenbank geschlosen wird
    virtual void onCloseDatabaseSlot( void ) override;
    //! wenn ein Datentelegramm empfangen wurde
    virtual void onCommandRecivedSlot( void ) override;
    //! wenn der Transfer ausbleibt
    void onTransferTimeoutSlot( void );
    //! wenn das nächste Log abgerüfen werden soll/kann
    void onNextTransferRequest( void );
    void onReadLogDirectoryClickSlot( void );
    void onReadLogContentClickSlot( void );
    void onLogListClickeSlot( const QModelIndex &index );
    void onWriterDoneSlot( int _countProcessed );
    void onNewDiveStartSlot( int newDiveNum );
    void onLogDetailDeleteClickSlot( void );
    void onLogDetailExportClickSlot( void );
    void onDeleteDoneSlot( int diveNum );
    void onNewDiveDoneSlot( int diveNum );
    void itemSelectionChangedSlot( void );
    void onDeviceComboChangedSlot( int index );

    public slots:
    void onAddLogdirEntrySlot( const QString &entry, bool inDatabase = false );
  };
}  // namespace spx
#endif  // LOGFORM_HPP
