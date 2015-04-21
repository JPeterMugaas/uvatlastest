//-------------------------------------------------------------------------------------
// DirectXTest.h
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>

#include <directxmath.h>
#include <directxpackedvector.h>

#define _DIRECTX_TEST_NAME_ "UVAtlas"

#define print printf
#define printe printf

#define printxmv(v) print("%s: %f,%f,%f,%f\n", #v, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetW(v))

#if 0
#define printdigest(str,digest) print( "%s: 0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x\n", str, \
                                       digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7], \
                                       digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], digest[14], digest[15] );
#else
#define printdigest(str,digest) print( "%s: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", str, \
                                       digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7], \
                                       digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], digest[14], digest[15] );
#endif

#define MESH_MEDIA_PATH L"\\\\durfs\\durango\\TestContent\\graphics\\directxmesh\\media\\"

#define TEX_MEDIA_PATH L"\\\\durfs\\durango\\TestContent\\graphics\\directxtex\\media\\"

#define TEMP_PATH L"%TEMP%\\"