/* 
 * File:   GromacsForceField.cpp
 * Author: kulkem
 * 
 * Created on May 30, 2016, 3:11 PM
 */

#include "GromacsForceField.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>
#include "martinize/fileutil.h"

using namespace std;
using namespace boost::filesystem;
#define BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_VERSION 3

bool file_exists(string fileName)
{
    return exists(path(fileName));
}

void get_files_with_extension(string dir, string extension, vector<file_descr>& files)
{
    path dir_path(dir);
    
    if(!exists(dir_path)) return;
    directory_iterator end_itr;
    for(directory_iterator itr(dir_path); itr != end_itr; ++itr)
        if(!is_directory(itr->status()))
            if(itr->path().extension() == extension)
            {
                file_descr fd;
                fd.location = itr->path().native();
                fd.name = itr->path().filename().native();
                fd.extension = itr->path().extension().native();
                fd.corename = fd.name.substr(0, fd.name.find_last_of('.'));
                files.push_back(fd);
            }
}

void addViableFF(const path& dir_path, vector<ff_descr>& ff_list)
{
	if (!exists(dir_path)) return;
	directory_iterator end_itr; // default construction yields past-the-end
        bool ff_exist = false;
        bool mtp_exist = false;
        bool ff_start_exist = false;
	for (directory_iterator itr(dir_path);itr != end_itr;++itr)
	{
		if (is_directory(itr->status()))
			addViableFF(itr->path(), ff_list);
		else 
                {
                    if (itr->path().filename() == "forcefield.doc") ff_exist = true;
                    if (itr->path().filename() == "forcefield.itp") ff_start_exist = true;
                    if (itr->path().extension() == ".mtp" ) mtp_exist = true;
                }
	}
        if(ff_exist && mtp_exist && ff_start_exist)
        {
            ff_descr ff;
            ff.location = dir_path.native();
            ff.folder = dir_path.filename().native();
            
            boost::filesystem::ifstream in_descr;
            in_descr.open((ff.location + "/forcefield.doc").c_str(), boost::filesystem::ifstream::in);
            if (!in_descr.fail())
            {
                    getline(in_descr, ff.descr, '\n');
                    in_descr.close();
            }
            else
                    ff.descr = "could not open";
            
            ff_list.push_back(ff);
        }
}

void get_ffs(vector<ff_descr>& ff_list)
{
    //looking into pathvariable and splitting it up
    char* path_env = getenv("GMXLIB");

    stringstream parse_path(path_env);
    string path_current;
    bool parse_eof;
    bool parse_error;

    do
    {
            getline(parse_path, path_current, ':');
            parse_eof = parse_path.eof();
            parse_error = parse_path.fail();
            if (parse_error) break;
            path cur_path(path_current);
            addViableFF(cur_path, ff_list);
    } while (!parse_eof);
}

string getFileInGMXLib(string fileName)
{
    //looking into pathvariable and splitting it up
    char* path_env = getenv("GMXLIB");

    stringstream parse_path(path_env);
    string path_current;
    bool parse_eof;
    bool parse_error;

    do
    {
        getline(parse_path, path_current, ':');
        parse_eof = parse_path.eof();
        parse_error = parse_path.fail();
        if (parse_error) break;
        path cur_path(path_current);
        directory_iterator end_itr; // default construction yields past-the-end
        for (directory_iterator itr(cur_path);itr != end_itr;++itr)
            if (itr->path().filename() == fileName) return cur_path.native();
    } while (!parse_eof);
    
    return "";
}

ff_descr get_ff()
{
    int ff_index = -1;
    ff_descr ff;
    vector<ff_descr> ff_list;
    get_ffs(ff_list);

    //list the files and choose forcefield
    if (ff_list.size() > 0)
    {
        printf("Select a force field:\n");
        for (int i = 0; i < ff_list.size(); ++i)
            printf("[%i] %s\n", i, ff_list[i].descr.c_str());
        while (ff_index < 0 || ff_index >= ff_list.size() ) scanf("%d", &ff_index);
    
        ff = ff_list[ff_index];
        ff_list.clear();
    }
    else
    {
        ff.folder = "";
        ff.location = "";
        ff.descr = "";
    }
    
    return ff;
}

tsResult readFileStBeadType(string location, string fileName, vector<stBeadTypes>* beadList)
{
    FILE* fIn;
    string file = location + "/" + fileName;
    if(! (fIn = fopen(file.c_str(), "r")) ) {printf("could not open file %s\n", file.c_str());return TS_NOT_FOUND;}
    
    char line[MAX_LINE_LENGTH];
    char word[MAX_LINE_LENGTH];
    tsResult r;
    tsResult r2;
    int lineNumber = 0;
    bool readAtomTypes = false;
    
    do
    {
        do
        {
            r = GetLine(line, fIn);
            ++lineNumber;
            trimmComments(line);
        } while(r != TS_EOF && strlen(line) == 0);
        
        if(strlen(line) > 0)
        {
            int linePointer = 0;
            r2 = GetWord(word, line, &linePointer);
            
            if(!strncmp(word, "#include", MAX_LINE_LENGTH))
            {
                if(r2 == TS_EOF) {printf("Error in Line %i of file %s\n", lineNumber, fileName.c_str());return TS_BADFORMAT;}
                r2 = GetWord(word, line, &linePointer, " \n\t", " \n\t", "\""); if(strlen(word) == 0) {printf("Error in Line %i of file %s\n", lineNumber, fileName.c_str());return TS_BADFORMAT;}
                string NextFileLocation = "";
                if(file_exists(location + "/" + word)) NextFileLocation = location; else NextFileLocation = getFileInGMXLib(word);
                if(NextFileLocation != "")
                {
                    printf("Reading file %s\n", word);
                    readFileStBeadType(NextFileLocation, word, beadList);
                } else { printf("Warning: could not find file %s\n", word);}
            }
            
            //Reading atom types
            if(line[0] == '[')
            {
                trimmToInsideWord(line);
                if(!strncmp(line, "atomtypes", MAX_LINE_LENGTH))
                    readAtomTypes = true;
                else
                    readAtomTypes = false;
            } else
            {
                if(readAtomTypes)
                {
                    linePointer = 0;
                    r2 = GetWord(word, line, &linePointer);if(r2 == TS_EOF) {printf("Error in Line %i of file %s\n", lineNumber, fileName.c_str());return TS_BADFORMAT;}
                    stBeadTypes beadtype;
                    beadtype.name = word;
                    beadtype.mass = GetFloat(line, &linePointer, " \n\t", " \n\t", "", 0, &r2);if(r2 == TS_EOF) {printf("Error in Line %i of file %s\n", lineNumber, fileName.c_str());return TS_BADFORMAT;}
                    beadtype.charge = GetFloat(line, &linePointer, " \n\t", " \n\t", "", 0, &r2);if(r2 == TS_EOF) {printf("Error in Line %i of file %s\n", lineNumber, fileName.c_str());return TS_BADFORMAT;}
                    beadList->push_back(beadtype);
                }
            }
        }
        
    } while(r != TS_EOF);
    
    
    fclose(fIn);
    
    return TS_OK;
}

tsResult getStandardBeadTypes(ff_descr* ff, vector<stBeadTypes>* beadList)
{
    return readFileStBeadType(ff->location, "forcefield.itp", beadList);
}