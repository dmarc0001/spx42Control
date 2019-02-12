#include "SPXChartView.hpp"

namespace spx
{
  SPXChartView::SPXChartView( std::shared_ptr< Logger > logger, QWidget *parent )
      : QGraphicsView( parent )
      , currScene( new QGraphicsScene( this ) )
      , currChart( nullptr )
      , currRubberBand( nullptr )
      , cursorRubberBand( nullptr )
      , currRubberBandFlags( SPXChartView::NoRubberBand )
      , lg( logger )
      , currCallout( nullptr )
      , isCursorRubberBand( true )
  {
    init();
  }

  SPXChartView::SPXChartView( std::shared_ptr< Logger > logger, QChart *chart, QWidget *parent )
      : QGraphicsView( parent )
      , currScene( new QGraphicsScene( this ) )
      , currChart( chart )
      , currRubberBand( nullptr )
      , cursorRubberBand( nullptr )
      , currRubberBandFlags( SPXChartView::NoRubberBand )
      , lg( logger )
      , currCallout( nullptr )
      , isCursorRubberBand( true )
  {
    init();
  }

  SPXChartView::~SPXChartView()
  {
    delete currCallout;
    delete cursorRubberBand;
    delete currRubberBand;
  }

  void SPXChartView::init()
  {
    setFrameShape( QFrame::NoFrame );
    setBackgroundRole( QPalette::Window );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setScene( currScene );
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
    currScene->addItem( currChart );
    setCursorRubberBand( isCursorRubberBand );
  }

