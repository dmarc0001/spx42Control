#ifndef CHARTGRAPHICALVALUECALLOUT_HPP
#define CHARTGRAPHICALVALUECALLOUT_HPP

#include <QGraphicsSceneMouseEvent>
#include <QtCharts>
#include <QtGui/QFont>
#include <QtWidgets>

namespace sqx
{
  class ChartGraphicalValueCallout : public QGraphicsItem
  {
    private:
    // Speicher für den Text des Callout
    QString calloutLabelText;
    //! Text Rechteck
    QRectF calloutLabelTextRect;
    // 1 Rechteck um den Text mit Rahmen
    QRectF calloutLabelTextRectBound;
    //! Ankerpunkt des Callout
    QPointF calloutAnchor;
    //! Font des Textes
    QFont calloutLabelFont;
    //! Chart an dem das Callout klebt
    QChart *parentChart;

    public:
    //! Konstruktor
    ChartGraphicalValueCallout( QChart *parent );
    //! Callout Tedt setzten
    void setText( const QString &text );
    //! Setzte Anker des textes
    void setAnchor( QPointF point );
    //! Geometrie des Callout anpassen
    void updateGeometry();
    //! das umschliessende Rechteck
    QRectF boundingRect() const;
    //! zeichne das Callout
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

    protected:
    //! Mausclicks bemerken
    void mousePressEvent( QGraphicsSceneMouseEvent *event );
    //! Mausbewegungen bemerken
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
  };
}  // namespace sqx
#endif  // CHARTGRAPHICALVALUECALLOUT_HPP
