import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing ('analysis')
# add a list of strings for events to process
options.register ('eventsToProcess',
                                  '',
                                  VarParsing.multiplicity.list,
                                  VarParsing.varType.string,
                                  "Events to process")
options.parseArguments()

process = cms.Process("PickEvent")
process.source = cms.Source ("PoolSource",
          fileNames = cms.untracked.vstring (
              #file name, can be either local or remote. If local, add file: in the front.
'/store/mc/RunIISummer16DR80Premix/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/AODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/C0267563-E3B0-E611-8152-0CC47A7E6AA2.root'
          ),
          eventsToProcess = cms.untracked.VEventRange (
              # options.eventsToProcess
              #event coordinate: run:lumi:event
              '1:3853:10975421',
          )                               
)

process.Out = cms.OutputModule("PoolOutputModule",
        fileName = cms.untracked.string ('scan_8.root')#output file name
)

process.end = cms.EndPath(process.Out)
