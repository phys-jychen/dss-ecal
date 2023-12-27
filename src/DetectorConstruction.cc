#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4GlobalMagFieldMessenger.hh"

#include "DetectorConstruction.hh"
#include "SteppingAction.hh"

DetectorConstruction::DetectorConstruction(Config* c)
 : G4VUserDetectorConstruction(),
   config(c)
{}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.0, 0.0));
    visAttributes->SetVisibility(false);

    physiWorld = ConstructWorld();

    G4NistManager* nistManager = G4NistManager::Instance();

    // Elements and basic materials
    G4Element* elH = nistManager->FindOrBuildElement("H");
    G4Element* elC = nistManager->FindOrBuildElement("C");
    G4Element* elO = nistManager->FindOrBuildElement("O");
    G4Element* elSi = nistManager->FindOrBuildElement("Si");
    G4Element* elY = nistManager->FindOrBuildElement("Y");
    G4Element* elLu = nistManager->FindOrBuildElement("Lu");
    G4Material* C = nistManager->FindOrBuildMaterial("G4_C");
    G4Material* Si = nistManager->FindOrBuildMaterial("G4_Si");

    // Crystal: LYSO
    G4Material* LSO = new G4Material("LSO", 7.4 * g / cm3, 3);
    LSO->AddElement(elLu, 2);
    LSO->AddElement(elSi, 2);
    LSO->AddElement(elO, 5);
    G4Material* YSO = new G4Material("YSO", 4.5 * g / cm3, 3);
    YSO->AddElement(elY, 2);
    YSO->AddElement(elSi, 1);
    YSO->AddElement(elO, 5);
    G4Material* LYSO = new G4Material("LYSO", 7.1 * g / cm3, 2);
    LYSO->AddMaterial(LSO, 90 * perCent);
    LYSO->AddMaterial(YSO, 10 * perCent);

    // Wrapper: ESR
    G4Material* ESR = new G4Material("ESR", 1.38 * g / cm3, 3);
    ESR->AddElement(elC, 10);
    ESR->AddElement(elH, 8);
    ESR->AddElement(elO, 4);

    // Gap: carbon
    G4Material* sustain = C;

    // SiPM: Si
    G4Material* SiPM = Si;

    // PCB: FR4
    G4Material* quartz = nistManager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    G4Material* epoxy = new G4Material("epoxy", 1.3 * g / cm3, 3);
    epoxy->AddElement(elC, 15);
    epoxy->AddElement(elH, 44);
    epoxy->AddElement(elO, 7);
    G4Material* FR4 = new G4Material("FR4", 1.86 * g / cm3, 2);
    FR4->AddMaterial(quartz, 52.8 * perCent);
    FR4->AddMaterial(epoxy, 47.2 * perCent);

    // Optical adhesive:
    G4Material* glue = epoxy;

    // Parameters from YAML file
    // Crystal
    const G4int nCrystalColumns = config->conf["ECAL"]["nCrystalColumns"].as<G4int>();
    const G4int nCrystalConnect = config->conf["ECAL"]["nCrystalConnect"].as<G4int>();
    const G4int nLayer = config->conf["ECAL"]["nLayer"].as<G4int>();
    const G4double crystalUnitLength = config->conf["ECAL"]["CrystalLength"].as<G4double>() * cm;
    const G4double crystalWidth = config->conf["ECAL"]["CrystalWidth"].as<G4double>() * cm;
    const G4double crystalThick = config->conf["ECAL"]["CrystalThick"].as<G4double>() * cm;
    // Electronics
    const G4bool dualReadout = config->conf["ECAL"]["DualReadout"].as<G4bool>();
    const G4double SiPMLength = config->conf["ECAL"]["SiPMLength"].as<G4double>() * mm;
    const G4double SiPMWidth = config->conf["ECAL"]["SiPMWidth"].as<G4double>() * mm;
    const G4double PCBThick = config->conf["ECAL"]["PCBThick"].as<G4double>() * mm;

    // Exception handling --- very important!
    if (nCrystalColumns * crystalWidth == nCrystalConnect * crystalUnitLength)
    {
        G4cout << "--> Length and width of ECAL match." << G4endl;
        G4cout << "--> Continuing..." << G4endl;
    }
    else
    {
        G4ExceptionDescription msg;
        G4Exception("DetectorConstruction::Construct()", "Length and width of ECAL do no match!", FatalException, msg);
    }

    // Parameters of the components
    G4double ESRThick = 0.3 * mm;
    G4double gap = 0.1 * mm;
    G4double glueLength = crystalWidth;
    G4double glueWidth = crystalThick;
    G4double glueThick = 2 * ESRThick + gap;

    G4double crystalLength = nCrystalConnect * crystalUnitLength + (nCrystalColumns - 1) * glueThick;
    G4double ESROutLength = crystalLength + 2 * ESRThick;
    G4double ESROutWidth = crystalWidth + 2 * ESRThick;
    G4double ESROutThick = crystalThick + 2 * ESRThick;

    G4double gapOutLength = ESROutLength + gap;
    G4double gapOutWidth = ESROutWidth + gap;
    G4double gapOutThick = ESROutThick + gap;

    G4double thickness = gapOutThick;

    G4double SiPMThick = 0.6 * mm;
    G4double PCBLength = ESROutLength;
    G4double PCBWidth = thickness * nLayer;

    G4double crystalPositionZ = 0.5 * ESROutThick;
    G4double gluePositionZ = crystalPositionZ;
    G4double ESRPositionZ = crystalPositionZ;
    G4double gapPositionZ = 0.5 * gapOutThick;
    G4double SiPMPositionZ = ESRPositionZ;
    G4double PCBPositionZ = 0.5 * thickness * nLayer;

    G4bool checkOverlap = false;    // No overlap checking triggered

    // Rotation matrix: 90 degrees along z axis
    G4RotationMatrix rot;
    rot.rotateZ(90 * deg);

    // Crystal & ESR & SiPM
    G4Box* solidCrystal = new G4Box("LYSO",                                                          // Name
                                    0.5 * crystalLength, 0.5 * crystalWidth, 0.5 * crystalThick);    // Size
    G4Box* solidGlue = new G4Box("epoxy",                                                // Name
                                 0.5 * glueThick, 0.5 * glueLength, 0.5 * glueWidth);    // Size
    G4Box* solidESROut = new G4Box("ESR_Out",                                                    // Name
                                   0.5 * ESROutLength, 0.5 * ESROutWidth, 0.5 * ESROutThick);    // Size
    G4Box* solidESRIn = new G4Box("ESR_In",                                                        // Name
                                  0.5 * crystalLength, 0.5 * crystalWidth, 0.5 * crystalThick);    // Size
    G4SubtractionSolid* solidESR = new G4SubtractionSolid("ESR",          // Name
                                                          solidESROut,    // Minuend
                                                          solidESRIn);    // Subtrahend
    G4Box* solidSiPM = new G4Box("SiPM",                                                 // Name
                                 0.5 * SiPMThick, 0.5 * SiPMLength, 0.5 * SiPMWidth);    // Size

    G4LogicalVolume* logicCrystal = new G4LogicalVolume(solidCrystal,    // Solid
                                                        LYSO,            // Material
                                                        "LYSO");         // Name
    G4LogicalVolume* logicGlue = new G4LogicalVolume(solidGlue,    // Solid
                                                     glue,         // Material
                                                     "glue");      // Name
    G4LogicalVolume* logicESR = new G4LogicalVolume(solidESR,    // Solid
                                                    ESR,         // Material
                                                    "ESR");      // Name
    G4LogicalVolume* logicSiPM = new G4LogicalVolume(solidSiPM,    // Solid
                                                     SiPM,         // Material
                                                     "SiPM");      // Name

    for (G4int i_layer = 0; i_layer < nLayer; i_layer++)
    {
        if (i_layer % 2 == 0)
        {
            for (G4int i_y = 0; i_y < nCrystalColumns; i_y++)
            {
                for (G4int i_x = 0; i_x < nCrystalConnect; i_x++)
                {
                    new G4PVPlacement(0,                              // No rotation
                                      G4ThreeVector(-0.5 * crystalLength + 0.5 * crystalUnitLength + i_x * (glueThick + crystalUnitLength),
                                                    -0.5 * gapOutLength + (0.5 + i_y) * gapOutWidth,
                                                    i_layer * thickness + crystalPositionZ),
                                      logicCrystal,                   // Logical volume
                                      "LYSO",                         // Name
                                      logicWorld,                     // Mother volume
                                      false,                          // No Boolean operation
                                      10000 * i_layer + 100 * i_y,    // Copy number
                                      checkOverlap);
                    if (i_x < nCrystalConnect - 1)
                        new G4PVPlacement(0,             // No rotation
                                          G4ThreeVector(-0.5 * crystalLength + crystalUnitLength + 0.5 * glueThick + i_x * (glueThick + crystalUnitLength),
                                                        -0.5 * gapOutLength + (0.5 + i_y) * gapOutWidth,
                                                        i_layer * thickness + gluePositionZ),
                                          logicGlue,     // Logical volume
                                          "glue",        // Name
                                          logicWorld,    // Mother volume
                                          false,         // No Boolean operation
                                          -1,            // Copy number
                                          checkOverlap);
                }
                new G4PVPlacement(0,             // No rotation
                                  G4ThreeVector(0,
                                                -0.5 * gapOutLength + (0.5 + i_y) * gapOutWidth,
                                                i_layer * thickness + ESRPositionZ),
                                  logicESR,      // Logical volume
                                  "ESR",         // Name
                                  logicWorld,    // Mother volume
                                  false,         // No Boolean operation
                                  -1,            // Copy number
                                  checkOverlap);
                new G4PVPlacement(0,             // No rotation
                                  G4ThreeVector(0.5 * gapOutLength + 0.5 * SiPMThick,
                                                -0.5 * gapOutLength + (0.5 + i_y) * gapOutWidth,
                                                i_layer * thickness + SiPMPositionZ),
                                  logicSiPM,     // Logical volume
                                  "SiPM",        // Name
                                  logicWorld,    // Mother volume
                                  false,         // No Boolean operation
                                  -1,            // Copy number
                                  checkOverlap);
                if (dualReadout)
                    new G4PVPlacement(0,             // No rotation
                                      G4ThreeVector(-0.5 * gapOutLength - 0.5 * SiPMThick,
                                                    -0.5 * gapOutLength + (0.5 + i_y) * gapOutWidth,
                                                    i_layer * thickness + SiPMPositionZ),
                                      logicSiPM,     // Logical volume
                                      "SiPM",        // Name
                                      logicWorld,    // Mother volume
                                      false,         // No Boolean operation
                                      -1,            // Copy number
                                      checkOverlap);
            }
        }
        else if (i_layer % 2 == 1)
        {
            for (G4int i_x = 0; i_x < nCrystalColumns; i_x++)
            {
                for (G4int i_y = 0; i_y < nCrystalConnect; i_y++)
                {
                    new G4PVPlacement(G4Transform3D(rot,        // Rotation matrix
                                                    G4ThreeVector(-0.5 * gapOutLength + (0.5 + i_x) * gapOutWidth,
                                                                  -0.5 * crystalLength + 0.5 * crystalUnitLength + i_y * (glueThick + crystalUnitLength),
                                                                  i_layer * thickness + crystalPositionZ)),
                                      logicCrystal,             // Logical volume
                                      "LYSO",                   // Name
                                      logicWorld,               // Mother volume
                                      false,                    // No Boolean operation
                                      10000 * i_layer + i_x,    // Copy number
                                      checkOverlap);
                    if (i_y < nCrystalConnect - 1)
                        new G4PVPlacement(G4Transform3D(rot,   // Rotation matrix
                                                        G4ThreeVector(-0.5 * gapOutLength + (0.5 + i_x) * gapOutWidth,
                                                                      -0.5 * crystalLength + crystalUnitLength + 0.5 * glueThick + i_y * (glueThick + crystalUnitLength),
                                                                      i_layer * thickness + gluePositionZ)),
                                          logicGlue,           // Logical volume
                                          "glue",              // Name
                                          logicWorld,          // Mother volume
                                          false,               // No Boolean operation
                                          -1,                  // Copy number
                                          checkOverlap);
                }
                new G4PVPlacement(G4Transform3D(rot,    // Rotation matrix
                                                G4ThreeVector(-0.5 * gapOutLength + (0.5 + i_x) * gapOutWidth,
                                                              0,
                                                              i_layer * thickness + ESRPositionZ)),
                                  logicESR,             // Logical volume
                                  "ESR",                // Name
                                  logicWorld,           // Mother volume
                                  false,                // No Boolean operation
                                  -1,                   // Copy number
                                  checkOverlap);
                new G4PVPlacement(G4Transform3D(rot,    // No rotation
                                                G4ThreeVector(-0.5 * gapOutLength + (0.5 + i_x) * gapOutWidth,
                                                              0.5 * gapOutLength + 0.5 * SiPMThick,
                                                              i_layer * thickness + SiPMPositionZ)),
                                  logicSiPM,            // Logical volume
                                  "SiPM",               // Name
                                  logicWorld,           // Mother volume
                                  false,                // No Boolean operation
                                  -1,                   // Copy number
                                  checkOverlap);
                if (dualReadout)
                    new G4PVPlacement(G4Transform3D(rot,    // No rotation
                                                    G4ThreeVector(-0.5 * gapOutLength + (0.5 + i_x) * gapOutWidth,
                                                                  -0.5 * gapOutLength - 0.5 * SiPMThick,
                                                                  i_layer * thickness + SiPMPositionZ)),
                                      logicSiPM,            // Logical volume
                                      "SiPM",               // Name
                                      logicWorld,           // Mother volume
                                      false,                // No Boolean operation
                                      -1,                   // Copy number
                                      checkOverlap);
            }
        }
    }

    // Gap
    G4Box* solidGapOut = new G4Box("Gap_Out",                                                    // Name
                                   0.5 * gapOutLength, 0.5 * gapOutWidth, 0.5 * gapOutThick);    // Size
    G4Box* solidGapIn = new G4Box("Gap_In",                                                     // Name
                                  0.5 * ESROutLength, 0.5 * ESROutWidth, 0.5 * ESROutThick);    // Size
    G4SubtractionSolid* solidGap = new G4SubtractionSolid("Gap",          // Name
                                                          solidGapOut,    // Minuend
                                                          solidGapIn);    // Subtrahend

    G4LogicalVolume* logicGap = new G4LogicalVolume(solidGap,    // Solid
                                                    sustain,     // Material
                                                    "Gap");      // Name

    for (G4int i_layer = 0; i_layer < nLayer; i_layer++)
    {
        if (i_layer % 2 == 0)
            for (G4int i_y = 0; i_y < nCrystalColumns; i_y++)
                new G4PVPlacement(0,             // No rotation
                                  G4ThreeVector(0,
                                                -0.5 * gapOutLength + (0.5 + i_y) * gapOutWidth,
                                                i_layer * thickness + gapPositionZ),
                                  logicGap,      // Logical volume
                                  "Gap",         // Name
                                  logicWorld,    // Mother volume
                                  false,         // No Boolean operation
                                  -1,            // Copy number
                                  checkOverlap);
        else if (i_layer % 2 == 1)
            for (G4int i_x = 0; i_x < nCrystalColumns; i_x++)
                new G4PVPlacement(G4Transform3D(rot,    // Rotation matrix
                                                G4ThreeVector(-0.5 * gapOutLength + (0.5 + i_x) * gapOutWidth,
                                                              0,
                                                              i_layer * thickness + gapPositionZ)),
                                  logicGap,             // Logical volume
                                  "Gap",                // Name
                                  logicWorld,           // Mother volume
                                  false,                // No Boolean operation
                                  -1,                   // Copy number
                                  checkOverlap);
    }

    // PCB
    G4Box* solidPCB = new G4Box("FR4",                                               // Name
                                0.5 * PCBThick, 0.5 * PCBLength, 0.5 * PCBWidth);    // Size
    G4LogicalVolume* logicPCB = new G4LogicalVolume(solidPCB,    // Solid
                                                    FR4,         // Material
                                                    "FR4");      // Name
    new G4PVPlacement(0,             // No rotation
                      G4ThreeVector(0.5 * gapOutLength + SiPMThick + 0.5 * PCBThick,
                                    0,
                                    PCBPositionZ),
                      logicPCB,      // Logical volume
                      "FR4",         // Name
                      logicWorld,    // Mother volume
                      false,         // No Boolean operation
                      -1,            // Copy number
                      checkOverlap);
    new G4PVPlacement(G4Transform3D(rot,    // Rotation matrix
                                    G4ThreeVector(0,
                                                  0.5 * gapOutLength + SiPMThick + 0.5 * PCBThick,
                                                  PCBPositionZ)),
                      logicPCB,             // Logical volume
                      "FR4",                // Name
                      logicWorld,           // Mother volume
                      false,                // No Boolean operation
                      -1,                   // Copy number
                      checkOverlap);
    if (dualReadout)
    {
        new G4PVPlacement(0,             // No rotation
                          G4ThreeVector(-0.5 * gapOutLength - SiPMThick - 0.5 * PCBThick,
                                        0,
                                        PCBPositionZ),
                          logicPCB,      // Logical volume
                          "FR4",         // Name
                          logicWorld,    // Mother volume
                          false,         // No Boolean operation
                          -1,            // Copy number
                          checkOverlap);
        new G4PVPlacement(G4Transform3D(rot,    // Rotation matrix
                                        G4ThreeVector(0,
                                                      -0.5 * gapOutLength - SiPMThick - 0.5 * PCBThick,
                                                      PCBPositionZ)),
                          logicPCB,             // Logical volume
                          "FR4",                // Name
                          logicWorld,           // Mother volume
                          false,                // No Boolean operation
                          -1,                   // Copy number
                          checkOverlap);
    }

