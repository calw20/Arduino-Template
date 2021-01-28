//The main debug handling class
//Licensed under MIT, See the LICENCE file.
//https://github.com/calw20/Arduino-Template
//Cal.W 2020
#include "debug.h"

bool GenericCrashable::init(){
    return true;
}

CrashType GenericCrashable::setStatus(CrashType newStatus){
    CrashType oldStatus = status;
    status = newStatus;
    return oldStatus;
}

void GenericCrashable::printErrorInfo(const char* func, const char* file, u16 failLine, bool forcePrint){
    if (forcePrint || inError || status != CrashType::None) {
        DBG_PRINTF("[%s] Error in [%s] at line %u in %s\r\n", CrashTypeText[status],
            func, failLine, file); 
    }
}

void GenericCrashable::genericError(const char* func, const char* file, u16 failLine){
    printErrorInfo(func, file, failLine);
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

void CrashableModule::crashParentTree(CrashType crashType, const char* func, const char* file, u16 failLine){
    inError = true;
    status = crashType; //Whilst the fail functs *should* do this, other CrashTypes like ::Fatal dont have methods 

    switch (status){
    case CrashType::Minor:
        minorFailure(func, file, failLine);
        parent->minorFailure(func, file, failLine);
        break;
    case CrashType::Major:
        majorFailure(func, file, failLine);
        parent->majorFailure(func, file, failLine);
        break;
    case CrashType::Critical:
        criticalFailure(func, file, failLine);
        parent->criticalFailure(func, file, failLine);
        break;
    default:
        genericError(func, file, failLine);
        parent->genericError(func, file, failLine);
        break;
    }
    //[TODO] Decide recovery is assumed....
    //inError = false; //We recovered from the error?!
};

//Original C code by "paxdiablo"; https://stackoverflow.com/a/7776146
//Adapted by Cal.W 2020-01-28
void hexDump(const char * desc, const void * addr, const int len) {
    int i;
    unsigned char buff[17];
    const unsigned char * pc = (const unsigned char *)addr;

    //Output description if given.
    if (desc != NULL)
        DBG_PRINTF("%s:\n\r", desc);

    //Length checks.
    if (len == 0) {
        DBG_PRINTF("  Zero Length\n\r");
        return;
    }
    else if (len < 0) {
        DBG_PRINTF("  Negative Length: %d\n\r", len);
        return;
    }

    //Process every byte in the data.
    for (i = 0; i < len; i++) {
        //Multiple of 16 means new line (with line offset).
        if ((i % 16) == 0) {
            //Don't print ASCII buffer for the "zeroth" line.
            if (i != 0)
                DBG_PRINTF("  %s\n\r", buff);

            //Output the offset.
            DBG_PRINTF("  %04x ", i);
        }

        //Now the hex code for the specific character.
        DBG_PRINTF(" %02x", pc[i]);

        //And buffer a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) //isprint() may be better.
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    //Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        DBG_PRINTF("   ");
        i++;
    }

    //And print the final ASCII buffer.
    DBG_PRINTF("  %s\n\r", buff);
}