#pragma once
#include <string>
#include <sstream>

typedef void* HANDLE;

// -------------------------------------------------------------------------------------------------

class MappedFile
{
public:
	MappedFile(const std::string& filePath, std::size_t size);
	~MappedFile();

	void WriteFromStringStream(const std::stringstream& sstream) const;
	void ReadToStringStream(std::stringstream& sstream) const;

private:
	HANDLE m_file;
	HANDLE m_mapping;
	char* m_data;
	std::size_t m_size;
};

// -------------------------------------------------------------------------------------------------
