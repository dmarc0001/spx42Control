#include "SPXChartView.hpp"

namespace spx
{
  int SPXChartView::counter{0};

  SPXChartView::SPXChartView( std::shared_ptr< Logger > logger, QWidget *parent )
      : QGraphicsView( parent )
      , m_scene( new QGraphicsScene( this ) )
      , currChart( nullptr )
      , m_rubberBand( nullptr )
      , m_rubberBandFlags( SPXChartView::NoRubberBand )
      , lg( logger )
      , currCallout( nullptr )
  {
    currNumber = ++SPXChartView::counter;
    init();
  }

  SPXChartView::SPXChartView( std::shared_ptr< Logger > logger, QChart *chart, QWidget *parent )
      : QGraphicsView( parent )
      , m_scene( new QGraphicsScene( this ) )
      , currChart( chart )
      , m_rubberBand( nullptr )
      , m_rubberBandFlags( SPXChartView::NoRubberBand )
      , lg( logger )
      , currCallout( nullptr )
  {
    currNumber = ++SPXChartView::counter;
    init();
  }

  SPXChartView::~SPXChartView()
  {
    SPXChartView::counter--;
    delete currCallout;
  }

  void SPXChartView::init()
  {
    setFrameShape( QFrame::NoFrame );
    setBackgroundRole( QPalette::Window );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setScene( m_scene );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    //
    setDragMode( QGraphicsView::NoDrag );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setMouseTracking( true );
    //
    if ( !currChart )
      // ein Dummy machen
      currChart = new QChart();
    else
      // ein echter Chart ist da, also veruche mal was
      axisAndSeries();
    m_scene->addItem( currChart );
  }

  void SPXChartView::axisAndSeries()
  {
    ppo2Series = nullptr;
    depthAreaSeries = nullptr;
    dtAxis = nullptr;
    tempSeries = nullptr;
    depthSeries = nullptr;
    currSeries = nullptr;
    minTime = QDateTime();
    maxTime = QDateTime();
    auto allAxes = currChart->axes( Qt::Horizontal );
    if ( allAxes.count() > 0 )
    {
      dtAxis = static_cast< QDateTimeAxis * >( allAxes.first() );
      if ( dtAxis->type() == QAbstractAxis::AxisTypeDateTime )
      {
        // das ist die Zeitachse
        minTime = dtAxis->min();
        maxTime = dtAxis->max();
      }
      else
      {
        // keine zeitachse, alles zurück
        dtAxis = nullptr;
      }
    }

    auto seriesList = currChart->series();
    for ( auto *cSeries : seriesList )
    {
      if ( cSeries->type() == QAbstractSeries::SeriesTypeLine || cSeries->type() == QAbstractSeries::SeriesTypeArea )
      {
        if ( cSeries->name().compare( ChartDataWorker::depthSeriesName, Qt::CaseInsensitive ) == 0 )
        {
          depthSeries = static_cast< QLineSeries * >( cSeries );
          currSeries = depthSeries;
          connect( depthSeries, &QLineSeries::hovered, this, &SPXChartView::tooltip );
        }
        else if ( cSeries->name().compare( ChartDataWorker::depthAreaSeriesName, Qt::CaseInsensitive ) == 0 )
        {
          // TODO: kann raus, wenn es funkt
          depthAreaSeries = static_cast< QAreaSeries * >( cSeries );
          connect( depthAreaSeries, &QAreaSeries::hovered, this, &SPXChartView::tooltip );
        }
        else if ( cSeries->name().compare( ChartDataWorker::tempSeriesName, Qt::CaseInsensitive ) == 0 )
        {
          tempSeries = static_cast< QLineSeries * >( cSeries );
          connect( tempSeries, &QLineSeries::hovered, this, &SPXChartView::tooltip );
        }
        else if ( cSeries->name().compare( ChartDataWorker::ppo2SeriesName, Qt::CaseInsensitive ) == 0 )
        {
          ppo2Series = static_cast< QLineSeries * >( cSeries );
          currSeries = ppo2Series;
          connect( ppo2Series, &QLineSeries::hovered, this, &SPXChartView::tooltip );
        }
      }
    }
  }

  QChart *SPXChartView::chart() const
  {
    return currChart;
  }

  void SPXChartView::setChart( QChart *chart )
  {
    Q_ASSERT( chart );
    //
    // das selbe chart ist schon da, ignoriere dann
    //
    if ( currChart == chart )
      return;
    //
    // ist das alte chart tatsächlich vorhanden
    //
    if ( currChart )
      // entferne es aus der Umgebung
      m_scene->removeItem( currChart );
    //
    // Merke als aktuell
    //
    currChart = chart;
    // zufügen
    m_scene->addItem( currChart );
    //
    // Bearbeiten
    //
    axisAndSeries();
    c_resize();
  }

