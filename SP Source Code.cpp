#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>

using namespace std;

struct Vehicle {
    int    id;
    string ownerName;
    string plateNumber;
    string type;
    string brand;
    string color;
    string nationalID;
    string phone;
    time_t entryTime;
    bool   isActive;
    int    subType;
    bool   isVIP;
    bool   wantsCleaning;
    bool   isFamily;
    int    familyGroupID;
};

struct Slot {
    int    slotID;
    string slotType;
    bool   isOccupied;
    int    vehicleID;
    int    floor;
};

struct Ticket {
    int    ticketID;
    int    vehicleID;
    int    slotID;
    time_t entryTime;
    time_t exitTime;
    double fee;
    bool   isActive;
    int    subType;
};

struct Admin {
    int    id;
    string username;
    string password;
};

struct LogEntry {
    string action;
    string ownerName;
    string plateNumber;
    int    floor;
    int    slot;
    bool   isVIP;
    bool   isFamily;
    int    subType;
    double amount;
    time_t timestamp;
};

struct PricingRules {
    double firstHourPrice;
    double extraHourPrice;
    double dailyPrice;
    double dailyCleanExtra;
    double monthlyMorning;
    double monthlyMorningClean;
    double monthlyOvernight;
    double monthlyOvernightClean;
    double familyBase;
    double familyClean;
    double cleaningHourly;
    double vipAddOn;
    double vipAddOnClean;
};

// ==================== GLOBALS ====================

Vehicle      vehicles[100];
int          vehicleCount = 0;
Slot         slots[100];
Ticket       tickets[200];
int          ticketCount = 0;
Admin        admins[3];
int          adminCount = 0;
LogEntry     logs[500];
int          logCount = 0;
PricingRules pricing;
double       todayProfit = 0;
double       weeklyProfit = 0;
double       monthlyProfit = 0;
bool         loggedIn = false;
int          loggedInAdminID = -1;
int          familyGroupCounter = 0;

// ==================== VALIDATION HELPERS ==================== (d0)

bool isAlphaOnly(string s) {
    for (int i = 0; i < s.length(); i++)
        if (!isalpha(s[i]) && s[i] != ' ') return false;
    return true;
}

bool isDigitsOnly(string s) {
    for (int i = 0; i < s.length(); i++)
        if (!isdigit(s[i])) return false;
    return true;
}

bool isValidPlate(string s) {
    if (s.length() < 6 || s.length() > 7) return false;
    for (int i = 0; i < s.length(); i++)
        if (!isalnum(s[i]) && s[i] != '-') return false;
    return true;
}

bool isValidPhone(string s) {
    if (s.length() < 14 && s.length() > 10) return false;
    for (int i = 0; i < s.length(); i++)
        if (!isdigit(s[i]) && s[i] != '+') return false;
    return true;
}

bool isValidNationalID(string s) {
    if (s.length() != 14) return false;
    return isDigitsOnly(s);
}

bool plateExists(string plate) {
    for (int i = 0; i < vehicleCount; i++)
        if (vehicles[i].plateNumber == plate && vehicles[i].isActive)
            return true;
    return false;
}

// ==================== INPUT HELPERS ==================== (d0)

string readNonEmptyLine(string prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        if (!input.empty()) return input;
        cout << "  [!] Cannot be empty. Try again.\n";
    }
}

