#include "src/debug/debug.h"
#include "src/data_types/data_types.h"


class Mod2 : public CrashableModule {
    public:
        Mod2(UnCrashable &uncrashableParent, bool addSelfToParent = true) 
            : CrashableModule(uncrashableParent, addSelfToParent) {};

        void minorFailure(const char* func, const char* file, u16 failLine);

};

void Mod2::minorFailure(const char* func, const char* file, u16 failLine){
    DBG_PRINTLN("Minor Failure in Mod2");
}




void setup(){
    Serial.begin(115200);
    Serial.println("=~=~=~=~=~=~= Start =~=~=~=~=~=~=");
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

}

void loop(){
	
}