#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <cstring>
#include <cstddef>
#include <cctype>
#include <stdexcept>
#include <utility>
#include <chrono>

using namespace std;
struct CaseInsensitiveHash {
    size_t operator()(const string& key) const {
        string lowercase = key;
        transform(lowercase.begin(), lowercase.end(), lowercase.begin(),
                 [](unsigned char c){ return tolower(c); });
        return hash<string>{}(lowercase);
    }
};
struct CaseInsensitiveEqual {
    bool operator()(const string& left, const string& right) const {
        if (left.size() != right.size()) return false;
        for (size_t i = 0; i < left.size(); ++i) {
            if (tolower(static_cast<unsigned char>(left[i])) != 
                tolower(static_cast<unsigned char>(right[i]))) return false;
        }
        return true;
    }
};
class Date {
private:
    int year, month, day;

public:
    Date() : year(0), month(0), day(0) {}
    
    Date(const string& dateStr) {
        year = month = day = 0;
        if (dateStr.empty()) {
            return;
        }
        
        if (dateStr.length() != 10) {
            throw invalid_argument("Invalid date format. Please use DD-MM-YYYY");
        }
        
        if (dateStr[2] != '-' || dateStr[5] != '-') {
            throw invalid_argument("Invalid date format. Please use '-' as delimiter");
        }
        
        string dayStr = dateStr.substr(0, 2);
        string monthStr = dateStr.substr(3, 2);
        string yearStr = dateStr.substr(6, 4);
        
        if (!isdigit(dayStr[0]) || !isdigit(dayStr[1]) ||
            !isdigit(monthStr[0]) || !isdigit(monthStr[1]) ||
            !isdigit(yearStr[0]) || !isdigit(yearStr[1]) ||
            !isdigit(yearStr[2]) || !isdigit(yearStr[3])) {
            throw invalid_argument("Date components must be numbers");
        }
        
        day = stoi(dayStr);
        month = stoi(monthStr);
        year = stoi(yearStr);
        
        if (!isValid()) {
            throw invalid_argument("Invalid date. Please enter a valid date");
        }
    }

    string toString() const {
        if (year == 0 && month == 0 && day == 0) {
            return "Not set";
        }
        stringstream ss;
        ss << setfill('0') << setw(2) << day << "-"
           << setfill('0') << setw(2) << month << "-"
           << setfill('0') << setw(4) << year;
        return ss.str();
    }
        bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    bool operator<=(const Date& other) const {
        return *this < other || *this == other;
    }

    bool operator>(const Date& other) const {
        return !(*this <= other);
    }

    bool operator>=(const Date& other) const {
        return !(*this < other);
    }

    bool operator==(const Date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }
        bool isValid() const {
        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        
        const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int maxDays = daysInMonth[month];
        
        if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
            maxDays = 29;
        }
        
        if (day < 1 || day > maxDays) return false;
        return true;
    }
};

class Patient {
public:
    int id;
    string name;
    string medicalHistory;
    string department;
    string condition;
    Date admissionDate;
    Date dischargeDate;
    int roomNumber;

    Patient(int id, const string& name, const string& medicalHistory, const string& department, 
            const string& condition, const string& admissionDateStr, const string& dischargeDateStr, int roomNumber)
        : id(id), name(name), medicalHistory(medicalHistory), department(department),
          condition(condition), admissionDate(admissionDateStr), dischargeDate(dischargeDateStr),
          roomNumber(roomNumber) {}

    void display() const {
        cout << "-------------------------------------\n";
        cout << "ID: " << id << "\n"
             << "Name: " << name << "\n"
             << "Medical History: " << medicalHistory << "\n"
             << "Department: " << department << "\n"
             << "Condition: " << condition << "\n"
             << "Admission Date: " << admissionDate.toString() << "\n"
             << "Discharge Date: " << dischargeDate.toString() << "\n"
             << "Room Number: " << roomNumber << "\n";
        cout << "-------------------------------------\n";
    }

    string toCSV() const {
        stringstream ss;
        ss << id << "," 
           << name << "," 
           << medicalHistory << "," 
           << department << "," 
           << condition << "," 
           << admissionDate.toString() << "," 
           << dischargeDate.toString() << "," 
           << roomNumber;
        return ss.str();
    }
};

class HospitalSystem {
private:
    vector<Patient> patients;
    string csvFilename;
    int nextPatientId;
    