int readIntChoice(string prompt, int minVal, int maxVal) {
    int choice;
    while (true) {
        cout << prompt;
        cin >> choice;
        if (!cin.fail() && choice >= minVal && choice <= maxVal) {
            cin.ignore();
            return choice;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "  [!] Enter a number between " << minVal << " and " << maxVal << ".\n";
    }
}

string readName(string prompt) {
    string input;
    while (true) {
        input = readNonEmptyLine(prompt);
        if (input.length() >= 2 && input.length() <= 50 && isAlphaOnly(input))
            return input;
        cout << "  [!] Name must be 2-50 letters only. Try again.\n";
    }
}

string readPlate(string prompt) {
    string input;
    while (true) {
        input = readNonEmptyLine(prompt);
        if (isValidPlate(input))
            return input;
        cout << "  [!] Plate must be 1-6 alphanumeric characters (e.g. ABC123). Try again.\n";
    }
}

string readPlateNew(string prompt) {
    string input;
    while (true) {
        input = readPlate(prompt);
        if (!plateExists(input))
            return input;
        cout << "  [!] A vehicle with this plate is already inside. Try again.\n";
    }
}

string readPhone(string prompt) {
    string input;
    while (true) {
        input = readNonEmptyLine(prompt);
        if (isValidPhone(input))
            return input;
        cout << "  [!] Phone must be 7-15 digits (+ allowed at start). Try again.\n";
    }
}

string readNationalID(string prompt) {
    string input;
    while (true) {
        input = readNonEmptyLine(prompt);
        if (isValidNationalID(input))
            return input;
        cout << "  [!] National ID must be exactly 14 digits. Try again.\n";
    }
}

string readBrand(string prompt) {
    string input;
    while (true) {
        input = readNonEmptyLine(prompt);
        if (input.length() >= 2 && input.length() <= 30)
            return input;
        cout << "  [!] Brand must be 2-30 characters. Try again.\n";
    }
}

string readColor(string prompt) {
    string input;
    while (true) {
        input = readNonEmptyLine(prompt);
        if (input.length() >= 2 && input.length() <= 20 && isAlphaOnly(input))
            return input;
        cout << "  [!] Color must be 2-20 letters only. Try again.\n";
    }
}

string readVehicleType(string prompt) {
    string input;
    while (true) {
        input = readNonEmptyLine(prompt);
        if (input == "Car" || input == "Motorcycle" || input == "Truck")
            return input;
        cout << "  [!] Enter exactly: Car, Motorcycle, or Truck.\n";
    }
}


//ely taht de ashan el pricing (d0)
double readPositiveDouble(string prompt) {
    double val;
    while (true) {
        cout << prompt;
        cin >> val;
        if (!cin.fail() && val > 0) {
            cin.ignore();
            return val;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "  [!] Must be a positive number. Try again.\n";
    }
}

// ==================== UTILITY ==================== (d0)

string subName(int sub) {
    if (sub == 1) return "Hourly";
    if (sub == 2) return "Daily (24h)";
    if (sub == 3) return "Monthly Morning";
    if (sub == 4) return "Monthly Overnight";
    if (sub == 5) return "Family Bundle";
    return "Unknown";
}

int findFreeSlot(int floorNum) {
    int start = (floorNum - 1) * 25;
    int end = start + 25;
    for (int i = start; i < end; i++)
        if (!slots[i].isOccupied) return i;
    return -1;
}

// ==================== PRICING ==================== (d0)

double calculatePrice(int sub, bool wantsCleaning, bool isVIP, time_t entryTime) {
    double base = 0;

    if (sub == 1) {
        time_t now = time(0);
        double hours = difftime(now, entryTime) / 3600.0;
        if (hours < 1.0) hours = 1.0;
        base = pricing.firstHourPrice + (hours - 1.0) * pricing.extraHourPrice;
        if (wantsCleaning) base += pricing.cleaningHourly;
    }
    else if (sub == 2) {
        base = wantsCleaning ? (pricing.dailyPrice + pricing.dailyCleanExtra) : pricing.dailyPrice;
    }
    else if (sub == 3) {
        base = wantsCleaning ? pricing.monthlyMorningClean : pricing.monthlyMorning;
    }
    else if (sub == 4) {
        base = wantsCleaning ? pricing.monthlyOvernightClean : pricing.monthlyOvernight;
    }
    else if (sub == 5) {
        base = wantsCleaning ? pricing.familyClean : pricing.familyBase;
    }

    if (isVIP) base += wantsCleaning ? pricing.vipAddOnClean : pricing.vipAddOn;
    return base;
}

void displayPricing(int v) {
    double price = calculatePrice(vehicles[v].subType, vehicles[v].wantsCleaning, vehicles[v].isVIP, vehicles[v].entryTime);

    cout << "\n  ---- Pricing Summary ----\n";
    cout << "  Subscription : " << subName(vehicles[v].subType) << "\n";

    if (vehicles[v].subType == 1) {
        time_t now = time(0);
        double hours = difftime(now, vehicles[v].entryTime) / 3600.0;
        if (hours < 1.0) hours = 1.0;
        cout << "  Hours Parked : " << hours << "h\n";
    }

    if (vehicles[v].subType == 5)
        cout << "  Bundle Total : " << (vehicles[v].wantsCleaning ? pricing.familyClean : pricing.familyBase) << " EGP (2 cars)\n";
    else
        cout << "  Price        : " << price << " EGP\n";

    if (vehicles[v].isVIP)
        cout << "  VIP Add-on   : +" << (vehicles[v].wantsCleaning ? pricing.vipAddOnClean : pricing.vipAddOn) << " EGP\n";

    cout << "  -------------------------\n";
}

// ==================== LOG ==================== (d00)

void addLog(string action, int vehicleIdx, double amount) {
    if (logCount >= 500) return;
    logs[logCount].action = action;
    logs[logCount].ownerName = vehicles[vehicleIdx].ownerName;
    logs[logCount].plateNumber = vehicles[vehicleIdx].plateNumber;
    logs[logCount].isVIP = vehicles[vehicleIdx].isVIP;
    logs[logCount].isFamily = vehicles[vehicleIdx].isFamily;
    logs[logCount].subType = vehicles[vehicleIdx].subType;
    logs[logCount].amount = amount;
    logs[logCount].timestamp = time(0);
    logs[logCount].floor = 0;
    logs[logCount].slot = 0;

    for (int i = 0; i < 100; i++) {
        if (slots[i].vehicleID == vehicles[vehicleIdx].id) {
            logs[logCount].floor = slots[i].floor;
            logs[logCount].slot = slots[i].slotID - (slots[i].floor - 1) * 25;
            break;
        }
    }
    logCount++;
}

// ==================== FILE SAVE / LOAD ====================(d00)

void saveData() {
    ofstream vf("vehicles.txt");
    vf << vehicleCount << "\n";
    for (int i = 0; i < vehicleCount; i++)
        vf << vehicles[i].id << "\n"
        << vehicles[i].ownerName << "\n"
        << vehicles[i].plateNumber << "\n"
        << vehicles[i].type << "\n"
        << vehicles[i].brand << "\n"
        << vehicles[i].color << "\n"
        << vehicles[i].nationalID << "\n"
        << vehicles[i].phone << "\n"
        << vehicles[i].entryTime << "\n"
        << vehicles[i].isActive << "\n"
        << vehicles[i].subType << "\n"
        << vehicles[i].isVIP << "\n"
        << vehicles[i].wantsCleaning << "\n"
        << vehicles[i].isFamily << "\n"
        << vehicles[i].familyGroupID << "\n";
    vf.close();

    ofstream sf("slots.txt");
    for (int i = 0; i < 100; i++)
        sf << slots[i].slotID << "\n"
        << slots[i].slotType << "\n"
        << slots[i].isOccupied << "\n"
        << slots[i].vehicleID << "\n"
        << slots[i].floor << "\n";
    sf.close();

    ofstream tf("tickets.txt");
    tf << ticketCount << "\n";
    for (int i = 0; i < ticketCount; i++)
        tf << tickets[i].ticketID << "\n"
        << tickets[i].vehicleID << "\n"
        << tickets[i].slotID << "\n"
        << tickets[i].entryTime << "\n"
        << tickets[i].exitTime << "\n"
        << tickets[i].fee << "\n"
        << tickets[i].isActive << "\n"
        << tickets[i].subType << "\n";
    tf.close();

    ofstream pf("pricing.txt");
    pf << pricing.firstHourPrice << "\n"
        << pricing.extraHourPrice << "\n"
        << pricing.dailyPrice << "\n"
        << pricing.dailyCleanExtra << "\n"
        << pricing.monthlyMorning << "\n"
        << pricing.monthlyMorningClean << "\n"
        << pricing.monthlyOvernight << "\n"
        << pricing.monthlyOvernightClean << "\n"
        << pricing.familyBase << "\n"
        << pricing.familyClean << "\n"
        << pricing.cleaningHourly << "\n"
        << pricing.vipAddOn << "\n"
        << pricing.vipAddOnClean << "\n";
    pf.close();

    ofstream lf("profits.txt");
    lf << todayProfit << "\n"
        << weeklyProfit << "\n"
        << monthlyProfit << "\n";
    lf.close();

    cout << "[+] Data saved.\n";
}

void loadData() {
    ifstream vf("vehicles.txt");
    if (vf.is_open()) {
        vf >> vehicleCount;
        vf.ignore();
        for (int i = 0; i < vehicleCount; i++) {
            vf >> vehicles[i].id;
            vf.ignore();
            getline(vf, vehicles[i].ownerName);
            getline(vf, vehicles[i].plateNumber);
            getline(vf, vehicles[i].type);
            getline(vf, vehicles[i].brand);
            getline(vf, vehicles[i].color);
            getline(vf, vehicles[i].nationalID);
            getline(vf, vehicles[i].phone);
            vf >> vehicles[i].entryTime
                >> vehicles[i].isActive
                >> vehicles[i].subType
                >> vehicles[i].isVIP
                >> vehicles[i].wantsCleaning
                >> vehicles[i].isFamily
                >> vehicles[i].familyGroupID;
            vf.ignore();
        }
        vf.close();
    }

    ifstream sf("slots.txt");
    if (sf.is_open()) {
        for (int i = 0; i < 100; i++) {
            sf >> slots[i].slotID;
            sf.ignore();
            getline(sf, slots[i].slotType);
            sf >> slots[i].isOccupied
                >> slots[i].vehicleID
                >> slots[i].floor;
            sf.ignore();
        }
        sf.close();
    }

    ifstream tf("tickets.txt");
    if (tf.is_open()) {
        tf >> ticketCount;
        tf.ignore();
        for (int i = 0; i < ticketCount; i++) {
            tf >> tickets[i].ticketID
                >> tickets[i].vehicleID
                >> tickets[i].slotID
                >> tickets[i].entryTime
                >> tickets[i].exitTime
                >> tickets[i].fee
                >> tickets[i].isActive
                >> tickets[i].subType;
            tf.ignore();
        }
        tf.close();
    }

    ifstream pf("pricing.txt");
    if (pf.is_open()) {
        pf >> pricing.firstHourPrice
            >> pricing.extraHourPrice
            >> pricing.dailyPrice
            >> pricing.dailyCleanExtra
            >> pricing.monthlyMorning
            >> pricing.monthlyMorningClean
            >> pricing.monthlyOvernight
            >> pricing.monthlyOvernightClean
            >> pricing.familyBase
            >> pricing.familyClean
            >> pricing.cleaningHourly
            >> pricing.vipAddOn
            >> pricing.vipAddOnClean;
        pf.close();
    }

    ifstream lf("profits.txt");
    if (lf.is_open()) {
        lf >> todayProfit >> weeklyProfit >> monthlyProfit;
        lf.close();
    }

    cout << "[+] Data loaded.\n";
}

// ==================== ADMIN LOGIN / LOGOUT ====================(d00)

void adminLogin() {
    cout << "\n========== ADMIN LOGIN ==========\n";
    string uname = readNonEmptyLine("Username: ");
    string pword = readNonEmptyLine("Password: ");

    for (int i = 0; i < adminCount; i++) {
        if (admins[i].username == uname && admins[i].password == pword) {
            loggedIn = true;
            loggedInAdminID = admins[i].id;
            cout << "\n[+] Welcome, " << uname << "!\n";
            return;
        }
    }
    cout << "\n[!] Invalid username or password.\n";
}

void adminLogout() {
    loggedIn = false;
    loggedInAdminID = -1;
    cout << "\n[+] Logged out.\n";
}

// ==================== REGISTER ====================(d0)

void registerNewCustomer() {
    cout << "\n--- [New Customer Registration] ---\n";
    cout << "Subscription Types:\n";
    cout << "  1. Hourly            - " << pricing.firstHourPrice << " EGP first hr, " << pricing.extraHourPrice << " EGP/hr after  (Cleaning: +" << pricing.cleaningHourly << " EGP)\n";
    cout << "  2. Daily  (24 Hours) - " << pricing.dailyPrice << " EGP             (Cleaning: +" << pricing.dailyCleanExtra << " EGP)\n";
    cout << "  3. Monthly Morning   - " << pricing.monthlyMorning << " EGP           (Cleaning Bundle: " << pricing.monthlyMorningClean << " EGP)\n";
    cout << "  4. Monthly Overnight - " << pricing.monthlyOvernight << " EGP          (Cleaning Bundle: " << pricing.monthlyOvernightClean << " EGP)\n";
    cout << "  5. Family Bundle     - " << pricing.familyBase << " EGP           (Cleaning Bundle: " << pricing.familyClean << " EGP)\n";

    int subType = readIntChoice("Choose Subscription (1-5): ", 1, 5);

    if (subType == 5) {
        cout << "\n[Family Bundle: Register 2 Cars]\n";
        int groupID = familyGroupCounter++;
        int tempIdx[2] = { -1, -1 };

        for (int c = 0; c < 2; c++) {
            if (vehicleCount >= 100) { cout << "[!] Vehicle database full.\n"; return; }

            cout << "\n  >> Car " << (c + 1) << " Details:\n";
            int idx = vehicleCount;

            vehicles[idx].id = vehicleCount + 1;
            vehicles[idx].ownerName = readName("  Owner Name   : ");
            vehicles[idx].plateNumber = readPlateNew("  Plate Number : ");
            vehicles[idx].brand = readBrand("  Car Brand    : ");
            vehicles[idx].color = readColor("  Car Color    : ");
            vehicles[idx].nationalID = readNationalID("  National ID  : ");
            vehicles[idx].phone = readPhone("  Phone        : ");
            vehicles[idx].type = "Car";
            vehicles[idx].subType = 5;
            vehicles[idx].isFamily = true;
            vehicles[idx].familyGroupID = groupID;
            vehicles[idx].entryTime = time(0);
            vehicles[idx].isActive = true;

            int vipChoice = readIntChoice("  VIP Upgrade? (1: Yes, 2: No): ", 1, 2);
            vehicles[idx].isVIP = (vipChoice == 1);

            int cleanChoice = readIntChoice("  Car Cleaning? (1: Yes, 2: No): ", 1, 2);
            vehicles[idx].wantsCleaning = (cleanChoice == 1);

            int freeFloor = -1;
            if (vehicles[idx].isVIP) {
                if (findFreeSlot(4) != -1) freeFloor = 4;
            }
            else {
                for (int f = 1; f <= 3; f++)
                    if (findFreeSlot(f) != -1) { freeFloor = f; break; }
            }

            if (freeFloor == -1) { cout << "[!] No slot for Car " << (c + 1) << ".\n"; continue; }

            int slotIdx = findFreeSlot(freeFloor);
            slots[slotIdx].isOccupied = true;
            slots[slotIdx].vehicleID = vehicles[idx].id;

            tickets[ticketCount].ticketID = ticketCount + 1;
            tickets[ticketCount].vehicleID = vehicles[idx].id;
            tickets[ticketCount].slotID = slots[slotIdx].slotID;
            tickets[ticketCount].entryTime = vehicles[idx].entryTime;
            tickets[ticketCount].exitTime = 0;
            tickets[ticketCount].fee = 0;
            tickets[ticketCount].isActive = true;
            tickets[ticketCount].subType = 5;

            double regPrice = calculatePrice(5, vehicles[idx].wantsCleaning, vehicles[idx].isVIP, vehicles[idx].entryTime);
            addLog("REGISTERED", idx, regPrice);
            todayProfit += regPrice;
            weeklyProfit += regPrice;
            monthlyProfit += regPrice;

            tempIdx[c] = idx;
            vehicleCount++;
            ticketCount++;

            cout << "\n  [Car " << (c + 1) << " - " << vehicles[idx].ownerName << "]\n";
            cout << "  Assigned Floor " << freeFloor << ", Slot " << (slotIdx - (freeFloor - 1) * 25 + 1) << "\n";
            displayPricing(idx);
        }

        if (tempIdx[0] != -1 && tempIdx[1] != -1)
            cout << "\n[!] Family Bundle Total: "
            << (vehicles[tempIdx[0]].wantsCleaning || vehicles[tempIdx[1]].wantsCleaning ? pricing.familyClean : pricing.familyBase)
            << " EGP for both cars.\n";
        cout << "[+] Family registered successfully!\n";
        return;
    }

    if (vehicleCount >= 100) { cout << "[!] Database full.\n"; return; }

    int idx = vehicleCount;

    int vipChoice = readIntChoice("VIP Upgrade? (1: Yes, 2: No): ", 1, 2);
    vehicles[idx].isVIP = (vipChoice == 1);

    int freeFloor = -1;
    if (vehicles[idx].isVIP) {
        if (findFreeSlot(4) != -1) freeFloor = 4;
    }
    else {
        for (int f = 1; f <= 3; f++)
            if (findFreeSlot(f) != -1) { freeFloor = f; break; }
    }
    if (freeFloor == -1) { cout << "[!] No available slots.\n"; return; }

    vehicles[idx].id = vehicleCount + 1;
    vehicles[idx].ownerName = readName("Owner Name   : ");
    vehicles[idx].plateNumber = readPlateNew("Plate Number : ");
    vehicles[idx].brand = readBrand("Car Brand    : ");
    vehicles[idx].color = readColor("Car Color    : ");
    vehicles[idx].nationalID = readNationalID("National ID  : ");
    vehicles[idx].phone = readPhone("Phone        : ");
    vehicles[idx].type = readVehicleType("Vehicle Type (Car/Motorcycle/Truck): ");
    vehicles[idx].subType = subType;
    vehicles[idx].entryTime = time(0);
    vehicles[idx].isActive = true;
    vehicles[idx].isFamily = false;
    vehicles[idx].familyGroupID = -1;

    int cleanChoice = readIntChoice("Car Cleaning? (1: Yes, 2: No): ", 1, 2);
    vehicles[idx].wantsCleaning = (cleanChoice == 1);

    int slotIdx = findFreeSlot(freeFloor);
    slots[slotIdx].isOccupied = true;
    slots[slotIdx].vehicleID = vehicles[idx].id;

    tickets[ticketCount].ticketID = ticketCount + 1;
    tickets[ticketCount].vehicleID = vehicles[idx].id;
    tickets[ticketCount].slotID = slots[slotIdx].slotID;
    tickets[ticketCount].entryTime = vehicles[idx].entryTime;
    tickets[ticketCount].exitTime = 0;
    tickets[ticketCount].fee = 0;
    tickets[ticketCount].isActive = true;
    tickets[ticketCount].subType = subType;

    double regPrice = calculatePrice(subType, vehicles[idx].wantsCleaning, vehicles[idx].isVIP, vehicles[idx].entryTime);
    addLog("REGISTERED", idx, regPrice);
    todayProfit += regPrice;
    weeklyProfit += regPrice;
    monthlyProfit += regPrice;

    vehicleCount++;
    ticketCount++;

    cout << "\n  Assigned to Floor " << freeFloor << ", Slot " << (slotIdx - (freeFloor - 1) * 25 + 1) << "\n";
    displayPricing(idx);
    cout << "[+] Customer Registered Successfully!\n";
}

// ==================== RECALL & EXIT ====================

void recallAndExit() {
    cout << "\n=========================\n";
    cout << "     RECALL & EXIT\n";
    cout << "=========================\n";

    string plate = readPlate("Enter Plate Number: ");
    bool found = false;

    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].plateNumber == plate && vehicles[i].isActive) {
            found = true;

            cout << "\n[!] Customer Found:\n";
            cout << "  Name         : " << vehicles[i].ownerName << "\n";
            cout << "  Plate        : " << vehicles[i].plateNumber << "\n";
            cout << "  Brand        : " << vehicles[i].brand << "\n";
            cout << "  Color        : " << vehicles[i].color << "\n";
            cout << "  National ID  : " << vehicles[i].nationalID << "\n";
            cout << "  Phone        : " << vehicles[i].phone << "\n";
            cout << "  Subscription : " << subName(vehicles[i].subType) << "\n";
            cout << "  VIP          : " << (vehicles[i].isVIP ? "Yes" : "No") << "\n";
            cout << "  Cleaning     : " << (vehicles[i].wantsCleaning ? "Yes" : "No") << "\n";
            cout << "  Family       : " << (vehicles[i].isFamily ? "Yes" : "No") << "\n";

            for (int s = 0; s < 100; s++) {
                if (slots[s].vehicleID == vehicles[i].id) {
                    cout << "  Floor        : " << slots[s].floor << "\n";
                    cout << "  Slot         : " << (slots[s].slotID - (slots[s].floor - 1) * 25) << "\n";
                    break;
                }
            }

            displayPricing(i);

            int response = readIntChoice("\n[?] Car exiting now? (1: Yes, 2: No): ", 1, 2);

            if (response == 1) {
                if (vehicles[i].isFamily)
                    cout << "\n[!] Family car - partner car recalled separately.\n";

                double exitPrice = calculatePrice(vehicles[i].subType, vehicles[i].wantsCleaning, vehicles[i].isVIP, vehicles[i].entryTime);

                for (int t = 0; t < ticketCount; t++) {
                    if (tickets[t].vehicleID == vehicles[i].id && tickets[t].isActive) {
                        tickets[t].exitTime = time(0);
                        tickets[t].fee = exitPrice;
                        tickets[t].isActive = false;
                        break;
                    }
                }

                for (int s = 0; s < 100; s++) {
                    if (slots[s].vehicleID == vehicles[i].id) {
                        int fl = slots[s].floor;
                        int slt = slots[s].slotID - (fl - 1) * 25;
                        slots[s].isOccupied = false;
                        slots[s].vehicleID = -1;
                        cout << "\n>>> Floor " << fl << ", Slot " << slt << " is now FREE.\n";
                        break;
                    }
                }

                addLog("EXITED", i, exitPrice);
                vehicles[i].isActive = false;
                cout << ">>> Exit processed for " << vehicles[i].ownerName << ".\n";
            }
            else {
                cout << "\n>>> Car remains in the garage.\n";
            }
            break;
        }
    }

    if (!found)
        cout << "\n[!] Plate not found or already exited.\n";
}

