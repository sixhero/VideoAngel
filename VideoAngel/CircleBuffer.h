#ifndef _CIRCLE_BUFFER_
#define _CIRCLE_BUFFER_

//功能--循环缓冲区
#include <stdint.h>
#include <memory>
#include <mutex>

class CircleBuffer
{
private:
	uint8_t* buffer;//环形缓冲区指针
	int64_t buffer_size;//缓冲区的尺寸大小
	int64_t buffer_free;//缓冲区未使用的大小
	int64_t buffer_start;//有效缓冲区起始位置--读取时的起始
	int64_t buffer_end;//有效缓冲区结束位置
	std::mutex m_mutex;

public:
	CircleBuffer() {}
	/// <summary>
	/// 初始化环形缓冲区结构
	/// </summary>
	/// <param name="size">设置缓冲区的大小</param>
	void InitCircleBuffer(int64_t size);

	/// <summary>
	/// 向环型缓冲区中写入数据
	/// </summary>
	/// <param name="data">待写入数据</param>
	/// <param name="data_length">数据长度</param>
	/// <returns></returns>
	bool WriteBuffer(const uint8_t* data, const int64_t data_length);

	/// <summary>
	/// 从缓冲区中取出数据
	/// </summary>
	/// <param name="data">向外输出数据的内存块，由调用者提供</param>
	/// <param name="data_length">要取出的数据长度</param>
	/// <returns>返回值-1失败，大于零取出数据的真实长度</returns>
	int64_t ReadBuffer(uint8_t* data, const int64_t data_length);

	/// <summary>
	/// 获取未使用缓冲区大小
	/// </summary>
	/// <returns></returns>
	int64_t GetBufferFree();

	/// <summary>
	/// 获取已使用缓冲区大小
	/// </summary>
	/// <returns></returns>
	int64_t GetBufferUsed();
};


#endif // !_CIRCLE_BUFFER_