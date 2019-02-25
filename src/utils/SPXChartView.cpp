#include "SPXChartView.hpp"

namespace spx
{
  /**
   * @brief SPXChartView::SPXChartView
   * @param logger
   * @param parent
   */
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
      , currSeries( nullptr )
      , depthSeries( nullptr )
      , ppo2Series( nullptr )
      , setpointSeries( nullptr )
      , temperaturSeries( nullptr )
  {
    init();
  }

  /**
   * @brief SPXChartView::SPXChartView
   * @param logger
   * @param chart
   * @param parent
   */
  SPXChartView::SPXChartView( std::shared_ptr< Logger > logger, QChart *chart, QWidget *parent )
      : QGraphicsView( parent )
      , currScene( new QGraphicsScene( this ) )
      , currChart( chart )
      , currRubberBand( nullptr )
      , cursorRubberBand( nullptr )
      , currRubberBandFlags( SPXChartView::NoRubberBand )
      , lg( logger )
      , currCallout( new ChartGraphicalValueCallout( logger, chart ) )
      , isCursorRubberBand( true )
      , currSeries( nullptr )
      , depthSeries( nullptr )
      , ppo2Series( nullptr )
      , setpointSeries( nullptr )
      , temperaturSeries( nullptr )
  {
    init();
  }

  /**
   * @brief SPXChartView::~SPXChartView
   */
  SPXChartView::~SPXChartView()
  {
    lg->debug( "SPXChartView::~SPXChartView..." );
    if ( currChart )
      tooltip( QPointF( 0.0, 0.0 ), false );
    lg->debug( "SPXChartView::~SPXChartView -> delete currCallout..." );
    delete currCallout;
    lg->debug( "SPXChartView::~SPXChartView -> delete cursor rubber band..." );
    delete cursorRubberBand;
    lg->debug( "SPXChartView::~SPXChartView -> delete rubber band..." );
    delete currRubberBand;
    lg->debug( "SPXChartView::~SPXChartView...OK" );
  }

  /**
   * @brief SPXChartView::init
   */
  void SPXChartView::init()
  {
    dummyTitle = tr( "DUMMY CHART" );
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
    {
      // ein Dummy machen
      currChart = new QChart();
      currChart->setTitle( dummyTitle );
    }
    else
      // ein echter Chart ist da, also veruche mal was
      axisAndSeries();
    currScene->addItem( currChart );
    setCursorRubberBand( isCursorRubberBand );
    showTimer.setSingleShot( true );
    hideTimer.setSingleShot( true );
    connect( &showTimer, &QTimer::timeout, this, &SPXChartView::onShowTimerTimeoutSlot );
    connect( &hideTimer, &QTimer::timeout, this, &SPXChartView::onHideTimerTimeoutSlot );
  }