// ==================== GARAGE DATABASE ====================(d00)

void viewAllCars() {
    cout << "\n========== ALL CARS IN GARAGE ==========\n";
    for (int f = 1; f <= 4; f++) {
        cout << "\n--- Floor " << f << (f == 4 ? " [VIP]" : " [Regular]") << " ---\n";
        bool anyActive = false;
        for (int s = 0; s < 100; s++) {
            if (slots[s].floor == f && slots[s].isOccupied) {
                for (int v = 0; v < vehicleCount; v++) {
                    if (vehicles[v].id == slots[s].vehicleID && vehicles[v].isActive) {
                        anyActive = true;
                        int localSlot = slots[s].slotID - (f - 1) * 25;
                        cout << "  Slot " << localSlot
                            << " | " << vehicles[v].ownerName
                            << " | " << vehicles[v].plateNumber
                            << " | " << vehicles[v].brand
                            << " | Sub: " << subName(vehicles[v].subType)
                            << (vehicles[v].isVIP ? " [VIP]" : "")
                            << (vehicles[v].isFamily ? " [FAM]" : "")
                            << (vehicles[v].wantsCleaning ? " [WASH]" : "")
                            << "\n";
                        break;
                    }
                }
            }
        }
        if (!anyActive) cout << "  (No active cars)\n";
    }
    cout << "==========================================\n";
}