        unordered_map<int, int> idToIndex;  
    unordered_map<string, vector<int>, CaseInsensitiveHash, CaseInsensitiveEqual> nameToIndices;
    unordered_map<string, vector<int>, CaseInsensitiveHash, CaseInsensitiveEqual> departmentToIndices;
    unordered_map<string, vector<int>, CaseInsensitiveHash, CaseInsensitiveEqual> conditionToIndices;
    unordered_map<int, vector<int>> roomToIndices;

public:
        int getPatientCount() const {
        return patients.size();
    }

    int getAvailableRooms() const {
        int available = 0;
        for (int i = 1; i <= 200; i++) {
            if (isRoomAvailable(i)) {
                available++;
            }
        }
        return available;
    }

    int getOccupiedRooms() const {
        return 200 - getAvailableRooms();
    }

    void buildIndices() {
        idToIndex.clear();
        nameToIndices.clear();
        departmentToIndices.clear();
        conditionToIndices.clear();
        roomToIndices.clear();
        
        for (size_t i = 0; i < patients.size(); i++) {
            idToIndex[patients[i].id] = i;
            nameToIndices[patients[i].name].push_back(i);
            departmentToIndices[patients[i].department].push_back(i);
            conditionToIndices[patients[i].condition].push_back(i);
            roomToIndices[patients[i].roomNumber].push_back(i);
        }
        
        nextPatientId = 1;
        for (const auto& patient : patients) {
            nextPatientId = max(nextPatientId, patient.id + 1);
        }
    }

    HospitalSystem(const string& filename) : csvFilename(filename), nextPatientId(1) {
        if (!loadFromCSV(csvFilename)) {
            throw runtime_error("Error: Could not open file " + filename + ". Please check if the file exists and try again.");
        }
    }

    bool loadFromCSV(const string& filename) {
        ifstream file(filename);
        if (!file) {
            return false;
        }

        patients.clear();
        string line;
        while (getline(file, line)) {
            if (line.substr(0, 2) != "//") {
                stringstream ss(line);
                string field;
                vector<string> fields;
                
                while (getline(ss, field, ',')) {
                    fields.push_back(field);
                }
                if (fields.size() == 8) {
                    try {
                        int id = stoi(fields[0]);
                        int room = stoi(fields[7]);
                        patients.emplace_back(id, fields[1], fields[2], fields[3], 
                                          fields[4], fields[5], fields[6], room);
                    } catch (const exception& e) {
                        cerr << "Error parsing line: " << line << endl;
                    }
                }
            }
        }

        buildIndices();
        
        cout << "Loaded " << patients.size() << " patient records from " << filename << endl;
        return true;
    }

    void saveToCSV() {
        ofstream file(csvFilename);
        if (!file.is_open()) {
            cout << "Error: Cannot open file for writing: " << csvFilename << endl;
            return;
        }
        
        file << "ID,Name,MedicalHistory,Department,Condition,AdmissionDate,DischargeDate,RoomNumber\n";
        
        for (const auto& patient : patients) {
            file << patient.toCSV() << endl;
        }
        
        cout << "Saved " << patients.size() << " patient records to " << csvFilename << endl;
    }

    bool isRoomAvailable(int roomNumber) const {
        if (roomNumber < 1 || roomNumber > 200) {
            throw invalid_argument("Room number must be between 1 and 200");
        }
        
        auto it = roomToIndices.find(roomNumber);
        if (it == roomToIndices.end()) {
            return true;  
        }

        for (int idx : it->second) {
            const Patient& patient = patients[idx];
            if (patient.admissionDate.isValid() && 
                (!patient.dischargeDate.isValid() || 
                 (Date() < patient.dischargeDate))) {  
                return false;
            }
        }
        return true;
    }



