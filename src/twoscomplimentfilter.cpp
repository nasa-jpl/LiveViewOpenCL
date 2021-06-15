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
    qDebug() << "PK Debug 5-30-21 TwosComplimentFilter::apply_filter is16bit: " << is16bit;

    uint16_t offset = is16bit ? 0x8000 : 0x2000; //16 or 14 bits
    size_t ndx;
    // memcpy(pic_buffer, pic_in, frSize * sizeof (uint16_t));

    for (ndx = 0; ndx < frSize; ndx++) {
        pic_in[ndx] = pic_in[ndx] ^ offset;
        // pic_buffer[ndx] = (pic_in[ndx] ^ offset) + 1;
    }

    // memcpy(pic_in, pic_buffer, frSize * sizeof(uint16_t));
}
