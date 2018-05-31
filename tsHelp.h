//***********************************************
// Author: Martin Kulke <martin.kulke@uni-greifswald.de>
// File:   tsHelp.h
// class:  tsHelp
//  
// Created on September 7, 2017, 11:45 AM
//
//***********************************************

#pragma once

#include "gromacs/commandline/cmdlinehelpcontext.h"
#include "tsOption.h"


namespace gmx {

class tsHelp
{
public:
    //TODO: put the const back after fixing the iterator
    tsHelp(tsOptions& options);
    virtual ~tsHelp();

    void writeHelp(const CommandLineHelpContext &context);
    tsHelp& showDescription(bool b);
    tsHelp& setLineLength(int length);
    tsHelp& setMinimumIntend(int intend);
    
private:
    class Impl;
    PrivateImplPointer<Impl> impl_;
};

} //namespace gmx