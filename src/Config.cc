#include "Config.hh"
using namespace std;

Config::Config() {}

Config::~Config() {}

void Config::Parse(const string& config_file)
{
    UI = G4UImanager::GetUIpointer();
    conf = YAML::LoadFile(config_file);

    if (conf["Project"].IsDefined())
        G4cout << "Configuration file loaded successfully" << G4endl;
    else
        throw config_file;
}

G4bool Config::IsLoad()
{
    return conf["Project"].IsDefined();
}

G4int Config::Run()
{
    // Choose the Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    if (conf["Global"]["useseed"].as<G4bool>())
        CLHEP::HepRandom::setTheSeed(conf["Global"]["seed"].as<G4long>());
    else
        CLHEP::HepRandom::setTheSeed(this->GetTimeNs());
    CLHEP::HepRandom::showEngineStatus();
    G4cout << "seed: " << CLHEP::HepRandom::getTheSeed() << G4endl;

    // Construct the default run manager
    // Verbose output class
    G4VSteppingVerbose::SetInstance(new SteppingVerbose);
    G4RunManager* runManager = new G4RunManager;

    // Set mandatory initialisation classes
    DetectorConstruction* detector = new DetectorConstruction(this);
    if (conf["Global"]["savegeo"].as<G4bool>())
    {
    	G4GDMLParser parser;
    	parser.Write("dss-ecal.gdml",detector->Construct());
    }
    runManager->SetUserInitialization(detector);

//    G4VUserPhysicsList* physics = new QGSP_BERT();
    G4VUserPhysicsList* physics = new FTFP_BERT();
    runManager->SetUserInitialization(physics);

    HistoManager* histo = new HistoManager(conf["Global"]["output"].as<string>().c_str(), conf["Global"]["savegeo"].as<G4bool>());
//    SteppingVerbose* stepV = new SteppingVerbose();

    PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(detector, histo, this);
    runManager->SetUserAction(primary);

    RunAction* runAction = new RunAction(primary, histo, this);
    runManager->SetUserAction(runAction);

    EventAction* eventAction = new EventAction(histo, this);
    runManager->SetUserAction(eventAction);

    TrackingAction* trackingAction = new TrackingAction(runAction, eventAction, this);
    runManager->SetUserAction(trackingAction);

    SteppingAction* steppingAction = new SteppingAction(detector, eventAction);
    runManager->SetUserAction(steppingAction);

    runManager->SetVerboseLevel(conf["Verbose"]["run"].as<G4int>());
    G4String command = "/control/execute ";

    UI->ApplyCommand(G4String("/control/verbose ") + G4String(conf["Verbose"]["control"].as<string>()));
    UI->ApplyCommand(G4String("/tracking/verbose ") + G4String(conf["Verbose"]["tracking"].as<string>()));
    UI->ApplyCommand(G4String("/event/verbose ") + G4String(conf["Verbose"]["event"].as<string>()));

    for (auto subconf : conf["Source"])
        UI->ApplyCommand("/gps/" + subconf.first.as<string>() + " " + subconf.second.as<string>());

    // Initialise G4 kernel
    runManager->Initialize();
    runManager->BeamOn(conf["Global"]["beamon"].as<G4int>());

    // Job termination
    delete runManager;
    if (access("dss-ecal.gdml", F_OK) == 0)
        remove("dss-ecal.gdml");

    return 1;
}

G4int Config::Print()
{
    ofstream fout("./default.yaml");

    fout << "# Notice: In YAML files, indentation matters!" << endl;
    fout << endl;
    fout << "# Project information" << endl;
    fout << "Project: DSS-ECAL" << endl;
    fout << "Author: Ji-Yuan CHEN (SJTU)" << endl;
    fout << "Email: jy_chen@sjtu.edu.cn" << endl;
    fout << endl << endl;
    fout << "# Global set-up" << endl;
    fout << "Global:" << endl;
    fout << "    useseed: true    # True: Use user-specified seed; False: Use system time in ns" << endl;
    fout << "    seed: 2022" << endl;
    fout << endl;
    fout << "    usemac: false    # Currently not applicable" << endl;
    fout << "    mac: ./vis.mac" << endl;
    fout << endl;
    fout << "    output: ./test.root    # Output ROOT file name" << endl;
    fout << "    beamon: 100" << endl;
    fout << "    savegeo: false" << endl;
    fout << endl << endl;
    fout << "# Structure of ECAL" << endl;
    fout << "# Warning: Be careful while editing this section!  Non-standard structures have not been fully tested!" << endl;
    fout << "ECAL:" << endl;
    fout << "    nCrystalInLayer: 21" << endl;
    fout << "    nLayer: 18" << endl;
    fout << endl;
    fout << "    crystalWidth: 2.5    # In cm" << endl;
    fout << "    crystalThick: 2.5    # In cm" << endl;
    fout << endl;
    fout << "    constructPCB: true" << endl;
    fout <<  endl << endl;
    fout << "# Particle source set-up" << endl;
    fout << "Source:" << endl;
    fout << "    particle: \"e-\"" << endl;
    fout << endl;
    fout << "    pos/type: \"Beam\"" << endl;
    fout << "    pos/centre: \"0 0 -100 mm\"" << endl;
    fout << "    pos/sigma_r: \"0 mm\"" << endl;
    fout << endl;
    fout << "    ang/type: \"beam1d\"" << endl;
    fout << "    ang/rot1: \"0 1 0\"" << endl;
    fout << "    ang/rot2: \"1 0 0\"" << endl;
    fout << endl;
    fout << "    ene/type: \"Mono\"" << endl;
    fout << "    ene/mono: \"8 GeV\"" << endl;
    fout << "    ene/sigma: \"0 MeV\"" << endl;
    fout << endl << endl;
    fout << "# Verbose" << endl;
    fout << "Verbose:" << endl;
    fout << "    run: 0" << endl;
    fout << "    control: 0" << endl;
    fout << "    event: 0" << endl;
    fout << "    tracking: 0" << endl;

    fout.close();

    cout << "Configuration file default.yaml successfully created!" << endl;
    return 1;
}
