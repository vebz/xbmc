/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

/**
* @todo this is to disable a change coming in update 3
* We should get rid of this and switch the hooks to const
* once update 3 is rtm and we know our developers have
* updated
*/
#define DELAYIMP_INSECURE_WRITABLE_HOOKS 1
#include <DelayImp.h>
#include "Application.h"
#include "utils/StringUtils.h"

static const std::string dlls[] = {
  "ssh.dll",
  "zlib.dll",
  "sqlite3.dll",
  "dnssd.dll",
  "libxslt.dll",
  "avcodec-57.dll",
  "avfilter-6.dll",
  "avformat-57.dll",
  "avutil-55.dll",
  "postproc-54.dll",
  "swresample-2.dll",
  "swscale-4.dll"
};

FARPROC WINAPI delayHookNotifyFunc (unsigned dliNotify, PDelayLoadInfo pdli)
{
  switch (dliNotify)
  {
    case dliNotePreLoadLibrary:
      for (size_t i = 0; i < ARRAYSIZE(dlls); ++i)
      {
        if (stricmp(pdli->szDll, dlls[i].c_str()) == 0)
        {
          HMODULE hMod = LoadLibraryEx(pdli->szDll, 0, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
          return (FARPROC)hMod;
        }
      }
      break;
  }
  return NULL;
}

FARPROC WINAPI delayHookFailureFunc (unsigned dliNotify, PDelayLoadInfo pdli)
{
  switch (dliNotify)
  {
    case dliFailLoadLib:
      g_application.Stop(1);
      std::string strError = StringUtils::Format("Uh oh, can't load %s, exiting.", pdli->szDll);
      MessageBox(NULL, strError.c_str(), "XBMC: Fatal Error", MB_OK|MB_ICONERROR);
      exit(1);
      break;
  }
  return NULL;
}

// assign hook functions
#if !defined(DELAYIMP_INSECURE_WRITABLE_HOOKS)
const
#endif
PfnDliHook __pfnDliNotifyHook2 = delayHookNotifyFunc;
#if !defined(DELAYIMP_INSECURE_WRITABLE_HOOKS)
const
#endif
PfnDliHook __pfnDliFailureHook2 = delayHookFailureFunc;
