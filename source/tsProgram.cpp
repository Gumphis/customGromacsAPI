/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsProgram.cpp
 * Author: kulkem
 * 
 * Created on July 27, 2017, 2:23 PM
 */

#include "gromacs/legacyheaders/copyrite.h"
#include "gromacs/utility/file.h"
#include "gromacs/commandline/cmdlineprogramcontext.h"
#include "gromacs/commandline/cmdlineinit.h"
#include "gromacs/utility/basenetwork.h"
#include "gromacs/selection/selectioncollection.h"
#include "gromacs/utility/futil.h"
#include "gromacs/utility/stringutil.h"
#include "gromacs/utility/fatalerror.h"
#include "gromacs/utility/sysinfo.h"
#include "gromacs/utility/futil.h"
#include "gromacs/math/utilities.h"
#include "gromacs/utility/exceptions.h"

#include "tsAbstractModule.h"
#include "tsProgram.h"
#include "tsHelp.h"
#include "tsCmdParser.h"

/*#include "gmxpre.h"
#include "gromacs/analysisdata/paralleloptions.h"
#include "gromacs/commandline/cmdlinehelpcontext.h"
#include "gromacs/commandline/cmdlinehelpwriter.h"
#include "gromacs/commandline/cmdlinemodule.h"
#include "gromacs/commandline/cmdlineparser.h"
#include "gromacs/fileio/trx.h"
#include "gromacs/options/filenameoptionmanager.h"
#include "gromacs/options/options.h"
#include "gromacs/pbcutil/pbc.h"
#include "gromacs/selection/selectionoptionmanager.h"
#include "gromacs/trajectoryanalysis/analysismodule.h"
#include "gromacs/utility/exceptions.h"
#include "gromacs/legacyheaders/copyrite.h"
#include "gromacs/options/basicoptions.h"
#include "gromacs/utility/basenetwork.h"
#include "gromacs/utility/stringutil.h"
#include "gromacs/commandline/cmdlinehelpcontext.h"
#include "gromacs/commandline/cmdlineinit.h"
#include "gromacs/commandline/cmdlinemodule.h"
#include "gromacs/options/basicoptions.h"
#include "gromacs/options/options.h"
#include "gromacs/utility/gmxassert.h"*/

