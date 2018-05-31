//***********************************************
// Author: Martin Kulke <martin.kulke@uni-greifswald.de>
// File:   tsOption.h
// class:  tsOption
//  
// Created on August 10, 2017, 3:58 PM
//
//***********************************************

#pragma once

#include "gromacs/utility/classhelpers.h"
#include "gromacs/utility/gmxassert.h"
#include "gromacs/utility/flags.h"
#include <boost/iterator/filter_iterator.hpp>
#include "tsUtility.h"

namespace gmx {

enum tsOptionFlag
{
    eftsOption_Set                        = 1<<0, //Option has been set.
    eftsOption_HasDefaultValue            = 1<<1, //The current value of the option is a programmatic default value.
    eftsOption_HasDefaultValueIfSet       = 1<<2, //Option has a defaultValueIfSet() specified.
    eftsOption_Required                   = 1<<3, //Option is required to be set.
    eftsOption_MultipleTimes              = 1<<4, //Option can be specified multiple times.
    eftsOption_Hidden                     = 1<<5, //Option is hidden from standard help.
    eftsOption_Input                      = 1<<6, //Option is of type input
    eftsOption_Output                     = 1<<7, //Option is of type output
    eftsOption_NoTag                      = 1<<8  //This option is specified at the start without a tag
};

class tsOptionInfo;
class tsOptions;
template <typename T> class tsTemplateOptionInfo;
template <typename T> class ConstArrayRef;

//*****************************************************************

class tsAbstractOption
{
public:
    typedef FlagsTemplate<tsOptionFlag> tsOptionFlags;
    
    virtual ~tsAbstractOption();
 
protected:
    explicit tsAbstractOption(const std::string& tag);
    void setDescription(const std::string& descr);      //Sets the description for the option.
    void setFlag(tsOptionFlag flag, bool bSet = true);  //Sets a flag for the option.
    bool hasFlag(tsOptionFlag flag) const;              //Returns true if a flag has been set
    void clearFlag(tsOptionFlag flag);                  //Clears a flag for the option.
    void setValueCount(int count);                      //Sets the number of values
    const std::string& getDescription() const;
    const std::string& getTag() const;
    virtual tsOptionInfo* createInfo() = 0;             //creates an tsOptionInfo object
    virtual std::string type() const = 0;
    virtual std::string formatTag() const = 0;
    virtual std::string formatValue() const = 0;
    virtual std::string formatDescription() const = 0;
    virtual std::string formatDefaultValue() const = 0;
    virtual int valueCount() const = 0;
    virtual const std::type_info& getType() const = 0;
    virtual const void* getOption() const = 0;
    virtual void setOption(void* data) = 0;
    virtual void setOptionFromString(const std::string& str) = 0;
    virtual void finishOption() = 0;
    virtual bool compare(const std::string& str) = 0;

    
    int minValueCount_;                                 //Minimum number of values required for the option.
    int maxValueCount_;                                 //Maximum number of values allowed for the option.
    
private:
    std::string tag_;                                   //tag for the command line
    std::string descr_;                                 //description
    tsOptionFlags flags_;                               //set flags
    
    friend class tsOptionInfo;
    friend class tsOptions;
};

//*****************************************************************

template <typename T, class derivedClass>
class tsTemplateOption : public tsAbstractOption
{
public:
    typedef derivedClass                     MyClass;
    typedef T                                value_type;
    typedef tsTemplateOptionInfo<value_type> InfoType;
    
