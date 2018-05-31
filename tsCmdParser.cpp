//***********************************************
// Author: Martin Kulke <martin.kulke@uni-greifswald.de>
// File:   tsCmdParser.cpp
// class:  tsCmdParser
//  
// Created on September 14, 2017, 4:03 PM
//
//***********************************************

#include "tsCmdParser.h"
#include "gromacs/utility/exceptions.h"
#include "gromacs/utility/basedefinitions.h"

namespace gmx 
{

tsCmdParser::tsCmdParser(tsOptions* options)
    : options_(options)
{
}

tsCmdParser::~tsCmdParser()
{
}

void tsCmdParser::parse(int *argc, char *argv[])
{
    tsOptions::assigner ass(options_);
    
    for (int i = 1; i != *argc; ++i)
    {
        std::string tag = arg_to_tag(argv[i]);
        if(tag.empty())
        {
            if(!ass.hasStarted()) 
                if(!bSkipUnknown_) 
                    GMX_THROW(InvalidInputError("In Command-Line - cannot assign the value \"" + tag + "\" to an option"));
                else
                    continue;
            //Differentiate the error messages more
            if(ass.setValueFromString(argv[i]) != TS_OK) 
                GMX_THROW(InvalidInputError("In Command-Line Option \"" + ass.getCurrentTag() + "\" - cannot assign the value \"" + argv[i] + "\""));
        } else
        {
            std::string temp = ass.hasStarted() ? ass.getCurrentTag() : "";
            if(ass.searchOption(tag) != TS_OK)
                if(!bSkipUnknown_)
                    GMX_THROW(InvalidInputError("Unknown command-line option " + tag));
            //TODO: Try to reorder this, else the getCurrentTag is not actual the right tag
            if(ass.start() != TS_OK)
                GMX_THROW(InvalidInputError("In Command-Line Option \"" + temp + "\" - expects " + std::to_string(ass.getMinValueNumber()) + "values but only got " + std::to_string(ass.getCurrentValueNumber())));
        }
    }
}

tsCmdParser& tsCmdParser::skipUnknown(bool bSkip) {bSkipUnknown_=bSkip;return *this;}

std::string tsCmdParser::arg_to_tag(const char* arg)
{
    if (arg[0] != '-' || arg[1] == '\0' || (arg[1] == '-' && arg[2] == '\0')) return "";    // Lone '-' or '--' is not an option.
    if (arg[1] == '-') return arg + 2;                                                      // Something starting with '--' is always an option.
    return arg + 1;
}

} //gmx namespace