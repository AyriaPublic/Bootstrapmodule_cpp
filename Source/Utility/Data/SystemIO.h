/*
    Initial author: Convery (tcn@ayria.se)
    Started: 07-08-2017
    License: MIT
    Notes:
        [Insert some line about how this is a utility for
        new developers to get up and running quickly; not
        just me being too lazy to use stdio             ]
*/

#pragma once

#include "../../Stdinclude.h"

// Open a file on disk and do IO.
inline std::string Readfile(std::string Path)
{
    std::FILE *Filehandle = std::fopen(Path.c_str(), "rb");
    if (!Filehandle) return "";

    std::fseek(Filehandle, 0, SEEK_END);
    auto Length = std::ftell(Filehandle);
    std::fseek(Filehandle, 0, SEEK_SET);

    auto Buffer = std::make_unique<char[]>(Length);
    std::fread(Buffer.get(), Length, 1, Filehandle);
    std::fclose(Filehandle);

    return std::string(Buffer.get(), Length);
}
inline bool Writefile(std::string Path, std::string Buffer)
{
    std::FILE *Filehandle = std::fopen(Path.c_str(), "wb");
    if (!Filehandle) return false;

    std::fwrite(Buffer.data(), Buffer.size(), 1, Filehandle);
    std::fclose(Filehandle);
    return true;
}
inline bool Fileexists(std::string Path)
{
    std::FILE *Filehandle = std::fopen(Path.c_str(), "rb");
    if (!Filehandle) return false;
    std::fclose(Filehandle);
    return true;
}

// Open a pipe and do IO.
inline std::string Readpipe(std::string Path)
{
    #if defined(_WIN32)
    std::FILE *Pipehandle = _popen(Path.c_str(), "rt");
    if (!Pipehandle) return "";
    #else
    std::FILE *Pipehandle = popen(Path.c_str(), "rt");
    if (!Pipehandle) return "";
    #endif

    std::string Result;
    auto Buffer = std::make_unique<char[]>(2048);
    while (std::fgets(Buffer.get(), 2048, Pipehandle))
    {
        Result += Buffer.get();
    }

    #if defined(_WIN32)
    _pclose(Pipehandle);
    #else
    pclose(Pipehandle);
    #endif

    return Result;
}
inline bool Writepipe(std::string Path, std::string Buffer)
{
    #if defined(_WIN32)
    std::FILE *Pipehandle = _popen(Path.c_str(), "wt");
    if (!Pipehandle) return false;
    #else
    std::FILE *Pipehandle = popen(Path.c_str(), "wt");
    if (!Pipehandle) return false;
    #endif

    std::fputs(Buffer.c_str(), Pipehandle);

    #if defined(_WIN32)
    _pclose(Pipehandle);
    #else
    pclose(Pipehandle);
    #endif

    return true;
}

// List all files in a directory.
#if defined (_WIN32)
inline bool Findfiles(std::string Searchpath, std::vector<std::string> *Filenames, std::string Extension)
{
    WIN32_FIND_DATAA Filedata;
    HANDLE Filehandle;

    // Append trailing slash, asterisk and extension.
    if (Searchpath.back() != '/') Searchpath.append("/");
    Searchpath.append("*");
    if(Extension.size()) Searchpath.append(Extension);

    // Find the first plugin.
    Filehandle = FindFirstFileA(Searchpath.c_str(), &Filedata);
    if (Filehandle == (void *)ERROR_INVALID_HANDLE || Filehandle == (void *)INVALID_HANDLE_VALUE)
    {
        if(Filehandle) FindClose(Filehandle);
        return false;
    }

    do
    {
        // Respect hidden files and folders.
        if (Filedata.cFileName[0] == '.')
            continue;

        // Add the file to the list.
        if (!(Filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            Filenames->push_back(Filedata.cFileName);

    } while (FindNextFileA(Filehandle, &Filedata));

    FindClose(Filehandle);
    return !!Filenames->size();
}

#else
inline bool Findfiles(std::string Searchpath, std::vector<std::string> *Filenames, std::string Extension)
{
    struct stat Fileinfo;
    dirent *Filedata;
    DIR *Filehandle;

    // Iterate through the directory.
    Filehandle = opendir(Searchpath.c_str());
    while ((Filedata = readdir(Filehandle)))
    {
        // Respect hidden files and folders.
        if (Filedata->d_name[0] == '.')
            continue;

        // Get extended fileinfo.
        std::string Filepath = Searchpath + "/" + Filedata->d_name;
        if (stat(Filepath.c_str(), &Fileinfo) == -1) continue;

        // Add the file to the list.
        if (!(Fileinfo.st_mode & S_IFDIR))
            if (!Extension.size())
                Filenames->push_back(Filedata->d_name);
            else
                if (std::strstr(Filedata->d_name, Extension.c_str())
                    Filenames->push_back(Filedata->d_name);
    }
    closedir(Filehandle);

    return !!Filenames->size();
}

#endif
