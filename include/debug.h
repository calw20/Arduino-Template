//My Commonly used Debug macros
//Cal.W 2020
#ifndef DEBUG_OPTIONS
#define DEBUG_OPTIONS

#if __has_include("build_headers.h")
    #include "build_headers.h"
#else
    #if __has_include("../build_headers.h")
        #include "../build_headers.h"
    #else
        #warning "Auto-Generated build_headers.h file not found."
    #endif
#endif
#ifndef BUILD_DATE
    #define BUILD_DATE "N.A."
#endif
#ifndef BUILD_VERSION
    #define BUILD_VERSION "N.A."
#endif

//Add in some niceties if possible
#if __has_include("printf.h")
    #include "printf.h"
#else
    #warning "'printf.h' could not be found, you will not have these macros."
#endif

//Are we in debug mode? If not then any / all the debug stuff can be left out.
#ifndef DO_DEBUG
    //^ I am using the pre-compiler to remove any of the debug stuff to reduce the
     //^ file size when completed. It's more for me to understand how it works then anything
     //^ really... plus its fun to build a dynamicly updating system.
    #define DO_DEBUG false
#endif

//^ Debug Print Functions
#if DO_DEBUG
    //^ Nice Debug Functions -> They only work if debugging is enabled  
    #pragma message "Debuging messages enabled."
    #define DBG_PRINT(text,...)    Serial.print(text, ##__VA_ARGS__)       //Normal Print
    #define DBG_PRINTLN(text,...)  Serial.println(text, ##__VA_ARGS__)     //Normal Println
    #define DBG_FPRINT(text,...)   DBG_PRINT(F(text), ##__VA_ARGS__)       //These two puts the string into SRAM (Saves Space)
    #define DBG_FPRINTLN(text,...) DBG_PRINTLN(F(text), ##__VA_ARGS__)
    #define DBG_FPRINT_SV(text,value,...)   do{ DBG_FPRINT(text); DBG_PRINT(value, ##__VA_ARGS__);   } while(0)
    #define DBG_FPRINT_SVLN(text,value,...) do{ DBG_FPRINT(text); DBG_PRINTLN(value, ##__VA_ARGS__); } while(0)

    #ifdef PRINTF
        #define DBG_PRINTF(...)  printf(Serial, __VA_ARGS__)
        #define DBG_PRINTFN(...) printfn(Serial, __VA_ARGS__)
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