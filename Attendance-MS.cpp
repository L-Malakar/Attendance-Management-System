#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <ctime>

using namespace std;

const string DATA_FILE = "attendance.txt";
const string CSV_FILE  = "attendance_report.csv";
const double LOW_ATTENDANCE_LIMIT = 75.0;

// Reads an integer safely (fixes the infinite loop bug)
int readInt() {
    int value;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Enter a number: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// Returns today's date as YYYY-MM-DD
string getToday() {
    time_t now = time(0);
    tm* t = localtime(&now);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);
    return string(buf);
}

string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

class Student {
private:
    int rollNo;
    string name;
    int totalClasses;
    int classesAttended;
    string lastDate;

public:
    Student(int r, const string& n, int total = 0, int attended = 0, const string& last = "-") {
        rollNo = r;
        name = n;
        totalClasses = total;
        classesAttended = attended;
        lastDate = last;
    }
    
    int getRollNo() const {
        return rollNo;
    }

    string getName() const {
        return name;
    }

    void setName(const string& n) {
        name = n;
    }

    double getPercentage() const {
        if (totalClasses == 0) return 0.0;
        return (static_cast<double>(classesAttended) / totalClasses) * 100.0;
    }

    bool alreadyMarkedToday() const {
        return lastDate == getToday();
    }

    bool markAttendance(bool present) {
        if (alreadyMarkedToday()) return false;
        totalClasses++;
        if (present) classesAttended++;
        lastDate = getToday();
        return true;
    }

    void display() const {
        double percentage = getPercentage();
        cout << left << setw(10) << rollNo
             << setw(20) << name
             << setw(15) << totalClasses
             << setw(12) << classesAttended
             << fixed << setprecision(2) << setw(10) << percentage;
        if (totalClasses > 0 && percentage < LOW_ATTENDANCE_LIMIT)
            cout << "LOW!";
        cout << endl;
    }

    string toFileString() const {
        stringstream ss;
        ss << rollNo << "|" << totalClasses << "|" << classesAttended
           << "|" << lastDate << "|" << name;
        return ss.str();
    }

    string toCsvString() const {
        stringstream ss;
        ss << rollNo << ",\"" << name << "\"," << totalClasses << ","
           << classesAttended << "," << fixed << setprecision(2) << getPercentage();
        return ss.str();
    }
};

class AttendanceManager {
private:
    vector<Student> students;

    int findIndex(int rollNo) const {
        for (size_t i = 0; i < students.size(); i++) {
            if (students[i].getRollNo() == rollNo) return (int)i;
        }
        return -1;
    }

    static void printHeader() {
        cout << "\n====================================================================\n";
        cout << "                        ATTENDANCE REPORT                           \n";
        cout << "====================================================================\n";
        cout << left << setw(10) << "Roll No"
             << setw(20) << "Name"
             << setw(15) << "Total Classes"
             << setw(12) << "Attended"
             << setw(10) << "Percent" << "Status" << endl;
        cout << "--------------------------------------------------------------------\n";
    }

public:
    void saveToFile() const {
        ofstream out(DATA_FILE.c_str());
        if (!out) {
            cout << "Warning: Could not save data to file!\n";
            return;
        }
        for (size_t i = 0; i < students.size(); i++) {
            out << students[i].toFileString() << "\n";
        }
    }

