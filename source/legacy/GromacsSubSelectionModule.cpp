/* 
 * File:   GromacsSubSelectionModule.cpp
 * Author: kulkem
 * 
 * Created on May 20, 2016, 2:25 PM
 */

#include <backup/tsutility.h>

#include "GromacsSubSelectionModule.h"
#include "gromacs/selection/poscalc.h"
#include "gromacs/options.h"
#include "gromacs/options/filenameoptionmanager.h"
#include "gromacs/topology/topology.h"


namespace gmx
{

void GromacsSubSelectionModule::initialize()
{
}

int GromacsSubSelectionModule::start()
{
    
}


void GromacsSubSelectionModule::finish()
{
    
}

void GromacsSubSelectionModule::initOptions(Options* options)
{
    const char * const debug_levels[] = { "no", "basic", "compile", "eval", "full" };
    const char *const *postypes = PositionCalculationCollection::typeEnumValues;
    
    if(settings_.bOTselrPos)
        options->addOption(StringOption(settings_.cOTselrPos).enumValueFromNullTerminatedArray(postypes).
        store(&settings_.cselrPos).defaultValue(postypes[0]).description("Selection reference positions"));
    
    if(settings_.bOTselType)
        options->addOption(StringOption(settings_.cOTselType).enumValueFromNullTerminatedArray(postypes).store(&settings_.cselType).
        defaultValue(postypes[0]).description("Default selection output positions"));
    
    if(settings_.bOTselDebug)
        options->addOption(StringOption(settings_.cOTselDebug).enumValue(debug_levels).defaultValue(debug_levels[settings_.cselDebug]).
        storeEnumIndex(&settings_.cselDebug).description("Print out selection trees for debugging").hidden(settings_.bHiddenselDebug));
    
}

void GromacsSubSelectionModule::finishOptions(Options* options)
{
        GMX_RELEASE_ASSERT(settings_.cselDebug >= 0 && settings_.cselDebug <= 4,"Debug level out of range");
        selection.setDebugLevel(settings_.cselDebug);
        selection.setReferencePosType(settings_.cselrPos.c_str());
        selection.setOutputPosType(settings_.cselType.c_str());
        
        selection.setIndexGroups(NULL);
        //TODO: index code here
        //TODO: selection manager here
        
        selection.compile();
}

int GetIndexWithName(char* name, char* resName, t_topology *topol, std::vector<int>* indicies)
{
    if(!topol) return -1;
    
    for(int i = 0; i < topol->atoms.nr; ++i)
    {
        if(!strcmp(name, *topol->atoms.atomname[i]) && 
        (resName == NULL || !strcmp(resName, *(topol->atoms.resinfo[topol->atoms.atom[i].resind].name))))
        {
            indicies->push_back(i);
        }
    }
    
    if(indicies->size() > 0) return 0;
    
    return -2;
}

int GetIndexWithName(char* name, const SubList &list, t_topology *topol, int* index)
{
    if(!topol) return -1;
    
    for(int i = 0; i < list.Length; ++i)
    {
        if(!strcmp(name, *(topol->atoms.atomname[list.Indicies[i]])))
        {
            *index = i;
            return 0;
        }
    }
    *index = -1;
    return -2;
}

int GetResidueTypeSubList(char* resName, ResidueSubList* list, t_topology* topol, const Selection &sel)
{
  if(!list || !topol) return -1;
  
  bool allres = false;
  if(!resName)
  {
    allres = true;
  }
  
  t_atoms atoms = topol->atoms;
  
  int AtomNumber = atoms.nr;
  t_atom* Atoms = atoms.atom;
  
  SubList* tmpList = new SubList[AtomNumber];
  int* tmpList2 = new int[AtomNumber];
  int rNumber = 0;
  int raNumber = -1;
  int aNumber = 0;
  int atNumber = 0;
  
  
  for(unsigned int i = 0; i < sel.atomCount(); i++)
  {
    unsigned long int index = sel.atomIndices().at(i);
    if(!allres)
    {
      if(!strcmp(resName, *(atoms.resinfo[Atoms[index].resind].name)))
      {
	if(Atoms[index].resind != raNumber)	//new sidechain
	{
	  if(raNumber != -1)
	  {
	    tmpList[rNumber].Indicies = new int[aNumber];
	    tmpList[rNumber].Length = aNumber;
	    for(int j = 0; j < aNumber; ++j)
	    {
	      tmpList[rNumber].Indicies[j] = tmpList2[j];
	    }
	    rNumber++;
	  }
	  raNumber = Atoms[index].resind;
	  aNumber = 0;
	  tmpList2[aNumber++] = index;
	  atNumber++;
	}
	else
	{
	  tmpList2[aNumber++] = index;
	  atNumber++;
	}
      }
    }
    else
    {
      if(Atoms[index].resind != raNumber)	//new sidechain
      {
	if(raNumber != -1)
	{
	  tmpList[rNumber].Indicies = new int[aNumber];
	  tmpList[rNumber].Length = aNumber;
	  for(int j = 0; j < aNumber; ++j)
	  {
	    tmpList[rNumber].Indicies[j] = tmpList2[j];
	  }
	  rNumber++;
	}
	raNumber = Atoms[index].resind;
	aNumber = 0;
	tmpList2[aNumber++] = index;
	atNumber++;
      }
      else
      {
	tmpList2[aNumber++] = index;
	atNumber++;
      }      
    }
  }
  if(aNumber)
  {
    tmpList[rNumber].Indicies = new int[aNumber];
    tmpList[rNumber].Length = aNumber;
    for(int j = 0; j < aNumber; ++j)
    {
      tmpList[rNumber].Indicies[j] = tmpList2[j];
    }
    rNumber++;
  }
  
  
  if(!atNumber)
  {
    for(int i = 0; i < rNumber; ++i)
    {
        if(tmpList[i].Indicies) {delete[] tmpList[i].Indicies; tmpList[i].Indicies = NULL;}
    }
    if(tmpList) {delete[] tmpList; tmpList = NULL;}
    if(tmpList2) {delete[] tmpList2; tmpList2 = NULL;}

    return 1;
  }
  
  list->Clear();
  list->List = new SubList[rNumber];
  list->Length = rNumber;
  
  for(int i = 0; i < rNumber; ++i)
  {
    list->List[i].Indicies = new int[tmpList[i].Length];
    list->List[i].Length = tmpList[i].Length;
    for(int j = 0; j < tmpList[i].Length; ++j)
    {
      list->List[i].Indicies[j] = tmpList[i].Indicies[j];
    }
  }
  
  for(int i = 0; i < rNumber; ++i)
  {
    if(tmpList[i].Indicies) {delete[] tmpList[i].Indicies; tmpList[i].Indicies = NULL;}
  }
  
  if(tmpList) {delete[] tmpList; tmpList = NULL;}
  if(tmpList2) {delete[] tmpList2; tmpList2 = NULL;}
  
  return 0;
}

}