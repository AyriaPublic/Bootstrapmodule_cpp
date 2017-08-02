/*
    Initial author: Convery (tcn@ayria.se)
    Started: 02-08-2017
    License: MIT
    Notes:
        Loads all plugins from the games directory.
*/

#include "../Stdinclude.h"

// Internal dev boolean for now.
bool Reserved = false;

// Global state for the plugins.
std::vector<void * /* Pluginhandle */> Freshplugins;
std::unordered_map<std::string /* Pluginname */, void * /* Pluginhandle */> Pluginmap;
constexpr const char *Pluginextension = sizeof(void *) == sizeof(uint64_t) ? "ayria64" : "ayria32";

// Wrappers for platform functionality.
void Freelibrary(void *Libraryhandle);
void *Loadlibrary(std::string Libraryname);
void *Getfunction(void *Libraryhandle, std::string Functionname);
bool Findfiles(std::string Searchpath, std::vector<std::string> *Filenames);

// Initialize all new plugins at once.
void Initializeplugins()
{
    for (auto &Item : Freshplugins)
    {
        auto Callback = Getfunction(Item, "onInitializationStart");
        (reinterpret_cast<void (*)(bool)>(Callback))(Reserved);
    }

    Freshplugins.clear();
}

// Load a plugin into the game.
void Loadplugin(const char *Pluginname)
{
    std::string Fullpath = std::string("./Plugins/") + Pluginname;

    /*
        TODO(Convery):
        At this point we should verify the digital signature
        of the requested plugin. Alternatively ask the user
        to whitelist it if it's manually installed.
    */

    auto Libraryhandle = Loadlibrary(Fullpath);
    if(Libraryhandle)
    {
        // Add the library to global state.
        Freshplugins.push_back(Libraryhandle);
        Pluginmap[Pluginname] = Libraryhandle;
        Infoprint(va("Loaded plugin \"%s\".", Pluginname));
    }
}

// Exported functions for the plugins to use.
extern "C"
{
    EXPORT_ATTR void Finalizeplugins()
    {
        static bool Finalized = false;
        if(Finalized) return;
        Finalized = true;

        for (auto &Item : Pluginmap)
        {
            auto Callback = Getfunction(Item.second, "onInitializationDone");
            (reinterpret_cast<void (*)(bool)>(Callback))(Reserved);
        }
    }
    EXPORT_ATTR void Broadcast(uint32_t MessageID, uint32_t Messagesize, const void *Messagedata)
    {
        for (auto &Item : Pluginmap)
        {
            auto Callback = Getfunction(Item.second, "onMessage");
            (reinterpret_cast<void (*)(uint32_t, uint32_t, const void *)>(Callback))(MessageID, Messagesize, Messagedata);
        }
    }
    EXPORT_ATTR void Hotpatch(const char *Pluginname)
    {
        for (auto &Item : Pluginmap)
        {
            if(std::strstr(Item.first.c_str(), Pluginname))
            {
                Freelibrary(Item.second);
            }
        }

        Loadplugin(Pluginname);
        Initializeplugins();
    }
}

// Load all plugins from the directory.
void Loadallplugins()
{
    std::vector<std::string> Filenames;

    // Find all plugins in the folder.
    if(Findfiles("./Plugins/", &Filenames))
    {
        for(auto &Item : Filenames)
        {
            Loadplugin(Item.c_str());
        }
    }

    // Initialize all new plugins.
    Initializeplugins();

    // Start a thread to call finalize if no other plugin does it.
    std::thread([]() { std::this_thread::sleep_for(std::chrono::seconds(3)); Finalizeplugins(); }).detach();
}

// Platform functionality.
#if defined (_WIN32)
#include <Windows.h>
#include <direct.h>

void Freelibrary(void *Libraryhandle)
{
    FreeLibrary(Libraryhandle);
}
void *Loadlibrary(std::string Libraryname)
{
    return LoadLibraryA(Libraryname.c_str());
}
void *Getfunction(void *Libraryhandle, std::string Functionname)
{
    return GetProcAddress(HMODULE(Libraryhandle), Functionname.c_str());
}
bool Findfiles(std::string Searchpath, std::vector<std::string> *Filenames)
{
    WIN32_FIND_DATAA Filedata;
    HANDLE Filehandle;

    // Append trailing slash, asterisk and extension.
    if (Searchpath.back() != '/') Searchpath.append("/");
    Searchpath.append("*");
    Searchpath.append(".");
    Searchpath.append(Pluginextension);

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
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>

void Freelibrary(void *Libraryhandle)
{
    dlclose(Libraryhandle);
}
void *Loadlibrary(std::string Libraryname)
{
    return dlopen(Libraryname.c_str(), RTLD_LAZY);
}
void *Getfunction(void *Libraryhandle, std::string Functionname)
{
    return dlsym(Libraryhandle, Functionname.c_str());
}
bool Findfiles(std::string Searchpath, std::vector<std::string> *Filenames)
{
    struct stat Fileinfo;
    dirent *Filedata;
    std::string Path;
    DIR *Filehandle;

    // Append trailing slash, asterisk and extension.
    if (Searchpath.back() != '/') Searchpath.append("/");
    Path = Searchpath;
    Searchpath.append("*");
    Searchpath.append(".");
    Searchpath.append(Pluginextension );

    // Iterate through the directory.
    Filehandle = opendir(Searchpath.c_str());
    while ((Filedata = readdir(Filehandle)))
    {
        // Respect hidden files and folders.
        if (Filedata->d_name[0] == '.')
            continue;

        // Get extended fileinfo.
        std::string Filepath = Path + "/" + Filedata->d_name;
        if (stat(Filepath.c_str(), &Fileinfo) == -1) continue;

        // Add the file to the list.
        if (!(Fileinfo.st_mode & S_IFDIR))
            Filenames->push_back(Filedata->d_name);
    }
    closedir(Filehandle);

    return !!Filenames->size();
}

#endif
