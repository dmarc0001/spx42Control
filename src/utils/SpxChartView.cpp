#include "SpxChartView.hpp"

namespace spx
{
  int SpxChartView::counter{0};

  SpxChartView::SpxChartView( std::shared_ptr< Logger > logger, QWidget *parent )
      : QGraphicsView( parent )
      , m_scene( new QGraphicsScene( this ) )
      , m_chart( nullptr )
      , m_rubberBand( nullptr )
      , m_rubberBandFlags( SpxChartView::NoRubberBand )
      , lg( logger )
      , currCallout( nullptr )
  {
    currNumber = ++SpxChartView::counter;
    init();
  }

  SpxChartView::SpxChartView( std::shared_ptr< Logger > logger, QChart *chart, QWidget *parent )
      : QGraphicsView( parent )
      , m_scene( new QGraphicsScene( this ) )
      , m_chart( chart )
      , m_rubberBand( nullptr )
      , m_rubberBandFlags( SpxChartView::NoRubberBand )
      , lg( logger )
      , currCallout( nullptr )
  {
    currNumber = ++SpxChartView::counter;
    init();
  }

  SpxChartView::~SpxChartView()
  {
    SpxChartView::counter--;
    delete currCallout;
    // QChartView::~QChartView();
  }

  void SpxChartView::init()
  {
    setFrameShape( QFrame::NoFrame );
    setBackgroundRole( QPalette::Window );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setScene( m_scene );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    if ( !m_chart )
      m_chart = new QChart();
    m_scene->addItem( m_chart );

    //

    setDragMode( QGraphicsView::NoDrag );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setMouseTracking( true );
    //
    axisAndSeries();
  }

  void SpxChartView::axisAndSeries()
  {
    // depthSeries = nullptr;
    // ppo2Series = nullptr;
    depthArea = nullptr;
    dtAxis = nullptr;
    tempSeries = nullptr;
    auto allAxes = m_chart->axes( Qt::Horizontal );
    if ( allAxes.count() > 0 )
    {
      dtAxis = static_cast< QDateTimeAxis * >( allAxes.first() );
    }

    auto seriesList = m_chart->series();
    for ( auto *currSerie : seriesList )
    {
      if ( currSerie->type() == QAbstractSeries::SeriesTypeLine || currSerie->type() == QAbstractSeries::SeriesTypeArea )
      {
        if ( currSerie->name().compare( ChartDataWorker::depthAreaSeriesName, Qt::CaseInsensitive ) == 0 )
        {
          depthArea = static_cast< QAreaSeries * >( currSerie );
        }
        else if ( currSerie->name().compare( ChartDataWorker::tempSeriesName, Qt::CaseInsensitive ) == 0 )
        {
          tempSeries = static_cast< QLineSeries * >( currSerie );
        }
      }
    }
  }

  QChart *SpxChartView::chart() const
  {
    return m_chart;
  }

  void SpxChartView::setChart( QChart *chart )
  {
    Q_ASSERT( chart );

    if ( m_chart == chart )
      return;

    if ( m_chart )
      m_scene->removeItem( m_chart );

    m_chart = chart;
    m_scene->addItem( m_chart );

    axisAndSeries();
    c_resize();
  }

  void SpxChartView::c_resize()
  {
    // Fit the chart into view if it has been rotated
    qreal sinA = qAbs( transform().m21() );
    qreal cosA = qAbs( transform().m11() );
    QSize chartSize = size();

    if ( sinA == 1.0 )
    {
      chartSize.setHeight( size().width() );
      chartSize.setWidth( size().height() );
    }
    else if ( sinA != 0.0 )
    {
      // Non-90 degree rotation, find largest square chart that can fit into the view.
      qreal minDimension = qMin( size().width(), size().height() );
      qreal h = ( minDimension - ( minDimension / ( ( sinA / cosA ) + 1.0 ) ) ) / sinA;
      chartSize.setHeight( static_cast< int >( h ) );
      chartSize.setWidth( static_cast< int >( h ) );
    }

    m_chart->resize( chartSize );
    setMinimumSize( m_chart->minimumSize().toSize().expandedTo( minimumSize() ) );
    setMaximumSize( maximumSize().boundedTo( m_chart->maximumSize().toSize() ) );
    setSceneRect( m_chart->geometry() );
  }

