#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "NtRtspApp.h"

// PAL dimensions
static const unsigned kWidth = 720;
static const unsigned kHeight = 576;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB;

int main()
{
    const RGB BAR_COLOUR[8] =
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
    size_t frameBytes = kWidth * kHeight * sizeof(RGB);
    RGB *frame = new RGB[kWidth * kHeight];
    unsigned columnWidth = kWidth / 8;

    // Generate complete frame
    for (unsigned y = 0; y < kHeight; y++)
    {
        for (unsigned x = 0; x < kWidth; x++)
        {
            unsigned col_idx = x / columnWidth;
            frame[y * kWidth + x] = BAR_COLOUR[col_idx];
        }
    }

    // Save as PPM
    FILE *fout = fopen("ebu_bars.ppm", "wb");
    fprintf(fout, "P6\n%u %u\n255\n", kWidth, kHeight);
    fwrite(frame, frameBytes, 1, fout);
    fclose(fout);

    free(frame);

    auto rtsp = new NtRtspApp();
    auto web = new WebServer();
    auto device = new CameraDeviceSource();

    rtsp->Start();
    web->run();
}