/* 
 * File:   GromacsSubAnalysisModule.h
 * Author: kulkem
 *
 * Created on May 18, 2016, 12:36 PM
 */

#ifndef GROMACSSUBANALYSISMODULE_H
#define	GROMACSSUBANALYSISMODULE_H

#include "gromacs/utility/classhelpers.h"
#include "GromacsSubModule.h"

namespace gmx
{
    
class GromacsSubAnalysisModuleSettings : public GromacsSubModuleSettings {
public:
    GromacsSubAnalysisModuleSettings() : GromacsSubModuleSettings() {}
    GromacsSubAnalysisModuleSettings(const GromacsSubAnalysisModuleSettings& orig) : GromacsSubModuleSettings(orig) {}
    virtual ~GromacsSubAnalysisModuleSettings() {}
    
    virtual bool isSettingInGroup(SettingDefinition definition) {return definition == GMX_SM || definition == GMX_SM_ANALYSIS;}
    virtual bool isSettingSet(SettingDefinition definition) {return definition == GMX_SM_ANALYSIS;}
        
};

class GromacsSubAnalysisModule : public GromacsSubModule {
public:
    GromacsSubAnalysisModule(const GromacsSubAnalysisModuleSettings &settings) : settings_(settings), GromacsSubModule(settings) {}
    virtual ~GromacsSubAnalysisModule() {}
    
    //start the submodule
    virtual int start() = 0;
    
    const GromacsSubAnalysisModuleSettings& getSettings() {return settings_;}
    
private:
    using GromacsSubModule::getSettings;
    GromacsSubAnalysisModuleSettings settings_;
    
    
    //initialize the module
    virtual void initialize() = 0;
    
    virtual void initOptions(Options* options) = 0;
    
    virtual void finishOptions(Options* options) = 0;
    
    //finish the module
    virtual void finish() = 0;
    
    friend class GromacsModule;
};

} //name space

#endif	/* GROMACSSUBANALYSISMODULE_H */