  void SPXChartView::c_resize()
  {
    //
    // Chart neu einpassen, testen ob einfach nur gedreht (Mobile device?)
    //
    qreal sinA = qAbs( transform().m21() );
    qreal cosA = qAbs( transform().m11() );
    QSize chartSize = size();
    // Kleine Ränder
    currChart->setMargins( QMargins( LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN ) );
    // 90 grad
    if ( sinA == 1.0 )
    {
      chartSize.setHeight( size().width() );
      chartSize.setWidth( size().height() );
    }
    //
    // irgendwieanders gedreht
    //
    else if ( sinA != 0.0 )
    {
      // Non-90 degree rotation, find largest square chart that can fit into the view.
      qreal minDimension = qMin( size().width(), size().height() );
      qreal h = ( minDimension - ( minDimension / ( ( sinA / cosA ) + 1.0 ) ) ) / sinA;
      chartSize.setHeight( static_cast< int >( h ) );
      chartSize.setWidth( static_cast< int >( h ) );
    }
    // und nun Größe anpassen
    currChart->resize( chartSize );
    setMinimumSize( currChart->minimumSize().toSize().expandedTo( minimumSize() ) );
    setMaximumSize( maximumSize().boundedTo( currChart->maximumSize().toSize() ) );
    setSceneRect( currChart->geometry() );
  }

  void SPXChartView::setTimeAxis( QDateTimeAxis *axis, QLineSeries *dataSeries, QRect &rect )
  {
    // QRectF tr =
    // QPoint start = currChart->mapFromScene( rectF->x(), rectF->y() ).toPoint();
    // QPoint end = currChart->mapFromScene( rectF->x() + rectF->width(), rectF->y() + rectF->height() ).toPoint();

    // QRect rRect( rectF->toRect() );

    QPointF startPoint = currChart->mapToValue( QPoint( rect.x(), rect.y() ), dataSeries );
    QPointF endPoint = currChart->mapToValue( QPoint( rect.x() + rect.width(), rect.y() + rect.height() ), dataSeries );
    // QPointF startPoint = currChart->mapToValue( start, dataSeries );
    // QPointF endPoint = currChart->mapToValue( end, dataSeries );

    qint64 start_ms = static_cast< qint64 >( startPoint.x() );
    qint64 end_ms = static_cast< qint64 >( endPoint.x() );
    axis->setRange( QDateTime::fromMSecsSinceEpoch( start_ms ), QDateTime::fromMSecsSinceEpoch( end_ms ) );
  }

  void SPXChartView::setRubberBand( const RubberBands &rubberBand )
  {
    m_rubberBandFlags = rubberBand;
    //
    // Ist kein Gummiband vorhanden, löschen!
    //
    if ( !m_rubberBandFlags )
    {
      delete m_rubberBand;
      m_rubberBand = nullptr;
      return;
    }
    //
    // ist kein gummiband vorhanden? Dann mache eines
    //
    if ( !m_rubberBand )
    {
      m_rubberBand = new QRubberBand( QRubberBand::Rectangle, this );
      m_rubberBand->setEnabled( true );
    }
  }

  SPXChartView::RubberBands SPXChartView::rubberBand() const
  {
    //
    // das aktuelle Gummiband (oder nullptr)
    return m_rubberBandFlags;
  }

  void SPXChartView::mousePressEvent( QMouseEvent *event )
  {
    QRectF plotArea = currChart->plotArea();
    if ( m_rubberBand && m_rubberBand->isEnabled() && event->button() == Qt::LeftButton && plotArea.contains( event->pos() ) )
    {
      m_rubberBandOrigin = event->pos();
      m_rubberBand->setGeometry( QRect( m_rubberBandOrigin, QSize() ) );
      m_rubberBand->show();
      event->accept();
    }
    else
    {
      QGraphicsView::mousePressEvent( event );
    }
  }

  void SPXChartView::mouseMoveEvent( QMouseEvent *event )
  {
    if ( m_rubberBand && m_rubberBand->isVisible() )
    {
      QRect rect = currChart->plotArea().toRect();
      int width = event->pos().x() - m_rubberBandOrigin.x();
      int height = event->pos().y() - m_rubberBandOrigin.y();
      if ( !m_rubberBandFlags.testFlag( VerticalRubberBand ) )
      {
        m_rubberBandOrigin.setY( rect.top() );
        height = rect.height();
      }
      if ( !m_rubberBandFlags.testFlag( HorizontalRubberBand ) )
      {
        m_rubberBandOrigin.setX( rect.left() );
        width = rect.width();
      }
      m_rubberBand->setGeometry( QRect( m_rubberBandOrigin.x(), m_rubberBandOrigin.y(), width, height ).normalized() );
    }
    else
    {
      //
      // in langsam...
      //
      QString unitLabel{"-"};
#ifdef DEBUG
      if ( currSeries )
      {
        qint64 normalTimeStamp = static_cast< qint64 >( currChart->mapToValue( event->pos(), currSeries ).x() );
        QString dateTimeStr = QDateTime::fromMSecsSinceEpoch( normalTimeStamp ).toString( "mm:ss" );
        lg->debug( QString( "TIME: %1, %2: %3" )
                       .arg( dateTimeStr )
                       .arg( unitLabel )
                       .arg( currChart->mapToValue( event->pos(), currSeries ).y(), 0, 'f', 2 ) );
      }
#endif
      QGraphicsView::mouseMoveEvent( event );
    }
  }

