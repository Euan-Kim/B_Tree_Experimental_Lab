#include <iostream>
#include <iomanip>
#include "student.h"

Student::Student() {}

Student::Student(const string& student_id, const string& name,
	const string& gender, const string& gpa,
	const string& height, const string& weight)
	:student_id(std::stoi(student_id)), name(name), gpa(std::stod(gpa)),
	height(std::stod(height)), weight(std::stod(weight))
{
	if (gender == "Male")
		this->gender = Gender::MALE;
	else
		this->gender = Gender::FEMALE;
}

std::ostream& operator<<(std::ostream& os, const Student& student)
{
	os << std::setw(10) << student.student_id << std::setw(25) << student.name;
	if (student.gender == Gender::MALE)
		os << std::setw(7) << "Male";
	else
		os << std::setw(7) << "Female";
	os << std::setw(5) << std::fixed << std::setprecision(2) << student.gpa
		<< std::setw(6) << std::setprecision(1) << student.height
		<< std::setw(5) << student.weight;
	return os;
}

StudentArray::StudentArray(std::size_t size) : size(size), n(0)
{
	arr = new Student[size];
}

StudentArray::~StudentArray()
{
	delete[]arr;
}

const Student& StudentArray::operator[](std::size_t idx) const
{
	return arr[idx];
}

void StudentArray::Append(const Student& student)
{
	if (size == n)
	{
		std::cout << "Array is already full!" << std::endl;
		return;
	}
	arr[n] = student;
	n++;
}

std::size_t StudentArray::GetN() const
{
	return n;
}