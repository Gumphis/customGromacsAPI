/* 
 * File:   GromacsModule.cpp
 * Author: kulkem
 * 
 * Created on May 13, 2016, 11:40 AM
 */

#include "GromacsModule.h"
#include "GromacsSubModule.h"
#include "gromacs/commandline/cmdlineinit.h"

#include "gmxpre.h"

#include "gromacs/analysisdata/paralleloptions.h"
#include "gromacs/commandline/cmdlinehelpcontext.h"
#include "gromacs/commandline/cmdlinehelpwriter.h"
#include "gromacs/commandline/cmdlinemodule.h"
#include "GromacsModuleManager.h"
#include "gromacs/commandline/cmdlineparser.h"
#include "gromacs/fileio/trx.h"
#include "gromacs/options/filenameoptionmanager.h"
#include "gromacs/options/options.h"
#include "gromacs/pbcutil/pbc.h"
#include "gromacs/selection/selectioncollection.h"
#include "gromacs/selection/selectionoptionmanager.h"
#include "gromacs/trajectoryanalysis/analysismodule.h"
#include "gromacs/utility/exceptions.h"
#include "gromacs/utility/file.h"
#include "gromacs/utility/gmxassert.h"

namespace gmx
{

/********************************************************************
 * GromacsModule::Impl
 */

struct subModuleDescr
{
    GromacsSubModule* module;
    std::string id;
};
    
class GromacsModule::Impl
{
public:
    class RunnerCommandLineModule;

    Impl(const GromacsModuleSettings &settings, GromacsModuleInterface* interface, const char *name, const char *description);
    ~Impl();

    void parseOptions(SelectionCollection *selections,
                      int *argc, char *argv[], bool onlyHelp = false);

    void initOptions(Options *options);
    void finishOptions(Options *options);
    void _writeHelp(const Options &options);

