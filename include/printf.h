//Create macros to add in printf
//Cal.W 2020
//Adapted from https://forum.arduino.cc/index.php?topic=370464.msg4117945#msg4117945

#ifndef PRINTF
#define PRINTF

#ifndef _PRINTF_BUFFER_LENGTH_
    #define _PRINTF_BUFFER_LENGTH_  64
#endif

static char _pf_buffer_[_PRINTF_BUFFER_LENGTH_];

#define printf(_obj_,a,...)                                           \
    do{                                                               \
        snprintf(_pf_buffer_, sizeof(_pf_buffer_), a, ##__VA_ARGS__); \
        _obj_.print(_pf_buffer_);                                     \
    }while(0)

#define printfn(_obj_,a,...)                                                \
    do{                                                                     \
        snprintf(_pf_buffer_, sizeof(_pf_buffer_), a"\r\n", ##__VA_ARGS__); \
        _obj_.print(_pf_buffer_);                                           \
    }while(0)

#endif