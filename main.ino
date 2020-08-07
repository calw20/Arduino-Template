//Example arduino project to show the use of this project "template"
//Cal.W 2020

//Fun Fact! For some if you define "DO_DEBUG" here (even before another include)
    //The compiler/linker/whatever magical thing WILL NOT pass it on, sorta.
    //For some reason it will pass it on to the next include for this file
    //Thus the Debug Print Example will work BUT it wont pass it on to any
    //Other file/lib that uses it, resulting in the Crashable Example not
    //displaying / not being in debug mode. 
    //Hence you need to add "["DO_DEBUG", true]" to the "current_header.json"
        //file in other headers.... Yay Ardnio!
//#define DO_DEBUG true //This causes pain :/
#include "src/debug/debug.h"
#include "src/data_types/data_types.h"


class Mod2 : public CrashableModule {
    public:
        Mod2(UnCrashable &uncrashableParent, bool addSelfToParent = true) 
            : CrashableModule(uncrashableParent, addSelfToParent) {};

        void minorFailure(const char* func, const char* file, u16 failLine) override;

};

void Mod2::minorFailure(const char* func, const char* file, u16 failLine){
    DBG_PRINTLN("Minor Failure in Mod2");
}




void setup(){
    Serial.begin(115200);
    Serial.println("=~=~=~=~=~=~= Start =~=~=~=~=~=~=");
    Serial.println("=>= Begin Debug Print Example =<=");
        Serial.println("This line will print no matter what.");
        DBG_FPRINTLN("This line will only be called if 'DO_DEBUG' is true");
        DBG_PRINT("These are simple wrappers around Serial.print/ln and");
        DBG_PRINTLN("have the ability to do things like store strings in sRAM (The F prefixed functions)");
        DBG_FPRINT("Some also do things like put the string ");
        DBG_FPRINT_SVLN("in sRAM but allow for a non-string input like: ", rand());
        
        #if DO_DEBUG
            Serial.println("This will only be added into the program if in debugging mode...");
        #endif
    Serial.println("=>=> End Debug Print Example <=<=");
    Serial.println("=-=- Begin Crashable Example =-=-");
        Serial.println(BUILD_DATE); 
    	Serial.println(BUILD_VERSION); 
        Serial.println(_PRINTF_BUFFER_LENGTH_);
        
        
        UnCrashable DemoRoot;
        CrashableModule Mod1(DemoRoot, false);
        Mod2 Mod2(DemoRoot);
        CrashableModule Mod3(DemoRoot, false);

        DemoRoot.addModule(Mod3);

        DemoRoot.minorFailure(__FUNCTION__, __FILE__, __LINE__);
    Serial.println("=-=-= End Crashable Example =-=-=");
    Serial.println("=+=+ Begin DataTypes Example =+=+");
        fStoredData tmp;
        tmp.val[0] = 5;
        tmp[1] = 7;

        Serial.println(tmp[0]);
        Serial.println(tmp.current);
        Serial.println(tmp[1]);
        Serial.println(tmp.previous);

        tmp = 99;
        Serial.println(tmp.current);
        Serial.println(tmp.previous);

        fStoredData foo = {88,77};
        Serial.println(foo[0]);
        Serial.println(foo[1]);

        tmp = foo;
        Serial.println(tmp[0]);
        Serial.println(tmp[1]);
    Serial.println("=+=+= End DataTypes Example =+=+=");
    Serial.println("=~=~=~=~=~=~= Stop =~=~=~=~=~=~=");

    Serial.println("=~=~=~=~=~=~= You will see this line if everything worked =~=~=~=~=~=~=");

}

void loop(){
	
}