//    logicAbsorber->SetVisAttributes(visAttributes);
//    (0.9, 0.9, 0.0)yellow

//    visAttributes = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));    // Green
//    logicPCB->SetVisAttributes(visAttributes);

//    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));    // White
//    logicCrystal->SetVisAttributes(visAttributes);

//    SteppingAction* steppingAction = SteppingAction::Instance();

    return physiWorld;
}

G4VPhysicalVolume* DetectorConstruction::ConstructWorld()
{
    G4NistManager* nistManager = G4NistManager::Instance();
    G4Material* Vacuum = nistManager->FindOrBuildMaterial("G4_Galactic");
    G4bool checkOverlaps = false;    // No overlap checking triggered

    // Full sphere shape
    G4double solidWorld_rmax = 200 * cm;
    G4Orb* solidWorld = new G4Orb("World",             // Name
                                  solidWorld_rmax);    // Size

    logicWorld = new G4LogicalVolume(solidWorld,    // Solid
                                     Vacuum,        // Material
                                     "World");      // Name
    G4VPhysicalVolume* physicalWorld = new G4PVPlacement(0,                  // No rotation
                                                         G4ThreeVector(),    // At (0,0,0)
                                                         logicWorld,         // Logical volume
                                                         "World",            // Name
                                                         0,                  // Mother volume
                                                         false,              // No boolean operation
                                                         0,                  // Copy number
                                                         checkOverlaps);
    logicWorld->SetVisAttributes(visAttributes);

    return physicalWorld;
}
