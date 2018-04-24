#ifndef DARKSUBFILTER_H
#define DARKSUBFILTER_H

#include <stdint.h>
#include <algorithm>
#include <mutex>
#include <vector>

#include <QtGlobal>

class DarkSubFilter
{
public:
    DarkSubFilter(int frame_width, int frame_height);
    virtual ~DarkSubFilter();

    void dsf_callback(uint16_t* in_frame, uint16_t* out_frame);
    void collect_mask(uint16_t* in_frame);
    void dark_subtract(uint16_t* in_frame, uint16_t* out_frame);

    void start_mask_collection();
    void finish_mask_collection();

    std::mutex mask_mutex;

private:
    bool mask_collected;
    unsigned int frWidth;
    unsigned int frHeight;
    size_t frSize;
    unsigned int nSamples;

    std::vector<double> mask_accum;
    std::vector<float> mask;

};

#endif // DARKSUBFILTER_H
