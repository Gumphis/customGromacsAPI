//***********************************************
// Author: Martin Kulke <martin.kulke@uni-greifswald.de>
// File:   tsHelp.cpp
// class:  tsHelp
//  
// Created on September 7, 2017, 11:45 AM
//
//***********************************************

#include "tsHelp.h"
#include "gromacs/commandline/cmdlinehelpcontext.h"
#include "gromacs/onlinehelp/helpwritercontext.h"
#include "gromacs/commandline/shellcompletions.h"
#include "gromacs/utility/stringutil.h"
#include "gromacs/utility/file.h"

namespace gmx {

//*****************************************************************
//Impl class

class tsHelp::Impl
{
public:
    Impl(tsOptions& options);
    ~Impl();
    
    tsOptions& options_;
    bool bShowDescription;
    int minIntend_;
    int lineLength_;
};

tsHelp::Impl::Impl(tsOptions& options)
    : options_(options), bShowDescription(true), minIntend_(13), lineLength_(78)
{
}

tsHelp::Impl::~Impl()
{
}

//*****************************************************************
    
tsHelp::tsHelp(tsOptions& options)
    : impl_(new Impl(options))
{
}

tsHelp::~tsHelp()
{
}

/*std::string
fileOptionFlagsAsString(const FileNameOptionInfo &option, bool bAbbrev)
{
    std::string type;
    if (!option.isRequired())
    {
        type = bAbbrev ? "Opt." : "Optional";
    }
    if (option.isLibraryFile())
    {
        if (!type.empty())
        {
            type.append(", ");
        }
        type.append(bAbbrev ? "Lib." : "Library");
    }
    return type;
}*/

void tsHelp::writeHelp(const CommandLineHelpContext &context)
{
    const HelpWriterContext &writerContext = context.writerContext();
    File& file = writerContext.outputFile();
    bool showHidden = context.showHidden();

    writerContext.writeTitle("\nSynopsis");

    //TODO: rewrite the wrappersettings class to be more convenient
    std::string synopsis = std::string(context.moduleDisplayName()) + " ";
    TextLineWrapperSettings wrapperSettings;
    wrapperSettings.setFirstLineIndent(0);
    wrapperSettings.setIndent(std::min((int)synopsis.length(), impl_->minIntend_));
    wrapperSettings.setLineLength(impl_->lineLength_);
    
    for(tsOptions::iterator it = impl_->options_.start(); it != impl_->options_.end(); ++it)
    {
        //TODO: check the void type
        tsTemplateOptionInfo<void> info(&(*it));
        if(showHidden || !info.isHidden()) synopsis += "[" + info.formatTag() + (!info.formatValue().empty() ? " " : "") + info.formatValue() + "] ";
    }
    file.writeLine(writerContext.substituteMarkupAndWrapToString(wrapperSettings, synopsis));
    file.writeLine();
    
    if (impl_->bShowDescription && !impl_->options_.getDescription().empty())
    {
        writerContext.writeTitle("Description");
        writerContext.writeTextBlock(impl_->options_.getDescription());
        file.writeLine();
    }
    
    writerContext.writeTitle("Options");
    writerContext.writeOptionListStart();
    for(tsOptions::iterator it = impl_->options_.start(); it != impl_->options_.end(); ++it)
    {
        tsTemplateOptionInfo<void> info(&(*it));
        //TODO: info and (see function above)
        if(showHidden || !info.isHidden()) writerContext.writeOptionItem(info.formatTag(), info.formatValue(), info.formatDefaultValue(), "", info.formatDescription());
    }
    writerContext.writeOptionListEnd();
}

tsHelp& tsHelp::showDescription(bool b) {impl_->bShowDescription = b; return *this;}
tsHelp& tsHelp::setLineLength(int length) {impl_->lineLength_ = length; return *this;}       
tsHelp& tsHelp::setMinimumIntend(int intend) {impl_->minIntend_ = intend; return *this;}

} //gmx namespace