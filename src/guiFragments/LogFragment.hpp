#ifndef LOGFORM_HPP
#define LOGFORM_HPP

#include <memory>
#include <algorithm>
#include <QWidget>
#include <QStringList>
#include <QStringListModel>
#include <QtCharts>

#include "IFragmentInterface.hpp"
#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "../config/SPX42Defs.hpp"
#include "../config/ProjectConst.hpp"
#include "../utils/IDataSeriesGenerator.hpp"
#include "../utils/DebugDataSeriesGenerator.hpp"
#include "../utils/DiveDataSeriesGenerator.hpp"

#include "ui_LogFragment.h"

namespace Ui {
  class LogFragment;
}

namespace spx42
{
  class LogFragment : public QWidget, IFragmentInterface
  {
    private:
      Q_OBJECT
      std::unique_ptr<Ui::LogFragment> ui;                          //! Zeiger auf GUI-Objekte
      std::unique_ptr<QStringListModel> model;                      //! Zeiger auf Strionglist Model
      QtCharts::QChart* chart;                                      //! Zeiger auf das Chart
      QtCharts::QChartView* chartView;                              //! Zeiger auf das ChartView
      QtCharts::QCategoryAxis* axisY;                               //! Y-Achse für Chart
      QString diveNumberStr;
      QString diveDateStr;
      QString diveDepthStr;
    public:
      explicit LogFragment(QWidget *parent, std::shared_ptr<Logger> logger , std::shared_ptr<SPX42Config> spxCfg);    //! Konstruktor
      ~LogFragment();

    protected:
      void changeEvent(QEvent *e);

    private:
      void prepareMiniChart(void);
      void getDiveDataForGraph( int deviceId, int diveNum );

    private slots:
      void readLogDirectorySlot(void);
      void readLogContentSlot(void);
      void logListViewClickedSlot( const QModelIndex &index );

    public slots:
      void addLogdirEntrySlot( const QString& entry );
      void addLogLineSlot( const QString& line );
  };
}
#endif // LOGFORM_HPP
