#include <iostream>

///
#include "VideoGlfw.h"
#include "VideoDesc.h"
#include "LowAudio.h"
#include "Common.h"


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
	int ret;
	VideoDesc video_desc;
	//ret = video_desc.InitVideoDesc(R"(C:\Users\sixhe\Desktop\dayu.mp3)");
	//ret = video_desc.InitVideoDesc(R"(E:\WebD\workdead.mp4)");
	//ret = video_desc.InitVideoDesc(R"(E:\vivi.mp4)");
	//ret = video_desc.InitVideoDesc(R"(rtmp://ns8.indexforce.com/home/mystream)");
	//ret = video_desc.InitVideoDesc("F:\\pr_work\\倒数.mp4");
	//ret = video_desc.InitVideoDesc(R"(E:\BiteMe-AvrilLavigne.mp4)"); 
	ret = video_desc.InitVideoDesc("F:\\au_work\\vivi\\vivi_.mp3");
	Sleep(20);


	VideoGlfw video_glfw;
	video_glfw.m_user_data = &video_desc;
	video_glfw.m_width = video_desc.GetVideoWidth();
	video_glfw.m_height = video_desc.GetVideoHeight();
	video_glfw.m_glfw_callback = glfw_callback;
	video_glfw.Start();

	LowAudio low_audio;
	low_audio.m_low_data_callback = low_audio_callback;
	low_audio.m_user_data = &video_desc;
	low_audio.InitAudio(video_desc.GetAudioChannels(), video_desc.GetAudioSampleRate());
	low_audio.Start();


	std::cout << "hello word";
	system("pause");
}