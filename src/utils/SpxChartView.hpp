#ifndef SPXCHARTVIEW_HPP
#define SPXCHARTVIEW_HPP

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPointF>
#include <QRubberBand>
#include <QtCharts>
#include <memory>
#include "database/ChartDataWorker.hpp"
#include "logging/Logger.hpp"
#include "utils/ChartGraphicalValueCallout.hpp"

class QGraphicsScene;
class QRubberBand;

namespace spx
{
  class QChartViewPrivate;

  class SpxChartView : public QGraphicsView
  {
    Q_OBJECT
    Q_ENUMS( RubberBand )
    public:
    enum RubberBand
    {
      NoRubberBand = 0x0,
      VerticalRubberBand = 0x1,
      HorizontalRubberBand = 0x2,
      RectangleRubberBand = 0x3
    };
    Q_DECLARE_FLAGS( RubberBands, RubberBand )
    QGraphicsScene *m_scene;
    QChart *m_chart;
    QPoint m_rubberBandOrigin;
    QRubberBand *m_rubberBand;
    SpxChartView::RubberBands m_rubberBandFlags;

    private:
    std::shared_ptr< Logger > lg;
    ChartGraphicalValueCallout *currCallout;
    // QLineSeries *depthSeries;
    // QLineSeries *ppo2Series;
    QAreaSeries *depthArea;
    QDateTimeAxis *dtAxis;
    QLineSeries *tempSeries;
    // QDateTime minRange;
    // QDateTime maxRamge;
    static int counter;
    int currNumber;

    public:
    explicit SpxChartView( std::shared_ptr< Logger > logger, QWidget *parent = nullptr );
    explicit SpxChartView( std::shared_ptr< Logger > logger, QChart *chart, QWidget *parent = nullptr );
    ~SpxChartView() override;
    void setRubberBand( const RubberBands &rubberBands );
    RubberBands rubberBand() const;

    QChart *chart() const;
    void setChart( QChart *chart );

    //! Maus release (genutzt für rechten Mausklick)
    protected:
    virtual void resizeEvent( QResizeEvent *event ) override;
    virtual void mousePressEvent( QMouseEvent *event ) override;
    virtual void mouseMoveEvent( QMouseEvent *event ) override;
    virtual void mouseReleaseEvent( QMouseEvent *event ) override;

    // QScopedPointer< QChartViewPrivate > d_ptr;

    private:
    void init( void );
    void axisAndSeries( void );
    void c_resize( void );
    void tooltip( QPointF point, bool state );

    private:
    Q_DISABLE_COPY( SpxChartView )
  };

}  // namespace spx
#endif  // SPXCHARTVIEW_HPP