    void addPatient() {
        string name, medHistory, department, condition;
        string admissionDateStr, dischargeDateStr;
        int roomNumber;
        
        cin.ignore();
        
        system("cls");
        cout << "\n=== Add New Patient ===\n";
        cout << "Please fill in the patient information:\n\n";
        
        while (true) {
            cout << "Enter patient name (2-50 characters): ";
            getline(cin, name);
            if (name.length() >= 2 && name.length() <= 50) {
                break;
            }
            cout << "Error: Name must be between 2 and 50 characters\n";
        }
        
        while (true) {
            cout << "Enter medical history (max 200 characters): ";
            getline(cin, medHistory);
            if (medHistory.length() <= 200) {
                break;
            }
            cout << "Error: Medical history must be less than 200 characters\n";
        }
        
        cout << "\nAvailable departments:\n";
        for (const auto& dept : departmentToIndices) {
            cout << "- " << dept.first << " (" << dept.second.size() << " patients)\n";
        }
        
        while (true) {
            cout << "\nEnter department (select from the list above): ";
            getline(cin, department);
            
            bool isValid = false;
            for (const auto& dept : departmentToIndices) {
                if (dept.first == department) {
                    isValid = true;
                    break;
                }
            }
            
            if (isValid) {
                break;
            }
            cout << "Error: Invalid department. Please select from the list above.\n";
        }
        
        while (true) {
            cout << "Enter condition (2-100 characters): ";
            getline(cin, condition);
            if (condition.length() >= 2 && condition.length() <= 100) {
                break;
            }
            cout << "Error: Condition must be between 2 and 100 characters\n";
        }
        
        while (true) {
            cout << "Enter admission date (DD-MM-YYYY): ";
            getline(cin, admissionDateStr);
            try {
                Date admissionDate(admissionDateStr);
                if (admissionDate < Date()) {
                    throw invalid_argument("Admission date cannot be in the past");
                }
                break;
            } catch (const invalid_argument& e) {
                cout << "Error: " << e.what() << "\n";
            }
        }
        
        while (true) {
            cout << "Enter discharge date (DD-MM-YYYY, leave empty if not discharged): ";
            getline(cin, dischargeDateStr);
            
            if (dischargeDateStr.empty()) {
                break;
            }
            
            try {
                Date admissionDate(admissionDateStr);
                Date dischargeDate(dischargeDateStr);
                
                if (dischargeDate < admissionDate) {
                    throw invalid_argument("Discharge date cannot be before admission date");
                }
                break;
            } catch (const invalid_argument& e) {
                cout << "Error: " << e.what() << "\n";
            }
        }

        cout << "\nAvailable Rooms (1-200):\n";
        for (int i = 1; i <= 200; i++) {
            // Check if room exists in roomToIndices
            auto it = roomToIndices.find(i);
            if (it == roomToIndices.end()) {
                cout << "- Room " << i << ": Available (0 current patients)\n";
            } else {
                int currentPatients = 0;
                for (int idx : it->second) {
                    if (patients[idx].admissionDate.isValid() && 
                        (!patients[idx].dischargeDate.isValid() || 
                         (Date() < patients[idx].dischargeDate))) {
                        currentPatients++;
                    }
                }
                
                if (currentPatients == 0) {
                    cout << "- Room " << i << ": Available (0 current patients)\n";
                } else {
                    cout << "- Room " << i << ": Occupied (" 
                         << currentPatients << " current patient(s), "
                         << it->second.size() << " total assignment(s))\n";
                }
                
                if (currentPatients > 1) {
                    cout << "  WARNING: Room is overbooked!\n";
                }
            }
        }
        
        while (true) {
            cout << "\nEnter room number (1-200): ";
            cin >> roomNumber;
            
            if (roomNumber < 1 || roomNumber > 200) {
                cout << "\nError: Room number must be between 1 and 200\n";
                system("pause");
                continue;
            }
            
            if (isRoomAvailable(roomNumber)) {
                break;
            }
            cout << "\nError: Room " << roomNumber << " is currently occupied.\n";
            cout << "Please select another room from the available list above.\n";
        }

        try {
            Patient tempPatient(nextPatientId, name, medHistory, department, 
                               condition, admissionDateStr, dischargeDateStr, roomNumber);
            
            if (!isValidPatient(tempPatient)) {
                cout << "\nError: Invalid patient data. Cannot add patient.\n";
                cout << "Please check all fields and try again.\n";
                return;
            }

            // Add patient to system
            patients.emplace_back(nextPatientId, name, medHistory, department,
                                 condition, admissionDateStr, dischargeDateStr, roomNumber);
            
            cout << "\nPatient added successfully!\n";
            cout << "Patient ID: " << nextPatientId << "\n";
            cout << "Room Number: " << roomNumber << "\n";
            cout << "Department: " << department << "\n";
            
            nextPatientId++;
            buildIndices();
            saveToCSV();
        } catch (const invalid_argument& e) {
            cout << "\nError: " << e.what() << "\n";
            cout << "Please try again with valid information.\n";
            return;
        }

        patients.emplace_back(nextPatientId, name, medHistory, department, 
                             condition, admissionDateStr, dischargeDateStr, roomNumber);
        
        cout << "Patient added with ID: " << nextPatientId << endl;
        nextPatientId++;
        
        buildIndices();
        saveToCSV();
    }
    bool isValidPatient(const Patient& patient) const {
       
        auto it = idToIndex.find(patient.id);
        if (it != idToIndex.end()) {
            cout << "Error: Patient ID already exists.\n";
            return false;
        }

        
        if (patient.id <= 0) {
            cout << "Error: Invalid patient ID. ID must be a positive number.\n";
            return false;
        }

        
        if (patient.roomNumber <= 0) {
            cout << "Error: Invalid room number.\n";
            return false;
        }

        
        if (patient.name.empty()) {
            cout << "Error: Patient name cannot be empty.\n";
            return false;
        }

        
        if (!patient.admissionDate.isValid()) {
            cout << "Error: Invalid admission date.\n";
            return false;
        }

        
        if (patient.dischargeDate.isValid() && !(patient.admissionDate < patient.dischargeDate)) {
            cout << "Error: Discharge date must be after admission date.\n";
            return false;
        }

        return true;
    }

