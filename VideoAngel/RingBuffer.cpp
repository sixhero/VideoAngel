#include "RingBuffer.h"
#include <atomic>

RingBuffer::RingBuffer(uint32_t size)
{
    Init(size);
}

void RingBuffer::Init(uint32_t size)
{
    // 设置缓冲大小为2的n次幂
    m_real_size = size;
    if (!is_power_of_2(size))
    {
        m_size = roundup_power_of_2(size);
    }
    else
    {
        m_size = size;
    }

    m_data = new uint8_t[m_size];
    m_in = 0;
    m_out = 0;
}

bool RingBuffer::is_power_of_2(uint32_t n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

uint32_t RingBuffer::roundup_power_of_2(uint32_t n)
{
    if (n == 0)
    {
        return 0;
    }
    uint32_t position = 0;
    for (uint32_t i = n; i != 0; i >>= 1)
    {
        position++;
    }

    return static_cast<uint32_t>(1 << position);
}

uint32_t RingBuffer::write_data(uint8_t *data, uint32_t size)
{
    if (get_used_size() >= m_real_size)
    {
        //已存储的数据大于设置的真实存储值，不在进行写入
        return 0;
    }

    size = std::min(size, get_free_size());

    uint32_t len = std::min(size, m_size - (m_in & (m_size - 1)));

    memcpy(m_data + (m_in & (m_size - 1)), data, len);
    memcpy(m_data, data + len, size - len);

    m_in += size;

    return size;
}

uint32_t RingBuffer::read_data(uint8_t *data, uint32_t size)
{
    size = std::min(size, m_in - m_out);
    uint32_t len = std::min(size, m_size - (m_out & (m_size - 1)));

    memcpy(data, m_data + (m_out & (m_size - 1)), len);
    memcpy(data + len, m_data, size - len);

    m_out += size;

    return size;
}

inline uint32_t RingBuffer::get_used_size()
{
    //return static_cast<uint32_t>(m_in.load(std::memory_order_acquire) - m_out.load(std::memory_order_acquire));
    return static_cast<uint32_t>(m_in - m_out);
}

inline uint32_t RingBuffer::get_free_size()
{
    //return static_cast<uint32_t>(m_size - m_in.load(std::memory_order_acquire) + m_out.load(std::memory_order_acquire));
    return static_cast<uint32_t>(m_size - m_in + m_out);
}

uint32_t RingBuffer::get_real_free_size()
{
    return static_cast<uint32_t>(m_real_size - m_in + m_out);
}


RingBuffer::~RingBuffer()
{
    if (m_data)
    {
        delete[] m_data;
        m_data = nullptr;
    }
}