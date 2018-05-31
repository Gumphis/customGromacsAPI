/* 
 * File:   GromacsModuleManager.h
 * Author: kulkem
 *
 * Created on May 13, 2016, 2:14 PM
 */

#ifndef GROMACSMODULEMANAGER_H
#define	GROMACSMODULEMANAGER_H

#include "gromacs/commandline/cmdlinemodule.h"
#include "gromacs/onlinehelp/helptopicinterface.h"
#include "gromacs/utility/classhelpers.h"
#include "gromacs/utility/uniqueptr.h"
#include "gromacs/legacyheaders/copyrite.h"
#include "gromacs/options/options.h"
#include "gromacs/utility/gmxassert.h"
#include "GromacsModule.h"

namespace gmx
{

class CommandLineModuleGroup;
class CommandLineModuleGroupData;
class CommandLineProgramContext;
class FileOutputRedirectorInterface;

//! Smart pointer type for managing a CommandLineModuleInterface.
typedef gmx_unique_ptr<CommandLineModuleInterface>::type
    CommandLineModulePointer;

class GromacsModuleManager
{
    public:
        //! Function pointer type for a C main function.
        typedef int (*CMainFunction)(int argc, char *argv[]);

        static int runAsMainSingleModule(int argc, char *argv[],
                          CommandLineModuleInterface *module,  const GromacsModuleSettings &settings);
       
        GromacsModuleManager(const char                *binaryName,
                                 CommandLineProgramContext *programContext);
        ~GromacsModuleManager();

        void setQuiet(bool bQuiet);
        void setOutputRedirector(FileOutputRedirectorInterface *output);

        void setSingleModule(CommandLineModuleInterface *module);
        void addModule(CommandLineModulePointer module);
        void addModuleCMain(const char *name, const char *shortDescription,
                            CMainFunction mainFunction);
        template <class Module>
        void registerModule()
        {
            addModule(CommandLineModulePointer(new Module));
        }

        void addHelpTopic(HelpTopicPointer topic);

        int run(int argc, char *argv[],  const GromacsModuleSettings &_settings);

    private:
        class Impl;

        PrivateImplPointer<Impl> impl_;
};

class GromacsCommonOptionsHolder
{
    public:
        GromacsCommonOptionsHolder();
        GromacsCommonOptionsHolder(const GromacsModuleSettings &settings);
        ~GromacsCommonOptionsHolder();

        //! Initializes the common options.
        void initOptions();
        /*! \brief
         * Finishes option parsing.
         *
         * \returns `false` if the wrapper binary should quit without executing
         *     any module.
         */
        bool finishOptions();

        //! Adjust defaults based on module settings.
        void adjustFromSettings(const CommandLineModuleSettings &settings);
        // Adjust the module settings
        void adjustSettings(const GromacsModuleSettings &settings);

        //! Returns the internal Options object.
        Options *options() { return &options_; }
        //! Returns the settings for printing startup information.
        const BinaryInformationSettings &binaryInfoSettings() const
        {
            return binaryInfoSettings_;
        }

        /*! \brief
         * Returns `true` if common options are set such that the wrapper
         * binary should quit, without running the actual module.
         */
        bool shouldIgnoreActualModule() const
        {
            return bHelp_ || bVersion_;
        }
        //! Returns whether common options specify showing help.
        bool shouldShowHelp() const { return bHelp_; }
        //! Returns whether common options specify showing hidden options in help.
        bool shouldShowHidden() const { return bHidden_; }
        //! Returns whether common options specify quiet execution.
        bool shouldBeQuiet() const
        {
            return bQuiet_ && !bVersion_;
        }
        //! Returns whether backups should be made.
        bool shouldBackup() const { return bBackup_; }

        //! Returns the nice level.
        int niceLevel() const { return niceLevel_; }
        //! Returns whether floating-point exception should be enabled
        bool enableFPExceptions() const { return bFpexcept_; }
        //! Returns the debug level.
        int debugLevel() const { return debugLevel_; }

        //! Returns the file to which startup information should be printed.
        FILE *startupInfoFile() const { return (bVersion_ ? stdout : stderr); }

    private:
        Options                      options_;
        //! Settings for what to write in the startup header.
        BinaryInformationSettings    binaryInfoSettings_;
        bool                         bHelp_;
        bool                         bHidden_;
        bool                         bQuiet_;
        bool                         bVersion_;
        bool                         bCopyright_;
        int                          niceLevel_;
        bool                         bBackup_;
        bool                         bFpexcept_;
        int                          debugLevel_;

        GMX_DISALLOW_COPY_AND_ASSIGN(GromacsCommonOptionsHolder);
};

} // namespace gmx

#endif
