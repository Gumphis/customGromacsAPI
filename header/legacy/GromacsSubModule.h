/* 
 * File:   GromacsSubModule.h
 * Author: kulkem
 *
 * Created on May 18, 2016, 10:55 AM
 */

#ifndef GROMACSSUBMODULE_H
#define	GROMACSSUBMODULE_H

#include "gromacs/utility/classhelpers.h"
#include "gromacs/options/options.h"

namespace gmx
{

enum SettingDefinition
{
    GMX_SM,
    GMX_SM_ANALYSIS,
    GMX_SM_ANALYSISTRAJECTORY,
    GMS_SM_INPUTTRAJECTORY,
    GMS_SM_INPUTTOPOLOGY,
    GMS_SM_INPUTINDEX,
    GMS_SM_SELECTION
};

class GromacsSubModuleSettings {
public:
    GromacsSubModuleSettings() : bQuiet(false) {}
    GromacsSubModuleSettings(const GromacsSubModuleSettings& orig) : bQuiet(orig.bQuiet) {}
    virtual ~GromacsSubModuleSettings() {}
    
    virtual bool isSettingInGroup(SettingDefinition definition) {return definition == GMX_SM;}
    virtual bool isSettingSet(SettingDefinition definition) {return definition == GMX_SM;}
    
    bool bQuiet;
};
    
class GromacsSubModule {
public:
    GromacsSubModule(const GromacsSubModuleSettings &settings) : settings_(settings) {}
    virtual ~GromacsSubModule() {}
    
    //start the submodule
    virtual int start() = 0;
    
    const GromacsSubModuleSettings& getSettings() {return settings_;}
    
private:
    GromacsSubModuleSettings settings_;
    
protected:
        
    //finish the module
    virtual void finish() = 0;
    
    //initialize the module
    virtual void initialize() = 0;
    
    virtual void initOptions(Options* options) = 0;
    
    virtual void finishOptions(Options* options) = 0;
    
    friend class GromacsModule;
};

} //name space

#endif	/* GROMACSSUBMODULE_H */

