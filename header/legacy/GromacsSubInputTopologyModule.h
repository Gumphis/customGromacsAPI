/* 
 * File:   GromacsSubInputTopologyModule.h
 * Author: kulkem
 *
 * Created on May 19, 2016, 11:17 AM
 */

#ifndef GROMACSSUBINPUTTOPOLOGYMODULE_H
#define	GROMACSSUBINPUTTOPOLOGYMODULE_H

#include "gromacs/utility/classhelpers.h"
#include "gromacs/topology/topology.h"
#include "gromacs/fileio/filenm.h"
#include <cstring>
#include "GromacsSubModule.h"

namespace gmx
{

class GromacsSubInputTopologyModuleSettings : public GromacsSubModuleSettings {
public:
    GromacsSubInputTopologyModuleSettings() : GromacsSubModuleSettings()
    {
        cfName = "topol";
        bAllowTop = true;    
        bAllowTpr = true;         
        bOTfName = true;
        cOTfName = "s";
        stFileExtension = efTPR;
    }
    GromacsSubInputTopologyModuleSettings(const GromacsSubInputTopologyModuleSettings& orig) : GromacsSubModuleSettings(orig),
    cfName(orig.cfName), bAllowTpr(orig.bAllowTpr), bAllowTop(orig.bAllowTop), bOTfName(orig.bOTfName),
    cOTfName(orig.cOTfName), stFileExtension(orig.stFileExtension) {}
    virtual ~GromacsSubInputTopologyModuleSettings() {}
    
    virtual bool isSettingInGroup(SettingDefinition definition) {return definition == GMX_SM || definition == GMS_SM_INPUTTOPOLOGY;}
    virtual bool isSettingSet(SettingDefinition definition) {return definition == GMS_SM_INPUTTOPOLOGY;}
    
    std::string cfName;         //file Name
    //TODO: not supported yet
    bool bAllowTpr;             //allow .pdb format
    bool bAllowTop;             //allow .gro format
    //end not supported yet
    bool bOTfName;              //add the file name to the input options
    const char* cOTfName;       //use this tag for the options
    int stFileExtension;        //Standard file Extension (from filenm.h enum list)
        
};
    
class GromacsTopologyInformation
{
public:
    bool hasTopology() const { return top_ != NULL; } //! Returns true if a topology file was loaded.
    bool hasFullTopology() const { return bTop_; } //! Returns true if a full topology file was loaded.
    t_topology *topology() const { return top_; } //! Returns the loaded topology, or NULL if not loaded.
    int ePBC() const { return ePBC_; } //! Returns the ePBC field from the topology.
    
    /*! \brief
    * Gets the configuration from the topology.
    *
    * \param[out] x     Topology coordinate pointer to initialize.
    *      (can be NULL, in which case it is not used).
    * \param[out] box   Box size from the topology file
    *      (can be NULL, in which case it is not used).
    * \throws  APIError if topology coordinates are not available and
    *      \p x is not NULL.
    *
    * If TrajectoryAnalysisSettings::efUseTopX has not been specified,
    * \p x should be NULL.
    *
    * The pointer returned in \p *x should not be freed.
    */
    void getTopologyConf(rvec **x, matrix box) const;
private:
    GromacsTopologyInformation();
    ~GromacsTopologyInformation();

    
    t_topology          *top_; //! The topology structure, or NULL if no topology loaded.  
    bool                 bTop_; //! true if full tpx file was loaded, false otherwise.
    rvec                *xtop_; //! Coordinates from the topology (can be NULL).
    matrix               boxtop_; //! The box loaded from the topology file.
    int                  ePBC_; //! The ePBC field loaded from the topology file.
    
    GMX_DISALLOW_COPY_AND_ASSIGN(GromacsTopologyInformation);
    
    friend class GromacsSubInputTopologyModule;
};
    
class GromacsSubInputTopologyModule : public GromacsSubModule {
public:
    GromacsSubInputTopologyModule(const GromacsSubInputTopologyModuleSettings &settings) : settings_(settings), GromacsSubModule(settings) {}
    virtual ~GromacsSubInputTopologyModule() {}
    
    //start the submodule 
    virtual int start();
    
    const GromacsTopologyInformation& getTopology() {return topInfo_;}
    
    const GromacsSubInputTopologyModuleSettings& getSettings() {return settings_;}
    
private:
    using GromacsSubModule::getSettings;
    GromacsSubInputTopologyModuleSettings settings_;
    
protected:
    GromacsTopologyInformation topInfo_;
    
    //initialize the module
    virtual void initialize();
    
    virtual void initOptions(Options* options);
    
    virtual void finishOptions(Options* options);
    
    //finish the module
    virtual void finish();
    
    friend class GromacsModule;
    friend class GromacsSubAnalysisTrajectoryModule;
};

} //name space
#endif	/* GROMACSSUBINPUTTOPOLOGYMODULE_H */

