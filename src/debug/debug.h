//My Debug Libary
//See main.ino for an example
//Licensed under MIT, See the LICENCE file. 
//Cal.W 2020
#ifndef __DEBUG_H__
#define __DEBUG_H__

//This is very much needed -> See printf.h for more info
#include "Arduino.h" 

#if __has_include("build_headers.h")
    #include "build_headers.h"
#else
    #warning "Auto-Generated build_headers.h file not found."    
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
    
    //Where should the debug output be written to?
    #ifndef DBG_OUT_OBJ
        #define DBG_OUT_OBJ Serial
    #endif

    #define DBG_PRINT(text,...)    DBG_OUT_OBJ.print(text, ##__VA_ARGS__)    //Normal Print
    #define DBG_PRINTLN(text,...)  DBG_OUT_OBJ.println(text, ##__VA_ARGS__)  //Normal Println
    #define DBG_FPRINT(text,...)   DBG_PRINT(F(text), ##__VA_ARGS__)         //These two puts the string into SRAM (Saves Space)
    #define DBG_FPRINTLN(text,...) DBG_PRINTLN(F(text), ##__VA_ARGS__)
    #define DBG_FPRINT_SV(text,value,...)   do{ DBG_FPRINT(text); DBG_PRINT(value, ##__VA_ARGS__);   } while(0)
    #define DBG_FPRINT_SVLN(text,value,...) do{ DBG_FPRINT(text); DBG_PRINTLN(value, ##__VA_ARGS__); } while(0)

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


//When creating a list what should the inital array length be?
#ifndef INITAL_LIST_LENGTH
    #define INITAL_LIST_LENGTH 5
#endif

#ifndef INC_CAPACITY
    #define INC_CAPACITY(capacity) capacity + 2
#endif

/*As of Apr 2020 ONLY the debug handler has a critical error function
    //This is the container to hold the functions to be run on a critical failure
    typedef void (*CriticalFailureFunctions) (const char* func, const char* file, u16 failLine);
    class CriticalFailureFunctionList {
        public:
            CriticalFailureFunctionList();
            ~CriticalFailureFunctionList();
            bool appendFunction(void appFunc&); 
            void runFunctions();
        private:
            CriticalFailureFunctions* funcList;
            int length, capacity;
    };
*/

typedef void (*DebugPrintFunction) (String);
//This is the container to hold the functions to be run on a debug print
class DebugPrintFunctionList {
    public:
        //DebugPrintFunctionList(int newCapacity);
        DebugPrintFunctionList(int newCapacity = INITAL_LIST_LENGTH);
        ~DebugPrintFunctionList();
        bool append(DebugPrintFunction appFunc);
        //bool remove(int funcIndex); //^May not include?
        void runFunctions(String printValues);

    private:
        int length, capacity;
        DebugPrintFunction* funcList;
};

//The debugger class for simple multi-class debugging
class DebugHandler {
    public:
        DebugHandler();
        void criticalFailure(const char* func, const char* file, u16 failLine);
        //void printDebug(String printValues);       
        void printDebug(String printValues = "HTB");
    private:
        //CriticalFailureFunctionList critFuncs; //?See line 84
        static void debugPrintHeader(String printValues);
        DebugPrintFunctionList debugFuncs;
};

#ifndef CRITICAL_LED
    #define CRITICAL_LED LED_BUILTIN
#endif

//Wrap the function in a macro to make debugging easier
#define CRITICAL_FAIL(Debugger) Debugger.criticalFailure(__FUNCTION__, __FILE__, __LINE__)


#endif