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
    std::unique_ptr< Ui::LogFragment > ui;              //! Zeiger auf GUI-Objekte
    std::unique_ptr< DiveMiniChart > miniChart;         //! Zeiger auf das eigene Minichart
    QtCharts::QChart *dummyChart;                       //! Zeiger auf das weisse, leere chart
    std::unique_ptr< QtCharts::QChartView > chartView;  //! Zeiger auf das ChartView
    QTimer transferTimeout;                             //! timer für timeout bei transfers
    LogDetailWalker logWriter;                          //! schreibt logdetails queue in die DB
    QFuture< int > dbWriterFuture;                      //! nebenläufig daten in DB schreiben
    QQueue< int > logDetailRead;                        //! Liste mit zu lesenden Logdetails
    QFuture< bool > dbDeleteFuture;                     //! nebenläufig daten aus DB löschen
    bool logWriterTableExist;                           //! Ergebnis des Logwriter Threads, bei -1 gab es einen Fehler
    const QIcon savedIcon;                              //! icon fur anzeige log ist in db
    const QIcon nullIcon;                               //! icon null
    QString fragmentTitlePattern;                       //! das Muster (lokalisierungsfähig) für Fragmentüberschrift
    QString diveNumberStr;
    QString diveDateStr;
    QString diveDepthStr;
    QString dbWriteNumTemplate;
    QString dbWriteNumIDLE;
    QString dbDeleteNumTemplate;

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
    void setGuiConnected( bool isConnected );
    void processLogDetails( void );                             //! schreibe alle Daten aus der Queue in die Datenbank
    void testForSavedDetails( void );                           //! schaue nach ob die Details dazu bereits gesichert wurden
    void tryStartLogWriterThread( void );                       //! den Thread starten (restarten)
    std::shared_ptr< QVector< int > > getSelectedInDb( void );  //! gib alle selektierten Einträge, die in der DB sind

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

    public slots:
    void onAddLogdirEntrySlot( const QString &entry );
  };
}  // namespace spx
#endif  // LOGFORM_HPP
