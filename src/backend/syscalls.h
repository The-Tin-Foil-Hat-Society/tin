#pragma once

enum sys_calls
{
    PrintInt        = 1,
    PrintFloat      = 2,
    PrintString     = 4,
    ReadInt         = 5,
    ReadFloat       = 6,
    ReadString      = 8,
    Sbrk            = 9,
    Exit            = 10,
    PrintChar       = 11,
    ReadChar        = 12,
    Open            = 13,
    Read            = 14,
    Write           = 15,
    Close           = 16,
    Exit2           = 17,
    Lseek           = 18,
    Sleep           = 19,
    Cwd             = 20,
    Time            = 21,
    PrintHex        = 22,
    PrintBin        = 23,
    PrintUsgn       = 24,
    SetSeed         = 25,
    RandInt         = 26,
    RandIntRng      = 27,
    RandFloat       = 28
};