    virtual     ~tsTemplateOption()                      {if(bDeleteSelf) TS_SAFE_DELETE(storage_)};
    MyClass&    hidden(bool b = true)                    {setFlag(eftsOption_Hidden, b); return me();}
    MyClass&    setDescription(const std::string& descr) {tsAbstractOption::setDescription(descr); return me();}
    MyClass&    required(bool b = true)                  {setFlag(eftsOption_Required, true); return me();}
    MyClass&    store(value_type* pValue)                {storage_=pValue; return me();}
    MyClass&    defaultValue(value_type value)           {setFlag(eftsOption_HasDefaultValue);defValue=value;return me();}
    MyClass&    defaultValueIfSet(value_type value)      {setFlag(eftsOption_HasDefaultValueIfSet);defValueIfSet=value;return me();}
    MyClass&    Input()                                  {setFlag(eftsOption_Input);setFlag(eftsOption_Input, false);return me();}
    MyClass&    Output()                                 {setFlag(eftsOption_Input, false);setFlag(eftsOption_Input);return me();}
    MyClass&    InputOutput()                            {setFlag(eftsOption_Input);setFlag(eftsOption_Input);return me();}
    
protected:
    explicit tsTemplateOption(const std::string& tag) 
        : tsAbstractOption(tag), storage_(NULL), bDeleteSelf(false) 
    {
        setFlag(eftsOption_Input);
    }
    virtual std::string formatValue() const = 0;
    virtual std::string formatDefaultValue() const = 0;
    virtual void setOptionFromString(const std::string& str) = 0;
    
    virtual int valueCount() const                          {return minValueCount_;}
    virtual tsOptionInfo* createInfo()                      {return static_cast<tsOptionInfo*>(new InfoType(static_cast<MyClass*>(this)));}
    virtual std::string type() const                        {return std::string(typeid(value_type).name());}
    virtual const std::type_info& getType() const           {return typeid(value_type);}
    virtual std::string formatTag() const                   {return "-" + getTag();}
    virtual std::string formatDescription() const           {return getDescription();}
    virtual const void* getOption() const                   {return static_cast<const void*>(storage_);}
    virtual void setOption(void* data)                      {*storage_=*(static_cast<value_type*>(data));}
    virtual void setOption(value_type data)                 {if(storage_) *storage_=data;}
    virtual bool compare(const std::string& str)            {return !getTag().compare(str);}
    virtual void finishOption()                                   
    {
        if(hasFlag(eftsOption_Set) && hasFlag(eftsOption_HasDefaultValueIfSet))
        {
            if(storage_)
                *storage_ = defValueIfSet;
            else
            {
                storage_ = new value_type(defValueIfSet);
                bDeleteSelf = true;
            }
        }
        else if(!hasFlag(eftsOption_Set) && hasFlag(eftsOption_HasDefaultValue))
        {
            if(storage_)
                *storage_ = defValue;
            else
            {
                storage_ = new value_type(defValue);
                bDeleteSelf = true;
            }
        }
    }
    const value_type* getDefaultValue() const
    {
        if(hasFlag(eftsOption_HasDefaultValue)) return &defValue;
        if(hasFlag(eftsOption_HasDefaultValueIfSet)) return &defValueIfSet;
        return NULL;
    }
    value_type* getDefaultValue()
    {
        if(hasFlag(eftsOption_HasDefaultValue)) return &defValue;
        if(hasFlag(eftsOption_HasDefaultValueIfSet)) return &defValueIfSet;
        return NULL;
    }
    const value_type* getValue() const                      {return storage_;}
    value_type* getValue()                                  {return storage_;}
    
