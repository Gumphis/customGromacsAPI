//***********************************************
// Author: Martin Kulke <martin.kulke@uni-greifswald.de>
// File:   tsOption.cpp
// class:  tsOption
//  
// Created on August 10, 2017, 3:58 PM
//
//***********************************************

#include <vector>

#include "tsOption.h"
#include "gromacs/utility/stringutil.h"
#include "gromacs/utility/gmxassert.h"
#include "gromacs/utility/exceptions.h"
#include "gromacs/utility/arrayref.h"
#include "tsUtility.h"

namespace gmx {

//*****************************************************************
//Impl class

class tsOptions::Impl
{
public:
    //! Sets the name and title.
    Impl(const std::string& name, const std::string& title);
    ~Impl();

    tsOptions* findSubSection(const std::string& name) const;               //search subsection
    tsAbstractOption* findOption(const std::string& tag) const;             //search option
    tsAbstractManager* findManager(const std::string& name) const;

    std::string                     name_;              //Name for the Options object.
    std::string                     title_;             //Description title for the Options object.
    std::string                     description_;       //Full description for the Options object.
    std::vector<tsAbstractManager*> managers_;          //the option managers
    std::vector<tsOptions*>         subSections_;       //the Subsections
    std::vector<tsAbstractOption*>  options_;           //the options
    std::vector<tsOptionInfo*>      optionInfos_;       //the option infos;
    tsOptions*                      parent_;
};

tsOptions::Impl::Impl(const std::string& name, const std::string& title)
    : name_(name), title_(title), parent_(NULL)
{
}

tsOptions::Impl::~Impl()
{
}

tsOptions* tsOptions::Impl::findSubSection(const std::string& name) const
{
    for(int i = 0; i < subSections_.size(); ++i)
        if(!subSections_[i]->getName().compare(name))
            return subSections_[i];
    return NULL;
}

tsAbstractOption* tsOptions::Impl::findOption(const std::string& tag) const
{
    for(int i = 0; i < options_.size(); ++i)
        if(options_[i]->compare(tag))
            return options_[i];
    return NULL;
}

tsAbstractManager* tsOptions::Impl::findManager(const std::string& name) const
{
    for(int i = 0; i < managers_.size(); ++i)
        if(!managers_[i]->getName().compare(name))
            return managers_[i];
    return NULL;
}

//*****************************************************************
//tsOption

tsOptions::tsOptions(const std::string& name, const std::string& title)
    : impl_(new Impl(name, title))
{
}

tsOptions::~tsOptions()
{
    for(int i = 0; i < impl_->optionInfos_.size(); ++i)
        TS_SAFE_DELETE(impl_->optionInfos_[i]);
    impl_->optionInfos_.clear();
    for(int i = 0; i < impl_->options_.size(); ++i)
        TS_SAFE_DELETE(impl_->options_[i]);
    impl_->options_.clear();
    for(int i = 0; i < impl_->subSections_.size(); ++i)
        TS_SAFE_DELETE(impl_->subSections_[i]);
    impl_->subSections_.clear();
    for(int i = 0; i < impl_->managers_.size(); ++i)
        TS_SAFE_DELETE(impl_->managers_[i]);
    impl_->managers_;
}

void tsOptions::finish()
{
    for(int i = 0; i < impl_->options_.size(); ++i)
        impl_->options_[i]->finishOption();
    for(int i = 0; i < impl_->subSections_.size(); ++i)
        impl_->subSections_[i]->finish();
}

const std::string& tsOptions::getName() const                                {return impl_->name_;}
const std::string& tsOptions::getTitle() const                               {return impl_->title_;}
const std::string& tsOptions::getDescription() const                         {return impl_->description_;}
void tsOptions::setDescription(const std::string& desc)                      {impl_->description_ = desc;}
void tsOptions::setDescription(const ConstArrayRef<const char*>& descArray)  {impl_->description_ = joinStrings(descArray, "\n");}
void tsOptions::addManager(tsAbstractManager* manager)                  
{
    GMX_RELEASE_ASSERT(impl_->options_.empty(),"Can only add a manager before options");
    GMX_RELEASE_ASSERT(!impl_->findManager(manager->getName()), "Cannot add the same Manager more then once");
    impl_->managers_.push_back(manager);
}
tsOptions* tsOptions::addSubSection(const std::string& name, const std::string& title)
{
    GMX_RELEASE_ASSERT(!impl_->findSubSection(name),"Duplicate subsection name");
    impl_->subSections_.push_back(new tsOptions(name, title));
    impl_->subSections_.back()->impl_->parent_ = this;
    return *impl_->subSections_.end();
}
tsOptionInfo* tsOptions::addOption(tsAbstractOption* option)
{
    if(impl_->findOption(option->tag_)) GMX_THROW(APIError("Duplicate option: " + option->tag_));
    impl_->options_.push_back(option);
    tsOptionInfo* info = option->createInfo();
    impl_->optionInfos_.push_back(info);
    return info;
}
tsOptions::iterator tsOptions::start() {return tsOptionIterator(this, this, 0);}
tsOptions::iterator tsOptions::end() {return tsOptionIterator(this, this, impl_->options_.size());}

tsOptions::tsOptionAssigner::tsOptionAssigner(tsOptions* options, tsOptions* cur, int index)
    : options_(options), curOptions_(cur), optionIndex(index), bAssigning(false), count(0)
{
}

tsOptions::tsOptionAssigner::tsOptionAssigner(tsOptionIterator& iter)
    : options_(iter.baseOptions()), curOptions_(iter.curOptions()), optionIndex(iter.curIndex()), 
        bAssigning(false), count(0)
{
}

tsOptions::tsOptionAssigner::~tsOptionAssigner()
{
}

tsOptions::tsOptionAssigner& tsOptions::tsOptionAssigner::operator=(tsOptionIterator& iter)
{
    options_=iter.baseOptions();
    curOptions_=iter.curOptions();
    optionIndex=iter.curIndex();
    bAssigning=false;
}

tsResult tsOptions::tsOptionAssigner::searchOption(const std::string& tag)
{
    iterator end = options_->end();
    for(iterator it = options_->start(); it != end; ++it)
        if(it->compare(tag))
        {
            *this=it;
            return TS_OK;
        }
    
    return TS_NOT_FOUND;
}

tsResult tsOptions::tsOptionAssigner::setValueFromString(const std::string& str)
{
    if(!bAssigning)
        return TS_ERROR;
    
    curOptions_->impl_->options_[optionIndex]->setOptionFromString(str);
    count++;
    if(curOptions_->impl_->options_[optionIndex]->maxValueCount_ >= count)
        return finish();
    
    return TS_OK;
}

tsResult tsOptions::tsOptionAssigner::start()
{
    if(!bAssigning)
    {
        curOptions_->impl_->options_[optionIndex]->setFlag(eftsOption_Set);
        if(curOptions_->impl_->options_[optionIndex]->maxValueCount_ > 0) bAssigning = true;
        count = 0;
        return TS_OK;
    }
    
    return TS_ERROR;
}

const std::string& tsOptions::tsOptionAssigner::getCurrentTag() const {return curOptions_->impl_->options_[optionIndex]->tag_;}
int tsOptions::tsOptionAssigner::getMinValueNumber() const {return curOptions_->impl_->options_[optionIndex]->minValueCount_;}
int tsOptions::tsOptionAssigner::getMaxValueNumber() const {return curOptions_->impl_->options_[optionIndex]->maxValueCount_;}
int tsOptions::tsOptionAssigner::getCurrentValueNumber() const {return count;}

bool tsOptions::tsOptionAssigner::hasStarted() {return bAssigning;}

tsResult tsOptions::tsOptionAssigner::finish()
{
    bAssigning = false;
    if(curOptions_->impl_->options_[optionIndex]->minValueCount_ > count)
        return TS_ERROR;
    
    return TS_OK;
}

tsAbstractOption*                   tsOptions::tsOptionAssigner::operator->()              {return curOptions_->impl_->options_[optionIndex];}
tsAbstractOption&                   tsOptions::tsOptionAssigner::operator*()               {return *curOptions_->impl_->options_[optionIndex];}
const tsAbstractOption&             tsOptions::tsOptionAssigner::operator*() const         {return *curOptions_->impl_->options_[optionIndex];}

//*****************************************************************
//tsOptions::tsOptionIterator

tsOptions::tsOptionIterator::tsOptionIterator(tsOptions* option, tsOptions* cur, int index)
    : options_(option), curOptions_(cur), optionIndex(index)
{
}

tsOptions::tsOptionIterator::~tsOptionIterator()
{
}

tsOptions::tsOptionIterator& tsOptions::tsOptionIterator::operator=(tsOptions* options)
{
    curOptions_=options;
    optionIndex= (options->impl_->options_.empty() ? -1 : 0);
    return *this;
}

tsOptions::tsOptionIterator::operator bool() const                                     {return optionIndex > -1;}
bool tsOptions::tsOptionIterator::operator==(const tsOptionIterator& iterator) const   {return optionIndex==iterator.curIndex() && curOptions_==iterator.curOptions();}
bool tsOptions::tsOptionIterator::operator!=(const tsOptionIterator& iterator) const   {return optionIndex!=iterator.curIndex() || curOptions_!=iterator.curOptions();}

tsOptions::tsOptionIterator& tsOptions::tsOptionIterator::operator+=(int movement)
{
    iterator end = options_->end();
    for(int i = 0; i < movement && !(optionIndex == end.curIndex() && curOptions_==end.curOptions()); ++i)
    {
        optionIndex+=1;
        if(optionIndex>=curOptions_->impl_->options_.size())
        {
            if(!curOptions_->impl_->subSections_.empty() && !curOptions_->impl_->subSections_[0]->impl_->options_.empty())
            {
                curOptions_=curOptions_->impl_->subSections_[0];
                optionIndex=0;
            } else
                getNextParentChild();
        }
    }
    return *this;
}

tsOptions::tsOptionIterator&        tsOptions::tsOptionIterator::operator++()              {*this+=1; return *this;}
tsOptions::tsOptionIterator         tsOptions::tsOptionIterator::operator++(int)           {tsOptionIterator temp(*this); temp+=1; return temp;}
tsOptions::tsOptionIterator         tsOptions::tsOptionIterator::operator+(int movement)   {tsOptionIterator temp(*this); temp+=movement; return temp;}
const tsAbstractOption*             tsOptions::tsOptionIterator::base() const              {return curOptions_->impl_->options_[optionIndex];}
const tsOptions*                    tsOptions::tsOptionIterator::baseOptions() const       {return options_;}
tsOptions*                          tsOptions::tsOptionIterator::baseOptions()             {return options_;}
const tsOptions*                    tsOptions::tsOptionIterator::curOptions() const        {return curOptions_;}
tsOptions*                          tsOptions::tsOptionIterator::curOptions()              {return curOptions_;}
const int                           tsOptions::tsOptionIterator::curIndex() const          {return optionIndex;}
int                                 tsOptions::tsOptionIterator::curIndex()                {return optionIndex;}
tsAbstractOption*                   tsOptions::tsOptionIterator::operator->()              {return curOptions_->impl_->options_[optionIndex];}
tsAbstractOption&                   tsOptions::tsOptionIterator::operator*()               {return *curOptions_->impl_->options_[optionIndex];}
const tsAbstractOption&             tsOptions::tsOptionIterator::operator*() const         {return *curOptions_->impl_->options_[optionIndex];}

void tsOptions::tsOptionIterator::getNextParentChild()
{
    if(options_==curOptions_)
    {
        optionIndex=curOptions_->impl_->options_.size();
        return;
    }
    tsOptions* temp = curOptions_;
    curOptions_=curOptions_->impl_->parent_;
    if(curOptions_->impl_->subSections_.back() == temp)
        getNextParentChild();
    else
    {
        for(int i = 0; i < curOptions_->impl_->subSections_.size(); ++i)
            if(curOptions_->impl_->subSections_[i]==temp)
                for(int j = i+1; j < curOptions_->impl_->subSections_.size(); ++i)
                {
                    if(!curOptions_->impl_->subSections_[j]->impl_->options_.empty())
                    {
                        optionIndex=0;
                        curOptions_=curOptions_->impl_->subSections_[j];
                        return;
                    }
                    getNextParentChild();
                }
        GMX_ASSERT(true, "Iteration function getNextParentChild failed" );
    }
}

//*****************************************************************
//tsAbstractOption

tsAbstractOption::tsAbstractOption(const std::string& tag)
    : minValueCount_(1), maxValueCount_(1), tag_(tag), descr_(""), flags_()
{
}

tsAbstractOption::~tsAbstractOption()
{
}

void tsAbstractOption::setDescription(const std::string& descr)     { descr_ = descr; }              
void tsAbstractOption::setFlag(tsOptionFlag flag, bool bSet)        { flags_.set(flag, bSet); }                  
bool tsAbstractOption::hasFlag(tsOptionFlag flag) const             { return flags_.test(flag); }     
void tsAbstractOption::clearFlag(tsOptionFlag flag)                 { flags_.clear(flag); }               
void tsAbstractOption::setValueCount(int count)                     { minValueCount_ = count; maxValueCount_ = count;}
const std::string& tsAbstractOption::getDescription() const         { return descr_;}
const std::string& tsAbstractOption::getTag() const                 { return tag_;}

//*****************************************************************
//tsOptionInfo

tsOptionInfo::tsOptionInfo(tsAbstractOption *option)
    : option_(option)
{
}

tsOptionInfo::~tsOptionInfo()
{
}

bool tsOptionInfo::isSet() const                                    {return option_->hasFlag(eftsOption_Set);}
bool tsOptionInfo::isHidden() const                                 {return option_->hasFlag(eftsOption_Hidden);}
bool tsOptionInfo::isRequired() const                               {return option_->hasFlag(eftsOption_Required);}
int tsOptionInfo::minValueCount() const                             {return option_->minValueCount_;}
int tsOptionInfo::maxValueCount() const                             {return option_->maxValueCount_;}
const std::string& tsOptionInfo::getTag() const                     {return option_->tag_;}
const std::string& tsOptionInfo::type() const                       {return option_->type();}
std::string tsOptionInfo::formatTag() const                         {return option_->formatTag();}               
std::string tsOptionInfo::formatValue() const                       {return option_->formatValue();}
std::string tsOptionInfo::formatDefaultValue() const                {return option_->formatDefaultValue();}
std::string tsOptionInfo::formatDescription() const                 {return option_->formatDescription();}
bool tsOptionInfo::isInput() const                                  {return option_->hasFlag(eftsOption_Input);}
bool tsOptionInfo::isOutput() const                                 {return option_->hasFlag(eftsOption_Output);}
bool tsOptionInfo::isInputOutput() const                            {return option_->hasFlag(eftsOption_Input) && option_->hasFlag(eftsOption_Output);}
bool tsOptionInfo::isExplicitInput() const                          {return option_->hasFlag(eftsOption_Input) && !option_->hasFlag(eftsOption_Output);}
bool tsOptionInfo::isExplicitOutput() const                         {return !option_->hasFlag(eftsOption_Input) && option_->hasFlag(eftsOption_Output);}
bool tsOptionInfo::hasDefaultValue() const                          {return option_->hasFlag(eftsOption_HasDefaultValue);}
bool tsOptionInfo::hasDefaultValueIfSet() const                     {return option_->hasFlag(eftsOption_HasDefaultValueIfSet);}


int tsOptionInfo::valueCount() const                                {return option_->valueCount();}
tsAbstractOption*       tsOptionInfo::getOption()                   {return option_;}
const tsAbstractOption* tsOptionInfo::getOption() const             {return option_;}

//*****************************************************************
//tsAbstractManager


tsAbstractManager::tsAbstractManager(const std::string& name) : name_(name)
{
}

tsAbstractManager::~tsAbstractManager()
{
}

const std::string& tsAbstractManager::getName() const {return name_;}
const std::string& tsAbstractManager::getHelpTitle() const {return helpTitle_;}
void tsAbstractManager::setHelpTitle(const std::string& title) {helpTitle_ = title;}

//*****************************************************************
//tsIntegerOption

tsIntegerOption::tsIntegerOption(const std::string& tag)
    : tsTemplateOption(tag)
{
    minValueCount_=1;
    minValueCount_=1;
}

tsIntegerOption::~tsIntegerOption()
{
}

std::string tsIntegerOption::formatValue() const                    {return "<int>";}
std::string tsIntegerOption::formatDefaultValue() const             {return getDefaultValue() ? std::to_string(*getDefaultValue()) : "";}
void tsIntegerOption::setOptionFromString(const std::string& str)   {setOption(std::stoi(str));}

//*****************************************************************
//tsIntegerOption

tsRealOption::tsRealOption(const std::string& tag)
    : tsTemplateOption(tag)
{
    minValueCount_=1;
    minValueCount_=1;
}

tsRealOption::~tsRealOption()
{
}

std::string tsRealOption::formatValue() const                   {return "<real>";}
std::string tsRealOption::formatDefaultValue() const            {return getDefaultValue() ? std::to_string(*getDefaultValue()) : "";}
void tsRealOption::setOptionFromString(const std::string& str)  {setOption(std::stof(str));}

//*****************************************************************
//tsBooleanOption

tsBooleanOption::tsBooleanOption(const std::string& tag)
    : tsTemplateOption(tag)
{
    minValueCount_=0;
    minValueCount_=0;
}

tsBooleanOption::~tsBooleanOption()
{
}

std::string tsBooleanOption::formatValue() const                    {return "";}
std::string tsBooleanOption::formatTag() const                      {return "-[no]" + getTag();}
std::string tsBooleanOption::formatDefaultValue() const             {return getDefaultValue() ? (*getDefaultValue() ? "yes" : "no") : "";}
void tsBooleanOption::setOptionFromString(const std::string& str)   {}
bool tsBooleanOption::compare(const std::string& str)
{
    if(!getTag().compare(str)) {setOption(true); return true;}
    if(!std::string("no" + getTag()).compare(str)) {setOption(false); return true;}
    return false;
}

//*****************************************************************
//tsStringOption

tsStringOption::tsStringOption(const std::string& tag)
    : tsTemplateOption(tag)
{
    minValueCount_=1;
    minValueCount_=1;
}

tsStringOption::~tsStringOption()
{
}

std::string tsStringOption::formatValue() const                     {return "<string>";}
std::string tsStringOption::formatDefaultValue() const              {return getDefaultValue() ? *getDefaultValue() : "";}
void tsStringOption::setOptionFromString(const std::string& str)    {setOption(str);}

} //gmx namespace