    void updatePatient() {
        int id;
        cout << "Enter patient ID to update: ";
        cin >> id;
        
        
        auto it = idToIndex.find(id);
        if (it == idToIndex.end()) {
            cout << "Patient with ID " << id << " not found." << endl;
            return;
        }
        
        Patient& patient = patients[it->second];
        
        int choice;
        cout << "What do you want to update?\n"
             << "1. Name\n"
             << "2. Medical History\n"
             << "3. Department\n"
             << "4. Condition\n"
             << "5. Admission Date\n"
             << "6. Discharge Date\n"
             << "7. Room Number\n"
             << "Choice: ";
        cin >> choice;
        
        cin.ignore();
        string newValue;
        
        switch (choice) {
            case 1:
                cout << "Enter new name: ";
                getline(cin, newValue);
                patient.name = newValue;
                break;
            case 2:
                cout << "Enter new medical history: ";
                getline(cin, newValue);
                patient.medicalHistory = newValue;
                break;
            case 3:
                cout << "Enter new department: ";
                getline(cin, newValue);
                patient.department = newValue;
                break;
            case 4:
                cout << "Enter new condition: ";
                getline(cin, newValue);
                patient.condition = newValue;
                break;
            case 5:
                cout << "Enter new admission date (DD-MM-YYYY): ";
                getline(cin, newValue);
                patient.admissionDate = Date(newValue);
                break;
            case 6:
                cout << "Enter new discharge date (DD-MM-YYYY): ";
                getline(cin, newValue);
                patient.dischargeDate = Date(newValue);
                break;
            case 7:
                int newRoom;
                cout << "Enter new room number: ";
                cin >> newRoom;
                patient.roomNumber = newRoom;
                break;
            default:
                cout << "Invalid choice.\n";
                return;
        }
        
        cout << "Patient updated successfully.\n";
        buildIndices();
        saveToCSV();
    }

    void deletePatient() {
        int id;
        cout << "Enter patient ID to delete: ";
        cin >> id;
        
        
        auto it = idToIndex.find(id);
        if (it == idToIndex.end()) {
            cout << "Patient with ID " << id << " not found." << endl;
            return;
        }
        
        patients.erase(patients.begin() + it->second);
        cout << "Patient with ID " << id << " deleted successfully." << endl;
        
        buildIndices();
        saveToCSV();
    }

    void searchById() {
        cout << "\nEnter patient ID to search: ";
        int id;
        cin >> id;
        
        auto it = idToIndex.find(id);
        if (it != idToIndex.end()) {
            patients[it->second].display();
        } else {
            cout << "\nNo patient found with ID: " << id << "\n";
        }
    }

    void searchByName() {
        string name;
        cin.ignore();
        cout << "Enter patient name (or partial name): ";
        getline(cin, name);
        
        transform(name.begin(), name.end(), name.begin(), ::tolower);
        
        vector<int> results;
        for (const auto& entry : nameToIndices) {
            string entryNameLower = entry.first;
            transform(entryNameLower.begin(), entryNameLower.end(), entryNameLower.begin(), ::tolower);
            
            if (entryNameLower.find(name) != string::npos) {
                results.insert(results.end(), entry.second.begin(), entry.second.end());
            }
        }
        
        if (results.empty()) {
            cout << "No patients found with name containing '" << name << "'." << endl;
            return;
        }
        
        cout << "Found " << results.size() << " patients:\n";
        for (int idx : results) {
            patients[idx].display();
        }
    }

