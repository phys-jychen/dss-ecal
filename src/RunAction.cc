#include "RunAction.hh"
#include "HistoManager.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include <iomanip>

RunAction::RunAction(PrimaryGeneratorAction* kin,HistoManager* histo,Config* c)
 : fPrimary(kin), fHistoManager(histo), config(c)
{}

RunAction::~RunAction()
{ 
//    delete fHistoManager;
}

void RunAction::BeginOfRunAction(const G4Run*)
{ 
    G4cout << "....................00000000000000000000...................." << G4endl;

    // Initialise arrays
    fDecayCount = fTimeCount = 0;
    for (G4int i = 0; i < 3; i++)
        fEkinTot[i] = fPbalance[i] = fEventTime[i] = 0.0;
    fPrimaryTime = 0.0;
          
    // Histograms
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    if ( analysisManager->IsActive() )
        analysisManager->OpenFile();
  
    // Inform the runManager to save random number seed
    G4RunManager::GetRunManager()->SetRandomNumberStore(false);

    fHistoManager->book();
}

void RunAction::ParticleCount(G4String name, G4double Ekin)
{
    fParticleCount[name]++;
    fEmean[name] += Ekin;
    G4cout << ">>>>>>>>>>>> PrimaryEnergy = " << Ekin << " >>>>>>>>>>>>" << G4endl;
//    fHistoManager ->fParticleInfo.fPrimaryEnergy.emplace_back(Ekin);

    // Update minimum and maximum
    if (fParticleCount[name] == 1)
        fEmin[name] = fEmax[name] = Ekin;
    if (Ekin < fEmin[name])
        fEmin[name] = Ekin;
    if (Ekin > fEmax[name])
        fEmax[name] = Ekin;  
    G4cout << "....................11111111111111111111...................." << G4endl;
}

void RunAction::Balance(G4double Ekin, G4double Pbal)
{
    G4cout << "....................22222222222222222222...................." << G4endl;
    fDecayCount++;
    fEkinTot[0] += Ekin;

    // Update minimum and maximum
    if (fDecayCount == 1)
        fEkinTot[1] = fEkinTot[2] = Ekin;
    if (Ekin < fEkinTot[1])
        fEkinTot[1] = Ekin;
    if (Ekin > fEkinTot[2])
        fEkinTot[2] = Ekin;
  
    fPbalance[0] += Pbal;

    // Update minimum and maximum
    if (fDecayCount == 1)
        fPbalance[1] = fPbalance[2] = Pbal;  
    if (Pbal < fPbalance[1])
        fPbalance[1] = Pbal;
    if (Pbal > fPbalance[2])
        fPbalance[2] = Pbal;    
}

void RunAction::EventTiming(G4double time)
{
    G4cout << "....................33333333333333333333...................." << G4endl;
    fTimeCount++;  
//    G4cout << "fTimeCount " << fTimeCount << G4endl;
    fEventTime[0] += time;
    if (fTimeCount == 1)
        fEventTime[1] = fEventTime[2] = time;  
    if (time < fEventTime[1])
        fEventTime[1] = time;
    if (time > fEventTime[2])
        fEventTime[2] = time;             
}

