// 仿制Linux内核中的环形缓冲区
#include <stdint.h>
#include <algorithm>
//#include <atomic>
//#include <mutex>

class RingBuffer
{
public:
    /// @brief 缓冲区地址
    uint8_t *m_data;

    /// @brief 缓冲区大小
    uint32_t m_size;

    /// @brief 写入位置，缓冲区头
    //std::atomic<uint32_t> m_in;
    uint32_t m_in;

    /// @brief 读取位置，缓冲区尾
    //std::atomic<uint32_t> m_out;
    uint32_t m_out;

    /// @brief 缓冲区的实际可使用大小
    uint32_t m_real_size;


    //std::mutex m_mutex;

public:

    RingBuffer() {}
    ~RingBuffer();

    RingBuffer(uint32_t size);


    void Init(uint32_t size);

    /// @brief 判断数字是否为2的n次幂
    /// @param n 判断值
    /// @return true为2的n次幂
    bool is_power_of_2(uint32_t n);

    /// @brief 将数字向上取整为2的n次幂
    /// @param n 判断值
    /// @return 向上取整后的值
    uint32_t roundup_power_of_2(uint32_t n);

    /// @brief 写入数据
    /// @param data 源数据地址
    /// @param size 源数据长度
    /// @return 实际写入的长度
    uint32_t write_data(uint8_t *data, uint32_t size);

    /// @brief 读取数据
    /// @param data 目的数据地址
    /// @param size 目的数据长度
    /// @return 实际读取的长度
    uint32_t read_data(uint8_t *data, uint32_t size);

    /// @brief 获取缓冲区已经使用的大小
    /// @return
    inline uint32_t get_used_size();

    /// @brief 获取缓冲区未使用的大小
    /// @return
    inline uint32_t get_free_size();

    /// @brief 获取缓冲区真实未使用大小
    /// @return 
    uint32_t get_real_free_size();
};