  void SpxChartView::setRubberBand( const RubberBands &rubberBand )
  {
    m_rubberBandFlags = rubberBand;

    if ( !m_rubberBandFlags )
    {
      delete m_rubberBand;
      m_rubberBand = nullptr;
      return;
    }

    if ( !m_rubberBand )
    {
      m_rubberBand = new QRubberBand( QRubberBand::Rectangle, this );
      m_rubberBand->setEnabled( true );
    }
  }

  SpxChartView::RubberBands SpxChartView::rubberBand() const
  {
    return m_rubberBandFlags;
  }

  void SpxChartView::mousePressEvent( QMouseEvent *event )
  {
    QRectF plotArea = m_chart->plotArea();
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

  void SpxChartView::mouseMoveEvent( QMouseEvent *event )
  {
    if ( m_rubberBand && m_rubberBand->isVisible() )
    {
      QRect rect = m_chart->plotArea().toRect();
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
      QGraphicsView::mouseMoveEvent( event );
    }
  }

  void SpxChartView::mouseReleaseEvent( QMouseEvent *event )
  {
    if ( m_rubberBand && m_rubberBand->isVisible() )
    {
      if ( event->button() == Qt::LeftButton )
      {
        m_rubberBand->hide();
        QRectF rect = m_rubberBand->geometry();
        //
        // Since plotArea uses QRectF and rubberband uses QRect, we can't just blindly use
        // rubberband's dimensions for vertical and horizontal rubberbands, where one
        // dimension must match the corresponding plotArea dimension exactly.
        //
        // notification:
        //  void  plotAreaChanged(const QRectF &plotArea)
        if ( m_rubberBandFlags == VerticalRubberBand )
        {
          rect.setX( m_chart->plotArea().x() );
          rect.setWidth( m_chart->plotArea().width() );
        }
        else if ( m_rubberBandFlags == HorizontalRubberBand )
        {
          rect.setY( m_chart->plotArea().y() );
          rect.setHeight( m_chart->plotArea().height() );
        }
        m_chart->zoomIn( rect );
        /*
        //
        // werte beim lösen der Maus
        //
        auto const widgetPos = event->localPos();
        lg->debug( QString( " #### widget: x:%1 y:%2" ).arg( widgetPos.x() ).arg( widgetPos.y() ) );
        lg->debug( QString( " #### rect: x:%1 y:%2" ).arg( rect.x() ).arg( rect.y() ) );

        auto const scenePos = mapToScene( QPoint( static_cast< int >( rect.x() ), static_cast< int >( rect.y() ) ) );
        lg->debug( QString( " #### scene: x:%1 y:%2" ).arg( scenePos.x() ).arg( scenePos.y() ) );

        auto const chartItemPos = m_chart->mapFromScene( scenePos );
        lg->debug( QString( " #### chart: x:%1 y:%2" ).arg( chartItemPos.x() ).arg( chartItemPos.y() ) );
        if ( depthArea )
        {
          auto const depthValues = m_chart->mapToValue( chartItemPos, depthArea );
          auto timeStamp = depthValues.toPoint().x();
          auto depth = depthValues.y();
          lg->debug( QString( "###############  DEPTH tm %1  Y: %2" ).arg( timeStamp ).arg( depth, 2, 'f', 1 ) );
        }
        if ( tempSeries )
        {
          auto const tempValues = m_chart->mapToValue( chartItemPos, tempSeries );
          auto timeStamp = tempValues.toPoint().x();
          auto temp = tempValues.y();
          lg->debug( QString( "###############  TEMP tm  %1  Y: %2" ).arg( timeStamp ).arg( temp, 2, 'f', 1 ) );
        }
        m_chart->zoomIn( rect );
        */
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
        QRectF rect = m_chart->plotArea();
        if ( m_rubberBandFlags == VerticalRubberBand )
        {
          qreal adjustment = rect.height() / 2;
          rect.adjust( 0, -adjustment, 0, adjustment );
        }
        else if ( m_rubberBandFlags == HorizontalRubberBand )
        {
          qreal adjustment = rect.width() / 2;
          rect.adjust( -adjustment, 0, adjustment, 0 );
        }
        m_chart->zoomIn( rect );
      }
      else
      {
        m_chart->zoomOut();
      }
      event->accept();
    }
    else if ( m_rubberBand && event->button() == Qt::MidButton )
    {
      m_chart->zoomReset();
    }
    else
    {
      QGraphicsView::mouseReleaseEvent( event );
    }
  }

  void SpxChartView::resizeEvent( QResizeEvent *event )
  {
    QGraphicsView::resizeEvent( event );
    c_resize();
  }

  void SpxChartView::tooltip( QPointF point, bool state )
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
