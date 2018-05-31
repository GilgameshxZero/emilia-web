/*
Standard
*/

/*
All utilities dealing with files.

When in doubt: directories are specified with an ending '/'. Sometimes functions may not work if this format is not followed.
Assume functions don't work with unicode/UTF8 multibyte strings, unless specified.

Directory: string ending in / which points to a directory
File: just the filename of a file
Path: concatenated directory and filename to a file

Windows-specific.
*/

#pragma once

#include "WindowsLAMInclude.h"
#include "UtilityString.h"

#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <set>
#include <sstream>

namespace Rain {
	//turn a string into LPCWSTR with a "\\?\" prefix, for use in long paths unicode functions
	std::wstring pathToLongPath(std::string path);
	std::wstring pathToLongPath(std::wstring path);

	//test if file exists; either relative or absolute path work
	bool fileExists(std::string file);
	bool dirExists(std::string dir);

	//append \\ on the end if doesn't have
	std::string *standardizeDirPath(std::string *dir);
	std::string standardizeDirPath(std::string dir);

	//returns current directory of the executable (not the same as the path to the executable, sometimes)
	std::string getWorkingDirectory();

	//works with Unicode/UTF8-multibyte
	std::string getExePath();

	//converts a path to a file into either the filename or the directory for that file 
	std::string getPathDir(std::string path);
	std::string getPathFile(std::string path);

	//Get all files or all directories in a directory, in a certain format. NOTE: Takes and returns UTF8 multibyte strings - but works with unicode directories.
	std::vector<std::string> getFiles(std::string directory, std::string format = "*");
	std::vector<std::string> getDirs(std::string directory, std::string format = "*");

	//gets all files and directories under a directory, recursively
	//works with unicode/multibyte UTF8
	//use absolute paths in ignore
	std::vector<std::string> getFilesRec(std::string directory, std::string format = "*", std::set<std::string> *ignore = NULL);
	std::vector<std::string> getDirsRec(std::string directory, std::string format = "*", std::set<std::string> *ignore = NULL);

	//creates parent directories until specified directory created
	void createDirRec(std::string dir);

	//removes all directories and files under a directory, but not the directory itself
	//works with unicode/multibyte UTF8
	//second argument specifies files/directories to not remove; use \ end to specify dir
	//use absolute paths in ignore
	void rmDirRec(std::string dir, std::set<std::string> *ignore = NULL);

	//copies directory structure over, replacing any files in the destination, but not deleting any unrelated files
	void cpyDirRec(std::string src, std::string dst, std::set<std::string> *ignore = NULL);

	//converts any path to an absolute path
	std::string pathToAbsolute(std::string path);

	//allows for retrieval of file information and testing if two paths point to the same file
	BY_HANDLE_FILE_INFORMATION getFileInformation(std::string path);
	bool isEquivalentPath(std::string path1, std::string path2);

	//get last modified time of files
	//works with unicode/multibyte UTF8
	FILETIME getLastModTime(std::string path);

	//output information to a file, shorthand
	void printToFile(std::string filename, std::string *output, bool append = false);
	void printToFile(std::string filename, std::string output, bool append = false);

	//file size in bytes
	std::size_t getFileSize(std::string file);

	//put the whole file into a string
	//returns nothing, because copy constructor might be expensive
	void readFileToStr(std::string filePath, std::string &fileData);

	//multiple lines, each into a separate string in the vector.
	std::vector<std::string> readMultilineFile(std::string filePath);

	//read parameters from standard parameter string, organized in lines (terminated by \n) of key:value, possibly with whitespace inbetween elements
	std::map<std::string, std::string> readParameterStream(std::stringstream &paramStream);
	std::map<std::string, std::string> readParameterString(std::string paramString);
	std::map<std::string, std::string> readParameterFile(std::string filePath);
}