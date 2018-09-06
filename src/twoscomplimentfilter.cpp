#include "twoscomplimentfilter.h"

TwosComplimentFilter::TwosComplimentFilter(unsigned int frame_height, unsigned int frame_width) :
    frHeight(frame_height), frWidth(frame_width)
{
}

uint16_t* TwosComplimentFilter::apply_filter(uint16_t *pic_in)
{
    qDebug() << "hello";
    unsigned int row, col;
    for (row = 0; row < frHeight; row++) {
        for (col = 0; col< frWidth; col++) {
            pic_buffer[col + row * frWidth] = pic_in[col + row * frWidth] ^ 1<<15;
        }
    }
    memcpy(pic_in, pic_buffer, MAX_SIZE * sizeof(uint16_t));
    return pic_in;
}
