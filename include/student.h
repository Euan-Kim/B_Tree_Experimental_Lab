#ifndef __STUDENT_H__
#define __STUDENT_H__

#include <iostream>
#include <string>

enum class Gender
{
	MALE,
	FEMALE
};

struct Student
{
	using string = std::string;
	unsigned int student_id;
	string name;
	Gender gender;
	double gpa, height, weight;
	Student();
	Student(const string& student_id, const string& name,
		const string& gender, const string& gpa,
		const string& height, const string& weight);
};

std::ostream& operator<<(std::ostream& os, const Student& student);

class StudentArray
{
private:
	Student* arr;
	std::size_t size, n;
public:
	StudentArray(std::size_t size);
	~StudentArray();
	const Student& operator[](std::size_t idx) const;
	void Append(const Student& student);
	std::size_t GetN() const;
};

#endif