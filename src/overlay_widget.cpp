#include "overlay_widget.h"
#include "constants.h"

/* #define QDEBUG */

overlay_widget::overlay_widget(FrameWorker *fw, QWidget *parent) : QWidget(parent)
{
    /*! \brief Establishes a plot for a specified image type.
     * \param image_type Determines the type of graph that will be output by profile_widget
     * \author Jackie Ryan
     * \author Noah Levy */

    leftWidget = new frameview_widget(fw, DSF, fw->settings);
    rightWidget = new line_widget(fw, SPATIAL_PROFILE);

    //QSizePolicy qsp(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //qsp.setHeightForWidth(true);
    //topWidget->setSizePolicy(qsp);
    //topWidget->heightForWidth(200);

    widgetLayout = new QHBoxLayout(this);
    widgetLayout->addWidget(leftWidget);
    widgetLayout->addWidget(rightWidget);
    this->setLayout(widgetLayout);

    connect(leftWidget->qcp, &QCustomPlot::mousePress, this, &overlay_widget::leftPlotClick);
    connect(rightWidget->qcp, &QCustomPlot::mousePress, this, &overlay_widget::rightPlotClick);
}

overlay_widget::~overlay_widget()
{
    delete overlay_img;
}

// public functions
double overlay_widget::getFloor()
{
    /*! \brief Return the value of the floor for this widget as a double */
    return floor;
}
double overlay_widget::getCeiling()
{
    /*! \brief Return the value of the ceiling for this widget as a double */
    return ceiling;
}

void overlay_widget::leftPlotClick(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton) {
        qDebug() << "Only left-right button";
        leftWidget->qcp->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(leftWidget->qcp, &QCustomPlot::customContextMenuRequested, this, &overlay_widget::leftPopupDialog);
    }
}

void overlay_widget::rightPlotClick(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton) {
        qDebug() << "Only right-right button";
        rightWidget->qcp->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(rightWidget->qcp, &QCustomPlot::customContextMenuRequested, this, &overlay_widget::rightPopupDialog);
    }
}

void overlay_widget::leftPopupDialog(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = leftWidget->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu *myMenu = new QMenu(this);
    myMenu->addAction("Live View", this, &overlay_widget::liveView);
    myMenu->addAction("Dark Subtraction",  this, &overlay_widget::darkSubtraction);
    myMenu->addAction("Standard Deviation",  this, &overlay_widget::standardDeviation);

    // Show context menu at handling position
    myMenu->exec(globalPos);
}

void overlay_widget::rightPopupDialog(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = rightWidget->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu *myMenu = new QMenu(this);
    myMenu->addAction("Spectral Profile", this, &overlay_widget::spectralProfile);
    myMenu->addAction("Spectral Mean",  this, &overlay_widget::spectralMean);
    myMenu->addAction("Spatial Profile",  this, &overlay_widget::spatialProfile);
    myMenu->addAction("Spatial Mean",  this, &overlay_widget::spatialMean);

    // Show context menu at handling position
    myMenu->exec(globalPos);
}

void overlay_widget::liveView()
{

}

void overlay_widget::darkSubtraction()
{

}

void overlay_widget::standardDeviation()
{

}

void overlay_widget::spectralProfile()
{
    image_type = SPECTRAL_PROFILE;
    rightWidget->setOverlayPlot(image_type);
}

void overlay_widget::spectralMean()
{
    image_type = SPECTRAL_MEAN;
    rightWidget->setOverlayPlot(image_type);

}

void overlay_widget::spatialProfile()
{
    image_type = SPATIAL_PROFILE;
    rightWidget->setOverlayPlot(image_type);
}

void overlay_widget::spatialMean()
{
    image_type = SPATIAL_MEAN;
    rightWidget->setOverlayPlot(image_type);
}

void overlay_widget::updateCeiling(int c)
{
    /*! \brief Change the value of the ceiling for this widget to the input parameter and replot the color scale. */
    ceiling = (double)c;
    rescaleRange();
}

void overlay_widget::updateFloor(int f)
{
    /*! \brief Change the value of the floor for this widget to the input parameter and replot the color scale. */
    floor = (double)f;
    rescaleRange();
}

void overlay_widget::rescaleRange()
{
    /*! \brief Set the color scale of the display to the last used values for this widget */
    //qcp->yAxis->setRange(QCPRange(floor, ceiling));
}

