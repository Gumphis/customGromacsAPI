/* 
 * File:   GromacsSubAnalysisTrajectoryModule.h
 * Author: kulkem
 *
 * Created on May 19, 2016, 5:35 PM
 */

#ifndef GROMACSSUBANALYSISTRAJECTORYMODULE_H
#define	GROMACSSUBANALYSISTRAJECTORYMODULE_H

#include "gromacs/utility/classhelpers.h"
#include "GromacsSubAnalysisModule.h"
#include "GromacsSubInputTopologyModule.h"
#include "GromacsSubInputTrajectoryModule.h"
#include <cstring>

namespace gmx
{
   
class GromacsSubAnalysisTrajectoryModuleSettings : public GromacsSubAnalysisModuleSettings {
public:
    GromacsSubAnalysisTrajectoryModuleSettings() : GromacsSubAnalysisModuleSettings()
    {
        topModule = NULL;
        trajModule = NULL;
    }
    GromacsSubAnalysisTrajectoryModuleSettings(const GromacsSubAnalysisTrajectoryModuleSettings& orig) : GromacsSubAnalysisModuleSettings(orig),
    trajSettings(orig.trajSettings), topSettings(orig.topSettings), topModule(orig.topModule), trajModule(orig.trajModule) {}

    virtual ~GromacsSubAnalysisTrajectoryModuleSettings() {}
    
    virtual bool isSettingInGroup(SettingDefinition definition) {return definition == GMX_SM || definition == GMX_SM_ANALYSISTRAJECTORY || definition == GMX_SM_ANALYSIS;}
    virtual bool isSettingSet(SettingDefinition definition) {return definition == GMX_SM_ANALYSISTRAJECTORY;}
    
    GromacsSubInputTrajectoryModuleSettings trajSettings; //Settings for the trajectory
    GromacsSubInputTopologyModuleSettings topSettings; //Settings for the topology
    /*can give a sub module pointer instead of setting the options
     The submodule has to be added to your module*/
    GromacsSubInputTopologyModule* topModule;
    /*can give a sub module pointer instead of setting the options
     The submodule has to be added to your module*/
    GromacsSubInputTrajectoryModule* trajModule;
        
};
    
class GromacsSubAnalysisTrajectoryModule : public GromacsSubAnalysisModule {
public:
    GromacsSubAnalysisTrajectoryModule(const GromacsSubAnalysisTrajectoryModuleSettings &settings) : settings_(settings), bFreeTop(false),
            bFreeTraj(false), GromacsSubAnalysisModule(settings) {}
    virtual ~GromacsSubAnalysisTrajectoryModule() {}
    
    //start the submodule
    virtual int start();
    
    const GromacsSubAnalysisTrajectoryModuleSettings& getSettings() {return settings_;}
    
    void changeInterface(const GromacsSubInputTrajectoryModuleInterface& interface) {settings_.trajModule->settings_.interface = interface;}
    
private:
    using GromacsSubAnalysisModule::getSettings;
    GromacsSubAnalysisTrajectoryModuleSettings settings_;
    bool bFreeTop;
    bool bFreeTraj;
    
protected:    
    
    //finish the module
    virtual void finish();
    
     //initialize the module
    virtual void initialize();
    
    virtual void initOptions(Options* options);
    
    virtual void finishOptions(Options* options);
    
    friend class GromacsModule;
};

} // name space

#endif	/* GROMACSSUBANALYSISTRAJECTORYMODULE_H */