void viewEmptySlots() {
    cout << "\n========== EMPTY SLOTS ==========\n";
    for (int f = 1; f <= 4; f++) {
        cout << "\n--- Floor " << f << (f == 4 ? " [VIP]" : " [Regular]") << " ---\n";
        bool anyFree = false;
        for (int s = 0; s < 100; s++) {
            if (slots[s].floor == f && !slots[s].isOccupied) {
                anyFree = true;
                int localSlot = slots[s].slotID - (f - 1) * 25;
                cout << "  Slot " << localSlot << " [" << slots[s].slotType << "] - FREE\n";
            }
        }
        if (!anyFree) cout << "  (No free slots on this floor)\n";
    }
    cout << "==================================\n";
}

void searchVehicle() {
    cout << "\n--- [Search Vehicle] ---\n";
    string plate = readPlate("Enter Plate Number: ");
    bool found = false;

    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].plateNumber == plate) {
            found = true;
            cout << "\n  Vehicle Found:\n";
            cout << "  Owner        : " << vehicles[i].ownerName << "\n";
            cout << "  Plate        : " << vehicles[i].plateNumber << "\n";
            cout << "  Brand        : " << vehicles[i].brand << "\n";
            cout << "  Color        : " << vehicles[i].color << "\n";
            cout << "  National ID  : " << vehicles[i].nationalID << "\n";
            cout << "  Phone        : " << vehicles[i].phone << "\n";
            cout << "  Subscription : " << subName(vehicles[i].subType) << "\n";
            cout << "  VIP          : " << (vehicles[i].isVIP ? "Yes" : "No") << "\n";
            cout << "  Cleaning     : " << (vehicles[i].wantsCleaning ? "Yes" : "No") << "\n";
            cout << "  Status       : " << (vehicles[i].isActive ? "Inside" : "Exited") << "\n";

            for (int s = 0; s < 100; s++) {
                if (slots[s].vehicleID == vehicles[i].id) {
                    cout << "  Floor        : " << slots[s].floor << "\n";
                    cout << "  Slot         : " << (slots[s].slotID - (slots[s].floor - 1) * 25) << "\n";
                    break;
                }
            }
            break;
        }
    }

    if (!found) cout << "[!] Vehicle not found.\n";
}

