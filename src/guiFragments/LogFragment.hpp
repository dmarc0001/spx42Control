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
#include "config/ProjectConst.hpp"
#include "config/SPX42Defs.hpp"
#include "logging/Logger.hpp"
#include "utils/DebugDataSeriesGenerator.hpp"
#include "utils/DiveDataSeriesGenerator.hpp"
#include "utils/IDataSeriesGenerator.hpp"
#include "utils/SPX42Config.hpp"

#include "ui_LogFragment.h"

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
    explicit LogFragment( QWidget *parent, std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Config > spxCfg );  //! Konstruktor
    ~LogFragment();

    protected:
    void changeEvent( QEvent *e );

    private:
    void prepareMiniChart( void );
    void showDiveDataForGraph( int deviceId, int diveNum );
    float getMinYValue( const QLineSeries *series );
    float getMaxYValue( const QLineSeries *series );

    private slots:
    virtual void onlineStatusChangedSlot( bool isOnline ) Q_DECL_OVERRIDE;  //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void confLicChangedSlot( void ) Q_DECL_OVERRIDE;                //! Wenn sich die Lizenz ändert
    void readLogDirectorySlot( void );
    void readLogContentSlot( void );
    void logListViewClickedSlot( const QModelIndex &index );

    public slots:
    void addLogdirEntrySlot( const QString &entry );
    void addLogLineSlot( const QString &line );
  };
}
#endif  // LOGFORM_HPP
