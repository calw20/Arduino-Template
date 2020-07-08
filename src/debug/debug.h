//My Debug Libary
//See main.ino for an example
//Licensed under MIT, See the LICENCE file. 
//Cal.W 2020
#ifndef __DEBUG_H__
#define __DEBUG_H__

//This is very much needed -> See printf.h for more info
#include "Arduino.h"

//Attempt to include the auto-generated program header version.
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

//Are we in debug mode? If not then any / all the debug stuff can be left out.
#ifndef DO_DEBUG
    //^ I am using the pre-compiler to remove any of the debug stuff to reduce the
     //^ file size when completed. It's more for me to understand how it works then anything
     //^ really... plus its fun to build a dynamicly updating system.
    #define DO_DEBUG false
#endif

//Add in all the debug console print macros
#include "debug_console_print.h"

//What output pin is flashed on a critical failure? 
#ifndef CRITICAL_LED
    #define CRITICAL_LED LED_BUILTIN
#endif

//Wrap the function in a macro to make debugging easier
#define CRITICAL_MODULE_FAIL(Module) Module.criticalFailure(__FUNCTION__, __FILE__, __LINE__)
#define CRITICAL_FAIL(...) do{ status = CrashType::Critical; parent->criticalFailure(__FUNCTION__, __FILE__, __LINE__);}while(0)

//Start code defs

//How can a module crash?
const char CrashTypeText[6][10] = {"No", "Minor", "Major", "Fatal", /*"Irrecoverable",*/ "Critical"};
enum CrashType {None, Minor, Major, Fatal, /*Irrecoverable,*/  Critical };

class GenericCrashable {
    public:
        virtual bool init();
        virtual void genericError(const char* func, const char* file, u16 failLine); 
        virtual void minorFailure(const char* func, const char* file, u16 failLine);
        virtual void majorFailure(const char* func, const char* file, u16 failLine);
        virtual void criticalFailure(const char* func, const char* file, u16 failLine);
        virtual void printDebug(String printValues = "");
        virtual CrashType getStatus();

        virtual void printErrorInfo(const char* func, const char* file, u16 failLine, bool forcePrint = false); 
        
    protected:
        CrashType status = CrashType::None;
};


//The Following are linked together. - lets us do things like "parent->criticalFailure()"
class CrashableModule; //Just don't, its needed to keep the compiler happy

#ifndef MIN_CHILDREN_LENGTH
    #define MIN_CHILDREN_LENGTH 10
#endif

//The Level0 or Kernel base partent object
class UnCrashable : public GenericCrashable {
    public:
        UnCrashable();
        //~UnCrashable();
        virtual bool init();
        virtual bool addModule(CrashableModule &module);
        virtual bool addModule(CrashableModule &module, int id);
        virtual void genericError(const char* func, const char* file, u16 failLine);
        virtual void printDebug(String printValues);

    public:
        bool inError = false;

    protected:
        CrashableModule *modules[MIN_CHILDREN_LENGTH];
        UnCrashable *parent = this;

};


//A Crashable Module
class CrashableModule : public GenericCrashable{
    public:
        CrashableModule(UnCrashable &parent, bool addSelfToParent = true);
        //~CrashableModule();

    protected:
        UnCrashable *parent;
        

};

#define CHK_LETTER(letter) printValues.indexOf(letter) > -1

#endif