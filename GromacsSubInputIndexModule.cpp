/* 
 * File:   GromacsSubInputIndexModule.cpp
 * Author: kulkem
 * 
 * Created on May 23, 2016, 4:47 PM
 */

#include "GromacsSubInputIndexModule.h"
#include "gromacs/options.h"

namespace gmx
{

void GromacsSubInputIndexModule::initialize()
{
    if(settings_.bOTfName && !settings_.cOTfName) settings_.bOTfName = false;
}    
    
void GromacsSubInputIndexModule::initOptions(Options* options)
{
    if (settings_.bOTfName)
        options->addOption(FileNameOption(settings_.cOTfName).filetype(gmx::eftTrajectory).inputFile().
        defaultBasename(settings_.cfName.c_str()).description("index group file for selection").store(&settings_.cfName));
}

GromacsSubInputIndexModule::GromacsSubInputIndexModule(const GromacsSubInputIndexModuleSettings& settings) : settings_(settings), GromacsSubModule(settings)
{
    
}

GromacsSubInputIndexModule::~GromacsSubInputIndexModule()
{
    
}

int GromacsSubInputIndexModule::start()
{
    
}

void GromacsSubInputIndexModule::finishOptions(Options* options)
{
    
}

void GromacsSubInputIndexModule::finish()
{
    
}

} //name space