    void searchByDateRange() {
        cout << "\nEnter start date (DD-MM-YYYY): ";
        string startDateStr;
        cin.ignore();
        getline(cin, startDateStr);
        
        cout << "\nEnter end date (DD-MM-YYYY): ";
        string endDateStr;
        getline(cin, endDateStr);
        
        try {
            Date startDate(startDateStr);
            Date endDate(endDateStr);
            
            cout << "\nPatients admitted between " << startDate.toString() << " and " << endDate.toString() << ":\n";
            bool found = false;
            
            for (const auto& patient : patients) {
                if (patient.admissionDate.isValid() && 
                    !(startDate > patient.admissionDate) && 
                    !(patient.admissionDate > endDate)) {
                    patient.display();
                    found = true;
                }
            }
            
            if (!found) {
                cout << "\nNo patients found in the specified date range.\n";
            }
        } catch (const invalid_argument& e) {
            cout << "\nError: " << e.what() << "\n";
            cout << "Please enter valid dates in DD-MM-YYYY format.\n";
        }
    }

    void showPatientsByDepartment() {
        cout << "Available departments:\n";
        for (const auto& dept : departmentToIndices) {
            int activePatients = 0;
            for (int idx : dept.second) {
                if (!patients[idx].dischargeDate.isValid() || Date() < patients[idx].dischargeDate) {
                    activePatients++;
                }
            }
            cout << "- " << dept.first << " (" << activePatients << " active patients)\n";
        }
        
        string department;
        cin.ignore();
        cout << "Enter department name: ";
        getline(cin, department);
        
        auto it = departmentToIndices.find(department);
        if (it != departmentToIndices.end()) {
            if (it->second.empty()) {
                cout << "\nNo patients found in department: " << department << "\n";
                return;
            }
            cout << "\nPatients in department " << department << ":\n";
            for (int idx : it->second) {
                patients[idx].display();
            }
        } else {
            cout << "\nNo patients found in department: " << department << "\n";
        }
    }

    void showPatientsByCondition() {
        cout << "Available conditions:\n";
        for (const auto& cond : conditionToIndices) {
            cout << "- " << cond.first << " (" << cond.second.size() << " patients)\n";
        }
        
        string condition;
        cin.ignore();
        cout << "Enter condition: ";
        getline(cin, condition);
        
        auto it = conditionToIndices.find(condition);
        if (it != conditionToIndices.end()) {
            if (it->second.empty()) {
                cout << "\nNo patients found with condition: " << condition << "\n";
                return;
            }
            cout << "\nPatients with condition " << condition << ":\n";
            for (int idx : it->second) {
                patients[idx].display();
            }
        } else {
            cout << "\nNo patients found with condition: " << condition << "\n";
        }
    }
    
    void showPatientsByRoom() {
        cout << "\nEnter room number: ";
        int room;
        cin >> room;
        
        if (room < 1 || room > 200) {
            cout << "\nError: Room number must be between 1 and 200\n";
            return;
        }
        
        auto it = roomToIndices.find(room);
        if (it != roomToIndices.end()) {
            if (it->second.empty()) {
                cout << "\nNo patients found in room: " << room << "\n";
                return;
            }
            cout << "\nPatients in room " << room << ":\n";
            for (int idx : it->second) {
                patients[idx].display();
            }
        } else {
            cout << "\nNo patients found in room: " << room << "\n";
            cout << "Press any key to continue...\n";
            system("pause");
        }
    }

    void displayAllPatients() {
        if (patients.empty()) {
            cout << "No patient records found.\n";
            return;
        }
        
        cout << "Total patients: " << patients.size() << endl;
        for (const auto& patient : patients) {
            patient.display();
        }
    }

