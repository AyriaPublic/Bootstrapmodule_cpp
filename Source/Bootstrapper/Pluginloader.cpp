/*
    Initial author: Convery (tcn@ayria.se)
    Started: 03-01-2018
    License: MIT

    Extracts plugins to /tmp/ and loads them.
*/

#include "../Stdinclude.hpp"
#include "../Utility/zip_file.hpp"

// Secret developer boolean.
bool Reserved = false;

// Global plugin state.
std::vector<void *> Freshplugins;
std::unordered_map<std::string_view, void *> Loadedplugins;
constexpr const char *Pluignextension = sizeof(void *) == sizeof(uint32_t) ?  ".ayria32" : ".ayria64";

// Platform functionality.
std::string Temporarydir();
void Freemodule(void *Modulehandle);
void *Loadmodule(std::string_view Modulename);
void *Getfunction(void *Modulehandle, std::string_view Function);

// Initialize all fresh plugins.
void Initializeplugins()
{
    Printfunction();

    for(auto &Item : Freshplugins)
    {
        auto Callback = Getfunction(Item, "onInitializationStart");
        (reinterpret_cast<void (*)(bool)>(Callback))(Reserved);
    }

    Freshplugins.clear();
    Freshplugins.shrink_to_fit();
}

// Load a plugin from disk.
void Loadplugin(std::string_view Pluginname, std::string_view Fullpath)
{
    /*
        TODO(Convery):
        We should check the plugins code signature before loading
        it, alternatively ask the user to whitelist it in a client.
    */

    auto Modulehandle = Loadmodule(Fullpath);
    if(Modulehandle)
    {
        // Add a module to the global state.
        Freshplugins.push_back(Modulehandle);
        Loadedplugins[Pluginname] = Modulehandle;
        Infoprint(va("Loaded plugin \"%s\".", Pluginname));
    }
    else
    {
        Infoprint(va("Failed to load plugin \"%s\".", Pluginname));
    }
}

// Called from the platformwrapper or 3 sec after init.
extern "C" EXPORT_ATTR void Finalizeplugins(bool Force)
{
    // Only call finalize once.
    static bool Initialized = false;
    if(Initialized && !Force) return;
    Initialized = true;

    for (auto &Item : Loadedplugins)
    {
        auto Callback = Getfunction(Item.second, "onInitializationDone");
        (reinterpret_cast<void (*)(bool)>(Callback))(Reserved);
    }
}

// Called from a client to update a plugin.
extern "C" EXPORT_ATTR void Hotpatch(const char *Pluginname, const char *Fullpath)
{
    // Unload any existing plugin with the name.
    for(auto &Item : Loadedplugins)
    {
        if(0 == Item.first.compare(Pluginname))
        {
            Freemodule(Item.second);
        }
    }

    // Load the plugin and finalize it directly.
    Loadplugin(Pluginname, Fullpath);
    Initializeplugins(/**/);
    Finalizeplugins(true);
}

// Notify all plugins about a message event.
extern "C" EXPORT_ATTR void Broadcast(uint32_t MessageID, uint32_t Messagesize, const void *Messagedata)
{
    for (auto &Item : Loadedplugins)
    {
        auto Callback = Getfunction(Item.second, "onMessage");
        (reinterpret_cast<void (*)(uint32_t, uint32_t, const void *)>(Callback))(MessageID, Messagesize, Messagedata);
    }
}

// Load all plugins from the default directory.
void Loadallplugins()
{
    // Enumerate all plugins in the directory.
    auto Pluginnames = Findfiles("./Plugins/", ".Ayria");
    Infoprint(va("Found %i plugins.", Pluginnames.size()));

    // Extract and load the plugins.
    for(auto &Item : Pluginnames)
    {
        miniz_cpp::zip_file Archive("./Plugins/" + Item);
        for(auto &Entry : Archive.namelist())
        {
            if(std::strstr(Entry.c_str(), Pluignextension))
            {
                // Remove any already extracted plugin.
                auto Path = Temporarydir() + "/" + Entry;
                std::remove(Path.c_str());

                // Write the file to disk.
                Writefile(Path, Archive.read(Entry));

                // Load the plugin from disk.
                Loadplugin(Entry, Path);
            }
        }
    }

    // Sideload any developer plugin.
    Loadplugin("Devplugin", "./Plugins/Developerplugin");

    // Initialize the new plugins.
    Initializeplugins();

    // Call finalization if no other plugins call it.
    std::thread([]() { std::this_thread::sleep_for(std::chrono::seconds(3)); Finalizeplugins(false); }).detach();
}

// Platform functionality.
#if defined(_WIN32)

std::string Temporarydir()
{
    char Buffer[1024]{};
    GetTempPathA(1024, Buffer);
    return std::move(Buffer);
}
void Freemodule(void *Modulehandle)
{
    FreeLibrary(HMODULE(Modulehandle));
}
void *Loadmodule(std::string_view Modulename)
{
    return LoadLibraryA(Modulename.data());
}
void *Getfunction(void *Modulehandle, std::string_view Functionname)
{
    return GetProcAddress(HMODULE(Modulehandle), Functionname.data());
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
void Freemodule(void *Modulehandle)
{
    dlclose(Modulehandle);
}
void *Loadmodule(std::string_view Modulename)
{
    return dlopen(Modulename.data(), RTLD_LAZY);
}
void *Getfunction(void *Modulehandle, std::string_view Functionname)
{
    return dlsym(Modulehandle, Functionname.data());
}

#endif