  void SPXChartView::axisAndSeries()
  {
    dtAxis = nullptr;
    currSeries = nullptr;
    auto allAxes = currChart->axes( Qt::Horizontal );
    if ( allAxes.count() > 0 )
    {
      dtAxis = static_cast< QDateTimeAxis * >( allAxes.first() );
      if ( dtAxis->type() != QAbstractAxis::AxisTypeDateTime )
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
          // das ist das grosse chart
          currSeries = static_cast< QLineSeries * >( cSeries );
          currSeries->blockSignals( false );
          // TODO: Funktioniert hier nicht
          connect( currSeries, &QLineSeries::hovered, this, &SPXChartView::tooltip );
        }
        else if ( cSeries->name().compare( ChartDataWorker::ppo2SeriesName, Qt::CaseInsensitive ) == 0 )
        {
          // das ist das kleine chart
          currSeries = static_cast< QLineSeries * >( cSeries );
          currSeries->blockSignals( false );
          // TODO: Funktioniert hier nicht
          connect( currSeries, &QLineSeries::hovered, this, &SPXChartView::tooltip );
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
      currScene->removeItem( currChart );
    //
    // Merke als aktuell
    //
    currChart = chart;
    // zufügen
    currScene->addItem( currChart );
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
    //
    // welcher Wert ist der startpunkt / endpunkt ?
    //
    QPointF startPoint = currChart->mapToValue( QPoint( rect.x(), rect.y() ), dataSeries );
    QPointF endPoint = currChart->mapToValue( QPoint( rect.x() + rect.width(), rect.y() + rect.height() ), dataSeries );
    //
    // umrechnen in QDateTime
    //
    qint64 start_ms = static_cast< qint64 >( startPoint.x() );
    qint64 end_ms = static_cast< qint64 >( endPoint.x() );
    //
    // Achse anpassen
    //
    axis->setRange( QDateTime::fromMSecsSinceEpoch( start_ms ), QDateTime::fromMSecsSinceEpoch( end_ms ) );
  }

  void SPXChartView::setRubberBand( const RubberBands &rubberBand )
  {
    currRubberBandFlags = rubberBand;
    //
    // Ist kein Gummibandflag vorhanden, das Band löschen!
    //
    if ( !currRubberBandFlags )
    {
      delete currRubberBand;
      currRubberBand = nullptr;
      return;
    }
    //
    // ist kein gummiband obwohl flags da sind? Dann mache eines
    //
    if ( !currRubberBand )
    {
      currRubberBand = new QRubberBand( QRubberBand::Rectangle, this );
      currRubberBand->setEnabled( true );
    }
  }

  void SPXChartView::setCursorRubberBand( bool isSet )
  {
    isCursorRubberBand = isSet;
    if ( isCursorRubberBand )
    {
      if ( !cursorRubberBand )
      {
        //
        // wenn kein Band da ist, erzeug eines und setzte einen Anfangszustand
        //
        cursorRubberBand = new QRubberBand( QRubberBand::Line, this );
      }
      cursorRubberBand->setEnabled( true );
      QRect rect = currChart->plotArea().toRect();
      rect.setX( 0 );
      rect.setWidth( 2 );
      cursorRubberBand->setGeometry( rect.normalized() );
      cursorRubberBand->show();
    }
    else
    {
      delete cursorRubberBand;
      cursorRubberBand = nullptr;
    }
  }

  SPXChartView::RubberBands SPXChartView::rubberBand() const
  {
    //
    // das aktuelle Gummiband (oder nullptr)
    //
    return currRubberBandFlags;
  }

  void SPXChartView::mousePressEvent( QMouseEvent *event )
  {
    QRectF plotArea = currChart->plotArea();
    //
    // den Cursor ausblenden, das Gummiband wird genutzt
    //
    if ( cursorRubberBand && event->button() == Qt::LeftButton )
    {
      cursorRubberBand->hide();
      cursorRubberBand->setEnabled( false );
    }
    //
    // wurde die Maus im chart links gedrückt?
    //
    if ( currRubberBand && currRubberBand->isEnabled() && event->button() == Qt::LeftButton && plotArea.contains( event->pos() ) )
    {
      currRubberBandOriginPos = event->pos();
      currRubberBand->setGeometry( QRect( currRubberBandOriginPos, QSize() ) );
      currRubberBand->show();
      event->accept();
    }
    else
    {
      QGraphicsView::mousePressEvent( event );
    }
  }

  void SPXChartView::mouseMoveEvent( QMouseEvent *event )
  {
    if ( currRubberBand && currRubberBand->isVisible() )
    {
      //
      // ist das Gummiband sichtbar?
      // dann folge der Maus
      //
      QRect rect = currChart->plotArea().toRect();
      int width = event->pos().x() - currRubberBandOriginPos.x();
      int height = event->pos().y() - currRubberBandOriginPos.y();
      if ( !currRubberBandFlags.testFlag( VerticalRubberBand ) )
      {
        currRubberBandOriginPos.setY( rect.top() );
        height = rect.height();
      }
      if ( !currRubberBandFlags.testFlag( HorizontalRubberBand ) )
      {
        currRubberBandOriginPos.setX( rect.left() );
        width = rect.width();
      }
      currRubberBand->setGeometry( QRect( currRubberBandOriginPos.x(), currRubberBandOriginPos.y(), width, height ).normalized() );
    }
    else
    {
      if ( cursorRubberBand && cursorRubberBand->isEnabled() && cursorRubberBand->isVisible() )
      {
        QRect rect = currChart->plotArea().toRect();
        rect.setX( event->pos().x() );
        rect.setWidth( 2 );
        cursorRubberBand->setGeometry( rect.normalized() );
        cursorRubberBand->show();
        emit onCursorChangedSig( event->pos().x() );
      }
      QGraphicsView::mouseMoveEvent( event );
    }
  }

  void SPXChartView::mouseReleaseEvent( QMouseEvent *event )
  {
    //
    // der Cursor ist sichtbar
    //
    if ( cursorRubberBand && event->button() == Qt::LeftButton )
    {
      cursorRubberBand->show();
      cursorRubberBand->setEnabled( true );
    }
    //
    // das Gummiband, wenn vorhanden beenden und zoom einleiten
    //
    if ( currRubberBand && currRubberBand->isVisible() )
    {
      if ( event->button() == Qt::LeftButton )
      {
        currRubberBand->hide();
        QRectF rRectF = currRubberBand->geometry();
        //
        // Since plotArea uses QRectF and rubberband uses QRect, we can't just blindly use
        // rubberband's dimensions for vertical and horizontal rubberbands, where one
        // dimension must match the corresponding plotArea dimension exactly.
        //
        if ( currRubberBandFlags == VerticalRubberBand )
        {
          rRectF.setX( currChart->plotArea().x() );
          rRectF.setWidth( currChart->plotArea().width() );
        }
        else if ( currRubberBandFlags == HorizontalRubberBand )
        {
          rRectF.setY( currChart->plotArea().y() );
          rRectF.setHeight( currChart->plotArea().height() );
        }
        lg->debug( "SPXChartView::mouseReleaseEvent -> zoom in..." );
        currChart->zoomIn( rRectF );
        //
        // Zeitachse anpassen
        //
        if ( dtAxis && currSeries )
        {
          QRect rect( currChart->plotArea().toRect() );
          setTimeAxis( dtAxis, currSeries, rect );
        }
        emit onZoomChangedSig( rRectF );
        event->accept();
      }
    }
    else if ( currRubberBand && event->button() == Qt::RightButton )
    {
      //
      // If vertical or horizontal rubberband mode, restrict zoom out to specified axis.
      // Since there is no suitable API for that, use zoomIn with rect bigger than the
      // plot area.
      //
      if ( currRubberBandFlags == VerticalRubberBand || currRubberBandFlags == HorizontalRubberBand )
      {
        QRectF rRectF = currChart->plotArea();
        if ( currRubberBandFlags == VerticalRubberBand )
        {
          qreal adjustment = rRectF.height() / 2;
          rRectF.adjust( 0, -adjustment, 0, adjustment );
        }
        else if ( currRubberBandFlags == HorizontalRubberBand )
        {
          qreal adjustment = rRectF.width() / 2;
          rRectF.adjust( -adjustment, 0, adjustment, 0 );
        }
        currChart->zoomIn( rRectF );
        //
        // Zeitachse anpassen
        //
        if ( dtAxis && currSeries )
        {
          QRect rect( currChart->plotArea().toRect() );
          setTimeAxis( dtAxis, currSeries, rect );
        }
        emit onZoomChangedSig( rRectF );
        lg->debug( "SPXChartView::mouseReleaseEvent -> zoom in..." );
      }
      else
      {
        currChart->zoomOut();
        //
        // Zeitachse anpassen
        //
        QRectF rRectF = currChart->plotArea();
        if ( dtAxis && currSeries )
        {
          QRect rect( currChart->plotArea().toRect() );
          setTimeAxis( dtAxis, currSeries, rect );
        }
        emit onZoomChangedSig( rRectF );
        lg->debug( "SPXChartView::mouseReleaseEvent -> zoom out..." );
      }
      event->accept();
    }
    else if ( currRubberBand && event->button() == Qt::MidButton )
    {
      currChart->zoomReset();
      if ( dtAxis )
      {
        lg->debug( "SPXChartView::mouseReleaseEvent -> axis reset..." );
        //
        // auch hier die Achse anpassen
        //
        if ( dtAxis && currSeries )
        {
          QRect rect( currChart->plotArea().toRect() );
          setTimeAxis( dtAxis, currSeries, rect );
        }
        emit onZoomChangedSig( QRectF() );
      }
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
    lg->debug( "SpxChartView::tooltip..." );
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

  void SPXChartView::onZoomChangedSlot( const QRectF &rectF )
  {
    if ( rectF.isNull() )
    {
      //
      // der Befehl zum RESET
      //
      currChart->zoomReset();
      if ( dtAxis )
      {
        lg->debug( "SPXChartView::onCursorMoveSlot -> axis slave reset..." );
        //
        // auch hier die Achse anpassen
        //
        if ( dtAxis && currSeries )
        {
          QRect rect( currChart->plotArea().toRect() );
          setTimeAxis( dtAxis, currSeries, rect );
        }
      }
      return;
    }
    QRectF localRectF( rectF );
    QRectF localPlotArea = currChart->plotArea();
    localRectF.setY( localPlotArea.y() );
    localRectF.setHeight( localPlotArea.height() );
    QRect crect( localRectF.toRect() );
    setTimeAxis( dtAxis, currSeries, crect );
    currChart->zoomIn( localRectF );
  }

  void SPXChartView::onCursorChangedSlot( int x_value )
  {
    if ( cursorRubberBand && cursorRubberBand->isEnabled() && cursorRubberBand->isVisible() )
    {
      QRect rect = currChart->plotArea().toRect();
      rect.setX( x_value );
      rect.setWidth( 2 );
      cursorRubberBand->setGeometry( rect.normalized() );
      cursorRubberBand->show();
    }
  }
}  // namespace spx