    void loadFromFile() {
        ifstream in(DATA_FILE.c_str());
        if (!in) return;

        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string rollS, totalS, attS, last, name;
            getline(ss, rollS, '|');
            getline(ss, totalS, '|');
            getline(ss, attS, '|');
            getline(ss, last, '|');
            getline(ss, name);
            try {
                students.push_back(Student(stoi(rollS), name, stoi(totalS), stoi(attS), last));
            } catch (...) {
            }
        }
        cout << students.size() << " student record(s) loaded from file.\n";
    }

    void addStudent() {
        cout << "\nEnter Roll Number: ";
        int rollNo = readInt();

        if (rollNo <= 0) {
            cout << "Error: Roll Number must be a positive number!\n";
            return;
        }
        if (findIndex(rollNo) != -1) {
            cout << "Error: A student with this Roll Number already exists!\n";
            return;
        }

        string name;
        cout << "Enter Student Name: ";
        getline(cin, name);

        if (name.empty()) {
            cout << "Error: Name cannot be empty!\n";
            return;
        }
        if (name.find('|') != string::npos) {
            cout << "Error: Name cannot contain the '|' character!\n";
            return;
        }

        students.push_back(Student(rollNo, name));
        saveToFile();
        cout << "Student added successfully!\n";
    }

    void markAttendance() {
        if (students.empty()) {
            cout << "\nNo students found in the system. Please add students first.\n";
            return;
        }

        cout << "\nEnter Roll Number to mark attendance: ";
        int rollNo = readInt();
        int idx = findIndex(rollNo);

        if (idx == -1) {
            cout << "Error: Student with Roll Number " << rollNo << " not found!\n";
            return;
        }
        if (students[idx].alreadyMarkedToday()) {
            cout << "Attendance for " << students[idx].getName() << " is already marked today.\n";
            return;
        }

        char status;
        cout << "Mark Attendance for " << students[idx].getName() << " (P for Present, A for Absent): ";
        cin >> status;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (status == 'P' || status == 'p') {
            students[idx].markAttendance(true);
            cout << "Attendance marked as PRESENT.\n";
        } else if (status == 'A' || status == 'a') {
            students[idx].markAttendance(false);
            cout << "Attendance marked as ABSENT.\n";
        } else {
            cout << "Invalid status entered. Attendance not recorded.\n";
            return;
        }
        saveToFile();
    }

    void markAllAttendance() {
        if (students.empty()) {
            cout << "\nNo students found in the system. Please add students first.\n";
            return;
        }

        cout << "\nMarking attendance for date: " << getToday() << "\n";
        cout << "(P = Present, A = Absent)\n";

        for (size_t i = 0; i < students.size(); i++) {
            if (students[i].alreadyMarkedToday()) {
                cout << students[i].getRollNo() << " - " << students[i].getName()
                     << ": already marked today, skipped.\n";
                continue;
            }

            char status;
            while (true) {
                cout << students[i].getRollNo() << " - " << students[i].getName() << " (P/A): ";
                cin >> status;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (status == 'P' || status == 'p' || status == 'A' || status == 'a') break;
                cout << "Please enter only P or A.\n";
            }
            students[i].markAttendance(status == 'P' || status == 'p');
        }
        saveToFile();
        cout << "Attendance saved for the whole class.\n";
    }

    void viewAttendanceReport() const {
        if (students.empty()) {
            cout << "\nNo student records available to display.\n";
            return;
        }

        cout << "\nSort report by:\n1. Roll Number\n2. Name\n3. Percentage (high to low)\nChoice: ";
        int sortChoice = readInt();

        vector<Student> sorted = students;

        if (sortChoice == 2) {
            sort(sorted.begin(), sorted.end(), [](const Student& a, const Student& b) {
                return toLower(a.getName()) < toLower(b.getName());
            });
        } else if (sortChoice == 3) {
            sort(sorted.begin(), sorted.end(), [](const Student& a, const Student& b) {
                return a.getPercentage() > b.getPercentage();
            });
        } else {
            sort(sorted.begin(), sorted.end(), [](const Student& a, const Student& b) {
                return a.getRollNo() < b.getRollNo();
            });
        }

        printHeader();
        for (size_t i = 0; i < sorted.size(); i++) {
            sorted[i].display();
        }
        cout << "====================================================================\n";
        cout << "LOW! = attendance below " << LOW_ATTENDANCE_LIMIT << "%\n";
    }

    void searchStudent() const {
        if (students.empty()) {
            cout << "\nNo student records available.\n";
            return;
        }

        string key;
        cout << "\nEnter Roll Number or part of Name to search: ";
        getline(cin, key);
        string lowKey = toLower(key);

        bool found = false;
        for (size_t i = 0; i < students.size(); i++) {
            bool rollMatch = (to_string(students[i].getRollNo()) == key);
            bool nameMatch = (toLower(students[i].getName()).find(lowKey) != string::npos);
            if (!key.empty() && (rollMatch || nameMatch)) {
                if (!found) printHeader();
                students[i].display();
                found = true;
            }
        }
        if (!found) cout << "No matching student found.\n";
    }

    void editStudent() {
        cout << "\nEnter Roll Number of student to edit: ";
        int rollNo = readInt();
        int idx = findIndex(rollNo);

        if (idx == -1) {
            cout << "Error: Student not found!\n";
            return;
        }

        string newName;
        cout << "Current name: " << students[idx].getName() << "\nEnter new name: ";
        getline(cin, newName);

        if (newName.empty() || newName.find('|') != string::npos) {
            cout << "Invalid name. No changes made.\n";
            return;
        }
        students[idx].setName(newName);
        saveToFile();
        cout << "Student name updated.\n";
    }

    void deleteStudent() {
        cout << "\nEnter Roll Number of student to delete: ";
        int rollNo = readInt();
        int idx = findIndex(rollNo);

        if (idx == -1) {
            cout << "Error: Student not found!\n";
            return;
        }

        char confirm;
        cout << "Delete " << students[idx].getName() << "? (Y/N): ";
        cin >> confirm;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (confirm == 'Y' || confirm == 'y') {
            students.erase(students.begin() + idx);
            saveToFile();
            cout << "Student deleted.\n";
        } else {
            cout << "Deletion cancelled.\n";
        }
    }

    void exportToCsv() const {
        if (students.empty()) {
            cout << "\nNothing to export.\n";
            return;
        }
        ofstream out(CSV_FILE.c_str());
        if (!out) {
            cout << "Error: Could not create CSV file!\n";
            return;
        }
        out << "Roll No,Name,Total Classes,Attended,Percentage\n";
        for (size_t i = 0; i < students.size(); i++) {
            out << students[i].toCsvString() << "\n";
        }
        cout << "Report exported to " << CSV_FILE << " (open it in Excel).\n";
    }
};

int main() {
    AttendanceManager manager;
    manager.loadFromFile();
    int choice;

    do {
        cout << "\n--- Student Attendance Management System ---\n";
        cout << "1. Add Student\n";
        cout << "2. Mark Attendance (one student)\n";
        cout << "3. Mark Attendance (whole class)\n";
        cout << "4. View Attendance Report\n";
        cout << "5. Search Student\n";
        cout << "6. Edit Student Name\n";
        cout << "7. Delete Student\n";
        cout << "8. Export Report to CSV\n";
        cout << "9. Exit\n";
        cout << "Enter your choice (1-9): ";
        choice = readInt();

        switch (choice) {
            case 1: manager.addStudent(); break;
            case 2: manager.markAttendance(); break;
            case 3: manager.markAllAttendance(); break;
            case 4: manager.viewAttendanceReport(); break;
            case 5: manager.searchStudent(); break;
            case 6: manager.editStudent(); break;
            case 7: manager.deleteStudent(); break;
            case 8: manager.exportToCsv(); break;
            case 9:
                manager.saveToFile();
                cout << "\nData saved. Goodbye!\n";
                break;
            default:
                cout << "\nInvalid choice! Please enter a number between 1 and 9.\n";
        }
    } while (choice != 9);

    return 0;
}
