#pragma once
#include <string>
#include <sstream>
#include <vector>

typedef void* HANDLE;

// -------------------------------------------------------------------------------------------------

class MappedFile
{
public:
	MappedFile(const std::string& filePath, std::size_t size);
	~MappedFile();

	void WriteFromStringStream(const std::stringstream& sstream) const;
	void ReadBytes(std::vector<uint8_t>& outBytes) const;

private:
	HANDLE m_file;
	HANDLE m_mapping;
	char* m_data;
	std::size_t m_size;
};

// -------------------------------------------------------------------------------------------------
