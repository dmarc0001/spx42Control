#ifndef LOGFORM_HPP
#define LOGFORM_HPP

#include <float.h>
#include <QMargins>
#include <QStringList>
#include <QStringListModel>
#include <QWidget>
#include <QtCharts>
#include <algorithm>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "config/ProjectConst.hpp"
#include "config/SPX42Defs.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "ui_LogFragment.h"
#include "utils/DebugDataSeriesGenerator.hpp"
#include "utils/DiveDataSeriesGenerator.hpp"
#include "utils/IDataSeriesGenerator.hpp"
#include "utils/SPX42Config.hpp"

namespace Ui
{
  class LogFragment;
}

namespace spx
{
  class LogFragment : public QWidget, IFragmentInterface
  {
    private:
    Q_OBJECT
    Q_INTERFACES( spx::IFragmentInterface )
    std::unique_ptr< Ui::LogFragment > ui;      //! Zeiger auf GUI-Objekte
    std::unique_ptr< QStringListModel > model;  //! Zeiger auf Strionglist Model
    std::unique_ptr< QtCharts::QChart > chart;  //! Zeiger auf das Chart
    QtCharts::QChart *dummyChart;               //! Zeiger auf das weisse, leere chart
    QtCharts::QChartView *chartView;            //! Zeiger auf das ChartView
    QtCharts::QCategoryAxis *axisY;             //! Y-Achse für Chart
    QString diveNumberStr;
    QString diveDateStr;
    QString diveDepthStr;

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
    float getMinYValue( const QLineSeries *series );
    float getMaxYValue( const QLineSeries *series );

    signals:
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;   //! eine Warnmeldung soll das Main darstellen

    private slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! wenn fehler in der BT VERbindung auftauchen
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird
    void onReadLogDirectorySlot( void );
    void onReadLogContentSlot( void );
    void onLogListViewClickedSlot( const QModelIndex &index );

    public slots:
    void onAddLogdirEntrySlot( const QString &entry );
    void onAddLogLineSlot( const QString &line );
  };
}
#endif  // LOGFORM_HPP
