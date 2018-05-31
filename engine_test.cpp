/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   engine_test.cpp
 * Author: kulkem
 *
 * Created on August 4, 2017, 5:25 PM
 */

#include <cstdlib>
#include "tsProgram.h"
#include "tsSystem.h"
#include "tsTopologyFileModule.h"

using namespace std;

class tsEngineTest : public gmx::tsProgramInterface
{
public:
    tsEngineTest() : tsProgramInterface(), system("system") {}
    virtual ~tsEngineTest() {}
    
    virtual void initModules(gmx::tsModules* modules);                                                  //Register your Modules
    virtual int  callback(gmx::tsModules* module);                                                      //callback function after parameter initialization
    virtual void initOptions(gmx::tsOptions* options, const gmx::tsProgramSettingsInfo& settings);        //initialize additional parameters
    virtual void finishOptions(gmx::tsOptions* options, const gmx::tsProgramSettingsInfo& settings) {};   //function after the initialization of the parameters
    
    gmx::tsSystem system;
    gmx::tsTopologyFileModule::ModuleHandlerType* moduleHandler;
};

void tsEngineTest::initModules(gmx::tsModules* modules)
{
    moduleHandler = modules->addModule(gmx::tsTopologyFileModule(system, "l", "Topology of the system").addIO<gmx::tprFileIO>("tll"));
}

void tsEngineTest::initOptions(gmx::tsOptions* options, const gmx::tsProgramSettingsInfo& settings)
{
}

int tsEngineTest::callback(gmx::tsModules* module)
{
   moduleHandler->readFile();
   
   printf("Atoms: %i, Atomtypes: %i, residues: %i, molecules: %i\n", system.topology().numAtoms(), system.topology().numAtomTypes(), system.topology().numResidue(), system.topology().numMol());
   
   return 0;
}

int main(int argc, char** argv) 
{ 
    return gmx::tsProgram::run<tsEngineTest>(argc, argv, gmx::tsProgramSettings("engine_test", "tests the gmx wrapper enginge"));
}

