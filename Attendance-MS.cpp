#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// Class representing an individual Student
class Student {
private:
    int rollNo;
    string name;
    int totalClasses;
    int classesAttended;

public:
    // Constructor to initialize student details
    Student(int r, string n) {
        rollNo = r;
        name = n;
        totalClasses = 0;
        classesAttended = 0;
    }

    // Getters
    int getRollNo() const {
        return rollNo;
    }

    string getName() const {
        return name;
    }

    // Function to mark attendance
    void markAttendance(bool present) {
        totalClasses++;
        if (present) {
            classesAttended++;
        }
    }

    // Function to display student information and attendance percentage
    void display() const {
        double percentage = 0.0;
        if (totalClasses > 0) {
            percentage = (static_cast<double>(classesAttended) / totalClasses) * 100.0;
        }

        cout << left << setw(10) << rollNo 
             << setw(20) << name 
             << setw(15) << totalClasses 
             << setw(18) << classesAttended 
             << fixed << setprecision(2) << percentage << "%" << endl;
    }
};

// Class managing the entire collection of students and operations
class AttendanceManager {
private:
    vector<Student> students;

public:
    // Add a new student
    void addStudent() {
        int rollNo;
        string name;

        cout << "\nEnter Roll Number: ";
        cin >> rollNo;

        // Check if roll number already exists
        for (size_t i = 0; i < students.size(); i++) {
            if (students[i].getRollNo() == rollNo) {
                cout << "Error: A student with this Roll Number already exists!\n";
                return;
            }
        }

        cout << "Enter Student Name: ";
        cin.ignore(); // Clear input buffer
        getline(cin, name);

        students.push_back(Student(rollNo, name));
        cout << "Student added successfully!\n";
    }

    // Mark attendance for a specific student
    void markAttendance() {
        if (students.empty()) {
            cout << "\nNo students found in the system. Please add students first.\n";
            return;
        }

        int rollNo;
        cout << "\nEnter Roll Number to mark attendance: ";
        cin >> rollNo;

        for (size_t i = 0; i < students.size(); i++) {
            if (students[i].getRollNo() == rollNo) {
                char status;
                cout << "Mark Attendance for " << students[i].getName() << " (P for Present, A for Absent): ";
                cin >> status;

                if (status == 'P' || status == 'p') {
                    students[i].markAttendance(true);
                    cout << "Attendance marked as PRESENT.\n";
                } else if (status == 'A' || status == 'a') {
                    students[i].markAttendance(false);
                    cout << "Attendance marked as ABSENT.\n";
                } else {
                    cout << "Invalid status entered. Attendance not recorded.\n";
                }
                return;
            }
        }
        cout << "Error: Student with Roll Number " << rollNo << " not found!\n";
    }

    // View attendance report for all students
    void viewAttendanceReport() const {
        if (students.empty()) {
            cout << "\nNo student records available to display.\n";
            return;
        }

        cout << "\n============================================================\n";
        cout << "                   ATTENDANCE REPORT                        \n";
        cout << "============================================================\n";
        cout << left << setw(10) << "Roll No" 
             << setw(20) << "Name" 
             << setw(15) << "Total Classes" 
             << setw(18) << "Attended" 
             << "Percentage" << endl;
        cout << "------------------------------------------------------------\n";

        for (size_t i = 0; i < students.size(); i++) {
            students[i].display();
        }
        cout << "============================================================\n";
    }
};

// Main function providing the interactive menu
int main() {
    AttendanceManager manager;
    int choice;

    do {
        cout << "\n--- Student Attendance Management System ---\n";
        cout << "1. Add Student\n";
        cout << "2. Mark Attendance\n";
        cout << "3. View Attendance Report\n";
        cout << "4. Exit\n";
        cout << "Enter your choice (1-4): ";
        cin >> choice;

        switch (choice) {
            case 1:
                manager.addStudent();
                break;
            case 2:
                manager.markAttendance();
                break;
            case 3:
                manager.viewAttendanceReport();
                break;
            case 4:
                cout << "\nExiting program. Good luck with your project presentation!\n";
                break;
            default:
                cout << "\nInvalid choice! Please enter a number between 1 and 4.\n";
        }
    } while (choice != 4);

    return 0;
}
