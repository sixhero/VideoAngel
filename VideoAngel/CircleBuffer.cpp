#include "CircleBuffer.h"

void CircleBuffer::InitCircleBuffer(int64_t size)
{
	buffer = new uint8_t[size];
	memset(buffer, 0, size);
	buffer_size = size;
	buffer_free = size;
	buffer_start = 0;
	buffer_end = 0;
}

bool CircleBuffer::WriteBuffer(const uint8_t* data, const int64_t data_length)
{
	bool ret = false;
	
	if (buffer_free <= 0 || buffer_free < data_length)
	{
		//缓冲区没有剩余空间
		return false;
	}
	//此值为正--缓冲区已经使用的空间大小
	//此值为负--缓冲区未使用的的空间大小
	int64_t size_tmp = buffer_end - buffer_start;
	m_mutex.lock();
	if (size_tmp >= 0)
	{
		//环形缓冲区起始结束位置正向排序

		int64_t max_end_minus = buffer_size - buffer_end;//缓冲区最大值位置减缓冲区有效结尾位置的差值
		if (data_length >= max_end_minus)
		{
			//存入的数据尺寸要经过缓冲区头尾

			memcpy(buffer + buffer_end, data, max_end_minus);
			memcpy(buffer, data + max_end_minus, data_length - max_end_minus);
			buffer_free -= data_length;
			buffer_end = data_length - max_end_minus;
		}
		else
		{
			//正常向后存入内存数据
			memcpy(buffer + buffer_end, data, data_length);
			buffer_free -= data_length;
			buffer_end += data_length;
		}
	}
	else
	{
		//环形缓冲区起始结束位置逆向排序，跨过了真实内存块的起始结束位置
		memcpy(buffer + buffer_end, data, data_length);
		buffer_free -= data_length;
		buffer_end += data_length;
	}
	m_mutex.unlock();
	return true;
}

int64_t CircleBuffer::ReadBuffer(uint8_t* data, const int64_t data_length)
{
	int64_t ret = -1;
	int64_t buffer_used = buffer_size - buffer_free;
	if (buffer_used <= 0)
	{
		//缓存空间没有数据
		return -1;
	}
	m_mutex.lock();
	if (buffer_used < data_length)
	{
		//取出数据小于要求的数据量

		//此值为正--缓冲区已经使用的空间大小
		//此值为负--缓冲区未使用的的空间大小
		int64_t size_tmp = buffer_end - buffer_start;
		if (size_tmp > 0)
		{
			memcpy(data, buffer + buffer_start, size_tmp);
			buffer_start += size_tmp;
			buffer_free += size_tmp;
		}
		else
		{
			memcpy(data, buffer + buffer_start, buffer_size - buffer_start);
			memcpy(data, buffer, buffer_end);
			buffer_start = buffer_end;
			buffer_free = buffer_end + (buffer_size - buffer_start);
		}
		m_mutex.unlock();
		return buffer_used;
	}
	else
	{
		//取出数据等于要求的数据量
		//此值为正--缓冲区已经使用的空间大小
		//此值为负--缓冲区未使用的的空间大小
		int64_t size_tmp = buffer_end - buffer_start;
		if (size_tmp > 0)
		{
			memcpy(data, buffer + buffer_start, data_length);
			buffer_start += data_length;
			buffer_free += data_length;
		}
		else
		{
			int64_t max_start_minus = buffer_size - buffer_start;
			if (max_start_minus > data_length)
			{
				memcpy(data, buffer + buffer_start, data_length);
				buffer_start += data_length;
				buffer_free += data_length;
			}
			else
			{
				memcpy(data, buffer + buffer_start, max_start_minus);
				memcpy(data, buffer, data_length - max_start_minus);
				buffer_start = data_length - max_start_minus;
				buffer_free += data_length;
			}
		}
		m_mutex.unlock();
		return data_length;
	}
	m_mutex.unlock();
	return -1;
}

int64_t CircleBuffer::GetBufferFree()
{
	return buffer_free;
}

int64_t CircleBuffer::GetBufferUsed()
{
	return buffer_size - buffer_free;
}