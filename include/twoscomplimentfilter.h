#ifndef TWOSCOMPLIMENTFILTER_H
#define TWOSCOMPLIMENTFILTER_H

#include <stdint.h>

#include "constants.h"
#include "image_type.h"

class TwosComplimentFilter
{
public:
    explicit TwosComplimentFilter(unsigned int frame_height, unsigned int frame_width);
    uint16_t *apply_filter(uint16_t *pic_in);

private:
    uint16_t pic_buffer[MAX_SIZE];
    unsigned int frHeight;
    unsigned int frWidth;
};

#endif // TWOSCOMPLIMENTFILTER_H
