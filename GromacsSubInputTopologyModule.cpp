/* 
 * File:   GromacsSubInputTopologyModule.cpp
 * Author: kulkem
 * 
 * Created on May 19, 2016, 11:17 AM
 */

#include "gromacs/topology/topology.h"
#include "gromacs/options.h"
#include "gromacs/options/filenameoptionmanager.h"
#include "gromacs/pbcutil/pbc.h"
#include "GromacsSubInputTopologyModule.h"
#include "gromacs/utility/cstringutil.h"
#include "gromacs/utility/exceptions.h"
#include "gromacs/fileio/tpxio.h"
#include "gromacs/math/vec.h"
#include "gromacs/utility/smalloc.h"

namespace gmx
{
    
void GromacsSubInputTopologyModule::initialize()
{
    if(!settings_.bAllowTop && !settings_.bAllowTop) settings_.bAllowTpr = true;
    if(settings_.bOTfName && !settings_.cOTfName) settings_.bOTfName = false;
}


void GromacsSubInputTopologyModule::initOptions(Options* options)
{
    if (settings_.bOTfName)
        options->addOption(FileNameOption(settings_.cOTfName).filetype(gmx::eftTopology).defaultType(settings_.stFileExtension).inputFile().
        defaultBasename(settings_.cfName.c_str()).description("topology file").store(&settings_.cfName));
    
}


int GromacsSubInputTopologyModule::start()
{
    if(topInfo_.hasTopology())
        return 0;
    char  title[STRLEN];

    snew(topInfo_.top_, 1);
    
    topInfo_.bTop_ = read_tps_conf(settings_.cfName.c_str(), title,topInfo_.top_, &topInfo_.ePBC_, &topInfo_.xtop_, NULL, topInfo_.boxtop_, TRUE);
    
    return 0;
}


void GromacsSubInputTopologyModule::finishOptions(Options* options)
{
    
}


void GromacsSubInputTopologyModule::finish()
{
    
}

GromacsTopologyInformation::GromacsTopologyInformation()
    : top_(NULL), bTop_(false), xtop_(NULL), ePBC_(-1)
{
    clear_mat(boxtop_);
}

GromacsTopologyInformation::~GromacsTopologyInformation()
{
    if (top_)
    {
        free_t_atoms(&top_->atoms, TRUE);
        done_top(top_);
        sfree(top_);
    }
    sfree(xtop_);
}

void GromacsTopologyInformation::getTopologyConf(rvec **x, matrix box) const
{
    if (box)
    {
        copy_mat(const_cast<rvec *>(boxtop_), box);
    }
    if (x)
    {
        if (!xtop_)
        {
            *x = NULL;
            GMX_THROW(APIError("Topology coordinates requested without setting efUseTopX"));
        }
        *x = xtop_;
    }
}

} // name space
