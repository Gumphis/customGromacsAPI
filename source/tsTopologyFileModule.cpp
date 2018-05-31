/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsTopologyFileModule.cpp
 * Author: kulkem
 * 
 * Created on August 1, 2017, 5:38 PM
 */

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include "gromacs/fileio/filenm.h"
#include "gromacs/fileio/tpxio.h"
#include "gromacs/fileio/confio.h"
#include "gromacs/utility/exceptions.h"
#include "tsTopologyFileModule.h"
#include "tsUtility.h"

namespace gmx
{

tsAbstractTopologyFileIO::tsAbstractTopologyFileIO() {}
tsAbstractTopologyFileIO::~tsAbstractTopologyFileIO() {}

gmx_bool tsAbstractTopologyFileIO::read(const std::string& fileName, tsSystem* system)  {return false;}
gmx_bool tsAbstractTopologyFileIO::write(const std::string& fileName, tsSystem* system) {return false;}

//*****************************************************************
    
tsTopologyFileModule::tsTopologyFileModule(tsSystem& system, const std::string& name, const std::string& description)
    : tsAbstractFileModule(name, description), system_(system)
{
    ioList_.push_back(std::pair<std::shared_ptr<tsAbstractTopologyFileIO>, std::string>(std::make_shared<tprFileIO>(), "tpr"));
    ioList_.push_back(std::pair<std::shared_ptr<tsAbstractTopologyFileIO>, std::string>(std::make_shared<confFileIO>(), "pdb"));
    ioList_.push_back(std::pair<std::shared_ptr<tsAbstractTopologyFileIO>, std::string>(std::make_shared<confFileIO>(), "gro"));
    ioList_.push_back(std::pair<std::shared_ptr<tsAbstractTopologyFileIO>, std::string>(std::make_shared<confFileIO>(), "g96"));
    ioList_.push_back(std::pair<std::shared_ptr<tsAbstractTopologyFileIO>, std::string>(std::make_shared<confFileIO>(), "brk"));
    ioList_.push_back(std::pair<std::shared_ptr<tsAbstractTopologyFileIO>, std::string>(std::make_shared<confFileIO>(), "ent"));
    defaultBasename("topol").defaultType("tpr").Input();
}

tsTopologyFileModule::~tsTopologyFileModule()
{
}

int tsTopologyFileModule::run() {return 0;}

void tsTopologyFileModule::finish()
{
}

void tsTopologyFileModule::initialize()
{
}

void tsTopologyFileModule::initOptions(tsOptions* options)
{
    store(&filename_);
    options->addOption(*this);
}

void tsTopologyFileModule::finishOptions(tsOptions* options)
{
}
   
std::string tsTopologyFileModule::type() {return "topology_file_module";}

void tsTopologyFileModule::writeFile() {writeFile(filename_);}
void tsTopologyFileModule::readFile() {readFile(filename_);}

void tsTopologyFileModule::writeFile(const std::string& fileName)
{
    if(!ioList_[getFileIO(fileName)].first->write(fileName,&system_)) 
        GMX_THROW(FileIOError((boost::format("could not write file %1%") % fileName).str()));
}

void tsTopologyFileModule::readFile(const std::string& fileName)
{   
    if(!ioList_[getFileIO(fileName)].first->read(fileName ,&system_))
        GMX_THROW(FileIOError((boost::format("could not read file %1%") % fileName).str()));
}

//*****************************************************************

tprFileIO::tprFileIO()
    :   tsAbstractTopologyFileIO()
{
}

tprFileIO::~tprFileIO()
{
}

gmx_bool tprFileIO::read(const std::string& fileName, tsSystem* system)
{
    t_tpxheader      header;
    int              natoms, version, generation;
    gmx_mtop_t*      mtop;
    
    read_tpxheader(fileName.c_str(), &header, TRUE, &version, &generation);
    mtop = new gmx_mtop_t;
    system->getCoordinates().resize(natoms);
    system->getVelocities().resize(natoms);
    system->ePBC() = read_tpx(fileName.c_str(), NULL, system->getBox(), &natoms,
                     (rvec*)(&(system->getCoordinates()[0])), (rvec*)(&(system->getVelocities()[0])), NULL, mtop);
    system->topology() = *mtop;
    done_mtop(mtop, true);
    TS_SAFE_DELETE(mtop);
    system->fullTopologyLoaded();
    
    return true;
}

gmx_bool tprFileIO::write(const std::string& fileName, tsSystem* system)
{
    return false;
}

//*****************************************************************

confFileIO::confFileIO()
    :   tsAbstractTopologyFileIO()
{
}

confFileIO::~confFileIO()
{
}

gmx_bool confFileIO::read(const std::string& fileName, tsSystem* system)
{
    int natoms, ePBC;
    get_stx_coordnum(fileName.c_str(), &natoms);
    char  title[STRLEN];
    system->getCoordinates().resize(natoms);
    system->getVelocities().resize(natoms);
    t_atoms* atoms = new t_atoms[natoms];
    
    read_stx_conf(fileName.c_str(), title, atoms, 
            (rvec*)(&(system->getCoordinates()[0])), (rvec*)(&(system->getVelocities()[0])), 
            &ePBC, system->getBox());
    
    system->topology().clear();
    int oldResIndex = atoms->atom[0].resind;
    int oldChainNum = atoms->resinfo[oldResIndex].chainnum;
    int resCounter = 1;
    
    int molIndex = system->topology().addMolecule("",atoms->resinfo[oldResIndex].chainid, oldChainNum);
    int resIndex = system->topology().addResidue(molIndex, *(atoms->resinfo[oldResIndex].name), resCounter++, atoms->resinfo[oldResIndex].nr);
    for(int at = 0; at < atoms->nr; ++at)
    {
        if(atoms->atom[at].resind != oldResIndex)
        {
            oldResIndex = atoms->atom[at].resind;
            if(atoms->resinfo[oldResIndex].chainnum != oldChainNum)
            {
                oldChainNum = atoms->resinfo[oldResIndex].chainnum;
                molIndex = system->topology().addMolecule("",atoms->resinfo[oldResIndex].chainid, oldChainNum);
            }
            resIndex = system->topology().addResidue(molIndex, *(atoms->resinfo[oldResIndex].name), resCounter++, atoms->resinfo[oldResIndex].nr);
        }
        system->topology().addAtom(molIndex, resIndex, "", atoms->atom[at].atomnumber, atoms->atom[at].m,
                                    atoms->atom[at].q, atoms->atom[at].mB, atoms->atom[at].qB, atoms->atom[at].elem, 
                                    NULL, NULL);
    }
    system->topology().Update();
    
    TS_SAFE_DELETE_ARRAY(atoms);
    system->topologyLoaded();
    
    return true;
}

gmx_bool confFileIO::write(const std::string& fileName, tsSystem* system)
{
    return false;
}

}