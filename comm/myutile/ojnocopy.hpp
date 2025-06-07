#pragma once
class OJnocopy
{

public:
    OJnocopy(const OJnocopy &obj) = delete;
    void operator=(const OJnocopy &obj) = delete;
    OJnocopy() = default;
    ~OJnocopy() = default;
};