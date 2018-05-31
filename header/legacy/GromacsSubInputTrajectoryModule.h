/* 
 * File:   GromacsSubInputTrajectoryModule.h
 * Author: kulkem
 *
 * Created on May 18, 2016, 1:28 PM
 */

#ifndef GROMACSSUBINPUTTRAJECTORYMODULE_H
#define	GROMACSSUBINPUTTRAJECTORYMODULE_H

#include "gromacs/utility/classhelpers.h"
#include <cstring>
#include "GromacsSubModule.h"
#include "GromacsSubInputTopologyModule.h"
#include "gromacs/fileio/trxio.h"
#include "gromacs/fileio/trx.h"

namespace gmx
{

class GromacsSubInputTrajectoryModuleSettings;    
    
class GromacsSubInputTrajectoryModuleInterface {
public:
    GromacsSubInputTrajectoryModuleInterface() {}
    virtual ~GromacsSubInputTrajectoryModuleInterface() {}
    virtual void initAnalysis(const GromacsSubInputTrajectoryModuleSettings& settings, const GromacsTopologyInformation& topology) {}
    virtual void initAfterFirstFrame(const GromacsSubInputTrajectoryModuleSettings& settings,  const t_trxframe& frame) {}
    virtual void analyzeFrame(int frnr,  const t_trxframe& frame) {}
    virtual void finishAnalysis() {} 
};
    
class GromacsSubInputTrajectoryModuleSettings : public GromacsSubModuleSettings {
public:
    GromacsSubInputTrajectoryModuleSettings() : GromacsSubModuleSettings()
    {
        cfName = "traj";
        bAllowPdb = true;    
        bAllowGro = true;    
        bAllowXtc = true;    
        bAllowTrr = true;    
        brmPBC = true;       
        topModule = NULL;
        bOTfName = true;
        cOTfName = "f";
        bOTrmPBC = true;
        cOTrmPBC = "rmpbc";
    }
    GromacsSubInputTrajectoryModuleSettings(const GromacsSubInputTrajectoryModuleSettings& orig) : GromacsSubModuleSettings(orig),
    cfName(orig.cfName), bAllowPdb(orig.bAllowPdb), bAllowGro(orig.bAllowGro), bAllowXtc(orig.bAllowXtc),
    bAllowTrr(orig.bAllowTrr), brmPBC(orig.brmPBC), topModule(orig.topModule), bOTfName(orig.bOTfName),
    cOTfName(orig.cOTfName), bOTrmPBC(orig.bOTrmPBC), cOTrmPBC(orig.cOTrmPBC), interface(orig.interface) {}
    virtual ~GromacsSubInputTrajectoryModuleSettings() {}
    
    virtual bool isSettingInGroup(SettingDefinition definition) {return definition == GMX_SM || definition == GMS_SM_INPUTTRAJECTORY;}
    virtual bool isSettingSet(SettingDefinition definition) {return definition == GMS_SM_INPUTTRAJECTORY;}
    
    std::string cfName;         //file Name
    //TODO: not supported yet
    bool bAllowPdb;             //allow .pdb format
    bool bAllowGro;             //allow .gro format
    bool bAllowXtc;             //allow .xtc format
    bool bAllowTrr;             //allow .trr format
    //end not supported yet
    bool brmPBC;                //remove pbc from trajectory
    GromacsSubInputTopologyModule* topModule; //pointer to the topology module for pbc removal
    bool bOTfName;              //add the file name to the input options
    const char* cOTfName;       //use this tag for the options
    bool bOTrmPBC;              //add the file name to the input options
    const char* cOTrmPBC;       //use this tag for the options
    GromacsSubInputTrajectoryModuleInterface interface;      //Callback function for analyzing frames
        
};
    
class GromacsSubInputTrajectoryModule : public GromacsSubModule {
public:
    GromacsSubInputTrajectoryModule(const GromacsSubInputTrajectoryModuleSettings &settings) : settings_(settings), GromacsSubModule(settings) {}
    virtual ~GromacsSubInputTrajectoryModule() {}
    
    //start the submodule
    virtual int start();
    
    const GromacsSubInputTrajectoryModuleSettings& getSettings() {return settings_;}
    
    void changeInterface(const GromacsSubInputTrajectoryModuleInterface& interface) {settings_.interface = interface;}
    
private:
    using GromacsSubModule::getSettings;
    GromacsSubInputTrajectoryModuleSettings settings_;
    
protected:    
    
    //finish the module
    virtual void finish();
    
     //initialize the module
    virtual void initialize();
    
    virtual void initOptions(Options* options);
    
    virtual void finishOptions(Options* options);
    
    friend class GromacsModule;
    friend class GromacsSubAnalysisTrajectoryModule;
};

} // name space

#endif	/* GROMACSSUBINPUTTRAJECTORYMODULE_H */

