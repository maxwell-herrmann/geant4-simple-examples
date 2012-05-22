//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "NXUserDetectorConstruction.hh"
#include "NXUIMessenger.hh"
#include "NXChamberParameterisation.hh"
#include "NXMagneticField.hh"
#include "NXSensitiveDetector.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4SDManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"

#include "G4NistManager.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NXUserDetectorConstruction::NXUserDetectorConstruction() :
    solidWorld(0),  logicWorld(0),  physiWorld(0),
    solidTarget(0), logicTarget(0), physiTarget(0), 
    solidTracker(0),logicTracker(0),physiTracker(0), 
    solidChamber(0),logicChamber(0),physiChamber(0), 
    TargetMater(0), ChamberMater(0),chamberParam(0),
    stepLimit(0), fpMagField(0),
    fWorldLength(0.),  fTargetLength(0.), fTrackerLength(0.),
    NbOfChambers(0) ,  ChamberWidth(0.),  ChamberSpacing(0.)
{
    //In NXMagneticField, the constructor does everything. now there is no MagneticField, because there is no G4threevector variable in () and it mean the MagneticField is zero.
    fpMagField = new NXMagneticField();
    detectorMessenger = new NXUIMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NXUserDetectorConstruction::~NXUserDetectorConstruction()
{
    delete fpMagField;
    delete stepLimit;
    delete chamberParam;
    delete detectorMessenger;             
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* NXUserDetectorConstruction::Construct()
{
    //--------- Material definition ---------

    G4double a, z;
    G4double density, temperature, pressure;
    G4int nel;

    //Air
    G4Element* N = new G4Element("Nitrogen", "N", z=7., a= 14.01*g/mole);
    G4Element* O = new G4Element("Oxygen"  , "O", z=8., a= 16.00*g/mole);

    G4Material* Air = new G4Material("Air", density= 1.29*mg/cm3, nel=2);
    Air->AddElement(N, 70*perCent);
    Air->AddElement(O, 30*perCent);

    //Lead
    G4Material* Pb = new G4Material("Lead", z=82., a= 207.19*g/mole, density= 11.35*g/cm3);

    //Xenon gas
    G4Material* Xenon = new G4Material("XenonGas", z=54., a=131.29*g/mole, density= 5.458*mg/cm3, kStateGas, temperature= 293.15*kelvin, pressure= 1*atmosphere);

    G4NistManager* man=G4NistManager::Instance();
    G4Material* PMMANist=man->FindOrBuildMaterial("G4_PLEXIGLASS");
    G4Material* Fe=man->FindOrBuildMaterial("G4_Fe");
    G4Material* Ta=man->FindOrBuildMaterial("G4_Ta");

    // Print all the materials defined.
    //
    G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;


    //--------- Definitions of Solids, Logical Volumes, Physical Volumes ---------

    //------------------------------ 
    // World
    //------------------------------ 

    fWorldLength= 1000*cm;
    G4double HalfWorldLength = 0.5*fWorldLength;

    G4GeometryManager::GetInstance()->SetWorldMaximumExtent(fWorldLength);
    G4cout << "Computed tolerance = "
        << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
        << " mm" << G4endl;

    solidWorld= new G4Box("world",HalfWorldLength,HalfWorldLength,HalfWorldLength);
    logicWorld= new G4LogicalVolume( solidWorld, Air, "World", 0, 0, 0);

    //  Must place the World Physical volume unrotated at (0,0,0).
    // 
    physiWorld = new G4PVPlacement(0,               // no rotation
            G4ThreeVector(), // at (0,0,0)
            logicWorld,      // its logical volume
            "World",         // its name
            0,               // its mother  volume
            false,           // no boolean operations
            0);              // copy number

    //------------------------------ 
    // Target
    //------------------------------

    TargetMater  = Pb;
    G4ThreeVector positionTarget = G4ThreeVector(0,0,-100*cm);
    fTargetLength = 1*cm;

    solidTarget = new G4Box("target", fTargetLength/2, fTargetLength/2, fTargetLength/2);
    logicTarget = new G4LogicalVolume(solidTarget,TargetMater,"Target",0,0,0);
    physiTarget = new G4PVPlacement(0,               // no rotation
            positionTarget,  // at (x,y,z)
            logicTarget,     // its logical volume				  
            "Target",        // its name
            logicWorld,      // its mother  volume
            false,           // no boolean operations
            0);              // copy number 

    G4cout << "Target is " << fTargetLength/cm << " cm of " 
        << TargetMater->GetName() << G4endl;

    //------------------------------ 
    // Tracker
    //------------------------------

    fTrackerLength = 5*cm; // Full length of Tracker
    G4double trackerSize = 0.5*fTrackerLength;   // Half length of the Tracker
    G4ThreeVector positionTracker = G4ThreeVector(0,0,0);

    solidTracker = new G4Box("tracker",trackerSize,trackerSize,trackerSize);
    logicTracker = new G4LogicalVolume(solidTracker , Air, "Tracker",0,0,0);  
    physiTracker = new G4PVPlacement(0,              // no rotation
            positionTracker, // at (x,y,z)
            logicTracker,    // its logical volume				  
            "Tracker",       // its name
            logicWorld,      // its mother  volume
            false,           // no boolean operations
            0);              // copy number 

    //------------------------------ 
    // Tracker segments
    //------------------------------
    // 
    // An example of Parameterised volumes
    // dummy values for G4Box -- modified by parameterised volume

    NbOfChambers = 1;
    ChamberWidth = 20*cm;
    ChamberSpacing = 80*cm;
    ChamberMater = Xenon;

    solidChamber = new G4Box("chamber", 100*cm, 100*cm, 10*cm); 
    logicChamber = new G4LogicalVolume(solidChamber,ChamberMater,"Chamber",0,0,0);

    G4double firstPosition = 0;
    G4double firstLength = 0;
    G4double lastLength  = 0;

    chamberParam = new NXChamberParameterisation(  firstPosition);

    // dummy value : kZAxis -- modified by parameterised volume
    //
    physiChamber = new G4PVParameterised(
            "Chamber",       // their name
            logicChamber,    // their logical volume
            logicTracker,    // Mother logical volume
            kZAxis,          // Are placed along this axis 
            NbOfChambers,    // Number of chambers
            chamberParam);   // The parametrisation

    G4cout << "There are " << NbOfChambers << " chambers in the tracker region. "
        << "The chambers are " << ChamberWidth/mm << " mm of " 
        << ChamberMater->GetName() << "\n The distance between chamber is "
        << ChamberSpacing/cm << " cm" << G4endl;

    //------------------------------------------------ 
    // Sensitive detectors
    //------------------------------------------------ 

    G4SDManager* SDman = G4SDManager::GetSDMpointer();

    G4String trackerChamberSDname = "ExN02/TrackerChamberSD";
    NXSensitiveDetector* aTrackerSD = new NXSensitiveDetector( trackerChamberSDname );
    SDman->AddNewDetector( aTrackerSD );
    logicChamber->SetSensitiveDetector( aTrackerSD );

    //--------- Visualization attributes -------------------------------

    G4VisAttributes* BoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    logicWorld  ->SetVisAttributes(BoxVisAtt);  
    logicTarget ->SetVisAttributes(BoxVisAtt);
    logicTracker->SetVisAttributes(BoxVisAtt);

    G4VisAttributes* ChamberVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
    logicChamber->SetVisAttributes(ChamberVisAtt);

    //--------- example of User Limits -------------------------------

    // below is an example of how to set tracking constraints in a given
    // logical volume(see also in NXPhysicsList how to setup the processes
    // G4StepLimiter or G4UserSpecialCuts).

    // Sets a max Step length in the tracker region, with G4StepLimiter
    //
    G4double maxStep = 0.5*ChamberWidth;
    stepLimit = new G4UserLimits(maxStep);
    logicTracker->SetUserLimits(stepLimit);

    // Set additional contraints on the track, with G4UserSpecialCuts
    //
    // G4double maxLength = 2*fTrackerLength, maxTime = 0.1*ns, minEkin = 10*MeV;
    // logicTracker->SetUserLimits(new G4UserLimits(maxStep,maxLength,maxTime,
    //                                               minEkin));

    return physiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NXUserDetectorConstruction::setTargetMaterial(G4String materialName)
{
    // search the material by its name 
    G4Material* pttoMaterial = G4Material::GetMaterial(materialName);  
    if (pttoMaterial)
    {TargetMater = pttoMaterial;
        logicTarget->SetMaterial(pttoMaterial); 
        G4cout << "\n----> The target is " << fTargetLength/cm << " cm of "
            << materialName << G4endl;
    }             
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NXUserDetectorConstruction::setChamberMaterial(G4String materialName)
{
    // search the material by its name 
    G4Material* pttoMaterial = G4Material::GetMaterial(materialName);  
    if (pttoMaterial)
    {ChamberMater = pttoMaterial;
        logicChamber->SetMaterial(pttoMaterial); 
        G4cout << "\n----> The chambers are " << ChamberWidth/cm << " cm of "
            << materialName << G4endl;
    }             
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NXUserDetectorConstruction::SetMagField(G4double fieldValue)
{
    fpMagField->SetFieldValue(fieldValue);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NXUserDetectorConstruction::SetMaxStep(G4double maxStep)
{
    if ((stepLimit)&&(maxStep>0.)) stepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
