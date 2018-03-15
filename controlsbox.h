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

private:
    FrameWorker* frame_handler;

    // QPushButton* resetButton;
    QLineEdit* dirEdit;
};

#endif // CONTROLSBOX_H
