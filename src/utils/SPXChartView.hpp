#ifndef SPXCHARTVIEW_HPP
#define SPXCHARTVIEW_HPP

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPointF>
#include <QRubberBand>
#include <QTimer>
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

  //! implementation als Ersatz für QChartView
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

    private:
    Q_DECLARE_FLAGS( RubberBands, RubberBand )
    //! Scene
    QGraphicsScene *currScene;
    //! das aktive Chart
    QChart *currChart;
    //! Startposition des Gummibandes
    QPoint currRubberBandOriginPos;
    //! Startpos für Callout
    QPointF oldCursorPos;
    //! Das aktuelle gummiband
    QRubberBand *currRubberBand;
    //! der Strich für den Cursor
    QRubberBand *cursorRubberBand;
    //! Flag für das gummiband (hier eigentlich immer horizontal)
    SPXChartView::RubberBands currRubberBandFlags;
    //! Mein Logger
    std::shared_ptr< Logger > lg;
    //! Hint...
    ChartGraphicalValueCallout *currCallout;
    //! Cursor erlaubt...
    bool isCursorRubberBand;
    //! Zeitachse
    QDateTimeAxis *dtAxis;
    //! die aktuelle Serie (Tiefe oder PPO2, je nach Chart)
    QLineSeries *currSeries;
    QLineSeries *depthSeries;
    QLineSeries *ppo2Series;
    //! Dummy Beschriftung für das hilfsschart
    QString dummyTitle;
    //! Timer für dei Anzeigeverzögerung der Callouts
    QTimer showTimer;
    QTimer hideTimer;

    public:
    //! Konstruktor allgemein
    explicit SPXChartView( std::shared_ptr< Logger > logger, QWidget *parent = nullptr );
    //! Konstruktor mit Chart
    explicit SPXChartView( std::shared_ptr< Logger > logger, QChart *chart, QWidget *parent = nullptr );
    //! Destruktor
    ~SPXChartView() override;
    //! Das Gummiband setzen
    void setRubberBand( const RubberBands &rubberBands );
    //! Cursoe einschalten
    void setCursorRubberBand( bool isSet = true );
    //! welche Gummibänder
    RubberBands rubberBand() const;
    //! gib das chart zurück
    QChart *chart() const;
    //! setzte ein neues Chart hinein (und gib den Besitzt an dem alten frei)
    void setChart( QChart *chart );

    protected:
    virtual void resizeEvent( QResizeEvent *event ) override;
    virtual void mousePressEvent( QMouseEvent *event ) override;
    virtual void mouseMoveEvent( QMouseEvent *event ) override;
    virtual void mouseReleaseEvent( QMouseEvent *event ) override;
    virtual void leaveEvent( QEvent *event ) override;

    private:
    Q_DISABLE_COPY( SPXChartView )
    //! initialisiere interne Sachen
    void init( void );
    //! finde Serien und Achsen
    void axisAndSeries( void );
    //! das Chart einpassen
    void c_resize( void );
    //! Zeitachse bei zoom anpassen
    void setTimeAxis( QDateTimeAxis *axis, QLineSeries *dataSeries, QRect &rect );
    //! zeige tooltip
    void tooltip( QPointF point, bool state );

    public slots:
    void onZoomChangedSlot( const QRectF &rect );
    void onCursorChangedSlot( int x_value );

    private slots:
    void onShowTimerTimeoutSlot( void );
    void onHideTimerTimeoutSlot( void );

    signals:
    void onZoomChangedSig( const QRectF &rect );
    void onCursorChangedSig( int x_value );
  };

}  // namespace spx
#endif  // SPXCHARTVIEW_HPP
