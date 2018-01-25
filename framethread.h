#ifndef FRAMETHREAD_H
#define FRAMETHREAD_H

#include <QThread>

class FrameThread : public QThread {
    public:
        void sleep(int ms) { QThread::msleep(ms); }
};


#endif // FRAMETHREAD_H
