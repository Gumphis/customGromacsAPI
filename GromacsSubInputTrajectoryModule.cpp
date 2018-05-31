/* 
 * File:   GromacsSubInputTrajectoryModule.cpp
 * Author: kulkem
 * 
 * Created on May 18, 2016, 1:28 PM
 */

#include "gromacs/fileio/trxio.h"
#include "gromacs/fileio/trx.h"
#include "gromacs/options.h"
#include "gromacs/options/filenameoptionmanager.h"
#include "gromacs/pbcutil/pbc.h"
#include "gromacs/pbcutil/rmpbc.h"
#include "GromacsSubInputTrajectoryModule.h"
#include "GromacsSubInputTopologyModule.h"

namespace gmx
{
    
void GromacsSubInputTrajectoryModule::initialize()
{
    if(!settings_.bAllowGro && !settings_.bAllowPdb && !settings_.bAllowTrr && !settings_.bAllowXtc) settings_.bAllowXtc = true;
    if(settings_.brmPBC && !settings_.topModule) settings_.brmPBC = false;
    if(settings_.bOTfName && !settings_.cOTfName) settings_.bOTfName = false;
    if(settings_.bOTrmPBC && !settings_.cOTrmPBC) settings_.bOTrmPBC = false;
}

void GromacsSubInputTrajectoryModule::initOptions(Options* options)
{
    if (settings_.bOTfName)
        options->addOption(FileNameOption(settings_.cOTfName).filetype(gmx::eftTrajectory).inputFile().
        defaultBasename(settings_.cfName.c_str()).description("trajectory for the analysis").store(&settings_.cfName));
    if (settings_.bOTrmPBC)
        options->addOption(BooleanOption(settings_.cOTrmPBC).defaultValue(settings_.brmPBC).
        store(&settings_.brmPBC).description("remove PBC"));
}

int GromacsSubInputTrajectoryModule::start()
{
    gmx_bool bFrame;
    t_trxstatus* fpStatus;
    output_env_t output;
    output_env_init_default(&output);
    t_trxframe frame;
    gmx_rmpbc_t rmpbc;
    t_idef* idef = NULL;
    const GromacsTopologyInformation& topol = settings_.topModule->getTopology();
    
    if(settings_.brmPBC)
    { 
        if(!topol.hasTopology())
            settings_.topModule->start();

        idef = &topol.topology()->idef;
    }
    
    settings_.interface.initAnalysis(settings_, topol);
    
    int frnr = 0;

    int nAtoms = read_first_frame(output, &fpStatus, settings_.cfName.c_str(), &frame, TRX_NEED_X);

    settings_.interface.initAfterFirstFrame(settings_,frame);
    
    if(settings_.brmPBC)
    {
        rmpbc = gmx_rmpbc_init(idef, frame.ePBC, frame.natoms);
    }

    do
    {
        if(settings_.brmPBC)
           gmx_rmpbc_trxfr( rmpbc, &frame);
        frnr++;
        settings_.interface.analyzeFrame(frnr, frame);
    }  while(read_next_frame(output, fpStatus, &frame));

    if(settings_.brmPBC)
        gmx_rmpbc_done(rmpbc);

    close_trj(fpStatus);
    
    settings_.interface.finishAnalysis();
    
    //    const TopologyInformation &topology = common.topologyInformation();
/*    module->initAnalysis(settings, topology);

    // Load first frame.
    common.initFirstFrame();
    module->initAfterFirstFrame(settings, common.frame());

    t_pbc  pbc;
    t_pbc *ppbc = settings.hasPBC() ? &pbc : NULL;

    int    nframes = 0;
    AnalysisDataParallelOptions         dataOptions;
    TrajectoryAnalysisModuleDataPointer pdata(
            module->startFrames(dataOptions, selections));
    do
    {
        common.initFrame();
        t_trxframe &frame = common.frame();
        if (ppbc != NULL)
        {
            set_pbc(ppbc, topology.ePBC(), frame.box);
        }

        selections.evaluate(&frame, ppbc);
        module->analyzeFrame(nframes, frame, ppbc, pdata.get());
        module->finishFrameSerial(nframes);

        ++nframes;
    }
    while (common.readNextFrame());
    module->finishFrames(pdata.get());
    if (pdata.get() != NULL)
    {
        pdata->finish();
    }
    pdata.reset();

    if (common.hasTrajectory())
    {
        fprintf(stderr, "Analyzed %d frames, last time %.3f\n",
                nframes, common.frame().time);
    }
    else
    {
        fprintf(stderr, "Analyzed topology coordinates\n");
    }

    // Restore the maximal groups for dynamic selections.
    selections.evaluateFinal(nframes);

    module->finishAnalysis(nframes);
    module->writeOutput();
*/
    
    return 0;
}

void GromacsSubInputTrajectoryModule::finishOptions(Options* options)
{
    
}

void GromacsSubInputTrajectoryModule::finish()
{
    
}

} // name space

