/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsAbstractModule.h
 * Author: kulkem
 *
 * Created on July 27, 2017, 7:42 PM
 */

#pragma once

#include <cstring>
#include "gromacs/utility/classhelpers.h"
#include "gromacs/utility/gmxassert.h"
#include "tsOption.h"


namespace gmx
{

class tsProgram;
class tsModules;
class tsModuleHandler;

//*****************************************************************

class tsAbstractModule {
public:
    typedef tsModuleHandler ModuleHandlerType;
    
    virtual ~tsAbstractModule();
    
protected:
    explicit tsAbstractModule(const std::string& name, const std::string& description);
    
    virtual int         run() = 0;                             //run the module
    virtual void        finish() = 0;                          //finish the module
    virtual void        initialize() = 0;                      //initialize the module
    virtual void        initOptions(tsOptions* options) = 0;   //initialize the options
    virtual void        finishOptions(tsOptions* options) = 0; //finish the options
    virtual std::string type() = 0;
    virtual tsModuleHandler* getHandler() = 0;                 //Allocates a new Handler pointer
    
    const std::string& getName() const;
    const std::string& getDescription() const;
    
private:
    std::string moduleName_;
    std::string moduleDescription_;
    
    friend class tsModuleHandler;
    friend class tsModules;
};

//*****************************************************************

class tsModuleHandler
{
public:
    virtual ~tsModuleHandler();

    template <class ModuleHandlerType>
    bool isType() const                                 //Test for Type
    {
        return toType<ModuleHandlerType>() != NULL;
    }

    template <class ModuleHandlerType>
    ModuleHandlerType* toType()                         //transform to Type
    {
        return dynamic_cast<ModuleHandlerType*>(this);
    }
    
    template <class ModuleHandlerType>
    const ModuleHandlerType* toType() const             //transform to Type
    {
        return dynamic_cast<const ModuleHandlerType*>(this);
    }

    const std::string&  name() const;                   //returns the name
    const std::string&  type() const;                   //returns the type
    const std::string&  description() const;            //returns the description
    int         run();                                  //run the module
    void        finish();                               //finish the module
    void        initialize();                           //initialize the module
    void        initOptions(tsOptions* options);        //initialize the options
    void        finishOptions(tsOptions* options);      //finish the options
    
protected:
    explicit tsModuleHandler(tsAbstractModule* module);

    tsAbstractModule*       getModule();
    const tsAbstractModule* getModule() const;

private:
    tsAbstractModule*  module_;                         //AbstractModule
    GMX_DISALLOW_COPY_AND_ASSIGN(tsModuleHandler);
    
    friend class tsModules;
};

//*****************************************************************
//class manages the pointers in pHandlerList and modules
//so they should not be freed


class tsModules
{
public:
    ~tsModules();
    
    tsModuleHandler* findModule(const std::string& name);
    
    template <class ModuleType>
    typename ModuleType::ModuleHandlerType* addModule(const ModuleType& module)
    {
        ModuleType* new_module = new ModuleType(std::move(module));
        tsModuleHandler* handler = addModule(static_cast<tsAbstractModule*>(new_module));
        GMX_ASSERT(handler->isType<typename ModuleType::ModuleHandlerType>(),
                   "Mismatching module handler type declaration and implementation");
        return handler->toType<typename ModuleType::ModuleHandlerType>();
    }
    
private:
    explicit tsModules();
    tsModuleHandler* addModule(tsAbstractModule* module);
    
    std::vector<tsModuleHandler*> pHandlerList;
    std::vector<tsAbstractModule*> modules;
    friend class tsProgram;
};

} //namespace gmx
