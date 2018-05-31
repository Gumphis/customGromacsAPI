/* 
 * File:   GromacsSubInputIndexModule.h
 * Author: kulkem
 *
 * Created on May 23, 2016, 4:47 PM
 */

#ifndef GROMACSSUBINPUTINDEXMODULE_H
#define	GROMACSSUBINPUTINDEXMODULE_H

#include "gromacs/utility/classhelpers.h"
#include <cstring>
#include "GromacsSubModule.h"

namespace gmx
{

class GromacsSubInputIndexModuleSettings : public GromacsSubModuleSettings {
public:
    GromacsSubInputIndexModuleSettings() : GromacsSubModuleSettings()
    {
        cfName = "index";
        bOTfName = true;
        cOTfName = "f";
    }
    GromacsSubInputIndexModuleSettings(const GromacsSubInputIndexModuleSettings& orig) : GromacsSubModuleSettings(orig),
    cfName(orig.cfName), bOTfName(orig.bOTfName), cOTfName(orig.cOTfName) {}
    virtual ~GromacsSubInputIndexModuleSettings() {}
    
    virtual bool isSettingInGroup(SettingDefinition definition) {return definition == GMX_SM || definition == GMS_SM_INPUTINDEX;}
    virtual bool isSettingSet(SettingDefinition definition) {return definition == GMS_SM_INPUTINDEX;}
    
    std::string cfName;         //file Name
    bool bOTfName;              //add the file name to the input options
    const char* cOTfName;       //use this tag for the options
};

class GromacsSubInputIndexModule : public GromacsSubModule {
public:
    GromacsSubInputIndexModule(const GromacsSubInputIndexModuleSettings &settings);
    virtual ~GromacsSubInputIndexModule();
    
    //start the submodule
    virtual int start();
    
    const GromacsSubInputIndexModuleSettings& getSettings() {return settings_;}
    
private:
    using GromacsSubModule::getSettings;
    GromacsSubInputIndexModuleSettings settings_;
    
protected:    
    
    //finish the module
    virtual void finish();
    
     //initialize the module
    virtual void initialize();
    
    virtual void initOptions(Options* options);
    
    virtual void finishOptions(Options* options);
    
    friend class GromacsModule;
};

} //name space

#endif	/* GROMACSSUBINPUTINDEXMODULE_H */

