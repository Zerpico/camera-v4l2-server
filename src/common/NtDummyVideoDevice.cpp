#include "NtDummyVideoDevice.h"

NtDummyVideoDevice *NtDummyVideoDevice::createNew(const DummyVideoDeviceParameters &params)
{
    NtDummyVideoDevice *device = new NtDummyVideoDevice(params);
    if (device)
    {
        if (device->getFd() == -1)
        {
            delete device;
            device = NULL;
        }
    }
    return device;
}

NtDummyVideoDevice::NtDummyVideoDevice(const DummyVideoDeviceParameters &params)
    : m_params(params)
{
}

NtDummyVideoDevice::~NtDummyVideoDevice()
{
}

// PAL dimensions
static const unsigned kWidth = 720;
static const unsigned kHeight = 576;

void generateFrame()
{
    const uint8_t BAR_COLOUR[8][3] =
        {
            {255, 255, 255}, // 100% White
            {255, 255, 0},   // Yellow
            {0, 255, 255},   // Cyan
            {0, 255, 0},     // Green
            {255, 0, 255},   // Magenta
            {255, 0, 0},     // Red
            {0, 0, 255},     // Blue
            {0, 0, 0},       // Black
        };

    // Allocate frame buffer
    size_t frameBytes = kWidth * kHeight * sizeof(BAR_COLOUR);
    uint8_t *frame = new uint8_t[kWidth * kHeight * 3];
    unsigned columnWidth = kWidth / 8;

    // Generate complete frame
    for (unsigned y = 0; y < kHeight; y++)
    {
        for (unsigned x = 0; x < kWidth; x += 3)
        {
            unsigned col_idx = x / columnWidth;
            frame[y * kWidth + x + 0] = BAR_COLOUR[col_idx][0];
            frame[y * kWidth + x + 1] = BAR_COLOUR[col_idx][1];
            frame[y * kWidth + x + 2] = BAR_COLOUR[col_idx][2];
        }
    }

    // Save as PPM
    FILE *fout = fopen("ebu_bars.ppm", "wb");
    fprintf(fout, "P6\n%u %u\n255\n", kWidth, kHeight);
    fwrite(frame, frameBytes, 1, fout);
    fclose(fout);

    free(frame);
}