#include "twoscomplimentfilter.h"

TwosComplimentFilter::TwosComplimentFilter(size_t frame_size) :
    frSize(frame_size)
{
    for (size_t ndx = 0; ndx < frSize; ndx++) {
        pic_buffer[ndx] = 0;
    }
}

void TwosComplimentFilter::apply_filter(uint16_t *pic_in, bool is16bit)
{
    uint16_t offset = is16bit ? 0xFFFF : 0x3FFF; //16 or 14 bits
    size_t ndx;

    for (ndx = 0; ndx < frSize; ndx++) {
            pic_buffer[ndx] = pic_in[ndx] ^ offset;
    }

    memcpy(pic_in, pic_buffer, frSize * sizeof(uint16_t));
}
