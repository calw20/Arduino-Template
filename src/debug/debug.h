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

//Start code defs

//How can a module crash?
const char ChrashTypeText[6][10] = {"No", "Minor", "Major", "Fatal", /*"Irrecoverable",*/ "Critical"};
enum ChrashType {None, Minor, Major, Fatal, /*Irrecoverable,*/  Critical };

class GenericCrashable {
    public:
        virtual void genericError(const char* func, const char* file, u16 failLine); 
        virtual void minorFailure(const char* func, const char* file, u16 failLine);
        virtual void majorFailure(const char* func, const char* file, u16 failLine);
        virtual void criticalFailure(const char* func, const char* file, u16 failLine);
        virtual ChrashType getStatus();
        
    private:
        ChrashType status = ChrashType::None;
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
        virtual bool addModule(CrashableModule &module);
        virtual bool addModule(CrashableModule &module, int id);
        virtual void genericError(const char* func, const char* file, u16 failLine);

    private:
        CrashableModule *modules[MIN_CHILDREN_LENGTH];

};


//A Chrashable Module
class CrashableModule : public GenericCrashable{
    public:
        CrashableModule(UnCrashable &parent, bool addSelfToParent = true);
        //~CrashableModule();

    private:
        UnCrashable *parent;
        

};

#endif