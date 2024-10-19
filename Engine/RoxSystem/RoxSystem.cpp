// Updated By the ROX_ENGINE
// Copyright (C) 2024 Torox Project
// Portions Copyright (C) 2013 nyan.developer@gmail.com (nya-engine)
//
// This file was modified by the Torox Project.
// Drop support for METRO, FLUENT style.
// Drop Support for Android, iOS
// Update the code to be compatible with the latest version of the engine.
// Optimasation and code cleaning for a better performance.
// 
// This file incorporates code from the nya-engine project, which is licensed under the MIT License.
// See the LICENSE-MIT file in the root directory for more information.
//
// This file is also part of the Rox-engine, which is licensed under a dual-license system:
// 1. Free Use License (for non-commercial and commercial use under specific conditions)
// 2. Commercial License (for use on proprietary platforms)
// See the LICENSE file in the root directory for the full Rox-engine license terms.

#include "RoxSystem.h"

#include <iomanip>

#if defined __APPLE__
    #include <mach-o/dyld.h>
    #include "TargetConditionals.h"
    #include <string>

#elif defined _WIN32
    #include <windows.h>
    #include <cstring>

    #if defined(_MSC_VER) && _MSC_VER >= 1900
		#if _WIN32_WINNT >= _WIN32_WINNT_WIN10

			#include "winapifamily.h"

		#endif
    #endif
#endif


#ifndef _WIN32
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include <iostream>
    #include <string.h>
#endif

namespace
{
    RoxLogger::RoxLoggerBase *system_log=0;
}

namespace RoxSystem
{
void setLog(RoxLogger::RoxLoggerBase *l) {
    system_log=l;
}

RoxLogger::RoxLoggerBase &log()
{
    if(!system_log)
        return RoxLogger::log();
    
    return *system_log;
}

const char *getAppPath()
{
    const size_t max_path=4096;
    static char path[max_path]="";
    static bool has_path=false;
    if(!has_path)
    {
#ifdef __APPLE__
        uint32_t path_length=max_path;
        _NSGetExecutablePath(path,
                             &path_length);
        
        std::string path_str(path);
        size_t p=path_str.rfind(".app");
        
        size_t p2=path_str.rfind("/",
                                 p);
        
        if(p2!=std::string::npos)
            path[p2+1]='\0';
        else
            path[0]='\0';
#elif defined _WIN32

        GetModuleFileNameA(0,
                           path,
                           max_path);
        
        char *last_slash=strrchr(path,
                                 '\\');
        if(last_slash)
            *(last_slash+1) = 0;

        for(size_t i=0;i<max_path;++i)
        {
            if(path[i]=='\\')
                path[i]='/';
        }

#elif EMSCRIPTEN
        strcpy(path,
               "/");
#else
        readlink("/proc/self/exe",
                 path,
                 max_path);
        
        char *last_slash=strrchr(path,
                                 '/');
        if(last_slash)
            *(last_slash+1)=0;
#endif
        has_path=true;
    }
    
    return path;
}

const char *getUserPath()
{
    const size_t max_path=4096;
    static char path[max_path]="";
    static bool has_path=false;
    if(!has_path)
    {
#ifdef _WIN32

        const char *p=getenv("APPDATA");
        if(!p)
            return 0;
        
        strcpy(path,
               p);
        strcat(path,
               "/");

        for(size_t i=0;i<max_path;++i)
        {
            if(path[i]=='\\')
                path[i]='/';
        }

#elif TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        if(!get_ios_user_path(path))
            return 0;
#else
        const char *p=getenv("HOME");
        if (!p)
            p=getpwuid(getuid())->pw_dir;
        if(!p)
            return 0;
        
        strcpy(path,
               p);
        strcat(path,
               "/");
#endif
        has_path=true;
    }
    
    return path;
}

#ifdef _WIN32
#include <ctime>

#pragma comment ( lib, "WINMM.LIB" )

unsigned long getTime()
{
    return ::std::time(nullptr);
}

#else

#include <sys/time.h>

unsigned long getTime()
{
    timeval tim;
    gettimeofday(&tim,
                 0);
    unsigned long sec=(unsigned long)tim.tv_sec;
    return (sec*1000+(tim.tv_usec/1000));
}

#endif

}