namespace gmx
{

//********************************************************************
// tsProgramSettings

tsProgramSettings::tsProgramSettings(const std::string& name, const std::string& description)
    : iprogNiceLevel_(19), bProgVersionOnly_(false), bProgHelpOnly_(false), bFileBackups_(true), bExcpFloatingPoint_(false),
      iprogDebuglevel_(0), bCmdCopyright_(false), bCmdExtendedInfo_(false), bFileGenerateHeader_(false),
      bSelDefaultIndexGroups_(true), bCmdHelpShowHidden_(false), bCmdQuiet_(false), bCmdHelpLongDescriptions_(true),
      sCmdSuffix_(""), sCmdPrefix_(""), sSelRefPosType_("atom"), name_(name), description_(description),
      iCmdLineLength_(78), iCmdHelpSynopsisIntend_(13)
{
}

tsProgramSettings::~tsProgramSettings()
{
}

tsProgramSettings& tsProgramSettings::progNiceLevel(int nl)                     {iprogNiceLevel_=nl; return *this;}     
tsProgramSettings& tsProgramSettings::progVersionOnly(bool v)                   {bProgVersionOnly_ = v; return *this;}    
tsProgramSettings& tsProgramSettings::progHelpOnly(bool h)                      {bProgHelpOnly_ = h; return *this;}          
tsProgramSettings& tsProgramSettings::fileBackups(bool b)                       {bFileBackups_ = b; return *this;}         
tsProgramSettings& tsProgramSettings::excpFloatingPoint(bool e)                 {bExcpFloatingPoint_ = e; return *this;}  
tsProgramSettings& tsProgramSettings::cmdHelpShowHidden(bool s)                 {bCmdHelpShowHidden_ = s; return *this;}      
tsProgramSettings& tsProgramSettings::cmdQuiet(bool q)                          {bCmdQuiet_ = q; return *this;}            
tsProgramSettings& tsProgramSettings::selDefaultIndexGroups(bool i)             {bSelDefaultIndexGroups_ = i; return *this;}      
tsProgramSettings& tsProgramSettings::progDebugLevel(int i)                     {iprogDebuglevel_ = i; return *this;}   
tsProgramSettings& tsProgramSettings::cmdCopyright(bool p)                      {bCmdCopyright_ = p; return *this;}
tsProgramSettings& tsProgramSettings::cmdExtendedInfo(bool ei)                  {bCmdExtendedInfo_ = ei; return *this;}      
tsProgramSettings& tsProgramSettings::fileGenerateHeader(bool gh)               {bFileGenerateHeader_ = gh; return *this;}
tsProgramSettings& tsProgramSettings::cmdHelpLongDescriptions(bool hd)          {bCmdHelpLongDescriptions_ = hd; return *this;}
tsProgramSettings& tsProgramSettings::cmdSuffix(const std::string& suffix)      {sCmdSuffix_ = suffix; return *this;}   
tsProgramSettings& tsProgramSettings::cmdPrefix(const std::string& prefix)      {sCmdPrefix_ = prefix; return *this;}   
tsProgramSettings& tsProgramSettings::selRefPosType(const std::string& rpt)     {sSelRefPosType_ = rpt; return *this;}
tsProgramSettings& tsProgramSettings::cmdLineLength(int length)                 {iCmdLineLength_ = length; return *this;}
tsProgramSettings& tsProgramSettings::cmdHelpSynopsisIntend(int intend)         {iCmdHelpSynopsisIntend_ = intend; return *this;}


//********************************************************************

tsProgramSettingsInfo::tsProgramSettingsInfo(tsProgramSettings* settings)  {settings_ = settings;}
tsProgramSettingsInfo::~tsProgramSettingsInfo() {}

int  tsProgramSettingsInfo::progNiceLevel()                     {return settings_->iprogNiceLevel_;}
int  tsProgramSettingsInfo::progDebugLevel()                    {return settings_->iprogDebuglevel_;}
const std::string& tsProgramSettingsInfo::cmdSuffix()           {return settings_->sCmdSuffix_;}
const std::string& tsProgramSettingsInfo::cmdPrefix()           {return settings_->sCmdPrefix_;}
const std::string& tsProgramSettingsInfo::selRefPosType()       {return settings_->sSelRefPosType_;}
const std::string& tsProgramSettingsInfo::getName()             {return settings_->name_;}
const std::string& tsProgramSettingsInfo::getDescription()      {return settings_->description_;}
bool tsProgramSettingsInfo::progVersionOnly()                   {return settings_->bProgVersionOnly_;}      
bool tsProgramSettingsInfo::progHelpOnly()                      {return settings_->bProgHelpOnly_;}      
bool tsProgramSettingsInfo::fileBackups()                       {return settings_->bFileBackups_;}      
bool tsProgramSettingsInfo::excpFloatingPoint()                 {return settings_->bExcpFloatingPoint_;}      
bool tsProgramSettingsInfo::cmdHelpShowHidden()                 {return settings_->bCmdHelpShowHidden_;}      
bool tsProgramSettingsInfo::cmdQuiet()                          {return settings_->bCmdQuiet_;}      
bool tsProgramSettingsInfo::selDefaultIndexGroups()             {return settings_->bSelDefaultIndexGroups_;}     
bool tsProgramSettingsInfo::cmdCopyright()                      {return settings_->bCmdCopyright_;}      
bool tsProgramSettingsInfo::cmdExtendedInfo()                   {return settings_->bCmdExtendedInfo_;}      
bool tsProgramSettingsInfo::fileGenerateHeader()                {return settings_->bFileGenerateHeader_;} 
bool tsProgramSettingsInfo::cmdHelpLongDescriptions()           {return settings_->bCmdHelpLongDescriptions_;}
int tsProgramSettingsInfo::cmdLineLength()                      {return settings_->iCmdLineLength_;}
int tsProgramSettingsInfo::cmdHelpSynopsisIntend()              {return settings_->iCmdHelpSynopsisIntend_;}

    
//********************************************************************
// GromacsModule::Impl
    
class tsProgram::Impl
{
public:
    Impl(tsProgramInterface* interface, tsProgramSettingsInfo& settings);
    ~Impl();

