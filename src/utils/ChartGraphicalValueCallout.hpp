#ifndef CHARTGRAPHICALVALUECALLOUT_HPP
#define CHARTGRAPHICALVALUECALLOUT_HPP

#include <QGraphicsSceneMouseEvent>
#include <QtCharts>
#include <QtGui/QFont>
#include <QtWidgets>
#include <memory>
#include "logging/Logger.hpp"

namespace spx
{
  class ChartGraphicalValueCallout : public QGraphicsItem
  {
    private:
    //! Mein Logger
    std::shared_ptr< Logger > lg;
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
    explicit ChartGraphicalValueCallout( std::shared_ptr< Logger > logger, QChart *parent );
    //! Callout Tedt setzten
    void setText( const QString &text );
    //! Setzte Anker des textes
    void setAnchor( const QPointF &point );
    //! Geometrie des Callout anpassen
    void updateGeometry();
    //! das umschliessende Rechteck
    QRectF boundingRect() const override;
    //! zeichne das Callout
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) override;

    protected:
    //! Mausclicks bemerken
    void mousePressEvent( QGraphicsSceneMouseEvent *event ) override;
    //! Mausbewegungen bemerken
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event ) override;
  };
}  // namespace spx
#endif  // CHARTGRAPHICALVALUECALLOUT_HPP
