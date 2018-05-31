/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsAbstractFileModule.h
 * Author: kulkem
 *
 * Created on August 1, 2017, 4:02 PM
 */

#pragma once

#include <cstring>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include "gromacs/utility/stringutil.h"
#include "gromacs/utility/exceptions.h"
#include "tsOption.h"
#include "tsAbstractModule.h"
#include "tsUtility.h"

namespace gmx
{

template <class FileIO, class derivedClass> class tsFileModuleHandler;

template< class FileIO, class derivedClass>
class tsAbstractFileModule : public tsAbstractModule, public tsTemplateOption<std::string, derivedClass > 
{
public:
    typedef tsFileModuleHandler<FileIO, derivedClass> ModuleHandlerType;
    typedef tsFileModuleHandler<FileIO, derivedClass> InfoType;
    typedef tsTemplateOption<std::string, derivedClass> MyOptionBase;
    typedef tsAbstractModule MyModuleBase;
    typedef derivedClass MyClass;
    
    virtual ~tsAbstractFileModule()
    {
        ioList_.clear();
        TS_SAFE_DELETE(info_);
    }
    
    template<class derivedFileIO>
    MyClass& addIO(std::string extension)                   //allocate a derived class as parameter, the class handles the freeing of the pointer
    {
        std::shared_ptr<derivedFileIO> io = std::make_shared<derivedFileIO>();
        if(int i = getIO(extension) >= 0)
        {
            ioList_[i].first.reset();
            ioList_[i].first = io;
        }
        else
            ioList_.push_back(std::pair< std::shared_ptr<FileIO>, std::string>(io, extension));
        return me();
    }
    
    MyClass& removeIO(std::string extension)
    {
        for(int i = 0; i < ioList_.size(); ++i)
            if(!ioList_[i].second.compare(extension))
            {
                ioList_[i].first.reset();
                ioList_.erase(ioList_.begin() + i);
                return me();
            }
        return me();
    }
    
    MyClass& libraryFile(bool bLibrary = true)          {bLibrary_ = bLibrary; return me();}
    MyClass& allowMissing(bool bAllow = true)           {bAllowMissing_ = bAllow; return me();}
    MyClass& defaultBasename(std::string basename)      {basename_ = basename; setCurrentDefaultValue(); return me();}
    MyClass& defaultType(std::string filetype)          {defaultType_ = filetype; setCurrentDefaultValue(); return me();}
    
protected:
    explicit tsAbstractFileModule(const std::string& name, const std::string& description)
    : MyModuleBase(name, description), MyOptionBase(name),
      defaultType_(""), bLibrary_(false), bAllowMissing_(false),
      info_(new tsFileModuleHandler<FileIO, derivedClass>(this))
    {
        MyOptionBase::setDescription(MyModuleBase::getDescription());
    }
    
    explicit tsAbstractFileModule(const tsAbstractFileModule& module)
    : MyModuleBase(module), MyOptionBase(module), basename_(module.basename_),
      defaultType_(module.defaultType_), bLibrary_(module.bLibrary_), bAllowMissing_(module.bAllowMissing_),
      bTrajectory_(module.bTrajectory_), bDirectory_(module.bDirectory_), ioList_(module.ioList_),
      info_(new tsFileModuleHandler<FileIO, derivedClass>(this))
    {
        MyOptionBase::setDescription(MyModuleBase::getDescription());
    }
    
    std::vector< std::pair< std::shared_ptr<FileIO>, std::string> > ioList_;
    std::string                                                     filename_;
    
    virtual int         run() = 0;                             //run the module
    virtual void        finish() = 0;                          //finish the module
    virtual void        initialize() = 0;                      //initialize the module
    virtual std::string type() = 0;
    virtual void        writeFile() = 0;
    virtual void        readFile() = 0;
    virtual void        readFile(const std::string& fileName) = 0;
    virtual void        writeFile(const std::string& fileName) = 0;
    virtual void        initOptions(tsOptions* options) = 0;
    virtual void        finishOptions(tsOptions* options) = 0;
    
    virtual tsModuleHandler* getHandler()  {return static_cast<tsModuleHandler*>(new ModuleHandlerType(static_cast<MyClass*>(this)));}
    virtual tsOptionInfo*    createInfo()  {return static_cast<tsOptionInfo*>(new InfoType(static_cast<MyClass*>(this)));}
     
    bool ioExist(std::string extension) const
    {
        return getIO(extension) != -1;
    }
    
    int getIO(std::string extension) const
    {
        for(int i = 0; i < ioList_.size(); ++i)
            if(!ioList_[i].second.compare(extension)) return i;
        return -1;
    }
    
    int getFileIO() {return getFileIO(filename_);}
    
