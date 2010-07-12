// Copyright (c) 2010 Jeroen Dierckx - Expertise Centre for Digital Media. All Rights reserved.
// This source code is developed for the flemish (Belgian) OSMA project (http://osma.phl.be/)
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UNITYBERKELIUMPLUGIN_GLOBAL_H
#define UNITYBERKELIUMPLUGIN_GLOBAL_H

// Windows and pocket pc
#if defined(_WIN32_WCE) || defined(WIN32)
	// Identifier was truncated to '255' characters in the debug information
	#pragma warning (disable: 4786)

	// STL classes are not exported in the dll
	/*!
		@note This can be ignored, since none of the stl members are public,
		so they shouldn't be exported in the dll
	*/
	#pragma warning (disable: 4251)

	#define WIN32_LEAN_AND_MEAN
	#define _CRT_SECURE_NO_WARNINGS
	#include <windows.h>

	// Windows plugin defines
	#define PLUGIN_API extern "C" __declspec(dllexport)

// Linux, Mac OS, ...
#else
	// Make compatible with the windows function
	#define stricmp strcasecmp

	// Plugin defines
	#define PLUGIN_API extern "C"
#endif


#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;

#include <cassert>


// Defines
#ifndef NULL
	#define NULL 0
#endif // NULL


#endif // UNITYBERKELIUMPLUGIN_GLOBAL_H