void todaysTasks() {
    cout << "\n========== TODAY'S TASKS ==========\n";
    time_t now = time(0);
    bool anyTask = false;

    cout << "\n--- Cars Needing Wash Today ---\n";
    for (int i = 0; i < vehicleCount; i++) {
        if (!vehicles[i].isActive || !vehicles[i].wantsCleaning) continue;

        double hoursSince = difftime(now, vehicles[i].entryTime) / 3600.0;
        double daysSince = hoursSince / 24.0;
        bool   needsWash = false;
        string reason = "";

        if (vehicles[i].subType == 1 && hoursSince < 24.0) { needsWash = true; reason = "Hourly - wash on arrival"; }
        else if (vehicles[i].subType == 2 && daysSince < 1.0) { needsWash = true; reason = "Daily - wash on arrival"; }
        else if (vehicles[i].isVIP && fmod(daysSince, 3.5) < 1.0) { needsWash = true; reason = "VIP - every 3.5 days"; }
        else if (fmod(daysSince, 7.0) < 1.0) { needsWash = true; reason = "Subscription - every 7 days"; }

        if (needsWash) {
            anyTask = true;
            for (int s = 0; s < 100; s++) {
                if (slots[s].vehicleID == vehicles[i].id) {
                    cout << "  >> " << vehicles[i].ownerName
                        << " | Plate: " << vehicles[i].plateNumber
                        << " | Floor " << slots[s].floor
                        << ", Slot " << (slots[s].slotID - (slots[s].floor - 1) * 25)
                        << " | " << reason << "\n";
                    break;
                }
            }
        }
    }
    if (!anyTask) cout << "  (No wash tasks today)\n";
    cout << "====================================\n";
}

