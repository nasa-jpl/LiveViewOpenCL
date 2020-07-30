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

    connect(leftWidget, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(leftPlotClick(QMouseEvent*)));
    connect(rightWidget, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(rightPlotClick(QMouseEvent*)));
    //connect(rightWidget, SIGNAL(mousePress(mousePressEvent*)), this, SLOT(rightPlotClick(mousePressEvent*)));

    //setCeiling((1<<16) -1);
    //setFloor(0);

    /*connect(reset_zoom_btn, SIGNAL(released()), this, SLOT(defaultZoom())); // disconnect?
    connect(qcp, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(moveCallout(QMouseEvent*)));
    connect(qcp, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(setCallout(QMouseEvent*)));
    connect(qcp->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(profileScrolledX(QCPRange)));
    connect(qcp->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(profileScrolledY(QCPRange)));
    connect(showCalloutCheck, SIGNAL(clicked()), this, SLOT(hideCallout()));
    connect(&rendertimer, SIGNAL(timeout()), this, SLOT(handleNewFrame()));
    //connect(frame_handler, &FrameWorker::crosshairChanged, this, &line_widget::updatePlotTitle);

    connect(frame_handler->Camera, &CameraModel::timeout, this, [=]() {
        QVector<double> zero_data(x.length(), 0);
        //qcp->graph(0)->setData(x, zero_data);
        qcp->replot();
        // renderTimer.stop();
    });
    if (frame_handler->running()) {
        renderTimer.start(FRAME_DISPLAY_PERIOD_MSECS);
    }
    rendertimer.start(FRAME_DISPLAY_PERIOD_MSECS);*/
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
        leftWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(leftWidget, SIGNAL(customContextMenuRequested(QPoint&)), this, SLOT(leftPopupDialog(QPoint&)));
    }
}

void overlay_widget::rightPlotClick(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton) {
        qDebug() << "Only right-right button";
        rightWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(rightWidget, SIGNAL(customContextMenuRequested(QPoint&)), this, SLOT(rightPopupDialog(QPoint&)));
    }
}

void overlay_widget::leftPopupDialog(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = leftWidget->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu *myMenu = new QMenu(this);
    myMenu->addAction("Insert", this, SLOT(addItem()));
    myMenu->addAction("Erase",  this, SLOT(eraseItem()));

    // Show context menu at handling position
    myMenu->exec(globalPos);

    QAction *selectedItem = myMenu->exec(globalPos);

    if (selectedItem)
    {
        // something was chosen, do stuff
    }
    else
    {
        // nothing was chosen
    }
}

void overlay_widget::rightPopupDialog(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = rightWidget->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu *myMenu = new QMenu(this);
    myMenu->addAction("Insert", this, SLOT(addItem()));
    myMenu->addAction("Erase",  this, SLOT(eraseItem()));

    // Show context menu at handling position
    myMenu->exec(globalPos);

    QAction *selectedItem = myMenu->exec(globalPos);

    if (selectedItem)
    {
        // something was chosen, do stuff
    }
    else
    {
        // nothing was chosen
    }
}

// public slots
void overlay_widget::handleNewFrame()
{
    /*! \brief Plots a specific dimension profile.
     * \paragraph
     * The switch statement is a bit silly here, I only use it to differentiate the plot title and the type of profile array to use.
     * The y-axis data is reversed in these images.
     * \author Jackie Ryan
     */

    //if (!this->isHidden() && frame_handler->running()) {
        //QPointF *center = frame_handler->getCenter();
        //if (image_type == SPECTRAL_MEAN || image_type == SPATIAL_MEAN || center->x() > -0.1) {
            //y = (this->*p_getOverlay)(*center);
            //qcp->graph(0)->setData(x, y);
            // replotting is slow when the data set is chaotic... TODO: develop an optimization here
            //qcp->replot();
            /*if (!hideTracer->isChecked()) {
                callout->setText(QString(" x: %1 \n y: %2 ").arg(static_cast<int>(tracer->graphKey()))
                             .arg(y[static_cast<int>(tracer->graphKey())]));
            }*/
        //}
    //}

        /*case VERT_OVERLAY:
            local_image_ptr = fw->curFrame->vertical_mean_profile; // vertical profiles
            for (int r = 0; r < frHeight; r++)
            {
                y[r] = double(local_image_ptr[r]);
                y_lh[r] = double(fw->curFrame->vertical_mean_profile_lh[r]);
                y_rh[r] = double(fw->curFrame->vertical_mean_profile_rh[r]);
            }
            // display overlay
            qcp->graph(1)->setData(x, y_lh);
            qcp->graph(2)->setData(x, y_rh);
            break;*/

    // display x and y:
        //qcp->graph(0)->setData(x, y);
        //qcp->replot();

        //if (callout->visible())
            //updateCalloutValue();
        switch (image_type) {
        //case SPATIAL_MEAN: plotTitle->setText(QString("Horizontal Mean Profile")); break;
        //case HORIZONTAL_CROSS: plotTitle->setText(QString("Horizontal Profile centered @ y = %1").arg(fw->crosshair_y)); break;
        //case SPECTRAL_MEAN: plotTitle->setText(QString("Vertical Mean Profile")); break;
        //case VERTICAL_CROSS: plotTitle->setText(QString("Vertical Profile centered @ x = %1").arg(fw->crosshair_x)); break;
        //case VERT_OVERLAY: plotTitle->setText(QString("Vertical Overlay")); break; // TODO: Add useful things here
        default: break;
        }
    //} else {
        //plotTitle->setText("No Crosshair designated");
        //allow_callouts = false;
        //qcp->graph(0)->clearData();
        //qcp->replot();
    //}
}
//}

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

