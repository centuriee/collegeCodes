// GIAN PAOLO D. PLARIZA; 2022-05204

#include <iostream>
#include <fstream> // for file handling
#include <iomanip> // for table width
#include <windows.h> // for Sleep() commands
using namespace std;

int main(int argc, char* argv[]) {

  cout << "Banker's Algorithm - Safety Algorithm\nProgrammed by: Gian Paolo D. Plariza\n" << endl;

  // open text file for reading
  cout << "Reading data file... ";
  ifstream file(argv[1]);

  // if no file name entered, program prompts for it
  if (file.fail()) {
    cout << "\nError opening file. Include file name upon running code like so: ./name.exe 'filename.txt'" << endl;
    Sleep(500);
    return 1;
  }

  // FILE PARSING PORTION
  int resourceCount, processCount;
  int test;
  string catcher; // to catch unneccessary strings in the code
  
  // get first 2 lines
  file >> resourceCount >> processCount;
  
  // stores instance counts for each instance in an array
  int resourceInstanceCount[resourceCount];
  for (int i = 0; i < resourceCount; i++) file >> resourceInstanceCount[i];
  
  // stores data for allocated resources and maximum resources in a matrix
  int allocatedResources[processCount][resourceCount], maximumResources[processCount][resourceCount];
  for (int i = 0; i < processCount; i++) {
    file >> catcher; // catches strings [P0, P1, P2...]
    for (int j = 0; j < resourceCount; j++) file >> allocatedResources[i][j];
    for (int j = 0; j < resourceCount; j++) file >> maximumResources[i][j];
  }
  
  // need matrix initialization
  int needMatrix[processCount][resourceCount];
  for (int i = 0; i < processCount; i++) {
    for (int j = 0; j < resourceCount; j++) needMatrix[i][j] = maximumResources[i][j] - allocatedResources[i][j];
  }
  
  // naming resources
  string resourceTypeName[resourceCount];
  for (int i = 1; i <= resourceCount; i++) resourceTypeName[i - 1] = "R" + to_string(i);
  
  // naming processes
  string processName[processCount];
  for (int i = 0; i < processCount; i++) processName[i] = "P" + to_string(i);

  Sleep(1500);
  cout << "done!" << endl << endl;
  Sleep(500);
  

  // DATA PRINTING PORTION
  cout << "The data file is interpreted below:" << endl << endl;
  cout << "There are " << resourceCount << " resources with the following number of instances:" << endl << endl;
  
  for (int i = 0; i < resourceCount; i++) cout << "Resource " << i << " (" << resourceTypeName[i] << ") = " << resourceInstanceCount[i] << " instances" << endl;
  Sleep(1500);

  // allocated resources matrix
  cout << endl << "Allocated resources for the " << processCount << " processes:" << endl << endl;
  cout << setw(10) << left;
  cout << "Process";
  
  for (int i = 0; i < resourceCount; i++) {
    cout << setw(4) << left;
    cout << resourceTypeName[i];
  }

  cout << endl;
  for (int i = 0; i < processCount; i++) {
    cout << setw(10) << left;
    cout << processName[i];
    for (int j = 0; j < resourceCount; j++) {
      cout << setw(4) << left;
      cout << allocatedResources[i][j];
    }
    cout << endl;
  }
  Sleep(1500);
  
  // maximum resources matrix
  cout << endl << "Maximum resources for the " << processCount << " processes:" << endl << endl;
  cout << setw(10) << left;
  cout << "Process";
  
  for (int i = 0; i < resourceCount; i++) {
    cout << setw(4) << left;
    cout << resourceTypeName[i];
  }
  
  cout << endl;
  for (int i = 0; i < processCount; i++) {
    cout << setw(10) << left;
    cout << processName[i];
    for (int j = 0; j < resourceCount; j++) {
      cout << setw(4) << left;
      cout << maximumResources[i][j];
    }
    cout << endl;
  }
  Sleep(1500);
  
  // need matrix
  cout << endl << "Need matrix for the " << processCount << " processes:" << endl << endl;
  cout << setw(10) << left;
  cout << "Process";
  
  for (int i = 0; i < resourceCount; i++) {
    cout << setw(4) << left;
    cout << resourceTypeName[i];
  }
  
  cout << endl;
  for (int i = 0; i < processCount; i++) {
    cout << setw(10) << left;
    cout << processName[i];
    for (int j = 0; j < resourceCount; j++) {
      cout << setw(4) << left;
      cout << needMatrix[i][j];
    }
    cout << endl;
  }
  cout << endl;
  Sleep(1500);
  
  // solving available instances
  // SOL'N.: resource instance count - total allocated resources for that specific resource
  int availableInstances[resourceCount];
  for (int i = 0; i < resourceCount; i++) {
    int total = 0;
    for (int j = 0; j < processCount; j++) total += allocatedResources[j][i];
    availableInstances[i] = resourceInstanceCount[i] - total;
  }
  
  cout << "Available instances for each resource type: (";
  for (int i = 0; i < resourceCount; i++) (i != resourceCount - 1) ? cout << resourceTypeName[i] << ", " : cout << resourceTypeName[i];
  cout << ") = (";
  for (int i = 0; i < resourceCount; i++) (i != resourceCount - 1) ? cout << availableInstances[i] << ", " : cout << availableInstances[i];
  cout << ")" << endl << endl;
  Sleep(2000);
  

  // SAFETY ALGORITHM PORTION
  // the algorithm is cyclic in nature. when a process is chosen for a sequence (ex. P2),
  // it does not go back to the starting index (P0), rather it starts where it left off (P3).

  cout << "Running the safety algorithm..." << endl << endl;
  Sleep(1000);
    
  bool isDone[processCount] = {0}; // flags if process has used resources or not
  string safeSequence[processCount]; // stores the safe sequence
  
  int i = 0;
  int startPoint = 0; // for cyclic algo
  int invalidCap = processCount; // if the amount of invalid processes reaches the invalid cap, system is in deadlock

  // while there are still processes waiting for resources
  while (i < processCount) {
    int invalidCount = 0; // records processes that cannot use the currently available resources
    for (int x = startPoint; x < processCount + startPoint; x++) {
      int j = x % processCount; // if x goes beyond processCount, it goes back to 0 and so on
      if (isDone[j] == true) continue; // skip processes that have finished
      bool isValid = true;

      // if the process cannot use the currently available resources, it is flagged as invalid
      for (int k = 0; k < resourceCount; k++) if (availableInstances[k] < needMatrix[j][k]) isValid = false;
      
      if (isValid == false) {
        invalidCount++;
        // if all processes cannot use the available resources, the invalid cap is reached
        // else, continue the algorithm until it successfully finds a process that complies
        if (invalidCount == invalidCap) {
          cout << "The system is deadlocked. There is no safe sequence." << endl;
          Sleep(2000);
          return 1;
        }
        else continue;
      }
      
      else {
        cout << processName[j] << " is given the resources..." << endl;
        Sleep(1000);
        
        // available = available - need
        cout << "Available is (";
        for (int k = 0; k < resourceCount; k++) (k != resourceCount - 1) ? cout << availableInstances[k] << ", " : cout << availableInstances[k];
        cout << ") - (";
        for (int k = 0; k < resourceCount; k++) (k != resourceCount - 1) ? cout << needMatrix[j][k] << ", " : cout << needMatrix[j][k];
        cout << ") = (";
        
        for (int k = 0; k < resourceCount; k++) availableInstances[k] -= needMatrix[j][k];
        for (int k = 0; k < resourceCount; k++) (k != resourceCount - 1) ? cout << availableInstances[k] << ", " : cout << availableInstances[k];
        cout << ")" << endl;
        Sleep(1500);
        
        isDone[j] = true; // process has used available resources
        safeSequence[i] = processName[j]; // add process to the safe sequence
        invalidCap--; // decrease invalid cap
        i++; // increment number of processes that have used available resources
        startPoint += x++; // set startpoint to process after the current one
        
        cout << processName[j] << " is done with the resources..." << endl;
        Sleep(1000);
        
        // available = available - max
        cout << "Available is (";
        for (int k = 0; k < resourceCount; k++) (k != resourceCount - 1) ? cout << availableInstances[k] << ", " : cout << availableInstances[k];
        cout << ") + (";
        for (int k = 0; k < resourceCount; k++) (k != resourceCount - 1) ? cout << maximumResources[j][k] << ", " : cout << maximumResources[j][k];
        cout << ") = (";
        
        for (int k = 0; k < resourceCount; k++) availableInstances[k] += maximumResources[j][k];
        for (int k = 0; k < resourceCount; k++) (k != resourceCount - 1) ? cout << availableInstances[k] << ", " : cout << availableInstances[k];
        cout << ")" << endl << endl;
        Sleep(1500);
        
        // print safe sequence
        cout << "Current safe sequence is <";
        for (int k = 0; k < i; k++) (k != i - 1) ? cout << safeSequence[k] << ", " : cout << safeSequence[k];
        cout << ">" << endl << endl;
        Sleep(1500);

        break; // start again from where the algorithm left off
      }
    }
  }

  file.close();
} 