    MyClass&    me()                                        {return static_cast<MyClass&>(*this);}
    
private:
    value_type* storage_;
    value_type defValue;
    value_type defValueIfSet;
    bool bDeleteSelf;
};

//*****************************************************************
//integer option

class tsIntegerOption : public tsTemplateOption<int, tsIntegerOption>
{
public:
    explicit tsIntegerOption(const std::string& tag);
    virtual ~tsIntegerOption();
    
protected:
    virtual std::string formatValue() const;
    virtual std::string formatDefaultValue() const;
    virtual void setOptionFromString(const std::string& str);
};

//*****************************************************************
//real option

class tsRealOption : public tsTemplateOption<float, tsRealOption>
{
public:
    explicit tsRealOption(const std::string& tag);
    virtual ~tsRealOption();
    
protected:
    virtual std::string formatValue() const;
    virtual std::string formatDefaultValue() const;
    virtual void setOptionFromString(const std::string& str);
};

//*****************************************************************
//boolean option

class tsBooleanOption : public tsTemplateOption<bool, tsBooleanOption>
{
public:
    explicit tsBooleanOption(const std::string& tag);
    virtual ~tsBooleanOption();
    
protected:
    virtual std::string formatValue() const;
    virtual std::string formatTag() const;
    virtual std::string formatDefaultValue() const;
    virtual void setOptionFromString(const std::string& str);
    virtual bool compare(const std::string& str);
};

//*****************************************************************
//string option

class tsStringOption : public tsTemplateOption<std::string, tsStringOption>
{
public:
    explicit tsStringOption(const std::string& tag);
    virtual ~tsStringOption();
    
protected:
    virtual std::string formatValue() const;
    virtual std::string formatDefaultValue() const;
    virtual void setOptionFromString(const std::string& str);
};

//*****************************************************************

class tsOptionInfo
{
public:
    virtual ~tsOptionInfo();

    template <class InfoType>
    bool isType() const                                 //Test for Type
    {
        return toType<InfoType>() != NULL;  
    }
    template <class InfoType>
    InfoType *toType()                                  //transform to Type
    {
        return dynamic_cast<InfoType*>(this);
    }
    template <class InfoType>
    const InfoType *toType() const                      //transform to Type
    {
        return dynamic_cast<const InfoType*>(this);
    }

    bool isSet() const;                                 //Returns true if the option has been set.
    bool isHidden() const;                              //Returns true if the option is a hidden option.
    bool isRequired() const;                            //Returns true if the option is required.
    bool isInput() const;
    bool isOutput() const;
    bool isInputOutput() const;
    bool isExplicitInput() const;
    bool isExplicitOutput() const;
    bool hasDefaultValue() const;
    bool hasDefaultValueIfSet() const;
    int minValueCount() const;                          //Returns the minimum number of values that this option accepts.
    int maxValueCount() const;                          //Returns the maximum number of values that this option accepts.
    const std::string& getTag() const;                  //Returns the name of the option.
    const std::string& type() const;                    //Returns the type of the option as a string.
    std::string formatTag() const;                      //Returns formated tag
    std::string formatValue() const;                    //Returns formated value
    std::string formatDefaultValue() const;             //Returns formated default value
    std::string formatDescription() const;              //Returns formated description
    int valueCount() const;                             //Returns the number of values given for the option.

protected:
    explicit tsOptionInfo(tsAbstractOption *option);

    tsAbstractOption*       getOption();
    const tsAbstractOption* getOption() const;

private:
    tsAbstractOption* option_;

    GMX_DISALLOW_COPY_AND_ASSIGN(tsOptionInfo);
};

//*****************************************************************

template <typename T>
class tsTemplateOptionInfo : public tsOptionInfo
{
public:
    virtual ~tsTemplateOptionInfo() {}
    explicit tsTemplateOptionInfo(tsAbstractOption *option) : tsOptionInfo(option) {}
};

//*****************************************************************

class tsAbstractManager
{
public:
    virtual  ~tsAbstractManager();
    void setHelpTitle(const std::string& title);
    const std::string& getHelpTitle() const;
    const std::string& getName() const;
    virtual void modifyValue(void* value) = 0;
    virtual bool filter(void* option) const = 0;

protected:
    explicit tsAbstractManager(const std::string& name);
    
private:
    std::string name_;
    std::string helpTitle_;
    
    GMX_DISALLOW_COPY_AND_ASSIGN(tsAbstractManager);
};

//*****************************************************************

template <class filterBaseClass, typename value_type>
class tsTemplateManager : public tsAbstractManager
{
public:
    virtual void modifyValue(value_type* value) = 0;
    
