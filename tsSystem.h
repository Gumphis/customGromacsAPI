/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsSystem.h
 * Author: kulkem
 *
 * Created on July 31, 2017, 3:40 PM
 */

#pragma once

#include "gromacs/utility/classhelpers.h"
#include "tsCoordinate.h"
#include "tsTopology.h"
#include <cstring>
#include <vector>

namespace gmx
{

class tsSystem
{
public:
    tsSystem(std::string name);
    ~tsSystem();
    bool hasTopology() const;      //! Returns true if a topology file was loaded.
    bool hasFullTopology() const;  //! Returns true if a full topology file was loaded.
    const tsTopology& topology() const;  //! Returns the loaded topology, or NULL if not loaded.
    int ePBC() const;              //! Returns the ePBC field from the topology.
    const std::vector<tsVector>& getCoordinates() const;
    const std::vector<tsVector>& getVelocities() const;
    const tsBox getBox() const;
    
    int& ePBC();
    tsTopology& topology();
    std::vector<tsVector>& getCoordinates();
    std::vector<tsVector>& getVelocities();
    tsBox& getBox();
    void topologyLoaded();
    void fullTopologyLoaded();
    
    
private:
    class Impl;
    PrivateImplPointer<Impl> impl_;
    
    GMX_DISALLOW_COPY_AND_ASSIGN(tsSystem);
};

} //gmx namespace