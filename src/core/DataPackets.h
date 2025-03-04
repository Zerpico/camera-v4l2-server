#pragma once

#include "spdlog/spdlog.h"
#include <memory>
#include <string>
#include <iostream>

#define MEM_ALIGN 64

enum class CodecType
{
    NONE,
    H264,
    H265,
    MPEG2,
    MPEG4,
    MJPEG,
    VP8,
    VP9
};

struct AVFrame
{
    explicit AVFrame(const size_t size = 0)
        : buffer(new uint8_t[size], std::default_delete<uint8_t[]>()),
          size(size),
          timestamp(0)
    {
    }

    std::shared_ptr<uint8_t> buffer;
    size_t size;
    uint32_t timestamp;
};

/**
 Safe structure for storing data pointer and operations on them (clear, resize, copy, swap)
 */
struct BasePacketData
{
    virtual ~BasePacketData() {};

    /* allocate memory on new size */
    void resize(const int &size)
    {
        if (size < 0)
        {
            SPDLOG_ERROR("invalid data_packet size: {}", size);
            return;
        }
        if (m_DataPtr != nullptr)
        {
            free(m_DataPtr);
            m_DataPtr = nullptr;
        }
        if (size > 0)
        {
            m_DataPtr = (uint8_t *)malloc(size);
        }

        m_sizePtr = size;
    }

    /* clear data and free pointer */
    void clear() { resize(0); }
    /* check if data empty */
    bool empty(void) const { return (m_sizePtr == 0); }
    /* get size data */
    int size(void) const { return m_sizePtr; }
    /* get pointer on data */
    uint8_t *data(void) const { return m_DataPtr; }

    virtual void copy(const uint8_t *buff, const int &sz)
    {
        if (sz <= 0)
            return;
        if (sz > m_sizePtr)
        {
            resize(sz);
        }
        memcpy(m_DataPtr, buff, sz);
    }

    virtual void swap(void *sw_data) {}

    int get_refId(void) const { return m_refId; };
    void set_refId(int id) { m_refId = id; };

protected:
    BasePacketData() : m_DataPtr(nullptr) {};
    int m_sizePtr = 0;
    uint8_t *m_DataPtr = nullptr;
    int m_refId = 0;
};

/**
 Structure for storing data of frame
 */
struct FrameData : BasePacketData
{
    FrameData(const int sizeFrame = 0) : BasePacketData()
    {
        resize(sizeFrame);
    }

    ~FrameData()
    {
        if (m_DataPtr != nullptr)
        {
            free(m_DataPtr);
            m_DataPtr = nullptr;
        }
    }

    void set_resolution(int width, int height)
    {
        m_width = width;
        m_height = height;
    }
    void set_format(int format) { m_format = format; }
    void set_index(uint64_t index) { m_fIndex = index; }

    int get_format() const { return m_format; }
    int get_width() const { return m_width; }
    int get_height() const { return m_height; }
    uint64_t get_index() const { return m_fIndex; }

    void swap(FrameData &sw_data)
    {
        std::swap(m_DataPtr, sw_data.m_DataPtr);
        std::swap(m_sizePtr, sw_data.m_sizePtr);
        std::swap(m_width, sw_data.m_width);
        std::swap(m_height, sw_data.m_height);
        std::swap(m_format, sw_data.m_format);
        std::swap(m_fIndex, sw_data.m_fIndex);
        std::swap(m_refId, sw_data.m_refId);
    }

    /// @brief copy data from cp_data to current object
    /// @param cp_data  source object to copy
    void copy(FrameData &cp_data)
    {
        auto sz = cp_data.size();
        if (sz <= 0)
            return;
        if (sz > m_sizePtr)
        {
            resize(sz);
        }
        memcpy(m_DataPtr, cp_data.data(), sz);

        m_width = cp_data.m_width;
        m_height = cp_data.m_height;
        m_format = cp_data.m_format;
        m_fIndex = cp_data.m_fIndex;
        m_refId = cp_data.m_refId;
    }

private:
    int m_width = 0;
    int m_height = 0;
    int m_format = 0;
    uint64_t m_fIndex = 0;
};

/**
 Structure for storing encoded packet of frame(s)
 */
struct PacketData : BasePacketData
{
    PacketData(const int sizeFrame = 0) : BasePacketData()
    {
        resize(sizeFrame);
    }
    ~PacketData()
    {
        if (m_DataPtr != nullptr)
        {
            free(m_DataPtr);
            m_DataPtr = nullptr;
        }
    }

    void swap(PacketData &sw_data)
    {
        std::swap(m_DataPtr, sw_data.m_DataPtr);
        std::swap(m_sizePtr, sw_data.m_sizePtr);
    }
};