void todaysLogs() {
    cout << "\n========== TODAY'S LOGS ==========\n";
    time_t now = time(0);
    tm todayInfo;
    localtime_s(&todayInfo, &now);

    bool anyLog = false;
    for (int i = 0; i < logCount; i++) {
        tm logTime;
        localtime_s(&logTime, &logs[i].timestamp);
        if (logTime.tm_mday == todayInfo.tm_mday &&
            logTime.tm_mon == todayInfo.tm_mon &&
            logTime.tm_year == todayInfo.tm_year) {

            anyLog = true;
            char timeBuf[20];
            strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &logTime);

            cout << "\n  [" << timeBuf << "] " << logs[i].action << "\n";
            cout << "  Name    : " << logs[i].ownerName << "\n";
            cout << "  Plate   : " << logs[i].plateNumber << "\n";
            cout << "  Sub     : " << subName(logs[i].subType) << "\n";
            cout << "  VIP     : " << (logs[i].isVIP ? "Yes" : "No") << "\n";
            cout << "  Family  : " << (logs[i].isFamily ? "Yes" : "No") << "\n";
            cout << "  Amount  : " << logs[i].amount << " EGP\n";
            cout << "  ----------------------------------\n";
        }
    }
    if (!anyLog) cout << "  (No activity today)\n";
    cout << "===================================\n";
}

