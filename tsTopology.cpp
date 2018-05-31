/* 
 * File:   tsTopology.cpp
 * Author: kulkem
 *
 * Created on July 24, 2017, 4:55 PM
 */

#include "tsTopology.h"
#include "gromacs/utility/exceptions.h"
#include "tsUtility.h"

namespace gmx
{

struct ts_residue_access
{
    ts_molecule* mol;
    ts_residue* res;
};

struct ts_atom_access
{
    ts_molecule* mol;
    ts_residue* res;
    ts_atom* at;
};

class tsTopology::Impl
{
public:
    
    Impl(std::string _name);
    ~Impl();
    
    std::vector<ts_molecule>        mol;                            //the molecules
    std::vector<ts_residue_access>  residue;                        //access to residues
    std::vector<ts_atom_access>     atoms;                          //access to atoms
    std::vector<ts_atomtype>        atomtypes;                      //list of atom types
    ts_ff_params                    ffparams;
    std::string                     name;                           //Name of the topology
    gmx_bool                        needUpdate;
    void Update();
    
    void recalculateLegacy();
};

tsTopology::Impl::Impl(std::string _name)
    : name(_name), needUpdate(false)
{
}

tsTopology::Impl::~Impl()
{
}

void tsTopology::Impl::Update()
{
    for(int m = 0; m < mol.size(); ++m)
        for(int res = 0; res < mol[m].residues.size(); ++res)
        {
            for(int atom = 0; atom < mol[m].residues[res].atoms.size(); ++atom)
            {
                ts_atom_access atacc;
                atacc.at = &(mol[m].residues[res].atoms[atom]);
                atacc.res = &(mol[m].residues[res]);
                atacc.mol = &(mol[m]);
                atoms.push_back(atacc);
            }
            ts_residue_access resacc;
            resacc.res = &(mol[m].residues[res]);
            resacc.mol = &(mol[m]);
            residue.push_back(resacc);
        }
    
    needUpdate = false;
}

//*****************************************************************

tsTopology::tsTopology(std::string name)
    : impl_(new Impl(name))
{
}

tsTopology::~tsTopology()
{
}

std::string           tsTopology::getName() const                               {return impl_->name;}
int                   tsTopology::numMol() const                                {return impl_->mol.size();}
const ts_molecule&    tsTopology::getMol(int molid) const                       {return impl_->mol[molid];}
const ts_molecule&    tsTopology::getMolByResidue(int resid) const              {return impl_->needUpdate ? ts_molecule() : *(impl_->residue[resid].mol);}
const ts_molecule&    tsTopology::getMolByAtom(int id) const                    {return impl_->needUpdate ? ts_molecule() : *(impl_->atoms[id].mol);}
int                   tsTopology::numResidue() const                            {return impl_->residue.size();}
const ts_residue&     tsTopology::getResidue(int resid) const                   {return impl_->needUpdate ? ts_residue() : *(impl_->residue[resid].res);}
const ts_residue&     tsTopology::getResidueByAtom(int id) const                {return impl_->needUpdate ? ts_residue() : *(impl_->atoms[id].res);}
int                   tsTopology::numAtoms() const                              {return impl_->atoms.size();}
const ts_atom&        tsTopology::getAtom(int id) const                         {return impl_->needUpdate ? ts_atom() : *(impl_->atoms[id].at);}
int                   tsTopology::numAtomTypes() const                          {return impl_->atomtypes.size();}
const ts_atomtype&    tsTopology::getAtomType(int atid) const                   {return impl_->atomtypes[atid];}
const ts_ff_params&   tsTopology::getFFParams() const                           {return impl_->ffparams;}

void tsTopology::clear()
{
    impl_->atoms.clear();
    impl_->atomtypes.clear();
    impl_->ffparams.cmap_grid.grids.clear();
    impl_->ffparams.functype.clear();
    impl_->ffparams.ilist.clear();
    impl_->ffparams.iparams.clear();
    impl_->mol.clear();
    impl_->needUpdate = false;
    impl_->residue.clear();
}

int tsTopology::addMolecule(std::string name, char chainid, int chainnum)
{
    impl_->needUpdate=true;
    impl_->mol.push_back(ts_molecule());
    int ret = impl_->mol.size()-1;
    impl_->mol[ret].name = name;
    impl_->mol[ret].chainid = chainid;
    impl_->mol[ret].chainnum = chainnum;
    
    return ret;
}

int tsTopology::addResidue(int mol, std::string name, int residue, int resid)
{
    impl_->needUpdate=true;
    impl_->mol[mol].residues.push_back(ts_residue());
    int ret = impl_->mol[mol].residues.size()-1;
    impl_->mol[mol].residues[ret].name = name;
    impl_->mol[mol].residues[ret].residue = residue;
    impl_->mol[mol].residues[ret].resid = resid;
    
    return ret;
}

int tsTopology::addAtom(int mol, int res, std::string atomName, int atomnumber, 
                        real m, real q, real mB, real qB, std::string elem,
                        ts_atomtype* atomType, ts_atomtype* atomTypeB)
{
    impl_->needUpdate=true;
    impl_->mol[mol].residues[res].atoms.push_back(ts_atom());
    int ret = impl_->mol[mol].residues[res].atoms.size()-1;
    impl_->mol[mol].residues[res].atoms[ret].atomName = atomName;
    impl_->mol[mol].residues[res].atoms[ret].atomType = atomType;
    impl_->mol[mol].residues[res].atoms[ret].atomTypeB = atomTypeB;
    impl_->mol[mol].residues[res].atoms[ret].atomnumber = atomnumber;
    impl_->mol[mol].residues[res].atoms[ret].elem = elem;
    impl_->mol[mol].residues[res].atoms[ret].m = m;
    impl_->mol[mol].residues[res].atoms[ret].mB = mB;
    impl_->mol[mol].residues[res].atoms[ret].q = q;
    impl_->mol[mol].residues[res].atoms[ret].qB = qB;
    
    return ret;
}

ts_molecule& tsTopology::getMol(int molid)                      {return impl_->mol[molid];}
ts_residue& tsTopology::getResidue(int molid, int resid)        {return impl_->mol[molid].residues[resid];}
ts_atom& tsTopology::getAtom(int molid, int resid, int atomid)  {return impl_->mol[molid].residues[resid].atoms[atomid];}
void tsTopology::Update()                                       {impl_->Update();}

tsTopology& tsTopology::operator = (const gmx_mtop_t& topol)
{
    clear();
    
    impl_->name = *(topol.name);
    
    //force field
    //*******************************************
    impl_->ffparams.cmap_grid.gridSpacing = topol.ffparams.cmap_grid.grid_spacing;
    for(int i = 0; i < topol.ffparams.cmap_grid.ngrid; ++i)
    {
        impl_->ffparams.cmap_grid.grids.push_back(std::vector<real>());
        for(int j = 0; j < 4*impl_->ffparams.cmap_grid.gridSpacing*impl_->ffparams.cmap_grid.gridSpacing; ++j)
            impl_->ffparams.cmap_grid.grids[i].push_back(topol.ffparams.cmap_grid.cmapdata[i].cmap[j]);
    }
    impl_->ffparams.fudgeQQ = topol.ffparams.fudgeQQ;
    impl_->ffparams.reppow = topol.ffparams.reppow;
    
    for(int i = 0; i < topol.ffparams.ntypes; ++i)
    {
        impl_->ffparams.functype.push_back(topol.ffparams.functype[i]);
        impl_->ffparams.iparams.push_back(topol.ffparams.iparams[i]);
    }
    
    //atomtypes
    //*******************************************

    for(int i = 0; i < topol.atomtypes.nr; ++i)
    {
        ts_atomtype at;
        at.S_hct = topol.atomtypes.S_hct[i];
        at.atomnumber = topol.atomtypes.atomnumber[i];
        at.gb_radius = topol.atomtypes.gb_radius[i];
        at.name = "";
        at.radius = topol.atomtypes.radius[i];
        at.surftens = topol.atomtypes.surftens[i];
        at.vol = topol.atomtypes.vol[i];
        impl_->atomtypes.push_back(at);
    }
    
    //atoms, residues, molecules, exclusions
    //*******************************************
    
    int natoms = 0;
    int chainNum = 0;
    int residueId = 0;
    impl_->ffparams.ilist.resize(F_NRE);
    for (int mb = 0; mb < topol.nmolblock; mb++)
    {
        
        gmx_molblock_t* molb = &topol.molblock[mb];
        gmx_moltype_t*  molt = &topol.moltype[molb->type];
        for(int i = 0; i < topol.molblock[mb].nmol; ++i)
        {
            ts_molecule molecule;
            molecule.name = *(molt->name);
            molecule.excls.push_back(std::vector<atom_id>());
            int index = molecule.excls.size() - 1;
            for(int curIndex = 0; curIndex < molt->excls.nr; ++curIndex)
                for(int at = molt->excls.index[curIndex]; at < molt->excls.index[curIndex+1]; ++at)
                    molecule.excls[index].push_back(molt->excls.a[at]+natoms);
            molecule.chainnum = chainNum++;
            //TODO: check if for every molecule really a different t_res struct exists
            molecule.chainid = molt->atoms.resinfo[0].chainid;
            ts_residue residue;
            residue.bb_entry = NULL;
            residue.name = *(molt->atoms.resinfo[molt->atoms.atom[0].resind].name);
            residue.resid = molt->atoms.resinfo[molt->atoms.atom[0].resind].nr;
            residue.residue = residueId++;
            int oldRes = residue.resid;
            for(int at = 0; at < molt->atoms.nr; ++at)
            {
                ts_atom atom;
                atom.atomName = *(molt->atoms.atomname[at]);
                atom.atomType = &(impl_->atomtypes[molt->atoms.atom[at].type]);
                atom.atomTypeB = &(impl_->atomtypes[molt->atoms.atom[at].typeB]);
                atom.atomnumber = molt->atoms.atom[at].atomnumber;
                atom.elem = molt->atoms.atom[at].elem;
                atom.m = molt->atoms.atom[at].m;
                atom.mB = molt->atoms.atom[at].mB;
                atom.q = molt->atoms.atom[at].q;
                atom.qB = molt->atoms.atom[at].qB;
                int newRes = molt->atoms.resinfo[molt->atoms.atom[at].resind].nr;
                if(newRes != oldRes)
                {
                    molecule.residues.push_back(residue);
                    residue.atoms.clear();
                    residue.bb_entry = NULL;
                    residue.name = *(molt->atoms.resinfo[molt->atoms.atom[at].resind].name);
                    residue.resid = newRes;
                    residue.residue = residueId++;
                    oldRes = newRes;
                }
                residue.atoms.push_back(atom);
            }
            
            for(int i = 0; i < F_NRE; ++i)
            {
                int index = (i == F_CONSTRNC ? F_CONSTR : i);
                for(int j = 0; j < molt->ilist[i].nr; ++j)
                    impl_->ffparams.ilist[index].push_back(molt->ilist[i].iatoms[j] + natoms);
            }
            
            natoms += molt->atoms.nr;
            molecule.residues.push_back(residue);
            impl_->mol.push_back(molecule);
        }
    }
    impl_->Update();
    
    //intermolecular interactions
    //*******************************************
    
    if(topol.bIntermolecularInteractions)
        for(int i = 0; i < F_NRE; ++i)
            for(int j = 0; j < topol.intermolecular_ilist[i].nr; ++j)
                impl_->ffparams.ilist[i].push_back(topol.intermolecular_ilist[i].iatoms[j] + natoms);
    
    
}

tsTopology& tsTopology::operator = (const t_topology& topol)
{
    clear();
    
    impl_->name = *(topol.name);
    
    //force field
    //*******************************************
    impl_->ffparams.cmap_grid.gridSpacing = topol.idef.cmap_grid.grid_spacing;
    for(int i = 0; i < topol.idef.cmap_grid.ngrid; ++i)
    {
        impl_->ffparams.cmap_grid.grids.push_back(std::vector<real>());
        for(int j = 0; j < 4*impl_->ffparams.cmap_grid.gridSpacing*impl_->ffparams.cmap_grid.gridSpacing; ++j)
            impl_->ffparams.cmap_grid.grids[i].push_back(topol.idef.cmap_grid.cmapdata[i].cmap[j]);
    }
    impl_->ffparams.fudgeQQ = topol.idef.fudgeQQ;
    impl_->ffparams.reppow = -1;
    for(int i = 0; i < F_NRE; ++i)
    {
        impl_->ffparams.ilist.push_back(std::vector<atom_id>());
        int index = impl_->ffparams.ilist.size() - 1;
        for(int j = 0; j < topol.idef.il[i].nr; ++j)
            impl_->ffparams.ilist[index].push_back(topol.idef.il[i].iatoms[j]);
    }
    
    //atomtypes
    //*******************************************

    for(int i = 0; i < topol.atomtypes.nr; ++i)
    {
        ts_atomtype at;
        at.S_hct = topol.atomtypes.S_hct[i];
        at.atomnumber = topol.atomtypes.atomnumber[i];
        at.gb_radius = topol.atomtypes.gb_radius[i];
        at.name = "";
        at.radius = topol.atomtypes.radius[i];
        at.surftens = topol.atomtypes.surftens[i];
        at.vol = topol.atomtypes.vol[i];
        impl_->atomtypes.push_back(at);
    }  
    
    //atoms, residues, molecules
    //*******************************************
    int residueId = 0;
    for(int mol = 0; mol < topol.mols.nr; ++mol)
    {
        ts_molecule molecule;
        molecule.chainid = topol.atoms.resinfo[topol.atoms.atom[topol.mols.index[mol]].resind].chainid;
        molecule.chainnum = topol.atoms.resinfo[topol.atoms.atom[topol.mols.index[mol]].resind].chainnum;
        molecule.name = "";
        ts_residue residue;
        residue.bb_entry = NULL;
        residue.name = *(topol.atoms.resinfo[topol.atoms.atom[topol.mols.index[mol]].resind].name);
        residue.resid = topol.atoms.resinfo[topol.atoms.atom[topol.mols.index[mol]].resind].nr;
        residue.residue = residueId++;
        int oldRes = residue.resid;
        for(int at = topol.mols.index[mol]; 
                at < (mol == topol.mols.nr - 1 ? topol.atoms.nr : topol.mols.index[mol+1]); 
                ++at)
        {
            ts_atom atom;
            atom.atomName = *(topol.atoms.atomname[at]);
            atom.atomType = &(impl_->atomtypes[topol.atoms.atom[at].type]);
            atom.atomTypeB = &(impl_->atomtypes[topol.atoms.atom[at].typeB]);
            atom.atomnumber = topol.atoms.atom[at].atomnumber;
            atom.elem = topol.atoms.atom[at].elem;
            atom.m = topol.atoms.atom[at].m;
            atom.mB = topol.atoms.atom[at].mB;
            atom.q = topol.atoms.atom[at].q;
            atom.qB = topol.atoms.atom[at].qB;
            int newRes = topol.atoms.resinfo[topol.atoms.atom[at].resind].nr;
            if(newRes != oldRes)
            {
                molecule.residues.push_back(residue);
                residue.atoms.clear();
                residue.bb_entry = NULL;
                residue.name = *(topol.atoms.resinfo[topol.atoms.atom[at].resind].name);
                residue.resid = newRes;
                residue.residue = residueId++;
                oldRes = newRes;
            }
            residue.atoms.push_back(atom);
        }
        molecule.residues.push_back(residue);
        impl_->mol.push_back(molecule);
    }
    impl_->Update();
    
    //exclusions
    //*******************************************
    for(int i = 0; i < topol.excls.nr; ++i)
    {
        ts_molecule* mol = impl_->atoms[topol.excls.a[topol.excls.index[i]]].mol;
        mol->excls.push_back(std::vector<atom_id>());
        int index = mol->excls.size() - 1;
        for(int at = topol.excls.index[i]; at < topol.excls.index[i+1]; ++at)
            mol->excls[index].push_back(topol.excls.a[at]);
    }
}
    
}   //gmx namespace