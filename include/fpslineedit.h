#ifndef FPSLINEEDIT_H
#define FPSLINEEDIT_H

#include <QLineEdit>
#include <QTimer>

class FPSLineEdit : public QLineEdit
{
public:
    FPSLineEdit(QWidget *parent = nullptr);

public slots:
    virtual void focusInEvent(QFocusEvent *event) override;
};

#endif // FPSLINEEDIT_H
