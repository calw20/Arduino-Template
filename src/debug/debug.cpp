//The main debug handling class
//Licensed under MIT, See the LICENCE file.
//Cal.W 2020
#include "debug.h"

void GenericCrashable::genericError(const char* func, const char* file, u16 failLine){
    DBG_PRINTF("[%s] Error in [%s] at line %u in %s", ChrashTypeText[status],
        func, failLine, file); DBG_PRINTLN();
}

void GenericCrashable::minorFailure(const char* func, const char* file, u16 failLine){
    status = ChrashType::Minor;
    genericError(func, file, failLine);
}

void GenericCrashable::majorFailure(const char* func, const char* file, u16 failLine) {
    status = ChrashType::Major;
    genericError(func, file, failLine);
}

void GenericCrashable::criticalFailure(const char* func, const char* file, u16 failLine) {
    status = ChrashType::Critical;
    genericError(func, file, failLine);
}

ChrashType GenericCrashable::getStatus(){
    return status;
}


UnCrashable::UnCrashable(){
    for (int i = 0; i < MIN_CHILDREN_LENGTH; i++){
        modules[i] = nullptr;
    }
}

bool UnCrashable::addModule(CrashableModule &module){
    for (int i = 0; i < (sizeof(modules)/sizeof(modules[0])); i++){
        if (!modules[i]) {
            modules[i] = &module;
            return true;
        }
    }
    return false;
}

bool UnCrashable::addModule(CrashableModule &module, int id){
    if (id >= sizeof(modules)/sizeof(modules[0]))
        return false;

    modules[id] = &module;
    return true;
}


void UnCrashable::genericError(const char* func, const char* file, u16 failLine){
    for (int i = 0; i < (sizeof(modules)/sizeof(modules[0])); i++){
        if (modules[i])
            modules[i]->genericError(func, file, failLine);
    }
}


//module default defs

CrashableModule::CrashableModule(UnCrashable &uncrashableParent, bool addSelfToParent = true){
    parent = &uncrashableParent;
    if (addSelfToParent) parent->addModule(*this); //Add itself to the parent.
}