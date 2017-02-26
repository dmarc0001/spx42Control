#include "LogFragment.hpp"

namespace spx42
{
  /**
   * @brief Konstruktor für das Logfragment
   * @param parent Elternteil
   * @param logger der Logger
   * @param spxCfg Konfig des SPX
   */
  LogFragment::LogFragment( QWidget *parent, std::shared_ptr<Logger> logger , std::shared_ptr<SPX42Config> spxCfg) :
    QWidget(parent),
    IFragmentInterface(logger, spxCfg),
    ui(new Ui::LogFragment() ),
    model( new QStringListModel() )
  {
    ui->setupUi(this);
    ui->transferProgressBar->setVisible(false);
    ui->logentryListView->setModel( model.get() );
    ui->logentryListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->logentryListView->setSelectionMode(QAbstractItemView::MultiSelection);
    diveNumberStr = tr("DIVE NUMBER: %1");
    diveDateStr   = tr("DIVE DATE: %1");
    diveDepthStr  = tr("DIVE DEPTH: %1");
    ui->diveNumberLabel->setText(diveNumberStr.arg("-") );
    ui->diveDateLabel->setText(diveDateStr.arg("-") );
    ui->diveDepthLabel->setText(diveDepthStr.arg("-") );
    //
    connect( ui->readLogdirPushButton, &QPushButton::clicked, this, &LogFragment::readLogDirectorySlot );
    connect( ui->readLogContentPushButton, &QPushButton::clicked, this, &LogFragment::readLogContentSlot );
    connect( ui->logentryListView, &QAbstractItemView::clicked, this, &LogFragment::logListViewClickedSlot );
  }

  /**
   * @brief Der Destruktor, Aufräumarbeiten
   */
  LogFragment::~LogFragment()
  {
    ui->logentryListView->setModel( Q_NULLPTR );
  }

  /**
   * @brief Systemänderungen
   * @param e event
   */
  void LogFragment::changeEvent(QEvent *e)
  {
    QWidget::changeEvent(e);
    switch (e->type()) {
      case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
      default:
        break;
    }
  }

  /**
   * @brief Slot für das Signal von button zum Directory lesen
   */
  void LogFragment::readLogDirectorySlot( void )
  {
    lg->debug("LogFragment::readLogDirectorySlot: ...");
    // DEBUG: erzeuge einfach Einträge bei Jedem Click
    static int entry=0;
    QString newEntry = QString("ENTRY: %1").arg( ++entry );
    addLogdirEntrySlot( newEntry );
    //
  }

  /**
   * @brief Slot für das Signal vom button zum _Inhalt des Logs lesen
   */
  void LogFragment::readLogContentSlot(void)
  {
    lg->debug("LogFragment::readLogContentSlot: ...");
    QModelIndexList indexList = ui->logentryListView->selectionModel()->selectedIndexes();
    if( model->rowCount() == 0 )
    {
      lg->warn("LogFragment::readLogContentSlot: no log entrys!");
      return;
    }
    if( indexList.isEmpty() )
    {
      lg->warn("LogFragment::readLogContentSlot: nothing selected, read all?");
      //TODO: Messagebox aufpoppen und Nutzer fragen
    }
    else
    {
      lg->debug( QString("LogFragment::readLogContentSlot: read %1 logs from spx42...").arg(indexList.count()) );
      // TODO: tatsächlich anfragen....
    }
  }

  /**
   * @brief Slot für das Signal wenn auf einen Log Directory Eintrag geklickt wird
   * @param index welcher Index war es?
   */
  void LogFragment::logListViewClickedSlot( const QModelIndex &index )
  {
    QString number ="-";
    QString date = "-";
    QString depth = "-";
    lg->debug( QString("LogFragment::logListViewClickedSlot: data: %1...").arg( index.data().toString() ) );
    // TODO: aus dem Eintrag die Nummer lesen
    // TODO: TG in der Database -> Parameter auslesen und in die Labels eintragen
    ui->diveNumberLabel->setText(diveNumberStr.arg(number) );
    ui->diveDateLabel->setText(diveDateStr.arg(date) );
    ui->diveDepthLabel->setText(diveDepthStr.arg(depth) );
  }

  /**
   * @brief Wird ein Log Verzeichniseintrag angeliefert....
   * @param entry Der Eintrag
   */
  void LogFragment::addLogdirEntrySlot( const QString& entry )
  {
    int row = model->rowCount();
    model->insertRows(row, 1 );
    QModelIndex index = model->index(row);
    model->setData(index, entry);
  }

  /**
   * @brief Slot für ankommende Logdaten (für eine Logdatei)
   * @param line die Logzeile
   */
  void LogFragment::addLogLineSlot( const QString& line )
  {
    lg->debug( QString("LogFragment::addLogLineSlot: logline: <").append(line).append(">") );
  }
}