void RunAction::PrimaryTiming(G4double ptime)
{
    G4cout << "....................44444444444444444444...................." << G4endl;
    fPrimaryTime += ptime;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    G4cout << "....................55555555555555555555...................." << G4endl;
    G4int nbEvents = run->GetNumberOfEvent();
    if (nbEvents == 0)
        return;
 
    G4ParticleDefinition* particle = fPrimary->GetParticleGun()->GetParticleDefinition();
    G4String partName = particle->GetParticleName();
    G4double eprimary = fPrimary->GetParticleGun()->GetParticleEnergy();
 
    G4cout << G4endl << "==================== Run Summary ====================" << G4endl;
    G4cout << G4endl << "" << nbEvents << " " << partName << " events of " << G4BestUnit(eprimary, "Energy") << G4endl;
    G4cout << G4endl << "=====================================================" << G4endl << G4endl;

    G4int prec = 4, wid = prec + 2;
    G4int dfprec = G4cout.precision(prec);

    std::map<G4String, G4int>::iterator it;
    for (it = fParticleCount.begin(); it != fParticleCount.end(); it++)
    { 
        G4String name = it->first;
        G4int count   = it->second;
        G4double eMean = fEmean[name] / count;
        G4double eMin = fEmin[name], eMax = fEmax[name];    

        G4cout << "  " << std::setw(13) << name << ": " << std::setw(7) << count
               << "  Emean = " << std::setw(wid) << G4BestUnit(eMean, "Energy")
               << "\t( "  << G4BestUnit(eMin, "Energy")
               << " --> " << G4BestUnit(eMax, "Energy") 
               << ")" << G4endl;           
    }
 
    // Energy momentum balance
    if (fDecayCount > 0)
    {
        G4double Ebmean = fEkinTot[0] / fDecayCount;
        G4double Pbmean = fPbalance[0] / fDecayCount;
            
        G4cout << G4endl << "Ekin Total (Q): mean = "
               << std::setw(wid) << G4BestUnit(Ebmean, "Energy")
               << "\t( "  << G4BestUnit(fEkinTot[1], "Energy")
               << " --> " << G4BestUnit(fEkinTot[2], "Energy")
               << ")" << G4endl;
              
        G4cout << G4endl << "Momentum balance (excluding gamma de-excitation): mean = " 
               << std::setw(wid) << G4BestUnit(Pbmean, "Energy")
               << "\t( "  << G4BestUnit(fPbalance[1], "Energy")
               << " --> " << G4BestUnit(fPbalance[2], "Energy")
               << ")" << G4endl;
    }
            
    // Total time of life
    if (fTimeCount > 0)
    {
        G4cout << fEventTime[0] << " " << fTimeCount << G4endl;
        G4double Tmean = fEventTime[0] / fTimeCount;
        G4double halfLife = Tmean * std::log(2.0);
       
        G4cout << G4endl << "Total time of life : mean = "
               << std::setw(wid) << G4BestUnit(Tmean, "Time")
               << "  half-life = "
               << std::setw(wid) << G4BestUnit(halfLife, "Time")
               << "   ( "  << G4BestUnit(fEventTime[1], "Time")
               << " --> "  << G4BestUnit(fEventTime[2], "Time")
               << ")" << G4endl;
    }
            
    // Activity of primary ion
    G4double pTimeMean = fPrimaryTime / nbEvents;
    G4double molMass = particle->GetAtomicMass() * g / mole;
    G4double nAtoms_perUnitOfMass = Avogadro / molMass;
    G4double Activity_perUnitOfMass = 0.0;
    if (pTimeMean > 0.0)
        Activity_perUnitOfMass = nAtoms_perUnitOfMass / pTimeMean;
   
    G4cout << G4endl << "Activity of " << partName << " = "
           << std::setw(wid)  << Activity_perUnitOfMass * g / becquerel
           << " Bq/g  ("      << Activity_perUnitOfMass * g / curie
           << " Ci/g)" 
           << G4endl << G4endl;
                                            
    // Remove all contents in fParticleCount
    fParticleCount.clear(); 
    fEmean.clear();
    fEmin.clear();
    fEmax.clear();

    // Restore default precision
    G4cout.precision(dfprec);
            
    // Normalise and save histograms
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance(); 
    G4double factor = 100.0 / nbEvents;
    analysisManager->ScaleH1(1, factor);
    analysisManager->ScaleH1(2, factor);
    analysisManager->ScaleH1(3, factor);
    analysisManager->ScaleH1(4, factor);
    analysisManager->ScaleH1(5, factor);   

    if ( analysisManager->IsActive() )
    {
        analysisManager->Write();
        analysisManager->CloseFile();
    } 

    fHistoManager->save();
}