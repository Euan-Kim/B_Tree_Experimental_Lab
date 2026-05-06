#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "io.h"

void ReadStudentCsv(StudentArray& arr, const char* name)
{
    // Open file
    std::ifstream file(name);

    if (!file.is_open()) {
        std::cout << "Failed to open the file." << std::endl;
        std::cout << "Make sure your csv file(default: student.csv) is in the same directory." << std::endl;
        std::exit(1);
    }

    // Read each line
    std::string line;
    std::getline(file, line); // Remove the first line(field name)
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string student_id, name, gender, gpa, height, weight;

        std::getline(ss, student_id, ',');
        std::getline(ss, name, ',');
        std::getline(ss, gender, ',');
        std::getline(ss, gpa, ',');
        std::getline(ss, height, ',');
        std::getline(ss, weight, ',');

        arr.Append(Student(student_id, name, gender, gpa, height, weight));
    }

    file.close();
}