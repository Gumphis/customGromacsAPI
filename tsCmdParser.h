//***********************************************
// Author: Martin Kulke <martin.kulke@uni-greifswald.de>
// File:   tsCmdParser.h
// class:  tsCmdParser
//  
// Created on September 14, 2017, 4:03 PM
//
//***********************************************

#pragma once

#include "tsOption.h"

namespace gmx
{

class tsCmdParser
{
public:
    explicit tsCmdParser(tsOptions* options);
    virtual ~tsCmdParser();
    
    void parse(int *argc, char *argv[]);
    tsCmdParser& skipUnknown(bool bSkip = true);

private:
    std::string arg_to_tag(const char* arg);
    tsOptions* options_;
    bool bSkipUnknown_;
};

} //namespace gmx