/* 
 * File:   tsTopology.h
 * Author: kulkem
 *
 * Created on July 24, 2017, 4:55 PM
 */

#pragma once

#include "gromacs/utility/classhelpers.h"
#include "gromacs/math/vectypes.h"
#include "gromacs/topology/idef.h"
#include "gromacs/topology/symtab.h"
#include "gromacs/topology/topology.h"
#include "tsCoordinate.h"
#include <cstring>
#include <vector>

namespace gmx
{

struct ts_atomtype
{
    std::string     name;         /* number of atomtypes                          */
    real            radius;     /* GBSA radius for each atomtype                */
    real            vol;        /* GBSA efective volume for each atomtype       */
    real            surftens;   /* implicit solvent surftens for each atomtype  */
    real            gb_radius;  /* GB radius for each atom type                 */
    real            S_hct;      /* Overlap factors for HCT/OBC GB models        */
    int             atomnumber; /* Atomic number, used for QM/MM                */
};

struct ts_atom
{
    int            atomnumber;  // Atomic Number or NOTSET (always iterated)
    real           m, q;        // Mass and charge                      
    real           mB, qB;      // Mass and charge for Free Energy calc                         
    std::string    elem;        // Element name                         
    std::string    atomName;
    ts_atomtype*   atomType;
    ts_atomtype*   atomTypeB;
};

struct ts_residue
{
    std::string                         name;           /* Pointer to the residue name          */
    int                                 residue;        // iterated residue number
    int                                 resid;          /* Resid number                       */
    std::vector<ts_atom>                atoms;          /* The atoms in this molecule           */    
    void*                               *bb_entry;      /* pointer to the building block (optional)   */
};

struct ts_molecule
{
    std::string                         name;
    char                                chainid;        /* Chain identifier written/read to pdb */
    int                                 chainnum;       /* Iincremented at TER or new chain id  */
    std::vector<ts_residue>             residues;       /* The residues in this molecule           */
    std::vector< std::vector<atom_id> > excls;          /* The exclusions                       */
};

struct ts_cmap
{
    int gridSpacing;
    std::vector< std::vector<real> > grids;             //vector of 4*gridspacing*gridspacing maps
};

struct ts_ff_params
{
    std::vector<int>            functype;       
    std::vector<t_iparams>      iparams;        
    double                      reppow;         /* The repulsion power for VdW: C12*r^-reppow   */
    real                        fudgeQQ;        /* The scaling factor for Coulomb 1-4: f*q1*q2  */
    ts_cmap                     cmap_grid;      /* The dihedral correction maps                 */
    std::vector< std::vector< atom_id > >       ilist;           /* List of interactions
                                                                * using system wide atom indices,
                                                                * either NULL or size F_NRE           */
};

struct tsTopology
{
public:
    tsTopology(std::string name);
    ~tsTopology();
    
    std::string                     getName() const;
    int                             numMol() const;
    const ts_molecule&              getMol(int molid) const;
    const ts_molecule&              getMolByResidue(int resid) const;
    const ts_molecule&              getMolByAtom(int id) const;
    int                             numResidue() const;
    const ts_residue&               getResidue(int resid) const;
    const ts_residue&               getResidueByAtom(int id) const;
    int                             numAtoms() const;
    const ts_atom&                  getAtom(int id) const;
    int                             numAtomTypes() const;
    const ts_atomtype&              getAtomType(int atid) const;
    const ts_ff_params&             getFFParams() const;
    
    tsTopology& operator = (const t_topology& topol);
    tsTopology& operator = (const gmx_mtop_t& topol);
    
    void                            clear();
    int                             addMolecule(std::string name, char chainid, int chainnum);      //returns index
    int                             addResidue(int mol, std::string name, int residue, int resid);  //returns index
    int                             addAtom(int mol, int res, std::string atomName, int atomnumber, 
                                            real m, real q, real mB, real qB, std::string elem,
                                            ts_atomtype* atomType, ts_atomtype* atomTypeB);         //returns index
    ts_molecule&                    getMol(int molid);                                              //returns reference
    ts_residue&                     getResidue(int molid, int resid);                               //returns reference
    ts_atom&                        getAtom(int molid, int resid, int atomid);                      //returns reference
    void                            Update();                                                       //recalculates links for the residue and atom list

private:
    class Impl;
    PrivateImplPointer<Impl> impl_;
};

} //gmx namespace