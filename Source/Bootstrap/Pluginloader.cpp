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
constexpr const char *Pluginextension = ".Ayria";

// Wrappers for platform functionality.
std::string Temporarydir();
void Freelibrary(void *Libraryhandle);
void *Loadlibrary(std::string Libraryname);
void *Getfunction(void *Libraryhandle, std::string Functionname);

// Initialize all new plugins at once.
void Initializeplugins()
{
    Printfunction();

    for (auto &Item : Freshplugins)
    {
        auto Callback = Getfunction(Item, "onInitializationStart");
        (reinterpret_cast<void (*)(bool)>(Callback))(Reserved);
    }

    Freshplugins.clear();
}

// Load a plugin into the game.
void Loadplugin(const char *Pluginname, const char *Fullpath)
{
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
    else
    {
        Infoprint(va("Failed to load plugin \"%s\".", Pluginname));
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

        std::string Path = std::string("./") + Pluginname;
        Loadplugin(Pluginname, Path.c_str());
        Initializeplugins();
    }
}

// Load all plugins from the directory.
void Loadallplugins()
{
    std::vector<std::string> Filenames;

    // Find all plugins in the folder.
    if(Findfiles("./Plugins/", &Filenames, Pluginextension))
    {
        Infoprint(va("Found %i plugins.", Filenames.size()));

        for(auto &Item : Filenames)
        {
            auto Collection = Ayriapackage::Readarchive("./Plugins/" + Item);
            auto Plugin = Ayriapackage::Find(Pluginextension, Collection);
            if(!Plugin) continue;

            // Temporary storage.
            auto Path = Temporarydir() + "/" + Plugin->first;
            std::remove(Path.c_str());

            // Write to disk.
            std::FILE *Filehandle = std::fopen(Path.c_str(), "wb");
            if(!Filehandle) continue;
            std::fwrite(Plugin->second.data(), Plugin->second.size(), 1, Filehandle);
            std::fclose(Filehandle);

            // Load the plugin from temp storage.
            Loadplugin(Plugin->first.c_str(), Path.c_str());
        }
    }

    // Sideload a developer plugin if available.
    Loadplugin("Devplugin", "./Plugins/Developerplugin");

    // Initialize all new plugins.
    Initializeplugins();

    // Start a thread to call finalize if no other plugin does it.
    std::thread([]() { std::this_thread::sleep_for(std::chrono::seconds(3)); Finalizeplugins(); }).detach();
}

// Platform functionality.
#if defined (_WIN32)
std::string Temporarydir()
{
    char Buffer[1024]{};
    GetTempPathA(1024, Buffer);
    return { Buffer };
}
void Freelibrary(void *Libraryhandle)
{
    FreeLibrary(HMODULE(Libraryhandle));
}
void *Loadlibrary(std::string Libraryname)
{
    return LoadLibraryA(Libraryname.c_str());
}
void *Getfunction(void *Libraryhandle, std::string Functionname)
{
    return GetProcAddress(HMODULE(Libraryhandle), Functionname.c_str());
}

#else

std::string Temporarydir()
{
    auto Folder = getenv("TMPDIR");
    if(Folder) return { Folder };

    Folder = getenv("TMP");
    if(Folder) return { Folder };

    Folder = getenv("TEMP");
    if(Folder) return { Folder };

    Folder = getenv("TEMPDIR");
    if(Folder) return { Folder };

    return "/tmp";
}
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

#endif
