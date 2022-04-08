#pragma once

//
// https://filippo.io/linux-syscall-table/
//
enum sys_calls
{
    Sys_Read = 0,
    Sys_Write = 1,
    Sys_Open = 2,
    Sys_Close = 3,

    Sys_Exit = 60
};
