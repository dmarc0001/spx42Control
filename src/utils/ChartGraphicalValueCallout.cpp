#include "ChartGraphicalValueCallout.hpp"

namespace spx
{
  /**
   * @brief ChartGraphicalValueCallout::ChartGraphicalValueCallout gefunden in den QT Beispielen
   * @param parent
   */
  ChartGraphicalValueCallout::ChartGraphicalValueCallout( QChart *parent ) : QGraphicsItem( parent ), parentChart( parent )
  {
  }

  /**
   * @brief ChartGraphicalValueCallout::boundingRect
   * @return
   */
  QRectF ChartGraphicalValueCallout::boundingRect() const
  {
    //! das umschliessende Rechteck um den Ankerpunkt feststellen
    QPointF anchor = mapFromParent( parentChart->mapToPosition( calloutAnchor ) );
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
    // ein abgerundetes Rechteck als visuellen Rahmen
    //
    path.addRoundedRect( calloutLabelTextRectBound, 5, 5 );
    //
    // wo ist der ankerpunkt
    //
    QPointF paintAnchor = mapFromParent( parentChart->mapToPosition( calloutAnchor ) );
    //
    // wenn der Textrahmen nicht den Anker umschliesst
    //
    if ( !calloutLabelTextRectBound.contains( paintAnchor ) )
    {
      QPointF point1, point2;

      // establish the position of the anchor point in relation to m_rect
      bool above = paintAnchor.y() <= calloutLabelTextRectBound.top();
      bool aboveCenter =
          paintAnchor.y() > calloutLabelTextRectBound.top() && paintAnchor.y() <= calloutLabelTextRectBound.center().y();
      bool belowCenter =
          paintAnchor.y() > calloutLabelTextRectBound.center().y() && paintAnchor.y() <= calloutLabelTextRectBound.bottom();
      bool below = paintAnchor.y() > calloutLabelTextRectBound.bottom();

      bool onLeft = paintAnchor.x() <= calloutLabelTextRectBound.left();
      bool leftOfCenter =
          paintAnchor.x() > calloutLabelTextRectBound.left() && paintAnchor.x() <= calloutLabelTextRectBound.center().x();
      bool rightOfCenter =
          paintAnchor.x() > calloutLabelTextRectBound.center().x() && paintAnchor.x() <= calloutLabelTextRectBound.right();
      bool onRight = paintAnchor.x() > calloutLabelTextRectBound.right();

      // get the nearest m_rect corner.
      qreal x = ( onRight + rightOfCenter ) * calloutLabelTextRectBound.width();
      qreal y = ( below + belowCenter ) * calloutLabelTextRectBound.height();
      bool cornerCase = ( above && onLeft ) || ( above && onRight ) || ( below && onLeft ) || ( below && onRight );
      bool vertical = qAbs( paintAnchor.x() - x ) > qAbs( paintAnchor.y() - y );

      qreal x1 = x + leftOfCenter * 10 - rightOfCenter * 20 + cornerCase * !vertical * ( onLeft * 10 - onRight * 20 );
      qreal y1 = y + aboveCenter * 10 - belowCenter * 20 + cornerCase * vertical * ( above * 10 - below * 20 );
      ;
      point1.setX( x1 );
      point1.setY( y1 );

      qreal x2 = x + leftOfCenter * 20 - rightOfCenter * 10 + cornerCase * !vertical * ( onLeft * 20 - onRight * 10 );
      ;
      qreal y2 = y + aboveCenter * 20 - belowCenter * 10 + cornerCase * vertical * ( above * 20 - below * 10 );
      ;
      point2.setX( x2 );
      point2.setY( y2 );

      path.moveTo( point1 );
      path.lineTo( paintAnchor );
      path.lineTo( point2 );
      path = path.simplified();

      /*


      //
      // neue Koordinaten für den kleinen Hunweispfeil machen
      //
      QPointF point1, point2;
      //
      // Position des Ankerpunktes relativ zum umschliessenden Rechteck machen
      //
      bool above = paintAnchor.y() <= calloutLabelTextRectBound.top();
      bool aboveCenter =
          paintAnchor.y() > calloutLabelTextRectBound.top() && paintAnchor.y() <= calloutLabelTextRectBound.center().y();
      bool belowCenter =
          paintAnchor.y() > calloutLabelTextRectBound.center().y() && paintAnchor.y() <= calloutLabelTextRectBound.bottom();
      bool below = paintAnchor.y() > calloutLabelTextRectBound.bottom();
      //
      bool onLeft = paintAnchor.x() <= calloutLabelTextRectBound.left();
      bool leftOfCenter =
          paintAnchor.x() > calloutLabelTextRectBound.left() && paintAnchor.x() <= calloutLabelTextRectBound.center().x();
      bool rightOfCenter =
          paintAnchor.x() > calloutLabelTextRectBound.center().x() && paintAnchor.x() <= calloutLabelTextRectBound.right();
      bool onRight = paintAnchor.x() > calloutLabelTextRectBound.right();
      //
      // suche die naheste Ecke
      //
      qreal x = ( onRight + rightOfCenter ) * calloutLabelTextRectBound.width();
      qreal y = ( below + belowCenter ) * calloutLabelTextRectBound.height();
      bool cornerCase = ( above && onLeft ) || ( above && onRight ) || ( below && onLeft ) || ( below && onRight );
      bool vertical = qAbs( paintAnchor.x() - x ) > qAbs( paintAnchor.y() - y );
      //
      // mache den abstand
      //
      qreal x1 = x + leftOfCenter * 10 - rightOfCenter * 20 + cornerCase * !vertical * ( onLeft * 10 - onRight * 20 );
      qreal y1 = y + aboveCenter * 10 - belowCenter * 20 + cornerCase * vertical * ( above * 10 - below * 20 );
      qreal x2 = x + leftOfCenter * 20 - rightOfCenter * 10 + cornerCase * !vertical * ( onLeft * 20 - onRight * 10 );
      qreal y2 = y + aboveCenter * 20 - belowCenter * 10 + cornerCase * vertical * ( above * 20 - below * 10 );
      //
      // definiere den Anfang und das Ende des kleinen Pfeils
      //
      point1.setX( x1 );
      point1.setY( y1 );
      point2.setX( x2 );
      point2.setY( y2 );
      //
      // Den Pfad zum zeichnen vorbereiten
      //
      path.moveTo( point1 );
      path.lineTo( paintAnchor );
      path.lineTo( point2 );
      path = path.simplified();
*/
    }
    //
    // Farbe setzen
    //
    painter->setBrush( QColor( 255, 255, 255 ) );
    //
    // Zeichne das kleine Scheisserchen
    //
    painter->drawPath( path );
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
    calloutLabelTextRect = metrics.boundingRect( QRect( 0, 0, 150, 150 ), Qt::AlignLeft, calloutLabelText );
    calloutLabelTextRect.translate( 5, 5 );
    prepareGeometryChange();
    calloutLabelTextRectBound = calloutLabelTextRect.adjusted( -5, -5, 5, 5 );
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
    prepareGeometryChange();
    setPos( parentChart->mapToPosition( calloutAnchor ) );  // + QPointF( 10.5, -50.0 ) );
  }

}  // namespace spx
