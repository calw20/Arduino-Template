#Cal.W 2020
#Autogen a header file for Arduino / C++ Projects
import datetime, json

#Basic const
STORAGE_FILE = "./current_header.json" #If None then no file will attempt to be created/used else the file spectided will be used
LANG = {
    "info_log": {
        "info_guard": "Include Guard enabled.",
        "info_print": "Printing Infomation to console.",
        "file404": "Failed to create the file at '{0}', have no idea why and I'm to lazy to implement real reasoning rn."
    },

    "comments": {
        "auto_gen": "//Auto Generated File to auto complete a few basic vars.\n", #This is put before the auto gen headers
        "custom":   "\n//Custom headers in the json file.\n" #This is before all the custom headers
    }
}

Build_Date = datetime.datetime.now().strftime("%d-%m-%y - %H:%M:%S")
#Default JSON Object
headerSettings = {
    #Basic Settings
    "header_file": "./build_headers.h", #The name of the header file
    "update_header_file": True, #Should the header file be updated?
    "print_headers": True, #Should the headers be logged to console?
    "info_print": True, #Should info statements be logged?
    "increment_version": True, #Should the 3rd number in the version be incremented?
    "include_header_guard": True, #Include the header guard?
    "header_guard_name": "AUTOGEN_H", #If auto_guard is enabled, what is the header name
    "define_block": "#define {0} {1}", #Needs to be in the form such that .format(CONST_NAME, VALUE) will be valid, IE "#ifndef {0}\n#define {0} {1}\n#endif" is valid.
    "header_injection": {
        "pre_guard": "//This is a auto generated file.\n//Cal.W 2020", #This is put BEFORE the include guard, be carefull
        "post_guard": "", #This is put INSIDE the include guard
        "pre_footer": "", #This is put JUST BEFORE the include guard is closed
    },


    #Header Settings
    "build_date": Build_Date, #This is auto-updated
    "build_version": [0, 0, -1, "Alpha"], #This NEEDS to be [int, int, int, str] as maths is done on the values. Also if the str is blank or None then no ' - ' will be added
    "other_headers": [
        #("DO_DEBUG", True),
        #("OS_ENABLED", False, "//#define {0} {1}"), #Dont need to close the line as new.
        #("Inject the 3rd pram.", None, "//Hellow World"),
    ]
}

if headerSettings["info_print"]: print("="*50, LANG["info_log"]["info_print"], sep="\n")

#A few shorthand define functions
DefineRaw = lambda cont, value: headerSettings["define_block"].format(str(cont), value) + "\n" #Literally a remapping of the format operator
DefineBol = lambda cont, value: DefineRaw(cont, ("true" if bool(value) else "false"))  #Returns a define statement that is forced to be a int
DefineStr = lambda cont, value: DefineRaw(cont, '"'+str(value)+'"')         #Returns a define statement that is forced to be a string
DefineFlt = lambda cont, value: DefineRaw(cont, float(value))               #Returns a define statement that is forced to be a float
DefineInt = lambda cont, value: DefineRaw(cont, int(value))                 #Returns a define statement that is forced to be a int

#infoPrint = lambda text, *args, **kwargs: if headerSettings["info_print"]: print(LANG["info_log"][text], *args, **args) 

buildVersion = lambda vArr: "{0} {1} {2}{3}".format(vArr[0], vArr[1], vArr[2], (" - " + str(vArr[3]) if str(vArr[3]).strip() else ""))

#Get or create the json storage file
if STORAGE_FILE is not None:
    #Get storage file and load the json
    try:
        with open(STORAGE_FILE, 'r') as jsonFile:
            headerSettings = json.load(jsonFile)
    except FileNotFoundError:
        #Can't find file so make it, and if that fails cry
        try:
            with open(STORAGE_FILE, 'w') as jsonFile:
                json.dump(headerSettings, jsonFile, indent=4)
        except:
            if headerSettings["info_print"]: print(LANG["info_log"]["file404"].format(STORAGE_FILE))

#Add the include guard if enabled

if headerSettings["include_header_guard"]:
    if headerSettings["info_print"]: print(LANG["info_log"]["info_guard"])
    header_guard = [
        headerSettings["header_injection"]["pre_guard"] + "\n" + "#ifndef {0}\n#define {0}\n".format(headerSettings["header_guard_name"]) + headerSettings["header_injection"]["post_guard"] + "\n" + ("\n" if headerSettings["header_injection"]["post_guard"] else ""),
        ((headerSettings["header_injection"]["pre_footer"] + "\n") if headerSettings["header_injection"]["pre_footer"] else "") + "#endif",
    ]
else:
    header_guard = [
        headerSettings["header_injection"]["pre_guard"] + "\n" + headerSettings["header_injection"]["post_guard"] + ("\n" if headerSettings["header_injection"]["post_guard"] else ""),
        ("\n" + headerSettings["header_injection"]["pre_footer"]) if headerSettings["header_injection"]["pre_footer"] else "",
    ]

#Create the header file
if headerSettings["update_header_file"]:
    #Update the header info only if we are updating / creating the header file
    headerSettings["build_date"] = Build_Date
    if headerSettings["increment_version"]:
        headerSettings["build_version"][2] += 1

    #Update / Create the header file
    with open(headerSettings["header_file"], 'w') as header_file:
        #Add the header
        header_file.write(header_guard[0])
        
        #Add the auto gen defines
        header_file.write(LANG["comments"]["auto_gen"])
        header_file.write(DefineStr("BUILD_DATE", Build_Date))
        header_file.write(DefineStr("BUILD_VERSION", buildVersion(headerSettings["build_version"])))
        
        #Add the custom defines
        if len(headerSettings["other_headers"]) > 0:
            header_file.write(LANG["comments"]["custom"])
            for hTuple in headerSettings["other_headers"]:
                if len(hTuple) > 2:
                    const, value, define_string = hTuple 
                    header_file.write(define_string.format(const, value)+"\n")
                else: 
                    const, value = hTuple  
                    if type(value) == str:
                        header_file.write(DefineStr(const, value))
                    elif type(value) == int:
                        header_file.write(DefineInt(const, value))
                    elif type(value) == float:
                        header_file.write(DefineFlt(const, value))
                    elif type(value) == bool:
                        header_file.write(DefineBol(const, value))
                    else:
                        header_file.write(DefineRaw(const, value))
        
        #Add the footer
        header_file.write(header_guard[1])
    
        if headerSettings["info_print"]: print("Created / Updated header file:", headerSettings["update_header_file"])

    with open(STORAGE_FILE, 'w') as jsonFile:
        json.dump(headerSettings, jsonFile, indent=4)

        if headerSettings["info_print"]: print("Updated JSON Storage file:", STORAGE_FILE)

else:
    if headerSettings["info_print"]: print("Header file not Created / Updated as that option is disabled.")

if headerSettings["print_headers"]:
    print("="*50)
    print("Auto Generating Compile Headers")
    print("BUILD_DATE:", headerSettings["build_date"])
    print("BUILD_VERSION:", buildVersion(headerSettings["build_version"]))
    if len(headerSettings["other_headers"]) > 0:
        print("-"*14, "Other Custom Headers", "-"*14)
        for hTuple in headerSettings["other_headers"]:
            if len(hTuple) > 2:
                const, value, define_format = hTuple
                print("Injected:", '"'+define_format.format(const, value)+'"')
            else:
                const, value = hTuple
                print(str(const)+":", value)
    print("="*50)

