#pragma once
#include <stdint.h>
#include <memory>

namespace gpupt
{
    struct svgfBuffers
    {
        std::shared_ptr<buffer> ColourBuffer = nullptr;
        std::shared_ptr<buffer> VarianceBuffer = nullptr;
        std::shared_ptr<buffer> MomentsBuffer = nullptr;
        std::shared_ptr<buffer> HistoryBufferColour = nullptr;
        std::shared_ptr<buffer> HistoryBufferMoments = nullptr;
        std::shared_ptr<buffer> MotionVectors = nullptr;
        void Init(uint32_t Width, uint32_t Height);
    };

    

}