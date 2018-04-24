#ifndef CONTROLSBOX_H
#define CONTROLSBOX_H

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QByteArray>

#include "frameworker.h"

class ControlsBox : public QWidget
{
    Q_OBJECT
public:
    explicit ControlsBox(FrameWorker* fw, QWidget* parent=NULL);
    ~ControlsBox();

public slots:
    void resetDir();
    void collectDSFMask();

private:
    FrameWorker* frame_handler;

    // QPushButton* resetButton;
    QLineEdit* dirEdit;

    QPushButton* maskButton;
    bool collecting_mask;
};

#endif // CONTROLSBOX_H