    int getFileIO(const std::string& filename)
    {
        //TODO: distinguish between empty and no extension - boost returns extension with a "."
        std::string extension = boost::filesystem::extension(filename);
        extension.erase(0,1);
        for(int i = 0; i < ioList_.size(); ++i)
            if(!ioList_[i].second.compare(extension))
                return i;
        GMX_THROW(APIError((boost::format("could not find a loader supporting the extension %1%") % extension).str()));
    }
    
    //Option access types
    virtual std::string formatValue() const
    {
        std::string res;
        for(int i = 0; i < 2 && i < ioList_.size(); ++i)
            res += (i > 0 ? "/." : ".") + ioList_[i].second;
        if(ioList_.size() > 2) res += "/...";
        if(res.empty())
            res = (bDirectory_ ? "<dir>" : "<file>");
        else
            res = "[<" + res + ">]";
        return res;
    }
    virtual std::string formatDefaultValue() const
    {
        return getDefaultValue() ? *getDefaultValue() : "";
    }
    virtual std::string formatDescription() const
    {
        std::string res = MyOptionBase::getDescription();
        res += ":";
        for(int i = 0; i < ioList_.size(); ++i)
            res += " " + ioList_[i].second;
        return res;
    }
    virtual void setOptionFromString(const std::string& str)    {setOption(str);}
    
    //TODO: think about the store and default value options - it overlaps with constructor and data
    
    using MyOptionBase::me;
    using MyOptionBase::getValue;
    using MyOptionBase::setOption;
    using MyOptionBase::getDefaultValue;
    using MyOptionBase::store;
    
private:
    void setCurrentDefaultValue()
    {
        if(basename_.empty()) {MyOptionBase::clearFlag(eftsOption_HasDefaultValue);}
        MyOptionBase::defaultValue(basename_ + (defaultType_.empty() ? (ioList_.empty() ? "" : "." + ioList_[0].second) : "." + defaultType_));
    }
    
    using MyOptionBase::getDescription;
    
    std::string basename_;
    std::string defaultType_;
    bool        bLibrary_;
    bool        bAllowMissing_;
    bool        bTrajectory_;
    bool        bDirectory_;
    tsFileModuleHandler<FileIO, derivedClass>* info_;
    
    friend class tsFileModuleHandler<FileIO, derivedClass>;
};

//*****************************************************************

template <class FileIO, class derivedClass>
class tsFileModuleHandler : public tsModuleHandler, public tsTemplateOptionInfo<std::string>
{
public:
    explicit tsFileModuleHandler(tsAbstractFileModule<FileIO, derivedClass>* module) 
        : tsModuleHandler(module), tsTemplateOptionInfo(module)
    {
    }
    
    virtual             ~tsFileModuleHandler()                      {}
    const std::string&  getFileName() const                         {return module()->filename_;}
    std::string&        getFileName()                               {return module()->filename_;}
    void                setFileName(const std::string& filename)    {module()->filename_ = filename;}
    void                writeFile()                                 {module()->writeFile();}
    void                readFile()                                  {module()->readFile();}
    void                writeFile(const std::string& filename)      {module()->writeFile(filename);}
    void                readFile(const std::string& filename)       {module()->readFile(filename);}
    void                removeIO(std::string extension)             {module()->removeIO(extension);}
    
    template <class DerivedFileIO>
    void addIO(const DerivedFileIO& io, std::string extension)
    {
        module()->addIO(static_cast<FileIO*>(new DerivedFileIO()), extension);
    }
    
    bool isLibraryFile() const {return module()->bLibrary_;}
    bool allowMissing() const {return module()->bAllowMissing_;}
    bool isDirectoryOption() const {return module()->bDirectory_;}
    bool isTrajectoryOption() const {return module()->bTrajectory_;}
    std::string defaultExtension() const {return module()->defaultType_;}
    std::vector<std::string> extensions() const
    {
        std::vector<std::string> res;
        for(int i = 0; i < module()->ioList_.size(); ++i)
            res.push_back(module()->ioList_[i].second);
        return res;
    }
    bool isValidExtension(std::string extension) const {return module()->ioExist(extension);}
    
private:
    const tsAbstractFileModule<FileIO, derivedClass>*        module() const   {return static_cast<const tsAbstractFileModule<FileIO, derivedClass>*>(tsModuleHandler::getModule());}
    tsAbstractFileModule<FileIO, derivedClass>*              module()         {return static_cast<tsAbstractFileModule<FileIO, derivedClass>*>(tsModuleHandler::getModule());}
};

//*****************************************************************

//TODO Manager Class
class tsFileNameManager : public tsTemplateManager<tsAbstractFileModule, std::string>
{
public:
    explicit tsFileNameManager(const std::string& name);
    virtual ~tsFileNameManager() {}
    virtual bool filter(void* x);
    virtual void modifyValue(std::string* value) = 0;
    
private:
    bool input_;
    bool ouput_;
};

} //gmx namespace


