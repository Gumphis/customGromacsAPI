/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsAbstractModule.cpp
 * Author: kulkem
 * 
 * Created on July 27, 2017, 7:42 PM
 */

#include <cstring>

#include "gromacs/commandline/cmdlineinit.h"

#include "gmxpre.h"

#include "gromacs/analysisdata/paralleloptions.h"
#include "gromacs/commandline/cmdlinehelpcontext.h"
#include "gromacs/commandline/cmdlinehelpwriter.h"
#include "gromacs/commandline/cmdlinemodule.h"
#include "gromacs/commandline/cmdlineparser.h"
#include "gromacs/fileio/trx.h"
#include "gromacs/options/filenameoptionmanager.h"
#include "gromacs/pbcutil/pbc.h"
#include "gromacs/selection/selectioncollection.h"
#include "gromacs/selection/selectionoptionmanager.h"
#include "gromacs/trajectoryanalysis/analysismodule.h"
#include "gromacs/utility/exceptions.h"
#include "gromacs/utility/file.h"

#include "tsAbstractModule.h"
#include "tsProgram.h"

namespace gmx
{
    
tsAbstractModule::tsAbstractModule(const std::string& name, const std::string& description)
        :   moduleName_(name), moduleDescription_(description)
{
}

tsAbstractModule::~tsAbstractModule()
{
}

const std::string& tsAbstractModule::getName() const        {return moduleName_;}
const std::string& tsAbstractModule::getDescription() const {return moduleDescription_;}

//*****************************************************************

tsModuleHandler::tsModuleHandler(tsAbstractModule* module)
    : module_(module)
{
}

tsModuleHandler::~tsModuleHandler()
{
}

const std::string& tsModuleHandler::name() const             {return module_->getName();}
const std::string& tsModuleHandler::type() const             {return module_->type();}
const std::string& tsModuleHandler::description() const      {return module_->getDescription();}
tsAbstractModule*       tsModuleHandler::getModule()         {return module_;}
const tsAbstractModule* tsModuleHandler::getModule() const   {return module_;}
int         tsModuleHandler::run()                           {return module_->run();}
void        tsModuleHandler::finish()                        {module_->finish();}       
void        tsModuleHandler::initialize()                    {module_->initialize();}       
void        tsModuleHandler::initOptions(tsOptions* options)   {module_->initOptions(options);}       
void        tsModuleHandler::finishOptions(tsOptions* options) {module_->finishOptions(options);}       

//*****************************************************************

tsModules::tsModules()
{
}

tsModules::~tsModules()
{
    for(int i = 0; i < pHandlerList.size(); ++i)
        TS_SAFE_DELETE(pHandlerList[i]);
    pHandlerList.clear();
    for(int i = 0; i < modules.size(); ++i)
        TS_SAFE_DELETE(modules[i]);
    modules.clear();
}

tsModuleHandler* tsModules::findModule(const std::string& name)
{
    for(int i = 0; i < pHandlerList.size(); ++i)
        if(!name.compare(pHandlerList[i]->name()))
            return pHandlerList[i];
    
    return NULL;
}

tsModuleHandler* tsModules::addModule(tsAbstractModule* module)
{
    if(findModule(module->getName()) != NULL) GMX_THROW(APIError("Duplicate module: " + module->getName()));
    tsModuleHandler* mhandler = module->getHandler();
    modules.push_back(module);
    pHandlerList.push_back(mhandler);
    
    return mhandler;
}

} //namespace gmx