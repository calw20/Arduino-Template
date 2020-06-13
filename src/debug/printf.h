//Create macros to add in printf
//Adapted from https://forum.arduino.cc/index.php?topic=370464.msg4117945#msg4117945
//Licensed under MIT, See the LICENCE file. 
//Cal.W 2020

#ifndef __PRINTF_H__
#define __PRINTF_H__

/* Right So
/  First pick a deity of your liking...
/  NOW; CURSE, SWEAR, KICK AND SCREAM AT IT FOR THE PAIN THAT WAS SUFFERED. 
\  (╯°□°）╯︵ ┻━┻)
/  Now thats' overwith; For some reason when "compiling"/"processing" a chain
/  of headers something borks and the pre-processor and/or linker kick and scream
/  about "expected unqualified-id before [blah]" crud. The "Arduino.h" does _something_
/  to fix it. If you know what please let me know as I have no idea why it works,
/  I just know it does. [Cal]
?  (ヘ･_･)ヘ┳━┳
*/
#include "Arduino.h"

//How many chars can the sprint buffer have allocated?
#ifndef _PRINTF_BUFFER_LENGTH_
    #define _PRINTF_BUFFER_LENGTH_ 128
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