    virtual ~tsTemplateManager() {}
    virtual bool filter(void* x) {return dynamic_cast<filterBaseClass*>(x);}
    
protected:
    explicit tsTemplateManager(const std::string& name) : tsAbstractManager(name) {}
    
private:
    virtual void modifyValue(void* value) {modifyValue(static_cast<value_type*>(value));}
};

//*****************************************************************

class tsOptions
{
public:
    class tsOptionIterator
    {
    public:
        explicit tsOptionIterator(tsOptions* options, tsOptions* cur = NULL, int index = -1);
        ~tsOptionIterator();

        tsOptionIterator&                   operator=(tsOptions* options);
        operator                            bool() const;
        bool                                operator==(const tsOptionIterator& iterator) const;
        bool                                operator!=(const tsOptionIterator& iterator) const;
        tsOptionIterator&                   operator+=(int movement);
        tsOptionIterator&                   operator++();
        tsOptionIterator                    operator++(int);
        tsOptionIterator                    operator+(int movement);
        const tsAbstractOption*             base() const;
        const tsOptions*                    baseOptions() const;
        tsOptions*                          baseOptions();
        const tsOptions*                    curOptions() const;
        tsOptions*                          curOptions();
        const int                           curIndex() const;
        int                                 curIndex();
        tsAbstractOption*                   operator->();
        tsAbstractOption&                   operator*();
        const tsAbstractOption&             operator*() const;

    private:
        void                                getNextParentChild();
        tsOptions* options_;
        tsOptions* curOptions_;
        int optionIndex;
    };
    
    class tsOptionAssigner
    {
    public:
        explicit tsOptionAssigner(tsOptions* options, tsOptions* cur = NULL, int index = -1);
        explicit tsOptionAssigner(tsOptionIterator& iter);
        ~tsOptionAssigner();
        
        tsOptionAssigner& operator=(tsOptionIterator& iter);
        tsAbstractOption* operator->();
        tsAbstractOption& operator*();
        const std::string& getCurrentTag() const;
        int getMinValueNumber() const;
        int getMaxValueNumber() const;
        int getCurrentValueNumber() const;
        const tsAbstractOption& operator*() const;
        tsResult searchOption(const std::string& tag);
        tsResult setValueFromString(const std::string& str);
        tsResult start();
        tsResult finish();
        bool     hasStarted();
        
    private:
        tsOptions* options_;
        tsOptions* curOptions_;
        int optionIndex;
        bool bAssigning;
        int count;
    };
    
    typedef tsOptionIterator iterator;
    
    typedef const tsOptionIterator const_iterator;
    typedef tsOptionAssigner assigner;
    
    explicit tsOptions(const std::string& name, const std::string& title);
    ~tsOptions();

    const std::string& getName() const;                                         //Returns the short name of the option collection.
    const std::string& getTitle() const;                                        //Returns the title of the option collection.
    const std::string& getDescription() const;                                  //Returns the full description of the option collection.
    void setDescription(const std::string& desc);                               //sets the description
    void setDescription(const ConstArrayRef<const char*>& descArray);
    tsOptions* addSubSection(const std::string& name, const std::string& title);//adds a subsection and returns pointer to it
    void finish();
    template <class Manager>
    void addManager(const Manager& man)
    {
        //TODO: handle the case of NULL
        addManager(static_cast<tsAbstractManager*>(new Manager(std::move(man))));
    }
    template <class OptionType>
    typename OptionType::InfoType* addOption(const OptionType& option)          //adds a option
    {
        OptionType* new_option = new OptionType(std::move(option));
        tsOptionInfo* info = addOption(static_cast<tsAbstractOption*>(new_option));
        GMX_ASSERT(info->isType<typename OptionType::InfoType>(),
                   "Mismatching option info type declaration and implementation");
        return info->toType<typename OptionType::InfoType>();
    }
    iterator start();
    iterator end();

private:
    void addManager(tsAbstractManager* manager);                                //adds a manager
    tsOptionInfo* addOption(tsAbstractOption* option);                          //adds a option
    class Impl;
    PrivateImplPointer<Impl> impl_;
};

} //namespace gmx