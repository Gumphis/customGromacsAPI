/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsSystem.cpp
 * Author: kulkem
 * 
 * Created on July 31, 2017, 3:40 PM
 */

#include "tsSystem.h"
#include "gromacs/utility/exceptions.h"
#include "tsUtility.h"

namespace gmx
{

class tsSystem::Impl
{
public:
    Impl(std::string name);
    ~Impl();
    
    tsTopology              topol_;     //! The topology structure
    std::vector<tsVector>   x_;         //! Coordinates from the topology
    std::vector<tsVector>   v_;         //! Velocities from the topology
    tsBox                box_;       //! The box loaded from the topology file.
    int                     ePBC_;      //! epbc field
    gmx_bool                bLoaded_;   //! Is the topology loaded
    gmx_bool                bFull_;     //! true if full tpx file was loaded, false otherwise.
};

tsSystem::Impl::Impl(std::string name)
    : topol_(name), bFull_(false), bLoaded_(false), x_(NULL), ePBC_(-1), box_(0)
{
}

tsSystem::Impl::~Impl()
{
}

//*****************************************************************

tsSystem::tsSystem(std::string name)
    : impl_(new Impl(name))
{
}

tsSystem::~tsSystem()
{
}

bool tsSystem::hasTopology() const              {return impl_->bLoaded_;}
bool tsSystem::hasFullTopology() const          {return impl_->bFull_;}        
const tsTopology& tsSystem::topology() const    {return impl_->topol_;}        
int tsSystem::ePBC() const                      {return impl_->ePBC_;}         
const std::vector<tsVector>& tsSystem::getCoordinates() const   {return impl_->x_;}
const std::vector<tsVector>& tsSystem::getVelocities() const    {return impl_->v_;}
const tsBox tsSystem::getBox() const         {return impl_->box_;}

int& tsSystem::ePBC()                           {return impl_->ePBC_;}
tsTopology& tsSystem::topology()                {return impl_->topol_;}
std::vector<tsVector>& tsSystem::getCoordinates()   {return impl_->x_;}
std::vector<tsVector>& tsSystem::getVelocities()    {return impl_->v_;}
tsBox& tsSystem::getBox()                    {return impl_->box_;}
void tsSystem::topologyLoaded()                 {impl_->bLoaded_ = true;}
void tsSystem::fullTopologyLoaded()             {impl_->bLoaded_ = true;impl_->bFull_ = true;}

} //gmx namespace