#include "ChartGraphicalValueCallout.hpp"

namespace spx
{
  /**
   * @brief ChartGraphicalValueCallout::ChartGraphicalValueCallout gefunden in den QT Beispielen
   * @param parent
   */
  ChartGraphicalValueCallout::ChartGraphicalValueCallout( std::shared_ptr< Logger > logger, QChart *parent )
      : QGraphicsItem( parent ), lg( logger ), parentChart( parent ), currSeries( nullptr )
  {
    // if ( parent != nullptr )
    // calloutLabelFont = parent->font();
    //
    // bessere Darstellung Monofont...
    //
    calloutLabelFont = QFont( "Monospace" );
    calloutLabelFont.setStyleHint( QFont::TypeWriter );
    //
    calloutLabelFont.setPointSize( 9 );
    calloutLabelFont.setWeight( QFont::Light );
  }

  /**
   * @brief ChartGraphicalValueCallout::boundingRect
   * @return
   */
  QRectF ChartGraphicalValueCallout::boundingRect() const
  {
    //! das umschliessende Rechteck um den Ankerpunkt feststellen
    QPointF anchor = mapFromParent( parentChart->mapToPosition( calloutAnchor, currSeries ) );
    QRectF rect;
    rect.setLeft( qMin( calloutLabelTextRectBound.left(), anchor.x() ) );
    rect.setRight( qMax( calloutLabelTextRectBound.right(), anchor.x() ) );
    rect.setTop( qMin( calloutLabelTextRectBound.top(), anchor.y() ) );
    rect.setBottom( qMax( calloutLabelTextRectBound.bottom(), anchor.y() ) );
    return rect;
  }

  /**
   * @brief ChartGraphicalValueCallout::paint
   * @param painter
   * @param option
   * @param widget
   */
  void ChartGraphicalValueCallout::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
  {
    Q_UNUSED( option )
    Q_UNUSED( widget )
    QPainterPath path;
    //
    // ein abgerundetes Rechteck als visuellen Rahmen für den Text
    //
    path.addRoundedRect( calloutLabelTextRectBound, 5, 5 );
    //
    // Farbe setzen
    // TODO: anpassen für Themes
    painter->setPen( QColor( 0, 0, 128 ) );
    painter->setBrush( QColor( 255, 255, 255 ) );
    //
    // Zeichne das kleine Scheisserchen
    //
    painter->drawPath( path );
    painter->setFont( calloutLabelFont );
    painter->drawText( calloutLabelTextRect, calloutLabelText );
  }

  /**
   * @brief ChartGraphicalValueCallout::mousePressEvent
   * @param event
   */
  void ChartGraphicalValueCallout::mousePressEvent( QGraphicsSceneMouseEvent *event )
  {
    event->setAccepted( true );
  }

  /**
   * @brief ChartGraphicalValueCallout::mouseMoveEvent
   * @param event
   */
  void ChartGraphicalValueCallout::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
  {
    //
    // Linke Maustaste?
    //
    if ( event->buttons() & Qt::LeftButton )
    {
      //
      // Die Position mal festhalten
      //
      setPos( mapToParent( event->pos() - event->buttonDownPos( Qt::LeftButton ) ) );
      event->setAccepted( true );
    }
    else
    {
      event->setAccepted( false );
    }
  }

  /**
   * @brief ChartGraphicalValueCallout::setText
   * @param text
   */
  void ChartGraphicalValueCallout::setText( const QString &text )
  {
    calloutLabelText = text;
    //
    // finde die Größe des umgebenden Rechrteckes heraus
    //
    QFontMetrics metrics( calloutLabelFont );
    calloutLabelTextRect = metrics.boundingRect( QRect( 0, 0, 250, 80 ), Qt::AlignLeft, calloutLabelText );
    // Verschiebe es vom Nullpunkt weg
    calloutLabelTextRect.translate( 6, 8 );
    prepareGeometryChange();
    // das umschliessende Rechteck etwas grösser
    calloutLabelTextRectBound = calloutLabelTextRect.adjusted( -8.0, -8.0, 8.0, 8.0 );
  }

  /**
   * @brief ChartGraphicalValueCallout::setAnchor
   * @param point
   */
  void ChartGraphicalValueCallout::setAnchor( const QPointF &point )
  {
    //
    // setze den Ankerpunkt
    //
    calloutAnchor = point;
  }

  /**
   * @brief ChartGraphicalValueCallout::updateGeometry
   */
  void ChartGraphicalValueCallout::updateGeometry()
  {
    // zerst ausführen zur Vorbereitung
    prepareGeometryChange();
    //
    // finde heraus ob genügend Platz wäre
    //
    QRectF plotArea = parentChart->plotArea();
    QPointF tempAnchor = calloutAnchor + QPointF( 16, -4 );

    if ( ( tempAnchor.x() + calloutLabelTextRectBound.width() ) > ( plotArea.x() + plotArea.width() ) )
    {
      tempAnchor -= QPointF( 32 + calloutLabelTextRectBound.width(), 0 );
    }
    if ( ( tempAnchor.y() + calloutLabelTextRectBound.height() ) > ( plotArea.y() + plotArea.height() ) )
    {
      tempAnchor -= QPointF( 0, 8 + calloutLabelTextRectBound.height() );
    }
    calloutAnchor = tempAnchor;
    //
    // hinschieben wo es soll
    //
    setPos( calloutAnchor );
  }

  /**
   * @brief ChartGraphicalValueCallout::setCurrentSeries
   * @param cs
   */
  void ChartGraphicalValueCallout::setCurrentSeries( QAbstractSeries *cs )
  {
    currSeries = cs;
  }
}  // namespace spx
