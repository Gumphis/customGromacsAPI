/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsTopologyFileModule.h
 * Author: kulkem
 *
 * Created on August 1, 2017, 5:38 PM
 */

#pragma once

#include "tsSystem.h"
#include "tsAbstractFileModule.h"

namespace gmx
{

class tsAbstractTopologyFileIO
{
public:
    virtual ~tsAbstractTopologyFileIO();
    
    virtual gmx_bool read(const std::string& fileName, tsSystem* system);
    virtual gmx_bool write(const std::string& fileName, tsSystem* system);
protected:
    tsAbstractTopologyFileIO();
};

class tsTopologyFileModule;

class tsTopologyFileModule : public tsAbstractFileModule<tsAbstractTopologyFileIO, class tsTopologyFileModule>
{
public:
    explicit tsTopologyFileModule(tsSystem& system, const std::string& name = "s", const std::string& description = "");
    virtual ~tsTopologyFileModule();
    
protected:
    virtual int         run();                             
    virtual void        finish();                          
    virtual void        initialize();                      
    virtual void        initOptions(tsOptions* options);     
    virtual void        finishOptions(tsOptions* options);   
    virtual std::string type();
    virtual void        writeFile();
    virtual void        readFile();
    virtual void        readFile(const std::string& fileName);
    virtual void        writeFile(const std::string& fileName);
    
private:
    tsSystem&   system_;
};

class tprFileIO : public tsAbstractTopologyFileIO
{
public:
    tprFileIO();
    ~tprFileIO();
    
    virtual gmx_bool read(const std::string& fileName, tsSystem* system);
    virtual gmx_bool write(const std::string& fileName, tsSystem* system);
};

class confFileIO : public tsAbstractTopologyFileIO
{
public:
    confFileIO();
    ~confFileIO();
    
    virtual gmx_bool read(const std::string& fileName, tsSystem* system);
    virtual gmx_bool write(const std::string& fileName, tsSystem* system);
};

} // gmx namespace