#include <iostream>

///
#include "VideoGlfw.h"
#include "VideoDesc.h"
#include "LowAudio.h"
#include "Common.h"
#include <chrono>
#include <thread>

int glfw_callback(VideoGlfw *video_glfw, uint8_t** data, int64_t* size)
{
	static VideoData video_data;
	VideoDesc* video_desc = (VideoDesc *)video_glfw->m_user_data;
	int ret = video_desc->GetVideoData(&video_data);
	*data = video_data._data;
	*size = video_data.size;
	return ret;
}

int low_audio_callback(LowAudio* video_glfw, uint8_t** data, int64_t* size)
{
	static AudioData audio_data;
	VideoDesc* video_desc = (VideoDesc*)video_glfw->m_user_data;
	int ret = video_desc->GetAudioData(&audio_data);
	*data = audio_data._data;
	*size = audio_data.size;
	return ret;
}

int main(int argc, char* argv[])
{
#if 1
	int ret;
	{
		VideoDesc video_desc;
		//ret = video_desc.InitVideoDesc(R"(C:\Users\sixhe\Desktop\dayu.mp3)");
		//ret = video_desc.InitVideoDesc(R"(E:\WebD\workdead.mp4)");
		//ret = video_desc.InitVideoDesc(R"(E:\vivi.mp4)");
		//ret = video_desc.InitVideoDesc(R"(rtmp://ns8.indexforce.com/home/mystream)");
		//ret = video_desc.InitVideoDesc(u8"F:\\pr_work\\倒数.mp4");
		//ret = video_desc.InitVideoDesc(R"(E:\BiteMe-AvrilLavigne.mp4)"); 
		//ret = video_desc.InitVideoDesc("F:\\au_work\\vivi\\vivi_.mp3");
		//ret = video_desc.InitVideoDesc(R"(http://devimages.apple.com/iphone/samples/bipbop/gear1/prog_index.m3u8)");
		//ret = video_desc.InitVideoDesc(R"(https://v11.toutiaovod.com/db52f952e1495d3e36e1a31adb78a567/64e6167d/video/tos/cn/tos-cn-ve-4/okEBiCgbjJDDeaBhCaniFf8aCIaRAAIgUW2eRA/)");
		//ret = video_desc.InitVideoDesc(R"(rtmp://media3.scctv.net/live/scctv_800)");
		//ret = video_desc.InitVideoDesc(R"(rtmp://mobliestream.c3tv.com:554/live/goodtv.sdp)");
		//ret = video_desc.InitVideoDesc(R"(https://sf1-hscdn-tos.pstatp.com/obj/media-fe/xgplayer_doc_video/flv/xgplayer-demo-360p.flv)");
		//ret = video_desc.InitVideoDesc(R"(https://1080p.jszyplay.com/play/negPBlra/index.m3u8)");
		//ret = video_desc.InitVideoDesc(R"(udp://192.168.0.119:6811)");
		//ret = video_desc.InitVideoDesc(R"(video=HD Webcam)");
		//ret = video_desc.InitVideoDesc(R"(http://live.cgtn.com/1000/prog_index.m3u8)");
		if(argc != 2)
		{
			std::cout<<"未输入播放参数使用默认值\n";
			// ret = video_desc.InitVideoDesc(R"(rtmp://ns8.indexforce.com/home/mystream)");
			ret = video_desc.InitVideoDesc(R"(E:\vivi.mp4)");
		}
		else
		{
			ret = video_desc.InitVideoDesc(argv[1]);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(20));


		VideoGlfw video_glfw;
		if (video_desc.GetVideoIndex() != -1)
		{
			video_glfw.m_user_data = &video_desc;
			video_glfw.m_width = video_desc.GetVideoWidth();
			video_glfw.m_height = video_desc.GetVideoHeight();
			video_desc.GetVideoDar(video_glfw.m_dar_w, video_glfw.m_dar_h);
			video_glfw.m_glfw_callback = glfw_callback;
			video_glfw.Start();
		}

		LowAudio low_audio;
		if (video_desc.GetAudioIndex() != -1)
		{
			low_audio.m_low_data_callback = low_audio_callback;
			low_audio.m_user_data = &video_desc;
			low_audio.InitAudio(video_desc.GetAudioChannels(), video_desc.GetAudioSampleRate());
			low_audio.Start();
		}

		// 视频界面退出时，清理数据退出

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		while (video_glfw.GlfwIsWorking())
		{
			// 视频数据正在显示
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		video_desc.Stop();
		low_audio.Stop();
	}
#endif
	std::cout << "hello word";
	int pause;
	std::cin>>pause;
}