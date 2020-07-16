#ifndef LOGFORM_HPP
#define LOGFORM_HPP

#include <float.h>
#include <QAbstractItemView>
#include <QFileDialog>
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
#include "config/AppConfigClass.hpp"
#include "config/ProjectConst.hpp"
#include "database/ChartDataWorker.hpp"
#include "database/LogDetailDeleter.hpp"
#include "database/LogDetailWalker.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Config.hpp"
#include "spx42/SPX42Defs.hpp"
#include "spx42/SPX42LogDirectoryEntry.hpp"
#include "uddf/SPX42UddfExport.hpp"
#include "ui_LogFragment.h"

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
    QtCharts::QChart *miniChart;                        //! Zeiger auf das eigene Minichart
    QtCharts::QChart *dummyChart;                       //! Zeiger auf das weisse, leere chart
    std::unique_ptr< QtCharts::QChartView > chartView;  //! Zeiger auf das ChartView
    std::unique_ptr< ChartDataWorker > chartWorker;     //! Prozess zum abarbeiten
    QFuture< bool > dbgetDataFuture;                    //! Nebenläufiges future Objekt
    QTimer transferTimeout;                             //! timer für timeout bei transfers
    std::unique_ptr< LogDetailWalker > logWriter;       //! schreibt logdetails queue in die DB
    LogDetailDeleter *detailDeleterThread{nullptr};     //! Thread zum löschen von Logdetails
    SPX42UDDFExport xmlExport;                          //! Objekt zum Erzeugen des XML Exportes
    DeviceAliasHash spxDevicesAliasHash;                //! Liste mit Devices aus der Datenbank
    QQueue< int > logDetailRead;                        //! Liste mit zu lesenden Logdetails
    LogDetailSetQueue logDetailQueue;                   //! Queue mit den Einträgen
    QFuture< bool > exportFuture;                       //! nebenläufig tauchgänge exportieren
    const QIcon savedIcon;                              //! icon fur anzeige log ist in db
    const QIcon nullIcon;                               //! icon null
    QString offlineDeviceAddr;            //! wenn das gerät offline ist und in der geräte combobox ein gerät ausgewählt ist
    QString fragmentTitlePattern;         //! das Muster (lokalisierungsfähig) für Fragmentüberschrift
    QString fragmentTitleOfflinePattern;  //! das ganze offline
    QString exportPath;                   //! Pfad für exporte
    QString diveNumberStr;
    QString diveDateStr;
    QString diveDepthStr;
    QString diveDepthShortStr;
    QString dbWriteNumTemplate;
    QString dbWriteNumIDLE;
    QString readetLogTemplate;
    QString dbDeleteNumTemplate;
    QString exportDiveStartTemplate;
    QString exportDiveEndTemplate;
    QString exportDiveErrorTemplate;

    public:
    //! Konstruktor
    explicit LogFragment( QWidget *parent,
                          std::shared_ptr< Logger > logger,
                          std::shared_ptr< SPX42Database > spx42Database,
                          std::shared_ptr< SPX42Config > spxCfg,
                          std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                          AppConfigClass *appCfg );

    private:
    LogFragment( const LogFragment & );

    public:
    ~LogFragment() override;                       //! Destruktor
    void setExportPath( const QString &_export );  //! setzte EXPORT Ptad

    protected:
    void changeEvent( QEvent *e ) override;

    private:
    void setGuiConnected( bool isConnected );                   //! setze alle Einstellungen für die GUI
    void processLogDetails( void );                             //! schreibe alle Daten aus der Queue in die Datenbank
    void testForSavedDetails( void );                           //! schaue nach ob die Details dazu bereits gesichert wurden
    void tryStartLogWriterThread( void );                       //! den Thread starten (restarten)
    std::shared_ptr< QVector< int > > getSelectedInDb( void );  //! gib alle selektierten Einträge, die in der DB sind

    signals:
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;   //! eine Warnmeldung soll das Main darstellen
    void onAkkuValueChangedSig( double aValue ) override;                           //! signalisiert, dass der Akku eine Spanniung hat

    public slots:
    void onSendBufferStateChangedSlot( bool isBusy );  //! signalisiert wenn sich der Kommando Sendepuffer Status ändert

    private slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! wenn fehler in der BT VERbindung auftauchen
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird
    virtual void onCommandRecivedSlot( void ) override;                              //! wenn ein Datentelegramm empfangen wurde
    void onTransferTimeoutSlot( void );                                              //! wenn der Transfer ausbleibt
    void onReadLogDirectoryClickSlot( void );                                        //! Button zum lesen des Verzeichisses vom SPX42
    void onReadLogContentClickSlot( void );                                          //! Button zum lesen der Logdaten vom SPX42
    void onLogListClickeSlot( const QModelIndex &index );                            //! in die Liste des Logverzeichisses geklickt
    void itemSelectionChangedSlot( void );                                           //! Eintrag in der Verzeichisliste
    void onDeviceComboChangedSlot( int index );                                      //! Eintrag in der Offline-Geräteliste geändert
    void onDeleteLogDetailClickSlot( void );                                         //! Button für Logdaten aus Datenbank löschen
    void onExportLogDetailClickSlot( void );      //! Button für das Exportieren der Logdaten aus der Datenbank
    void onWriteDiveStartSlot( int newDiveNum );  //! Schreiben der Daten eines neuen Tauchgangs begonnen
    void onWriteDiveDoneSlot( int diveNum );      //! Schreiben in die Datenbank füe einen Tauchgang beendet
    void onWriteFinishedSlot( int count );        //! Alle Tauchgange in der Queue daten sichern erfolgreich
    void onWriterCritSlot( LOGWRITEERR err );     //! Kritischer Fehler beim Schreiben
    void onDeleteDoneSlot( int diveNum );         //! Löschen aus der Datenbank für einen Tauchgang erfolgreich

    void onExportSingleDiveStartSlot( int diveNum );              //! UDDF dive Export gestartet
    void onExportSingleDiveEndSlot( int diveNum );                //! UDDF dive export für einen TG beendet
    void onExportEndSlot( bool wasOk, const QString &fileName );  //! UDDF export beendet

    public slots:
    void onAddLogdirEntrySlot( const QString &entry, bool inDatabase = false );
  };
}  // namespace spx
#endif  // LOGFORM_HPP