  void SPXChartView::mouseReleaseEvent( QMouseEvent *event )
  {
    if ( m_rubberBand && m_rubberBand->isVisible() )
    {
      if ( event->button() == Qt::LeftButton )
      {
        m_rubberBand->hide();
        QRectF rRectF = m_rubberBand->geometry();
        //
        // Since plotArea uses QRectF and rubberband uses QRect, we can't just blindly use
        // rubberband's dimensions for vertical and horizontal rubberbands, where one
        // dimension must match the corresponding plotArea dimension exactly.
        //
        // notification:
        //  void  plotAreaChanged(const QRectF &plotArea)
        if ( m_rubberBandFlags == VerticalRubberBand )
        {
          rRectF.setX( currChart->plotArea().x() );
          rRectF.setWidth( currChart->plotArea().width() );
        }
        else if ( m_rubberBandFlags == HorizontalRubberBand )
        {
          rRectF.setY( currChart->plotArea().y() );
          rRectF.setHeight( currChart->plotArea().height() );
        }
        lg->debug( "SPXChartView::mouseReleaseEvent -> zoom in..." );
        currChart->zoomIn( rRectF );
        //
        // Zeitachse versuchen
        //
        if ( dtAxis && currSeries )
        {
          QRect rect( currChart->plotArea().toRect() );
          setTimeAxis( dtAxis, currSeries, rect );
        }
        event->accept();
      }
    }
    else if ( m_rubberBand && event->button() == Qt::RightButton )
    {
      // If vertical or horizontal rubberband mode, restrict zoom out to specified axis.
      // Since there is no suitable API for that, use zoomIn with rect bigger than the
      // plot area.
      if ( m_rubberBandFlags == VerticalRubberBand || m_rubberBandFlags == HorizontalRubberBand )
      {
        QRectF rRectF = currChart->plotArea();
        if ( m_rubberBandFlags == VerticalRubberBand )
        {
          qreal adjustment = rRectF.height() / 2;
          rRectF.adjust( 0, -adjustment, 0, adjustment );
        }
        else if ( m_rubberBandFlags == HorizontalRubberBand )
        {
          qreal adjustment = rRectF.width() / 2;
          rRectF.adjust( -adjustment, 0, adjustment, 0 );
        }
        currChart->zoomIn( rRectF );
        //
        // Zeitachse versuchen
        //
        if ( dtAxis && currSeries )
        {
          QRect rect( currChart->plotArea().toRect() );
          setTimeAxis( dtAxis, currSeries, rect );
        }
        lg->debug( "SPXChartView::mouseReleaseEvent -> zoom in..." );
      }
      else
      {
        lg->debug( "SPXChartView::mouseReleaseEvent -> zoom out..." );
        currChart->zoomOut();
      }
      event->accept();
    }
    else if ( m_rubberBand && event->button() == Qt::MidButton )
    {
      if ( dtAxis )
      {
        lg->debug( "SPXChartView::mouseReleaseEvent -> axis reset..." );
        dtAxis->setRange( minTime, maxTime );
      }
      currChart->zoomReset();
    }
    else
    {
      QGraphicsView::mouseReleaseEvent( event );
    }
  }

  void SPXChartView::resizeEvent( QResizeEvent *event )
  {
    QGraphicsView::resizeEvent( event );
    c_resize();
  }

  void SPXChartView::tooltip( QPointF point, bool state )
  {
    lg->debug( QString( "SpxChartView::tooltip%1..." ).arg( currNumber ) );
    //
    if ( currCallout == nullptr )
      currCallout = new ChartGraphicalValueCallout( chart() );

    if ( state )
    {
      currCallout->setText( QString( "X: %1 \nY: %2 " ).arg( point.x() ).arg( point.y() ) );
      currCallout->setAnchor( point );
      currCallout->setZValue( 11 );
      currCallout->updateGeometry();
      currCallout->show();
    }
    else
    {
      currCallout->hide();
    }
  }

}  // namespace spx