void profitReport() {
    if (loggedInAdminID != 1) {
        cout << "\n[!] Access Denied. Only Waleed can view profit reports.\n";
        return;
    }
    cout << "\n========== PROFIT REPORT ==========\n";
    cout << "  Today's Profit  : " << todayProfit << " EGP\n";
    cout << "  Weekly Profit   : " << weeklyProfit << " EGP\n";
    cout << "  Monthly Profit  : " << monthlyProfit << " EGP\n";
    cout << "===================================\n";
}

void modifyPricing() {
    cout << "\n--- [Modify Pricing Rules] ---\n";
    cout << "  Current First Hour  : " << pricing.firstHourPrice << " EGP\n";
    cout << "  Current Extra Hour  : " << pricing.extraHourPrice << " EGP\n";
    cout << "  Current Daily       : " << pricing.dailyPrice << " EGP\n";
    cout << "  Current M. Morning  : " << pricing.monthlyMorning << " EGP\n";
    cout << "  Current M. Overnight: " << pricing.monthlyOvernight << " EGP\n";
    cout << "  Current Family      : " << pricing.familyBase << " EGP\n";

    pricing.firstHourPrice = readPositiveDouble("  New First Hour Price   : ");
    pricing.extraHourPrice = readPositiveDouble("  New Extra Hour Price   : ");
    pricing.dailyPrice = readPositiveDouble("  New Daily Price        : ");
    pricing.dailyCleanExtra = readPositiveDouble("  New Daily Clean Extra  : ");
    pricing.monthlyMorning = readPositiveDouble("  New Monthly Morning    : ");
    pricing.monthlyMorningClean = readPositiveDouble("  New M. Morning + Clean : ");
    pricing.monthlyOvernight = readPositiveDouble("  New Monthly Overnight  : ");
    pricing.monthlyOvernightClean = readPositiveDouble("  New M. Overnight+Clean : ");
    pricing.familyBase = readPositiveDouble("  New Family Base        : ");
    pricing.familyClean = readPositiveDouble("  New Family + Clean     : ");
    pricing.cleaningHourly = readPositiveDouble("  New Hourly Cleaning    : ");
    pricing.vipAddOn = readPositiveDouble("  New VIP Add-on         : ");
    pricing.vipAddOnClean = readPositiveDouble("  New VIP Add-on + Clean : ");

    cout << "[+] Pricing updated.\n";
}

