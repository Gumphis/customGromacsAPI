/* 
 * File:   GromacsSubSelectionModule.h
 * Author: kulkem
 *
 * Created on May 20, 2016, 2:25 PM
 */

#ifndef GROMACSSUBSELECTIONMODULE_H
#define	GROMACSSUBSELECTIONMODULE_H

#include "gromacs/utility/classhelpers.h"
#include <cstring>
#include "GromacsSubModule.h"
#include "gromacs/selection/selectioncollection.h"
#include "GromacsSubInputIndexModule.h"

namespace gmx
{

class GromacsSubSelectionModuleSettings : public GromacsSubModuleSettings {
public:
    GromacsSubSelectionModuleSettings() : GromacsSubModuleSettings()
    {
        cselrPos = "";
        cOTselrPos = "selrpos";
        bOTselrPos = true;
        cselType = "";
        cOTselType = "seltype";
        bOTselType = false;
        cselDebug = 0;
        cOTselDebug = "seldebug";
        bOTselDebug = true;
        bHiddenselDebug = true;
    }
    GromacsSubSelectionModuleSettings(const GromacsSubSelectionModuleSettings& orig) : GromacsSubModuleSettings(orig),
    cOTselrPos(orig.cOTselrPos), bOTselrPos(orig.bOTselrPos), cOTselType(orig.cOTselType), bOTselType(orig.bOTselType),
    cOTselDebug(orig.cOTselDebug), bOTselDebug(orig.bOTselDebug), bHiddenselDebug(orig.bHiddenselDebug) {}
    virtual ~GromacsSubSelectionModuleSettings() {}
    
    virtual bool isSettingInGroup(SettingDefinition definition) {return definition == GMX_SM || definition == GMS_SM_SELECTION;}
    virtual bool isSettingSet(SettingDefinition definition) {return definition == GMS_SM_SELECTION;}
    
    std::string cselrPos;       //reference Position
    const char* cOTselrPos;     //selection flag for reference position
    bool bOTselrPos;            //add flag for reference position
    std::string cselType;       //output position
    const char* cOTselType;     //selection flag for output position
    bool bOTselType;            //add flag for output position
    int cselDebug;              //selection debug level (0 - 4)
    const char* cOTselDebug;    //flag for the selection debugging ("no", "basic", "compile", "eval", "full")
    bool bOTselDebug;           //add flag for selection debugging
    bool bHiddenselDebug;       //make the selection debugging flag hidden
    GromacsSubInputIndexModuleSettings indexSettings;
};
    
class GromacsSubSelectionModule : public GromacsSubModule {
public:
    GromacsSubSelectionModule(const GromacsSubSelectionModuleSettings &settings) : settings_(settings), GromacsSubModule(settings) {}
    virtual ~GromacsSubSelectionModule() {}
    
    virtual int start(); //start the submodule
    const GromacsSubSelectionModuleSettings& getSettings() {return settings_;}
    
private:
    using GromacsSubModule::getSettings;
    GromacsSubSelectionModuleSettings settings_;
    
    SelectionCollection selection;
    
protected:    
    
    virtual void finish(); //finish the module
     
    virtual void initialize(); //initialize the module
    
    virtual void initOptions(Options* options);
    
    virtual void finishOptions(Options* options);
    
    friend class GromacsModule;
};

class SubList
{
public:
  int Length;
  int* Indicies;

  SubList() {Length = 0; Indicies = NULL;}
  ~SubList() {Clear();}
  
  void Clear() {if(Indicies) {delete[] Indicies; Indicies = NULL;}}
};

class ResidueSubList
{
public:
  int Length;
  SubList* List;
  
  ResidueSubList() {Length = 0; List = NULL;}
  ~ResidueSubList() {Clear();}
  
  void Clear()
  {
    if(List)
    {
      for(int i = 0; i < Length; ++i)
      {
        if(List[i].Indicies) {delete[] List[i].Indicies; List[i].Indicies = NULL;}
      }
    }
  }  
};

int GetIndexWithName(char* name, char* resName, t_topology *topol, std::vector<int>* indicies);
int GetIndexWithName(char* name, const SubList &list, t_topology *topol, int* index);
int GetResidueTypeSubList(char* resName, ResidueSubList* list, t_topology* topol, const Selection &sel);

} // name space

#endif	/* GROMACSSUBSELECTIONMODULE_H */

