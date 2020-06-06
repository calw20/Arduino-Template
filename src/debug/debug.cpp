//The main debug handling class
//Licensed under MIT, See the LICENCE file.
//Cal.W 2020
#include "debug.h"


//Build the array
DebugPrintFunctionList::DebugPrintFunctionList(int newCapacity){
    capacity = newCapacity;
    //capacity = INITAL_LIST_LENGTH;
    funcList = new DebugPrintFunction[capacity];
    length = 0;
}

//Need to nuke the internal array to remove a memory leak
DebugPrintFunctionList::~DebugPrintFunctionList(){
    if (funcList) delete [] funcList;
}

//Add an item to the internal array
bool DebugPrintFunctionList::append(DebugPrintFunction appFunc){
    DBG_PRINTFN("Append: %i, %p", &appFunc, appFunc);

    DebugPrintFunction* tmpList; //Create an array of debug functions
    //Check if the current array is full
    if (length >= capacity){
        //Make said array larger then the original based on the function defined as INC_CAPACITY
        tmpList = new DebugPrintFunction[INC_CAPACITY(capacity)];
        //Copy the functions in the current array to the new one
        for (int i = 0; i < capacity; i++){
            tmpList[i] = funcList[i];
        }
        capacity = INC_CAPACITY(capacity); //Increment the capacity
        delete [] funcList; //Nuke the old array
        funcList = tmpList; //Redefine the array to the new one
    }
    //Now we can add the new item
    funcList[length] = appFunc; //?The length is the last element in the list 
    DBG_PRINTFN("runFuncs2: %i, %p", funcList[i], funcList[i]);
    
    length++;
    return true;
}

/*Remove an item to the array
//! Not Implimented
bool DebugPrintFunctionList::remove(int funcIndex){
    return false;
}
*/

//Run all the functions in the list
void DebugPrintFunctionList::runFunctions(String printValues){
    for (int i = 0; i < capacity; i++){
        DBG_PRINTFN("runFuncs: %i, %p", funcList[i], funcList[i]);
        
        funcList[i](printValues); 
    }
}


DebugHandler::DebugHandler(){
    DBG_PRINTFN("Init: %i, %p, %s", &debugPrintHeader, debugPrintHeader);
    debugFuncs = DebugPrintFunctionList();
    debugFuncs.append(&debugPrintHeader);
}

//? If given a string, it will only print the wanted options
//? H - Headers, B - All the BMP280 Data, P - Pressure Info, A - Altitude Info, 
//? D - Temperature Info (in C), T - System Time, -H Everything but headers,
//? +H show what options where given

//Usefull shorthand
#define CHK_LETTER(letter) printValues.indexOf(letter) > -1
//The first function run when printDebug is called.
void DebugHandler::debugPrintHeader(String printValues){
    DBG_FPRINT("passedFunction");
    //Header
    if (CHK_LETTER("NH")) DBG_FPRINTLN("==================================================");
    if (CHK_LETTER("H")) {
        DBG_FPRINTFN("Build Date: ", "%s", BUILD_DATE);
        DBG_FPRINTFN("Build Version: ", "%s", BUILD_VERSION);
        if (CHK_LETTER("+H")) DBG_FPRINTFN("Given String: ", "%s", printValues);
    }

    //Time
    if (CHK_LETTER("TM"))
        DBG_FPRINTFN("System Uptime: ", "%f miniutes.", millis()/60000.00);
    else if (CHK_LETTER("T"))
        DBG_FPRINTFN("System Uptime: ", "%f seconds.", millis()/1000.00); 
}

void DebugHandler::printDebug(String printValues){
    #if !DO_DEBUG //^This only is present when debugging is not enabled 
        return; //The return is a catch all solution - not technically needed but won't hurt to have
    #endif

    if (CHK_LETTER("-H")) printValues = "TB";
    
    //Run through all the now added print debug functions
    debugFuncs.runFunctions(printValues);
}


//If we have had a failure then just die
//[TODO] Make name shorter
void DebugHandler::criticalFailure(const char* func, const char* file, u16 failLine){
    bool inError = true;
    DBG_FPRINTLN("Critical Error!");
    DBG_PRINTF("Error in [%s] at line %u in %s", func, failLine, file); DBG_FPRINTLN("");

    DBG_FPRINTLN("======== Debug Trace ========");
    printDebug();

    while(inError){
        digitalWrite(CRITICAL_LED, HIGH);   
        delay(100);
        digitalWrite(CRITICAL_LED, LOW);   
        delay(100);
    }
}

