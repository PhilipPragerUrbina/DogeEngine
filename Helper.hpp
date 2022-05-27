//
// Created by Philip on 5/8/2022.
//

#ifndef DOGENGINE_HELPER_HPP
#define DOGENGINE_HELPER_HPP
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#ifdef _WIN32
#include <windows.h>
#elif
#include <unistd.h>
#endif

//helper utilities class
class Helper {
public:
//convert a relative path(to executable) to absolute path.
//This is so when the program is run from another directory, things like shaders which are with the executable can still be accessed
static std::string relativePath(const std::string& filename){
    return getExeDirectory().string() + "/" + filename;
}
static float randomFloat(){
    return (((float) rand() / (RAND_MAX))-0.5f);
}
private:
    //get the path of the exe(found on stackoverflow)
    static std::experimental::filesystem::path getExeDirectory()
    {
#ifdef _WIN32
        // Windows specific
        wchar_t szPath[MAX_PATH];
        GetModuleFileNameW( NULL, szPath, MAX_PATH );
#else
        // Linux specific
    char szPath[PATH_MAX];
    ssize_t count = readlink( "/proc/self/exe", szPath, PATH_MAX );
    if( count < 0 || count >= PATH_MAX )
        return {}; // some error
    szPath[count] = '\0';
#endif
        return std::experimental::filesystem::path{ szPath }.parent_path() / ""; // to finish the folder path with (back)slash
    }

};

#endif DOGENGINE_HELPER_HPP