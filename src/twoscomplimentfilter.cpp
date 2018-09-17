#include "twoscomplimentfilter.h"

TwosComplimentFilter::TwosComplimentFilter(unsigned int frame_height, unsigned int frame_width) :
    frHeight(frame_height), frWidth(frame_width)
{
}

void TwosComplimentFilter::apply_filter(uint16_t *pic_in)
{
    unsigned int row, col;
    for (row = 0; row < frHeight; row++) {
        for (col = 0; col< frWidth; col++) {
            pic_buffer[col + row * frWidth] = pic_in[col + row * frWidth] ^ 0xFFFF;
        }
    }
    memcpy(pic_in, pic_buffer, frWidth * frHeight * sizeof(uint16_t));
}
