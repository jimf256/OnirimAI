#include "MappedFile.h"
#include <streambuf>
#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// -------------------------------------------------------------------------------------------------

// custom output stream buffer object that allows us to use our own backing buffer
class ostreambuf : public std::basic_streambuf<char, std::char_traits<char>>
{
public:
	ostreambuf(char_type* buffer, std::streamsize size)
	{
		setp(buffer, buffer, buffer + size);
	}
};

// -------------------------------------------------------------------------------------------------

MappedFile::MappedFile(const std::string& filePath, std::size_t size)
	: m_file(INVALID_HANDLE_VALUE)
	, m_mapping(NULL)
	, m_data(nullptr)
	, m_size(size)
{
	// open the file for read/write
	DWORD access = GENERIC_READ | GENERIC_WRITE;
	DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	m_file = CreateFileA(filePath.c_str(), access, shareMode, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	assert(m_file != INVALID_HANDLE_VALUE);
	if (m_file != INVALID_HANDLE_VALUE)
	{
		// create the file mapping
		m_mapping = CreateFileMappingA(m_file, NULL, PAGE_READWRITE, 0, static_cast<DWORD>(size), NULL);
		assert(m_mapping != NULL);
		if (m_mapping != NULL)
		{
			// map a view of the file to get the data pointer
			m_data = reinterpret_cast<char*>(MapViewOfFile(m_mapping, FILE_MAP_ALL_ACCESS, 0, 0, size));
			assert(m_data != nullptr);
		}
	}
}

// -------------------------------------------------------------------------------------------------

MappedFile::~MappedFile()
{
	// clean up file handle, file mapping and mapped view
	if (m_data != nullptr)
	{
		bool result = UnmapViewOfFile(m_data);
		assert(result);
	}
	if (m_mapping != NULL)
	{
		bool result = CloseHandle(m_mapping);
		assert(result);
	}
	if (m_file != INVALID_HANDLE_VALUE)
	{
		bool result = CloseHandle(m_file);
		assert(result);
	}
}

// -------------------------------------------------------------------------------------------------

void MappedFile::WriteFromStringStream(const std::stringstream& sstream) const
{
	if (m_data != nullptr)
	{
		memset(m_data, 0, m_size);
		ostreambuf buffer(m_data, m_size);
		std::ostream stream(&buffer);

		stream << sstream.rdbuf();
		stream.flush();
	}
}

// -------------------------------------------------------------------------------------------------

void MappedFile::ReadToStringStream(std::stringstream& sstream) const
{
	if (m_data != nullptr)
	{
		sstream.str("");
		for (int i = 0; i < m_size; ++i)
		{
			sstream.put(m_data[i]);
			if (m_data[i] == '\n')
			{
				break;
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------
