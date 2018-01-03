/*
    Initial author: Convery (tcn@ayria.se)
    Started: 03-01-2018
    License: MIT

    Simple filesystem IO.
*/

#pragma once
#include "../Stdinclude.hpp"

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

    return std::move(Buffer.get());
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

// List all files in a directory.
#if defined(_WIN32)
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
                if (std::strstr(Filedata->d_name, Extension.c_str()))
                    Filenames->push_back(Filedata->d_name);
    }
    closedir(Filehandle);

    return !!Filenames->size();
}
#endif
