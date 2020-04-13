#include "debug.h"

//? If given a string, it will only print the wanted options
//? H - Headers, B - All the BMP280 Data, P - Pressure Info, A - Altitude Info, 
//? D - Temperature Info (in C), T - System Time, -H Everything but headers,
//? +H show what options where given
void printDebug(String printValues){
    #if !DO_DEBUG //^This only is present when debugging is not enabled 
        return; //The return is a catch all solution - not technically needed but won't hurt to have
    #endif

    #define CHK_LETTER(letter) printValues.indexOf(letter) > -1
    
    if (CHK_LETTER("-H")) printValues = "TB";

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

    //Print all the other info
    EXTRA_DUMPS();
}

//If we have had a failure then just die
//[TODO] Make name shorter
void criticalFailure(const char* func, const char* file, u16 failLine){
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

