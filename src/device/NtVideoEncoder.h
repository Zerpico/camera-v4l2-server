#pragma once

class NtVideoEncoder
{
protected:
    // NtVideoEncoder() = default;
    NtVideoEncoder(NtVideoEncoder const &) = delete;
    void operator=(NtVideoEncoder const &x) = delete;

public:
    NtVideoEncoder();
    ~NtVideoEncoder();
};
