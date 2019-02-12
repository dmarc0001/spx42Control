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
  constexpr int LEFT_MARGIN = 12;
  constexpr int RIGHT_MARGIN = LEFT_MARGIN;
  constexpr int TOP_MARGIN = 4;
  constexpr int BOTTOM_MARGIN = TOP_MARGIN;

  class QChartViewPrivate;

  class SPXChartView : public QGraphicsView
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
    QChart *currChart;
    QPoint m_rubberBandOrigin;
    QRubberBand *m_rubberBand;
    SPXChartView::RubberBands m_rubberBandFlags;

    private:
    std::shared_ptr< Logger > lg;
    ChartGraphicalValueCallout *currCallout;
    QLineSeries *depthSeries;
    QLineSeries *ppo2Series;
    QAreaSeries *depthAreaSeries;
    QDateTimeAxis *dtAxis;
    QLineSeries *tempSeries;
    QLineSeries *currSeries;
    QDateTime minTime;
    QDateTime maxTime;
    static int counter;
    int currNumber;

    public:
    explicit SPXChartView( std::shared_ptr< Logger > logger, QWidget *parent = nullptr );
    explicit SPXChartView( std::shared_ptr< Logger > logger, QChart *chart, QWidget *parent = nullptr );
    ~SPXChartView() override;
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
    void setTimeAxis( QDateTimeAxis *axis, QLineSeries *dataSeries, QRect &rect );
    void tooltip( QPointF point, bool state );

    private:
    Q_DISABLE_COPY( SPXChartView )
  };

}  // namespace spx
#endif  // SPXCHARTVIEW_HPP