// ==================== MENUS ====================

void garageDatabaseMenu() {
    int choice;
    while (true) {
        cout << "\n========== GARAGE DATABASE ==========\n";
        cout << "1. View All Cars & Subscriptions\n";
        cout << "2. View Empty Slots\n";
        cout << "3. Search Vehicle by Plate\n";
        cout << "4. Today's Tasks (Wash Schedule)\n";
        cout << "5. Today's Logs\n";
        cout << "6. Profit Report\n";
        cout << "7. Back to Main Menu\n";
        cout << "======================================\n";

        choice = readIntChoice("Enter choice: ", 1, 7);

        if (choice == 1) viewAllCars();
        else if (choice == 2) viewEmptySlots();
        else if (choice == 3) searchVehicle();
        else if (choice == 4) todaysTasks();
        else if (choice == 5) todaysLogs();
        else if (choice == 6) profitReport();
        else if (choice == 7) break;
    }
}

void showMainMenu() {
    cout << "\n********** GARAGE SYSTEM **********\n";
    cout << "1. Register New Car\n";
    cout << "2. Recall & Exit Car\n";
    cout << "3. Garage Database\n";
    cout << "4. Modify Pricing Rules\n";
    cout << "5. Save Data\n";
    cout << "6. Logout\n";
    cout << "7. Exit Program\n";
    cout << "***********************************\n";
}

// ==================== INIT ====================

void initSlots() {
    for (int i = 0; i < 75; i++) {
        slots[i].slotID = i + 1;
        slots[i].slotType = "Regular";
        slots[i].isOccupied = false;
        slots[i].vehicleID = -1;
        slots[i].floor = i / 25 + 1;
    }
    for (int i = 75; i < 100; i++) {
        slots[i].slotID = i + 1;
        slots[i].slotType = "VIP";
        slots[i].isOccupied = false;
        slots[i].vehicleID = -1;
        slots[i].floor = 4;
    }
}

void initAdmins() {
    admins[0].id = 1;
    admins[0].username = "Prof.Omnia";
    admins[0].password = "afdal_doctor";

    admins[1].id = 2;
    admins[1].username = "Waleed";
    admins[1].password = "msh_Hamza";

    adminCount = 2;
}

void initPricing() {
    pricing.firstHourPrice = 20.0;
    pricing.extraHourPrice = 10.0;
    pricing.dailyPrice = 150.0;
    pricing.dailyCleanExtra = 100.0;
    pricing.monthlyMorning = 1500.0;
    pricing.monthlyMorningClean = 1850.0;
    pricing.monthlyOvernight = 2800.0;
    pricing.monthlyOvernightClean = 3200.0;
    pricing.familyBase = 4800.0;
    pricing.familyClean = 5500.0;
    pricing.cleaningHourly = 120.0;
    pricing.vipAddOn = 500.0;
    pricing.vipAddOnClean = 900.0;
}

// ==================== MAIN ====================

int main() {
    initSlots();
    initAdmins();
    initPricing();
    loadData();

    while (!loggedIn)
        adminLogin();

    int choice;
    while (true) {
        showMainMenu();
        choice = readIntChoice("Enter choice: ", 1, 7);

        if (choice == 1) registerNewCustomer();
        else if (choice == 2) recallAndExit();
        else if (choice == 3) garageDatabaseMenu();
        else if (choice == 4) modifyPricing();
        else if (choice == 5) saveData();
        else if (choice == 6) {
            adminLogout();
            cout << "1. Login again\n2. Exit Program\n";
            int sub = readIntChoice("Choice: ", 1, 2);
            if (sub == 1) { while (!loggedIn) adminLogin(); }
            else { saveData(); cout << "Shutting down...\n"; break; }
        }
        else if (choice == 7) {
            saveData();
            cout << "Shutting down...\n";
            break;
        }
    }

    return 0;
}