    CommandLineHelpContext helpContext;
    GromacsModuleInterface* interface_;
    GromacsModuleSettings settings_;
    const char* name_;
    const char* description_;
    std::vector<subModuleDescr> subModuleList_;
};


GromacsModule::Impl::Impl(const GromacsModuleSettings &settings, GromacsModuleInterface* interface, const char* name, const char *description)
    : helpContext(&File::standardError(), gmx::eHelpOutputFormat_Console, NULL, name),
        settings_(settings), interface_(interface), name_(name), description_(description)
{
}


GromacsModule::Impl::~Impl()
{
}


void GromacsModule::Impl::_writeHelp(const Options &options)
{
    CommandLineHelpWriter(options)
        .setShowDescriptions(true)
        .writeHelp(helpContext);
}


void
GromacsModule::Impl::initOptions(Options *options)
{
    interface_->initOptions(options, settings_);
    for(int i = 0; i < subModuleList_.size(); ++i)
        subModuleList_[i].module->initOptions(options);
}


void
GromacsModule::Impl::finishOptions(Options *options)
{
    interface_->finishOptions(options, settings_);
    for(int i = 0; i < subModuleList_.size(); ++i)
        subModuleList_[i].module->finishOptions(options);    
}


void
GromacsModule::Impl::parseOptions(
        SelectionCollection *selections,
        int *argc, char *argv[], bool onlyHelp)
{
    FileNameOptionManager  fileoptManager;
    SelectionOptionManager seloptManager(selections); //TODO: move the whole selection code into the module
    Options                options(NULL, NULL);
    Options                moduleOptions(name_, description_);

    options.addManager(&fileoptManager);
    options.addManager(&seloptManager);
    options.addSubSection(&moduleOptions);
    
    selections->setReferencePosType(settings_.ReferencePosType.c_str());
    initOptions(&moduleOptions);
    
    
    if (!onlyHelp)
    {
        CommandLineParser  parser(&options);
        try
        {
                parser.parse(argc, argv);
        }
        catch (InvalidInputError &err)
        {
            _writeHelp(options);
        }
        options.finish();
    } else
    {
        _writeHelp(options);
        return;
    }
    finishOptions(&moduleOptions);

    const bool bInteractive = File::standardInput().isInteractive();
    //seloptManager.parseRequestedFromStdin(bInteractive);
    
   // selections->compile();
}


/********************************************************************
 * GromacsModule
 */

GromacsModule::GromacsModule(const GromacsModuleSettings &settings, GromacsModuleInterface* interface, 
        const char *name, const char *description)
    : impl_(new Impl(settings, interface, name, description))
{
}


GromacsModule::~GromacsModule()
{
}


const GromacsModuleSettings& GromacsModule::getSettings()
{
    return impl_->settings_;
}


void GromacsModule::addSubModule(GromacsSubModule *SubModule, std::string identifier)
{
    subModuleDescr descr;
    descr.id = identifier;
    descr.module = SubModule;
    impl_->subModuleList_.push_back(descr);
}


GromacsSubModule* GromacsModule::getSubModule(std::string identifier)
{
    for(int i = 0; i < impl_->subModuleList_.size(); ++i)
    {
        if(!identifier.compare(impl_->subModuleList_[i].id))
            return impl_->subModuleList_[i].module;
    }
    
    return NULL;
}


int GromacsModule::run(int argc, char *argv[])
{
    SelectionCollection       selections;
    selections.setDebugLevel(impl_->settings_.iDebuglevel);
    
    impl_->parseOptions(&selections, &argc, argv);
    
    int ret = impl_->interface_->callback(argc, argv, &(*this));
    
    for(int i = 0; i < impl_->subModuleList_.size(); ++i)
        impl_->subModuleList_[i].module->finish();
    
    return ret;
}


void GromacsModule::writeHelp(const CommandLineHelpContext &context)
{
    SelectionCollection       selections;
    selections.setDebugLevel(impl_->settings_.iDebuglevel);
    
    impl_->parseOptions(&selections, NULL, NULL, true);
}

void GromacsModule::init(CommandLineModuleSettings* settings)
{
    for(int i = 0; i < impl_->subModuleList_.size(); ++i)
        impl_->subModuleList_[i].module->initialize(); 
}


/*! \internal \brief
 * Command line module for a trajectory analysis module.
 *
 * \ingroup module_trajectoryanalysis
 */
class GromacsModule::Impl::RunnerCommandLineModule : public CommandLineModuleInterface
{
    public:
        
        RunnerCommandLineModule(const char *name, const char *description, GromacsModule *module)
            : name_(name), description_(description), module_(module)
        {
        }

        virtual const char *name() const { return name_; }
        virtual const char *shortDescription() const { return description_; };

        virtual void init(CommandLineModuleSettings *settings);
        virtual int run(int argc, char *argv[]);
        virtual void writeHelp(const CommandLineHelpContext &context) const;

    private:
        const char             *name_;
        const char             *description_;
        GromacsModule          *module_;

        GMX_DISALLOW_COPY_AND_ASSIGN(RunnerCommandLineModule);
};

void GromacsModule::Impl::RunnerCommandLineModule::init(CommandLineModuleSettings * settings)
{
    module_->init(settings);   
}

int GromacsModule::Impl::RunnerCommandLineModule::run(int argc, char *argv[])
{
    return module_->run(argc, argv);
}

void GromacsModule::Impl::RunnerCommandLineModule::writeHelp(const CommandLineHelpContext &context) const
{
    //TODO: the module manager really should call init before calling writeHelp
    for(int i = 0; i < module_->impl_->subModuleList_.size(); ++i)
        module_->impl_->subModuleList_[i].module->initialize(); 
    
    module_->writeHelp(context);
}

// static
int GromacsModule::runAsMain(int argc, char *argv[])
{
    Impl::RunnerCommandLineModule module(NULL, NULL, &(*this));
    return GromacsModuleManager::runAsMainSingleModule(argc, argv, &module, impl_->settings_);
}

} // namespace gmx
