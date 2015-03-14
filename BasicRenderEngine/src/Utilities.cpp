/*
 * Utilities.cpp
 *
 *  Created on: Mar 14, 2015
 *      Author: Collin
 */

#include <fstream>
#include <stdexcept>

#include "Utilities.h"

std::string readFile(std::istream& is)
{
	std::string result;
	// Get size and resize string accordingly
	is.seekg(0, std::ios::end);
	std::size_t size = is.tellg();
	result.resize(size);
	is.seekg(0);
	// Read into string
	is.read(&result[0], size);

	return result;
}

std::string readFile(const std::string& filename)
{
	std::ifstream file(filename);
	if(!file)
	{
		throw std::runtime_error("In readFile: could not open file");
	}

	return readFile(file);
}
