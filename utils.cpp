//-------------------------------------------------------------------------------------
// utils.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"
#include "TestHelpers.h"

#include "uvatlas.h"

#include <algorithm>
#include <random>

using namespace DirectX;

//-------------------------------------------------------------------------------------
// UVAtlasApplyRemap (no duplicates)
bool Test09()
{
    bool success = true;

    // Identity (32)
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        auto destvb = CreateVertexBuffer( 32, 65535 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 32, 65535, 65535, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(32) identity failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsTestVBCorrect32( destvb.get(), 65535, VB_IDENTITY ) )
        {
            printe("\nERROR: UVAtlasApplyRemap(32) identity failed\n" );
            success = false;
        }
        else
        {
            auto ptr = reinterpret_cast<const Vertex32*>( destvb.get() );
            for( size_t j = 0; j < 65535; ++j )
            {
                float f = float(j);
                if ( ( ptr->pos.x != f )
                        || ( ptr->pos.y != (f + 0.1f) )
                        || ( ptr->pos.z != (f + 0.2f) )
                        || ( ptr->norm.x != (f + 0.3f) )
                        || ( ptr->norm.y != (f + 0.4f) ) 
                        || ( ptr->norm.z != (f + 0.5f) )
                        || ( ptr->tex.x != (f + 0.6f) )
                        || ( ptr->tex.y != (f + 0.7f) ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(32) identity failed (2)\n" );
                    success = false;
                    break;
                }
                ++ptr;
            }
        }

        // invalid args
        hr = UVAtlasApplyRemap( srcvb.get(), 32, 65535, 65535, nullptr, destvb.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: UVAtlasApplyRemap nullptr remap expected failure\n" );
            success = false;
        }

        hr = UVAtlasApplyRemap( srcvb.get(), UINT32_MAX, 65535, 65535, &remap.front(), destvb.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: UVAtlasApplyRemap expected failure for bad stride value (%08X)\n", hr );
            success = false;
        }

        hr = UVAtlasApplyRemap( srcvb.get(), 32, UINT32_MAX, UINT32_MAX, &remap.front(), destvb.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: UVAtlasApplyRemap expected failure for 32-max value verts (%08X)\n", hr );
            success = false;
        }

        hr = UVAtlasApplyRemap( srcvb.get(), 32, 65535, 256, &remap.front(), destvb.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: UVAtlasApplyRemap expected failure for newnverts < verts (%08X)\n", hr );
            success = false;
        }
    }

    // Reverse (32)
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( 65534 - j );

        auto destvb = CreateVertexBuffer( 32, 65535 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 32, 65535, 65535, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(32) reverse failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsTestVBCorrect32( destvb.get(), 65535, VB_REVERSE ) )
        {
            printe("\nERROR: UVAtlasApplyRemap(32) reverse failed\n" );
            success = false;
        }
    }

    // Shuffle (32)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        std::random_shuffle( remap.begin(), remap.end() );

        auto destvb = CreateVertexBuffer( 32, 65535 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 32, 65535, 65535, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(32) shuffle failed (%08X)\n", hr );
            printe("\t[%Iu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 65535; ++j )
            {
                auto ptr = reinterpret_cast<const Vertex32*>( destvb.get() + 32*j );
                float f = float( remap[j] );
                if ( ( ptr->pos.x != f )
                        || ( ptr->pos.y != (f + 0.1f) )
                        || ( ptr->pos.z != (f + 0.2f) )
                        || ( ptr->norm.x != (f + 0.3f) )
                        || ( ptr->norm.y != (f + 0.4f) ) 
                        || ( ptr->norm.z != (f + 0.5f) )
                        || ( ptr->tex.x != (f + 0.6f) )
                        || ( ptr->tex.y != (f + 0.7f) ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(32) shuffle failed\n" );
                    printe("\t[%Iu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
                    success = false;
                    break;
                }
            }
        }
    }

    // Identity (16)
    {
        auto srcvb = CreateVertexBuffer16( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        auto destvb = CreateVertexBuffer( 16, 65535 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 16, 65535, 65535, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(16) identity failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsTestVBCorrect16( destvb.get(), 65535, VB_IDENTITY ) )
        {
            printe("\nERROR: UVAtlasApplyRemap(16) identity failed\n" );
            success = false;
        }
        else
        {
            auto ptr = reinterpret_cast<const GUID*>( destvb.get() );
            for( size_t j = 0; j < 65535; ++j )
            {
                if ( memcmp( reinterpret_cast<const BYTE*>(ptr) + sizeof(DWORD),
                             reinterpret_cast<const BYTE*>(&g_VBGuid) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD) ) != 0
                     || ptr->Data1 != DWORD(j) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(16) identity failed (2)\n" );
                    success = false;
                    break;
                }
                ++ptr;
            }
        }
    }

    // Reverse (16)
    {
        auto srcvb = CreateVertexBuffer16( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( 65534 - j );

        auto destvb = CreateVertexBuffer( 16, 65535 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 16, 65535, 65535, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(16) reverse failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsTestVBCorrect16( destvb.get(), 65535, VB_REVERSE ) )
        {
            printe("\nERROR: UVAtlasApplyRemap(16) reverse failed\n" );
            success = false;
        }
    }

    // Shuffle (16)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcvb = CreateVertexBuffer16( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        std::random_shuffle( remap.begin(), remap.end() );

        auto destvb = CreateVertexBuffer( 16, 65535 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 16, 65535, 65535, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(16) shuffle failed (%08X)\n", hr );
            printe("\t[%Iu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 65535; ++j )
            {
                auto ptr = reinterpret_cast<const GUID*>( destvb.get() + 16*j );
                if ( memcmp( reinterpret_cast<const BYTE*>(ptr) + sizeof(DWORD),
                             reinterpret_cast<const BYTE*>(&g_VBGuid) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD) ) != 0
                     || ptr->Data1 != DWORD( remap[j] ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(16) shuffle failed\n" );
                    printe("\t[%Iu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
                    success = false;
                    break;
                }
            }
        }
    }

    // Face-mapped Cube (4)
    {
        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 24 ] );
        for( uint32_t j=0; j < 24; ++j)
            srcvb[ j ] = j;

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 24 ] );
        memset( destvb.get(), 0xff, sizeof(uint32_t) * 24 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), sizeof(uint32_t), 24, 24, s_remap, destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: UVAtlasApplyRemap fmcube failed (%08X)\n", hr );
            success = false;
        }
        else if ( memcmp( destvb.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: UVAtlasApplyRemap fmcube failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                printe("\t%Iu: %u .. %u\n", j, destvb[ j ], s_remap[ j ] );  
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// UVAtlasApplyRemap (with duplicates)
bool Test10()
{
    bool success = true;

    // Identity (32)
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 + 256 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        for( uint32_t j = 0; j < 256; ++j )
            remap.push_back( j );

        auto destvb = CreateVertexBuffer( 32, 65535 + 256 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 32, 65535, 65535 + 256, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(32) dups identity failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsTestVBCorrect32( destvb.get(), 65535, VB_IDENTITY ) )
        {
            printe("\nERROR: UVAtlasApplyRemap(32) dups identity failed\n" );
            success = false;
        }
        else
        {
            auto ptr = reinterpret_cast<const Vertex32*>( destvb.get() );
            for( size_t j = 0; j < 65535; ++j )
            {
                float f = float(j);
                if ( ( ptr->pos.x != f )
                        || ( ptr->pos.y != (f + 0.1f) )
                        || ( ptr->pos.z != (f + 0.2f) )
                        || ( ptr->norm.x != (f + 0.3f) )
                        || ( ptr->norm.y != (f + 0.4f) ) 
                        || ( ptr->norm.z != (f + 0.5f) )
                        || ( ptr->tex.x != (f + 0.6f) )
                        || ( ptr->tex.y != (f + 0.7f) ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(32) dups identity failed\n" );
                    success = false;
                    break;
                }
                ++ptr;
            }

            for( size_t j = 0; j < 256; ++j )
            {
                float f = float(j);
                if ( ( ptr->pos.x != f )
                        || ( ptr->pos.y != (f + 0.1f) )
                        || ( ptr->pos.z != (f + 0.2f) )
                        || ( ptr->norm.x != (f + 0.3f) )
                        || ( ptr->norm.y != (f + 0.4f) ) 
                        || ( ptr->norm.z != (f + 0.5f) )
                        || ( ptr->tex.x != (f + 0.6f) )
                        || ( ptr->tex.y != (f + 0.7f) ) )
                {
                    printe("ERROR: UVAtlasApplyRemap(32) dups identity failed (2)\n" );
                    success = false;
                    break;
                }
                ++ptr;
            }
        }
    }

    // Reverse (32)
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 + 256 );
        for( uint32_t j = 0; j < 256; ++j )
            remap.push_back( 255 - j );

        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( 65534 - j );

        auto destvb = CreateVertexBuffer( 32, 65535 + 256 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 32, 65535, 65535 + 256, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(32) dups reverse failed (%08X)\n", hr );
            success = false;
        }
        else
        {
            auto ptr = reinterpret_cast<const Vertex32*>( destvb.get() );
            for( size_t j = 0; j < 65535; ++j )
            {
                float f = float( remap[j] );
                if ( ( ptr->pos.x != f )
                        || ( ptr->pos.y != (f + 0.1f) )
                        || ( ptr->pos.z != (f + 0.2f) )
                        || ( ptr->norm.x != (f + 0.3f) )
                        || ( ptr->norm.y != (f + 0.4f) ) 
                        || ( ptr->norm.z != (f + 0.5f) )
                        || ( ptr->tex.x != (f + 0.6f) )
                        || ( ptr->tex.y != (f + 0.7f) ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(32) dups reverse failed\n" );
                    success = false;
                    break;
                }
                ++ptr;
            }

            for( size_t j = 0; j < 256; ++j )
            {
                float f = float( remap[j] );
                if ( ( ptr->pos.x != f )
                        || ( ptr->pos.y != (f + 0.1f) )
                        || ( ptr->pos.z != (f + 0.2f) )
                        || ( ptr->norm.x != (f + 0.3f) )
                        || ( ptr->norm.y != (f + 0.4f) ) 
                        || ( ptr->norm.z != (f + 0.5f) )
                        || ( ptr->tex.x != (f + 0.6f) )
                        || ( ptr->tex.y != (f + 0.7f) ) )
                {
                    printe("ERROR: UVAtlasApplyRemap(32) dups reverse failed (2)\n" );
                    success = false;
                    break;
                }
                ++ptr;
            }
        }
    }

    // Shuffle (32)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 + 256 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        for( uint32_t j = 0; j < 256; ++j )
            remap.push_back( j );

        std::random_shuffle( remap.begin(), remap.end() );

        auto destvb = CreateVertexBuffer( 32, 65535 + 256 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 32, 65535, 65535 + 256, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(32) dups shuffle failed (%08X)\n", hr );
            printe("\t[%Iu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
            success = false;
        }
        else
        {
            auto ptr = reinterpret_cast<const Vertex32*>( destvb.get() );
            for( size_t j = 0; j < 65535 + 256; ++j )
            {
                float f = float( remap[j] );
                if ( ( ptr->pos.x != f )
                        || ( ptr->pos.y != (f + 0.1f) )
                        || ( ptr->pos.z != (f + 0.2f) )
                        || ( ptr->norm.x != (f + 0.3f) )
                        || ( ptr->norm.y != (f + 0.4f) ) 
                        || ( ptr->norm.z != (f + 0.5f) )
                        || ( ptr->tex.x != (f + 0.6f) )
                        || ( ptr->tex.y != (f + 0.7f) ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(32) dups shuffle failed\n" );
                    printe("\t[%Iu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
                    success = false;
                    break;
                }
                ++ptr;
            }
        }
    }

    // Identity (16)
    {
        auto srcvb = CreateVertexBuffer16( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 + 256 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        for( uint32_t j = 0; j < 256; ++j )
            remap.push_back( j );

        auto destvb = CreateVertexBuffer( 16, 65535 + 256 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 16, 65535, 65535 + 256, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(16) dups identity failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsTestVBCorrect16( destvb.get(), 65535, VB_IDENTITY ) )
        {
            printe("\nERROR: UVAtlasApplyRemap(16) dups identity failed\n" );
            success = false;
        }
        else
        {
            auto ptr = reinterpret_cast<const GUID*>( destvb.get() );
            for( size_t j = 0; j < 65535; ++j )
            {
                if ( memcmp( reinterpret_cast<const BYTE*>(ptr) + sizeof(DWORD),
                             reinterpret_cast<const BYTE*>(&g_VBGuid) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD) ) != 0
                     || ptr->Data1 != DWORD( remap[j] ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(16) dups identity failed\n" );
                    success = false;
                    break;
                }
                ++ptr;
            }

            for( size_t j = 0; j < 256; ++j )
            {
                if ( memcmp( reinterpret_cast<const BYTE*>(ptr) + sizeof(DWORD),
                             reinterpret_cast<const BYTE*>(&g_VBGuid) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD) ) != 0
                     || ptr->Data1 != DWORD( remap[j] ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(16) dups identity failed (2)\n" );
                    success = false;
                    break;
                }
                ++ptr;
            }
        }
    }

    // Reverse (16)
    {
        auto srcvb = CreateVertexBuffer16( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 + 256 );
        for( uint32_t j = 0; j < 256; ++j )
            remap.push_back( 255 - j );

        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( 65534 - j );

        auto destvb = CreateVertexBuffer( 16, 65535 + 256 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 16, 65535, 65535 + 256, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(16) dups reverse failed (%08X)\n", hr );
            success = false;
        }
        else
        {
            auto ptr = reinterpret_cast<const GUID*>( destvb.get() );
            for( size_t j = 0; j < 65535; ++j )
            {
                if ( memcmp( reinterpret_cast<const BYTE*>(ptr) + sizeof(DWORD),
                             reinterpret_cast<const BYTE*>(&g_VBGuid) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD) ) != 0
                     || ptr->Data1 != DWORD( remap[j] ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(16) dups reverse failed\n" );
                    success = false;
                    break;
                }
                ++ptr;
            }

            for( size_t j = 0; j < 256; ++j )
            {
                if ( memcmp( reinterpret_cast<const BYTE*>(ptr) + sizeof(DWORD),
                             reinterpret_cast<const BYTE*>(&g_VBGuid) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD) ) != 0
                     || ptr->Data1 != DWORD( remap[j] ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(16) dups reverse failed (2)\n" );
                    success = false;
                    break;
                }
                ++ptr;
            }
        }
    }

    // Shuffle (16)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcvb = CreateVertexBuffer16( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 + 256 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        for( uint32_t j = 0; j < 256; ++j )
            remap.push_back( j );

        std::random_shuffle( remap.begin(), remap.end() );

        auto destvb = CreateVertexBuffer( 16, 65535 + 256 );

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), 16, 65535, 65535 + 256, &remap.front(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: UVAtlasApplyRemap(16) dups shuffle failed (%08X)\n", hr );
            printe("\t[%Iu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
            success = false;
        }
        else
        {
            auto ptr = reinterpret_cast<const GUID*>( destvb.get() );
            for( size_t j = 0; j < 65535 + 256; ++j )
            {
                if ( memcmp( reinterpret_cast<const BYTE*>(ptr) + sizeof(DWORD),
                             reinterpret_cast<const BYTE*>(&g_VBGuid) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD) ) != 0
                     || ptr->Data1 != DWORD( remap[j] ) )
                {
                    printe("\nERROR: UVAtlasApplyRemap(16) dups shuffle failed\n" );
                    printe("\t[%Iu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
                    success = false;
                    break;
                }
                ++ptr;
            }
        }
    }

    // Face-mapped Cube (4)
    {
        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 24 ] );
        for( uint32_t j=0; j < 24; ++j)
            srcvb[ j ] = j;

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 28 ] );
        memset( destvb.get(), 0xff, sizeof(uint32_t) * 28 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23,
                                            11, 7, 16, 2 };

        HRESULT hr = UVAtlasApplyRemap( srcvb.get(), sizeof(uint32_t), 24, 28, s_remap, destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: UVAtlasApplyRemap dups fmcube failed (%08X)\n", hr );
            success = false;
        }
        else if ( memcmp( destvb.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: UVAtlasApplyRemap dups fmcube failed\n" );
            success = false;
            for( size_t j = 0; j < 28; ++j )
            {
                printe("\t%Iu: %u .. %u\n", j, destvb[ j ], s_remap[ j ] );  
            }
        }
    }

    return success;
}
