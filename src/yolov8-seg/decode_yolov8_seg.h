#pragma once
#include"../../tensorRT/utils/utils.h"
#include"../../tensorRT/utils/kernel_function.h"

namespace yolov8seg
{
	void decodeDevice(utils::InitParameter param, float* src, int srcWidth, int srcHeight, int srcLength, float* dst, int dstWidth, int dstHeight);
	void transposeDevice(utils::InitParameter param, float* src, int srcWidth, int srcHeight, int srcArea, float* dst, int dstWidth, int dstHeight);
}