    void showStatistics() {
        if (patients.empty()) {
            cout << "No patient records found.\n";
            return;
        }
        
        cout << "\n=== Hospital Statistics ===\n";
        cout << "Total patients: " << patients.size() << endl;
        
        // Count patients by department
        cout << "\nPatients by Department:\n";
        for (const auto& dept : departmentToIndices) {
            cout << "- " << dept.first << ": " << dept.second.size() << endl;
        }
        
        // Count patients by condition
        cout << "\nPatients by Condition:\n";
        for (const auto& cond : conditionToIndices) {
            cout << "- " << cond.first << ": " << cond.second.size() << endl;
        }
        
        // Count currently admitted patients
        int admittedCount = 0;
        Date today("03-05-2025"); // Using today's date in DD-MM-YYYY format
        
        for (const auto& patient : patients) {
            if (patient.admissionDate.isValid() && 
                (!patient.dischargeDate.isValid() || today <= patient.dischargeDate)) {
                admittedCount++;
            }
        }
        
        cout << "\nCurrently admitted patients: " << admittedCount << endl;
        cout << "Discharged patients: " << (patients.size() - admittedCount) << endl;
        
        // Room utilization
        cout << "\nRoom Utilization (1-200):\n";
        for (int i = 1; i <= 200; i++) {
            // Check if room exists in roomToIndices
            auto it = roomToIndices.find(i);
            if (it == roomToIndices.end()) {
                cout << "- Room " << i << ": Available (0 current patients)\n";
            } else {
                int currentPatients = 0;
                for (int idx : it->second) {
                    if (patients[idx].admissionDate.isValid() && 
                        (!patients[idx].dischargeDate.isValid() || today <= patients[idx].dischargeDate)) {
                        currentPatients++;
                    }
                }
                
                if (currentPatients == 0) {
                    cout << "- Room " << i << ": Available (0 current patients)\n";
                } else {
                    cout << "- Room " << i << ": Occupied (" 
                         << currentPatients << " current patient(s), "
                         << it->second.size() << " total assignment(s))\n";
                }
                
                if (currentPatients > 1) {
                    cout << "  WARNING: Room is overbooked!\n";
                }
            }
        }
    }
};

int main() {
    // Welcome screen
    cout << "\n===================================\n";
    cout << "  Hospital Patient Record System\n";
    cout << "===================================\n\n";
    
    cout << "Enter CSV file name for patient records (e.g., patients.csv): ";
    string filename;
    cin >> filename;
    
    try {
        HospitalSystem hospital(filename);
        
        int choice;
        do {
            // Clear screen
            system("cls");
            
            // Display hospital statistics
            cout << "\n=== Hospital Statistics ===\n";
            cout << "Total Patients: " << hospital.getPatientCount() << "\n";
            cout << "Available Rooms: " << hospital.getAvailableRooms() << "\n";
            cout << "Occupied Rooms: " << hospital.getOccupiedRooms() << "\n\n";
            
            cout << "1. Add New Patient\n";
            cout << "2. Update Patient Information\n";
            cout << "3. Delete Patient Record\n";
            cout << "4. Search Patient by ID\n";
            cout << "5. Search Patient by Name\n";
            cout << "6. Search Patients by Date Range\n";
            cout << "7. Display Patients by Department\n";
            cout << "8. Display Patients by Condition\n";
            cout << "9. Display Patients by Room\n";
            cout << "10. Display All Patients\n";
            cout << "11. Show Hospital Statistics\n";
            cout << "0. Exit\n\n";
            
            cout << "Enter your choice (0-11): ";
            cin >> choice;
            
            // Validate choice
            if (choice < 0 || choice > 11) {
                cout << "\nError: Invalid choice. Please enter a number between 0 and 11.\n";
                system("pause");
                continue;
            }
            
            switch (choice) {
                case 1:
                    hospital.addPatient();
                    break;
                case 2:
                    hospital.updatePatient();
                    break;
                case 3:
                    hospital.deletePatient();
                    break;
                case 4:
                    hospital.searchById();
                    break;
                case 5:
                    hospital.searchByName();
                    break;
                case 6:
                    hospital.searchByDateRange();
                    break;
                case 7:
                    hospital.showPatientsByDepartment();
                    break;
                case 8:
                    hospital.showPatientsByCondition();
                    break;
                case 9:
                    hospital.showPatientsByRoom();
                    break;
                case 10:
                    hospital.displayAllPatients();
                    break;
                case 11:
                    hospital.showStatistics();
                    break;
                case 0:
                    cout << "\nThank you for using Hospital Management System!\n";
                    return 0;
                default:
                    cout << "\nInvalid choice! Please try again.\n";
                    break;
            }
        } while (true);
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;  // Exit 
    }
    
    return 0;
}