  /**
   * @brief SPXChartView::axisAndSeries
   */
  void SPXChartView::axisAndSeries()
  {
    dtAxis = nullptr;
    currSeries = nullptr;
    depthSeries = nullptr;
    ppo2Series = nullptr;
    setpointSeries = nullptr;
    temperaturSeries = nullptr;
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
          depthSeries = currSeries;
        }
        else if ( cSeries->name().compare( ChartDataWorker::ppo2SeriesName, Qt::CaseInsensitive ) == 0 )
        {
          // das ist das kleine chart
          currSeries = static_cast< QLineSeries * >( cSeries );
          ppo2Series = currSeries;
        }
        else if ( cSeries->name().compare( ChartDataWorker::tempSeriesName, Qt::CaseInsensitive ) == 0 )
        {
          temperaturSeries = static_cast< QLineSeries * >( cSeries );
        }
        else if ( cSeries->name().compare( ChartDataWorker::setpointSeriesName, Qt::CaseInsensitive ) == 0 )
        {
          setpointSeries = static_cast< QLineSeries * >( cSeries );
        }
      }
    }
    currCallout->setCurrentSeries( currSeries );
  }

  /**
   * @brief SPXChartView::chart
   * @return
   */
  QChart *SPXChartView::chart() const
  {
    return currChart;
  }

  /**
   * @brief SPXChartView::setChart
   * @param chart
   */
  void SPXChartView::setChart( QChart *chart )
  {
    Q_ASSERT( chart );
    //
    // das selbe chart ist schon da, ignoriere dann
    //
    if ( currChart == chart )
      return;
    //
    // ist ein Callout da (dann ist er ja auch gebunden)
    //
    if ( currCallout )
    {
      delete currCallout;
      currCallout = nullptr;
    }
    //
    // ist das alte chart tatsächlich vorhanden
    //
    if ( currChart )
    {
      // entferne es aus der Umgebung
      currScene->removeItem( currChart );
      if ( currChart->title().compare( dummyTitle ) == 0 )
      {
        // es ist das interne Notchart, entferne es
        delete currChart;
      }
    }
    //
    // Merke als aktuell
    //
    currChart = chart;
    // zufügen
    currScene->addItem( currChart );
    currCallout = new ChartGraphicalValueCallout( lg, chart );
    currCallout->setCurrentSeries( currSeries );
    //
    // Bearbeiten
    //
    axisAndSeries();
    c_resize();
  }

  /**
   * @brief SPXChartView::c_resize
   */
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

  /**
   * @brief SPXChartView::setTimeAxis
   * @param axis
   * @param dataSeries
   * @param rect
   */
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

  /**
   * @brief SPXChartView::setRubberBand
   * @param rubberBand
   */
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

  /**
   * @brief SPXChartView::setCursorRubberBand
   * @param isSet
   */
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

  /**
   * @brief SPXChartView::rubberBand
   * @return
   */
  SPXChartView::RubberBands SPXChartView::rubberBand() const
  {
    //
    // das aktuelle Gummiband (oder nullptr)
    //
    return currRubberBandFlags;
  }

  /**
   * @brief SPXChartView::mousePressEvent
   * @param event
   */
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
      oldCursorPos = QPointF();  // ungültig...
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

  /**
   * @brief SPXChartView::mouseMoveEvent
   * @param event
   */
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
      if ( oldCursorPos.isNull() )
      {
        oldCursorPos = QPointF( event->pos() );
      }
      else
      {
        //
        // timer neu starten, der slot wird ausgelöst,
        // wenn die Maus bis zum timeout nicht bewegt wird
        //
        showTimer.start( 250 );
        //
        // wenn sich der Kursor zu weit bewegt, tooltip löschen
        //
        QPointF diff = QPointF( event->pos() ) - oldCursorPos;
        if ( diff.manhattanLength() > 5 )
        {
          tooltip( oldCursorPos, false );
          oldCursorPos = event->pos();
        }
      }
      QGraphicsView::mouseMoveEvent( event );
    }
  }

  /**
   * @brief SPXChartView::mouseReleaseEvent
   * @param event
   */
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

  /**
   * @brief SPXChartView::leaveEvent
   * @param event
   */
  void SPXChartView::leaveEvent( QEvent *event )
  {
    //
    // die Maus verlässt uns...
    //
    lg->debug( "SPXChartView::leaveEvent..." );
    // nicht nachträglich anzeigen!
    showTimer.stop();
    hideTimer.start( 350 );
    event->ignore();
  }

  /**
   * @brief SPXChartView::onShowTimerTimeoutSlot
   */
  void SPXChartView::onShowTimerTimeoutSlot()
  {
    if ( cursorRubberBand && cursorRubberBand->isEnabled() && cursorRubberBand->isVisible() )
    {
      lg->debug( "SPXChartView::onShowTimerTimeoutSlot..." );
      if ( !oldCursorPos.isNull() )
      {
        tooltip( oldCursorPos, true );
      }
      else
      {
        //
        // Keine Position vorhanden, also nix zeigen
        //
        currCallout->hide();
      }
      //
      // präpariere den Timer hzum ausschalten des tooltip
      //
      hideTimer.start( 8000 );
    }
  }

  /**
   * @brief SPXChartView::onHideTimerTimeoutSlot
   */
  void SPXChartView::onHideTimerTimeoutSlot()
  {
    lg->debug( "SPXChartView::onHideTimerTimeoutSlot..." );
    //
    // nach ablauf der zeit den tooltip verbergen
    //
    currCallout->hide();
  }

  /**
   * @brief SPXChartView::resizeEvent
   * @param event
   */
  void SPXChartView::resizeEvent( QResizeEvent *event )
  {
    QGraphicsView::resizeEvent( event );
    c_resize();
  }

  /**
   * @brief SPXChartView::tooltip
   * @param point
   * @param state
   */
  void SPXChartView::tooltip( QPointF point, bool state )
  {
    //
    try
    {
      if ( state )
      {
        QString tipText;
        QPointF dataPoint = currChart->mapToValue( point, currSeries );
        QDateTime qtimeVal = QDateTime::fromMSecsSinceEpoch( static_cast< qint64 >( dataPoint.x() ) );
        if ( depthSeries )
        {
          //
          // Zeit und Tiefe in den Text einbringen
          //
          qreal currDepth = getValueForTime( depthSeries, dataPoint.x() );
          if ( currDepth > 10000.0 )
            return;
          //
          // der Text ist in Monospace, daher mit Leerzeichen auszurichten
          //
          tipText = QString( tr( "DIVETIME:%1\nDEPTH   :%2 m" ).arg( qtimeVal.toString( "H:mm:ss" ) ).arg( abs( currDepth ) ) );
          //
          // gibt es Temperaturen?
          //
          if ( temperaturSeries )
          {
            qreal currTemp = getValueForTime( temperaturSeries, dataPoint.x() );
            //
            // gibt es sinnvolle Werte, dann zum text dazu
            //
            if ( currTemp < 100.0 )
            {
              tipText.append( QString( tr( "\nTEMP    :%1 °C" ) ).arg( currTemp, 1, 'f', 1 ) );
            }
          }
        }
        else if ( ppo2Series )
        {
          //
          // Zeit, ppo2 und setpoint einbringen
          //
          qreal currppo2 = getValueForTime( ppo2Series, dataPoint.x() );
          if ( currppo2 > 10000.0 )
            return;
          //
          // der Text ist in Monospace, daher mit Leerzeichen auszurichten
          //
          tipText = QString( tr( "DIVETIME:%1\nPPO2    :%2 bar" ).arg( qtimeVal.toString( "H:mm:ss" ) ).arg( currppo2, 1, 'f', 1 ) );
          //
          // sind setpoint werte vorhanden?
          //
          if ( setpointSeries )
          {
            qreal currSetpoint = getValueForTime( setpointSeries, dataPoint.x() );
            //
            // gibt es sinnvolle Werte?
            //
            if ( currSetpoint < 3.0 )
            {
              tipText.append( QString( tr( "\nSETPT   :%1 bar" ) ).arg( currSetpoint, 1, 'f', 1 ) );
            }
          }
        }
        else
        {
          tipText = QString( tr( "DIVETIME:%1" ).arg( qtimeVal.toString( "H:mm:ss" ) ) );
        }
        currCallout->setText( tipText );
        currCallout->setAnchor( point );
        currCallout->setZValue( 11 );
        currCallout->updateGeometry();
        currCallout->show();
        currCallout->update();
      }
      else
      {
        currCallout->hide();
      }
    }
    catch ( const std::exception &ex )
    {
      lg->crit( QString( "SPXChartView::tooltip -> exception: %1" ).arg( ex.what() ) );
    }
  }

  /**
   * @brief SPXChartView::getValueForTime
   * @param series
   * @param timeVal
   * @return
   */
  qreal SPXChartView::getValueForTime( QLineSeries *series, qreal timeVal )
  {
    qreal currVal = 11000.0;
    // Distanz / Schrittweite der Skala, sollte gleichförmig sein, beim TG kan man logintervall nicht wechseln
    qreal half_distance = ( series->at( 1 ).x() - series->at( 0 ).x() ) / 2.0;
    // Bis mir was besseres einfällt stumpf suchen
    for ( int idx = 0; idx < series->count(); idx++ )
    {
      const QPointF &pRef = series->at( idx );
      if ( approximatelyEqual( pRef.x(), timeVal, half_distance ) )
      {
        currVal = pRef.y();
        break;
      }
    }
    return ( currVal );
  }

  /**
   * @brief SPXChartView::approximatelyEqual
   * @param a
   * @param b
   * @return
   */
  bool SPXChartView::approximatelyEqual( qreal a, qreal b, qreal dist )
  {
    return fabs( a - b ) <= dist;
  }

  /**
   * @brief SPXChartView::onZoomChangedSlot
   * @param rectF
   */
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

  /**
   * @brief SPXChartView::onCursorChangedSlot
   * @param x_value
   */
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
