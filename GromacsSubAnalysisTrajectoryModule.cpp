/* 
 * File:   GromacsSubAnalysisTrajectoryModule.cpp
 * Author: kulkem
 * 
 * Created on May 19, 2016, 5:35 PM
 */

#include "GromacsSubAnalysisTrajectoryModule.h"

namespace gmx
{
    
void GromacsSubAnalysisTrajectoryModule::initialize()
{
    if(!settings_.trajModule)
    {
        //TODO: Gromacs code (snew) doesnt support new with constructor
        settings_.trajModule = new GromacsSubInputTrajectoryModule(settings_.trajSettings);
        bFreeTraj = true;
    } else settings_.trajSettings = settings_.trajModule->getSettings();
    if(!settings_.trajModule->settings_.topModule)
    {
        if(!settings_.topModule)
        {
            //TODO: Gromacs code (snew) doesnt support new with constructor
            settings_.topModule = new GromacsSubInputTopologyModule(settings_.topSettings);
            bFreeTop = true;
            settings_.trajModule->settings_.topModule = settings_.topModule;
        } else settings_.topSettings = settings_.topModule->getSettings();
    } else if(settings_.topModule)
    {
        settings_.trajModule->settings_.topModule = settings_.topModule;
    } else settings_.topModule = settings_.trajModule->settings_.topModule;
    
}


void GromacsSubAnalysisTrajectoryModule::initOptions(Options* options)
{
    if(bFreeTraj) settings_.trajModule->initOptions(options);
    if(bFreeTop) settings_.topModule->initOptions(options);
}


int GromacsSubAnalysisTrajectoryModule::start()
{
    settings_.trajModule->start();
    
    return 0;
}


void GromacsSubAnalysisTrajectoryModule::finishOptions(Options* options)
{

}


void GromacsSubAnalysisTrajectoryModule::finish()
{
    if(bFreeTop)
    {
        delete settings_.topModule;
        settings_.topModule = NULL;
    }
    if(bFreeTraj)
    {
        delete settings_.trajModule;
        settings_.trajModule = NULL;
    }    
}

} // name space
