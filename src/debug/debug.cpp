//The main debug handling class
//Licensed under MIT, See the LICENCE file.
//Cal.W 2020
#include "debug.h"

bool GenericCrashable::init(){
    return true;
}

void GenericCrashable::printErrorInfo(const char* func, const char* file, u16 failLine, bool forcePrint){
    if (forcePrint || status != CrashType::None) {
        DBG_PRINTF("[%s] Error in [%s] at line %u in %s\r\n", ChrashTypeText[status],
            func, failLine, file); 
    }
}

void GenericCrashable::genericError(const char* func, const char* file, u16 failLine){
    printErrorInfo(func, file, failLine, true);
    printDebug();
}

void GenericCrashable::minorFailure(const char* func, const char* file, u16 failLine){
    status = CrashType::Minor;
    genericError(func, file, failLine);
}

void GenericCrashable::majorFailure(const char* func, const char* file, u16 failLine) {
    status = CrashType::Major;
    genericError(func, file, failLine);
}

void GenericCrashable::criticalFailure(const char* func, const char* file, u16 failLine) {
    status = CrashType::Critical;
    genericError(func, file, failLine);
}

void GenericCrashable::printDebug(String printValues){};

CrashType GenericCrashable::getStatus(){
    return status;
}


UnCrashable::UnCrashable(){
    for (int i = 0; i < MIN_CHILDREN_LENGTH; i++){
        modules[i] = nullptr;
    }
}

//Group Init
bool UnCrashable::init(){
    bool initSuccessfull = true;
    for (int i = 0; i < (sizeof(modules)/sizeof(modules[0])); i++){
        if (modules[i])
            if(!modules[i]->init())
                initSuccessfull = false;
    }
    return initSuccessfull;
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
    GenericCrashable::genericError(func, file, failLine);

    for (int i = 0; i < (sizeof(modules)/sizeof(modules[0])); i++){
        if (modules[i])
            modules[i]->genericError(func, file, failLine);
    }
}

void UnCrashable::printDebug(String printValues){
    for (int i = 0; i < (sizeof(modules)/sizeof(modules[0])); i++){
        if (modules[i])
            modules[i]->printDebug(printValues);
    }
}



CrashableModule::CrashableModule(UnCrashable &uncrashableParent, bool addSelfToParent){
    parent = &uncrashableParent;
    if (addSelfToParent) parent->addModule(*this); //Add itself to the parent.
}