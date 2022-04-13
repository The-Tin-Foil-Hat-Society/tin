#pragma once

//
// https://chromium.googlesource.com/chromiumos/docs/+/HEAD/constants/syscalls.md#arm64-64_bit
//
enum sys_calls
{
    Sys_Read = 63,
    Sys_Write = 64,
    Sys_OpenAt = 56, // "safer" version of Open
    Sys_Close = 57,

    Sys_Exit = 93
};
