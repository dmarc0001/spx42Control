#include "LogFragment.hpp"

using namespace QtCharts;
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
    model( new QStringListModel() ),
    //chart( new QtCharts::QChart() ),
    //chartView( new QtCharts::QChartView(chart.get()))
    chart( new QtCharts::QChart() ),
    chartView( new QtCharts::QChartView(chart)),
    axisY( new QCategoryAxis() )
  {
    lg->debug("LogFragment::LogFragment...");
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
    prepareMiniChart();
    delete ui->detailsGroupBox->layout()->replaceWidget( ui->diveProfileGraphicsView, chartView );
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
    lg->debug("LogFragment::~LogFragment...");
    ui->logentryListView->setModel( Q_NULLPTR );
    //ui->detailsGroupBox->layout()->replaceWidget( chartView, ui->diveProfileGraphicsView );
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
    // FIXME: natürlich noch die richtigen Daten einbauen
    getDiveDataForGraph( 1, 2 );
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

  void LogFragment::prepareMiniChart(void)
  {
    chart->legend()->hide();                                    // Keine Legende in der Minivorschau
    // Chart Titel aufhübschen
    QFont font;
    font.setPixelSize(10);
    chart->setTitleFont(font);
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setTitle(tr("PREVIEW"));
    // Hintergrund aufhübschen
    QBrush backgroundBrush(Qt::NoBrush);
    chart->setBackgroundBrush(backgroundBrush);
    // Malhintergrund auch noch
    QLinearGradient plotAreaGradient;
    plotAreaGradient.setStart(QPointF(0, 1));
    plotAreaGradient.setFinalStop(QPointF(1, 0));
    plotAreaGradient.setColorAt(0.0, QRgb(0x202040));
    plotAreaGradient.setColorAt(1.0, QRgb(0x2020f0));
    plotAreaGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart->setPlotAreaBackgroundBrush(plotAreaGradient);
    chart->setPlotAreaBackgroundVisible(true);
    //
    // Achse machen
    //
    // Y-Achse
    QPen axisPen(QRgb(0xd18952));
    axisPen.setWidth(1);
    axisY->setLinePen(axisPen);
    QBrush axisBrush(Qt::white);
    axisY->setLabelsBrush(axisBrush);
    // achsen grid lines and shades
    axisY->setGridLineVisible(false);
    axisY->setShadesPen(Qt::NoPen);
    axisY->setShadesBrush(QBrush(QColor(0x99, 0xcc, 0xcc, 0x55)));
    axisY->setShadesVisible(true);
    // Achsen Werte und Bereiche setzten
    axisY->setRange(0, 30);
    QLineSeries *se = new QLineSeries();
    chart->setAxisY(axisY, se);
    // Hübsch malen
    chartView->setRenderHint(QPainter::Antialiasing);
  }

  /**
   * @brief hole/erzeuge daten für einen Tauchgang wenn vorhanden
   * @param deviceId Geräteid in der Datenbank
   * @param diveNum Nummer des TG für das Gerät
   */
  void LogFragment::getDiveDataForGraph( int deviceId, int diveNum )
  {
    // Polimorphes Objekt hier mit DEBUG belegt
    IDataSeriesGenerator* gen = new DebugDataSeriesGenerator( lg, spxConfig );
    // Device-Id für Datenbank hinterlegen
    gen->setDeviceId( deviceId );
    // erzeuge Datenserie(n)
    QLineSeries *series = gen->makeDepthSerie( diveNum );
    // die Serie aufhübschen
    QPen pen(QRgb(0xfdb157));
    pen.setWidth(2);
    series->setPen(pen);
    // Chartobjekt etwas leeren
    chart->removeAllSeries();
    chart->removeAxis( axisY );
    chart->addSeries(series);                                   // Serie zufügen
    // Y-Achse
    // Achsen Werte und Bereiche setzten
    // vorher Skalierung testen
    QVector<QPointF> points = series->pointsVector();
    QVector<QPointF>::iterator it = points.begin();
    float min = 0.0;
    while( it != points.end() )
    {
      if( it->ry() < min )
        min = it->ry();
      it++;
    }
    min += (min/8.0f);
    axisY->setRange(min, 0.50f );
    //in chart setzten
    chart->setAxisY(axisY, series);

  }

}
