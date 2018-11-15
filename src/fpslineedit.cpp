#include "fpslineedit.h"

FPSLineEdit::FPSLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void FPSLineEdit::focusInEvent(QFocusEvent *event)
{
    // First let the base class process the event
    QLineEdit::focusInEvent(event);
    // Then select the text by a single shot timer, so that everything will
    // be processed before (calling selectAll() directly won't work)
    QTimer::singleShot(0, this, &QLineEdit::selectAll);
}
