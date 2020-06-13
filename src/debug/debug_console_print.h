//Print to console debug macros
//See main.ino for an example
//Licensed under MIT, See the LICENCE file. 
//Cal.W 2020
#ifndef __DEBUG_CONSOLE_PRINT__
#define __DEBUG_CONSOLE_PRINT__

#include "Arduino.h" 

//If present then include the macro-wrapped printf functions based on sprint
#if __has_include("printf.h")
    #include "printf.h"
#else
    #warning "'printf.h' could not be found, you will not have these macros."
#endif

//Disable all debug stuff by default. This will normally be done before this header.
#ifndef DO_DEBUG
    #define DO_DEBUG false
#endif

#ifndef PRINT_PRAGMA_MESSAGES
    #define PRINT_PRAGMA_MESSAGES false
#endif

//^ Debug Print Functions
#if DO_DEBUG
    //^ Nice Debug Functions -> They only work if debugging is enabled  
    #if PRINT_PRAGMA_MESSAGES
        #pragma message "Debuging messages enabled."
    #endif

    //Where should the debug output be written to?
    #ifndef DBG_OUT_OBJ
        #define DBG_OUT_OBJ Serial
    #endif

    #define DBG_PRINT(text,...)    DBG_OUT_OBJ.print(text, ##__VA_ARGS__)    //Normal Print
    #define DBG_PRINTLN(text,...)  DBG_OUT_OBJ.println(text, ##__VA_ARGS__)  //Normal Println
    #define DBG_FPRINT(text,...)   DBG_PRINT(F(text), ##__VA_ARGS__)         //These two puts the string into SRAM (Saves Space)
    #define DBG_FPRINTLN(text,...) DBG_PRINTLN(F(text), ##__VA_ARGS__)
    #define DBG_FPRINT_SV(text,value,...)   do{ DBG_FPRINT(text); DBG_PRINT(value, ##__VA_ARGS__);   } while(0) //These put the string in SRAM and allow 
    #define DBG_FPRINT_SVLN(text,value,...) do{ DBG_FPRINT(text); DBG_PRINTLN(value, ##__VA_ARGS__); } while(0) // non-string paramiters

    //Create printf vairied options of the above debug macros [if printf.h can be found]
    #ifdef __PRINTF_H__
        #define DBG_PRINTF(...)  printf(DBG_OUT_OBJ, __VA_ARGS__)
        #define DBG_PRINTFN(...) printfn(DBG_OUT_OBJ, __VA_ARGS__)

        #define DBG_FPRINTF(fText,sText,...)  do{ DBG_FPRINT(fText); DBG_PRINTF(sText, ##__VA_ARGS__);  }while(0)
        #define DBG_FPRINTFN(fText,sText,...) do{ DBG_FPRINT(fText); DBG_PRINTFN(sText, ##__VA_ARGS__); }while(0)
    #else
        #warning "printf.h not found! Nothing will be outputted when you use the debug print"
        #define DBG_PRINTF(...)
        #define DBG_PRINTFN(...)
        #define DBG_FPRINTF(...)
        #define DBG_FPRINTFN(...)
    #endif
#else
    //^ No Debug -> Don't even attempt to write to the serial prompt
    #warning "Debuging messages have been disabled!"
    #define DBG_PRINT(...)
    #define DBG_PRINTLN(...)
    #define DBG_FPRINT(...) 
    #define DBG_FPRINTLN(...)
    #define DBG_FPRINT_SV(...) 
    #define DBG_FPRINT_SVLN(...)
    #define DBG_PRINTF(...)
    #define DBG_PRINTFN(...)
    #define DBG_FPRINTF(...)
    #define DBG_FPRINTFN(...)
#endif

#endif