#ifndef LOGFORM_HPP
#define LOGFORM_HPP

#include <memory>
#include <QWidget>
#include <QStringList>
#include <QStringListModel>

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "../config/SPX42Defs.hpp"
#include "../config/ProjectConst.hpp"
#include "IFragmentInterface.hpp"

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
      QString diveNumberStr;
      QString diveDateStr;
      QString diveDepthStr;

    public:
      explicit LogFragment(QWidget *parent, std::shared_ptr<Logger> logger , std::shared_ptr<SPX42Config> spxCfg);    //! Konstruktor
      ~LogFragment();

    protected:
      void changeEvent(QEvent *e);

    private:
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
