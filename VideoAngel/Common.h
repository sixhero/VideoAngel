#ifndef _COMMON_
#define _COMMON_
#include <stdint.h>
/// @brief 视频解码后的视频结构体
typedef struct VideoData
{
	uint8_t* _data; // RGB图像数据，数据存储内存由外部提供
	int64_t size;	// RGB图像数据大小
	int64_t pts;	//视频显示时间
	VideoData()
	{
		_data = nullptr;
		size = 0;
		pts = 0;
	}
	~VideoData()
	{
		if (_data)
		{
			delete[] _data;
			_data = nullptr;
		}
	}
} VideoData, * _VideoData;

/// @brief 视频解码后的音频结构体
typedef struct AudioData
{
	uint8_t* _data; //音频数据，数据存储内存由外部提供
	int64_t size;	//音频数据大小
	int64_t pts;	//音频播放时间
	AudioData()
	{
		_data = nullptr;
		size = 0;
		pts = 0;
	}
	~AudioData()
	{
		if (_data)
		{
			delete[] _data;
			_data = nullptr;
		}
	}
} AudioData, * _AudioData;

#endif