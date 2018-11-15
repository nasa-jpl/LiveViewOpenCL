#ifndef TWOSCOMPLIMENTFILTER_H
#define TWOSCOMPLIMENTFILTER_H

#include <stdint.h>
#include <QDebug>

#include "constants.h"
#include "image_type.h"

class TwosComplimentFilter
{
public:
    explicit TwosComplimentFilter(size_t frame_size);
    void apply_filter(uint16_t *pic_in, bool is16bit);

private:
    uint16_t pic_buffer[MAX_SIZE];
    size_t frSize;
};

#endif // TWOSCOMPLIMENTFILTER_H
