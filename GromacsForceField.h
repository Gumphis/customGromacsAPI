/* 
 * File:   GromacsForceField.h
 * Author: kulkem
 *
 * Created on May 30, 2016, 3:11 PM
 */

#ifndef GROMACSFORCEFIELD_H
#define	GROMACSFORCEFIELD_H

#include <cstring>
#include <iostream>
#include <queue>

#include "GromacsUtility.h"

struct file_descr
{
    std::string location;
    std::string name;
    std::string extension;
    std::string corename;
};

struct ff_descr
{
    std::string location;
    std::string descr;
    std::string folder;
};

struct stBeadTypes
{
    std::string name;
    float mass;
    float charge;
};

//Gives back a list of found force fields
void get_ffs(std::vector<ff_descr>& ff_list);
//Gives back a single force field that the user has chosen
ff_descr get_ff();

//returns the path to the file and "" if not found in GMXLIB paths
std::string getFileInGMXLib(std::string fileName);

void get_files_with_extension(std::string dir, std::string extension, std::vector<file_descr>& files);

bool file_exists(std::string fileName);

tsResult getStandardBeadTypes(ff_descr* ff, std::vector<stBeadTypes>* beadList);

#endif	/* GROMACSFORCEFIELD_H */

