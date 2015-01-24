//--------------------------------------------------------------------------------------
// File: SDKMisc.h
//
// Various helper functionality that is shared between SDK samples
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef SDKMISC_H
#define SDKMISC_H
HRESULT WINAPI DXUTFindDXSDKMediaFileCch( __in_ecount(cchDest) WCHAR* strDestPath,
                                          int cchDest, 
                                          __in LPCWSTR strFilename );

//-----------------------------------------------------------------------------
// Resource cache for textures, fonts, meshs, and effects.  
// Use DXUTGetGlobalResourceCache() to access the global cache
//-----------------------------------------------------------------------------

enum DXUTCACHE_SOURCELOCATION
{
    DXUTCACHE_LOCATION_FILE,
    DXUTCACHE_LOCATION_RESOURCE
};







#endif