    void initOptions();
    void finishOptions();
    void writeHelp(const tsOptions &options);
    int  run(int argc, char* argv[]);

    CommandLineHelpContext          helpContext;
    tsProgramInterface*             interface_;
    tsProgramSettingsInfo&          settings_;
    BinaryInformationSettings       binaryInfoSettings_;
    tsOptions                       options_;
    tsModules                       modules_;
};


tsProgram::Impl::Impl(tsProgramInterface* interface, tsProgramSettingsInfo& settings) 
    : interface_(interface), settings_(settings), helpContext(&File::standardError(), gmx::eHelpOutputFormat_Console, NULL, settings.getName()),
      options_(settings.getName(), settings.getDescription()), binaryInfoSettings_()
{
    helpContext.setModuleDisplayName(settings.getName());
}


tsProgram::Impl::~Impl()
{
}


void tsProgram::Impl::writeHelp(const tsOptions &options)
{
    helpContext.setShowHidden(settings_.cmdHelpShowHidden());
    //TODO: get unconst cast away
    
    tsHelp(const_cast<tsOptions&>(options)).showDescription(settings_.cmdHelpLongDescriptions()).
            setLineLength(settings_.cmdLineLength()).setMinimumIntend(settings_.cmdHelpSynopsisIntend()).writeHelp(helpContext);
}


void tsProgram::Impl::initOptions()
{
    options_.addOption(tsBooleanOption("h").store(&(settings_.settings_->bProgHelpOnly_)).setDescription("Print help and quit").defaultValue(false));
    options_.addOption(tsBooleanOption("hidden").store(&(settings_.settings_->bCmdHelpShowHidden_)).hidden().setDescription("Show hidden options in help").defaultValue(false));
    options_.addOption(tsBooleanOption("quiet").store(&(settings_.settings_->bCmdQuiet_)).hidden().setDescription("Do not print common startup info or quotes").defaultValue(false));
    options_.addOption(tsBooleanOption("version").store(&(settings_.settings_->bProgVersionOnly_)).setDescription("Print extended version information and quit").defaultValue(false));
    options_.addOption(tsBooleanOption("copyright").store(&(settings_.settings_->bCmdCopyright_)).hidden().setDescription("Print copyright information on startup").defaultValue(false));
    options_.addOption(tsIntegerOption("nice").store(&(settings_.settings_->iprogNiceLevel_)).setDescription("Set the nicelevel (default depends on command)").defaultValue(20));
    options_.addOption(tsBooleanOption("backup").store(&(settings_.settings_->bFileBackups_)).hidden().setDescription("Write backups if output files exist").defaultValue(false));
    options_.addOption(tsBooleanOption("fpexcept").store(&(settings_.settings_->bExcpFloatingPoint_)).hidden().setDescription("Enable floating-point exceptions").defaultValue(false));
    options_.addOption(tsIntegerOption("debug").store(&(settings_.settings_->iprogDebuglevel_)).hidden().defaultValueIfSet(1).defaultValue(0)
                                    .setDescription("Write file with debug information, 1: short (default), 2: also x and f"));
    
    interface_->initOptions(&options_, settings_);
    for(int i = 0; i < modules_.pHandlerList.size(); ++i)
        modules_.pHandlerList[i]->initOptions(&options_);
}

void tsProgram::Impl::finishOptions()
{
    options_.finish();
    binaryInfoSettings_.extendedInfo(settings_.progVersionOnly());
    // The latter condition suppresses the copyright with
    // -quiet -version.
    binaryInfoSettings_.copyright(settings_.cmdCopyright() && !settings_.cmdQuiet() && !settings_.progVersionOnly());
   
    for(int i = 0; i < modules_.pHandlerList.size(); ++i)
        modules_.pHandlerList[i]->finishOptions(&options_);
    interface_->finishOptions(&options_, settings_);
}

int tsProgram::Impl::run(int argc, char* argv[])
{
    CommandLineProgramContext &programContext = gmx::initForCommandLine(&argc, &argv);
    const bool                     bMaster = (gmx_node_rank() == 0);
    bool                           bQuiet  = settings_.cmdQuiet() || !bMaster;
    
    interface_->initModules(&modules_);
    
    for(int i = 0; i < modules_.pHandlerList.size(); ++i)
            modules_.pHandlerList[i]->initialize();
    
    //TODO: Move this codeblock section somewhere more convenient
    SelectionCollection       selections;
    selections.setDebugLevel(settings_.progDebugLevel());
    selections.setReferencePosType(settings_.selRefPosType().c_str());
    
//    FileNameOptionManager  fileoptManager;
//    SelectionOptionManager seloptManager(&selections); //TODO: move the whole selection code into the module

    //TODO:
 //   options_.addManager(&fileoptManager);
 //   options_.addManager(&seloptManager);
    //******************************************************
    
    try
    {
        initOptions();
        tsCmdParser(&options_).skipUnknown(false).parse(&argc, argv);
        finishOptions();
    }
    catch (const std::exception &)
    {
        bQuiet |= settings_.cmdQuiet();
        if (!bQuiet) printBinaryInformation(stderr, programContext, binaryInfoSettings_);
        if(settings_.progHelpOnly()) {writeHelp(options_); return 0;}
        throw;
    }
    
    bQuiet |= settings_.cmdQuiet();
    if (!bQuiet) printBinaryInformation(stderr, programContext, binaryInfoSettings_);
    if (settings_.progVersionOnly()) {if(!bQuiet) gmx_thanx(stderr);return 0;}
    //TODO: reorder the code - this is really inconvenient and a lot is written doubled
    if(settings_.progHelpOnly()) {writeHelp(options_);if(!bQuiet) gmx_thanx(stderr);return 0;}
    
    gmx_set_max_backup_count(settings_.fileBackups() ? -1 : 0);

    // Open the debug file.
    if (settings_.progDebugLevel() > 0)
    {
        std::string filename(programContext.programName());
        if (gmx_node_num() > 1)
        {
            filename.append(formatString("%d", gmx_node_rank()));
        }
        filename.append(".debug");

        fprintf(stderr, "Will write debug log file: %s\n", filename.c_str());
        gmx_init_debug(settings_.progDebugLevel(), filename.c_str());
    }
    // Set the nice level unless disabled in the configuration.
    if (settings_.progNiceLevel() != 0)
    {
        static bool bNiceSet = false; // Only set it once.
        if (!bNiceSet)
        {
            // TODO: Diagnostic if this fails and the user explicitly requested it.
            gmx_set_nice(settings_.progDebugLevel());
            bNiceSet = true;
        }
    }
    if (settings_.excpFloatingPoint())
    {
        //TODO: currently it is always enabled for mdrun (verlet) and tests.
        gmx_feenableexcept();
    }
    
    const bool bInteractive = File::standardInput().isInteractive();
    //seloptManager.parseRequestedFromStdin(bInteractive);

    // selections->compile();

    int rc = interface_->callback(&modules_);

    for(int i = 0; i < modules_.pHandlerList.size(); ++i)
        modules_.pHandlerList[i]->finish();
    
    if (!bQuiet) gmx_thanx(stderr);
    
    return rc;
}

/********************************************************************
 * tsProgram
 */

tsProgram::tsProgram(tsProgramInterface* Interface, tsProgramSettingsInfo& settings)
    : impl_(new Impl(Interface, settings))
{
}

tsProgram::~tsProgram()
{
}

// static
int tsProgram::run(int argc, char *argv[])
{
    try
    {
        int rc = impl_->run(argc, argv);
        gmx::finalizeForCommandLine();
        return rc;
    }
    catch (const std::exception &ex)
    {
        printFatalErrorMessage(stderr, ex);
        return processExceptionAtExitForCommandLine(ex);
    }
}

} // namespace gmx
