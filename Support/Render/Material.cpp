#include "Material.hpp"

//=============================================================================

material::material( void )
{
    m_RefCount = 0;
}

//=============================================================================

material::~material( void )
{
    //#### This assert should be a valid thing to do, but we don't shut down
    // resources properly right now...blah...I hate that.
    //ASSERT( GetRefCount() == 0 );
}

//=============================================================================

xbool material::operator== ( material& RHS ) const
{
    if ( m_DiffuseMap.GetIndex()     != RHS.m_DiffuseMap.GetIndex() )       return FALSE;
    if ( m_EnvironmentMap.GetIndex() != RHS.m_EnvironmentMap.GetIndex() )   return FALSE;
    if ( m_DetailMap.GetIndex()      != RHS.m_DetailMap.GetIndex() )        return FALSE;
    if ( m_Type                      != RHS.m_Type )                        return FALSE;
    if ( m_DetailScale               != RHS.m_DetailScale )                 return FALSE;
    if ( m_FixedAlpha                != RHS.m_FixedAlpha )                  return FALSE;
    if ( m_Flags                     != RHS.m_Flags )                       return FALSE;

    if ( x_memcmp( &m_UVAnim, &RHS.m_UVAnim, sizeof(m_UVAnim) ) )
        return FALSE;

    return TRUE;
}

//=============================================================================

//=============================================================================
// XBOX XBOX XBOX XBOX XBOX XBOX XBOX XBOX XBOX XBOX XBOX XBOX XBOX XBOX XBOX !
//=============================================================================

#if (defined TARGET_XBOX) //&& !(defined CONFIG_RETAIL)

#include "Entropy.hpp"
#include "Render.hpp"

#include "xbox_render.hpp"
#include <XGraphics.h>

//=============================================================================

static u32 s_dwRigidDesc[]=
{
    D3DVSD_STREAM( 0 ) ,
    D3DVSD_REG   ( 0, D3DVSDT_FLOAT3      ) , // v0-position
    D3DVSD_REG   ( 1, D3DVSDT_NORMPACKED3 ) , // v1-normal
    D3DVSD_REG   ( 2, D3DVSDT_FLOAT2      ) , // v3-uv
    D3DVSD_STREAM( 1 ),
    D3DVSD_REG   ( 3, D3DVSDT_D3DCOLOR    ) , // colour
    D3DVSD_END( )
};

//=============================================================================

static u32 s_dwSkinDesc[]=
{
    D3DVSD_STREAM( 0 ) ,
    D3DVSD_REG   ( 0, D3DVSDT_FLOAT3      ) , // v0-position
    D3DVSD_REG   ( 1, D3DVSDT_NORMPACKED3 ) , // v1-normal
    D3DVSD_REG   ( 2, D3DVSDT_FLOAT4      ) , // v2-uv/weights
    D3DVSD_REG   ( 3, D3DVSDT_FLOAT2      ) , // v3-bones
    D3DVSD_END( )
};

//=============================================================================

static void DisplayBuffer( char* pTextIn )
{
#ifndef CONFIG_RETAIL

    char* pDst = pTextIn;
    char* pSrc = pTextIn;
    {
        u32 TotalBytes = x_strlen( pTextIn );
        s32 nLines     = 0;

        for( u32 j=0;j<TotalBytes;j++ )
        {
            if( pSrc[j]==0x0D )
                continue;
            if( pSrc[j]==0x0A )
            {
                nLines++;
                *pDst=0;
                pDst++;
            }
            else
            {
                *pDst = pSrc[j];
                pDst++;
            }
        }
        *pDst = 0;

        OutputDebugString("\n***********************************************************************************\n\n" );

        for( s32 iLine=0;iLine<nLines;iLine++ )
        {
            OutputDebugString( xfs("%3d: %s\n",iLine+1,pSrc ));
            pSrc += x_strlen( pSrc )+1;
        }
    }

#endif
}

//=============================================================================

#pragma auto_inline(off)

void vs::path::compile( const char* pShaderText,shader& Result )
{
#if COMPILE_SHADERS

    bool bSkin=( Flags.oPos_Skin || Flags.oSt_CastShadowSkin );
    u32 TotalBytes = x_strlen( pShaderText );
    u32 dwType;
    {
        // Compile shader /////////////////////////////////////////////////////

        XGBuffer* pMicrocode;
        XGBuffer* pErrorLog;
        {
        #ifdef X_RETAIL
            u32 Flags = SASM_USE_V2_OPTIMIZER;
        #else
            u32 Flags = SASM_DONOTOPTIMIZE;
        #endif
            HRESULT hr = XGAssembleShader(
                NULL,
                pShaderText,
                TotalBytes,
                Flags|SASM_SKIPPREPROCESSOR,
                NULL,
                &pMicrocode,
                &pErrorLog,
                NULL,
                NULL,
                NULL,
                (DWORD*)&dwType );
            if( hr )
            {
                DisplayBuffer( (char*)pShaderText );
            __asm int 3
            }
        }

        // Create shader object ///////////////////////////////////////////////

        u32* pShaderDesc = bSkin?s_dwSkinDesc:s_dwRigidDesc;
        Result.shader::shader(
            dwType,
            *pMicrocode,
            pShaderDesc
        );
    }

#endif
}

//=============================================================================

void ps::path::compile( const char* pShaderText,shader& Result )
{
#if COMPILE_SHADERS

    u32 TotalBytes = x_strlen( pShaderText );
    u32 dwType;
    {
        // Compile shader /////////////////////////////////////////////////////

        XGBuffer* pMicrocode;
        XGBuffer* pErrorLog;
        {
        #ifdef X_RETAIL
            u32 Flags = SASM_USE_V2_OPTIMIZER;
        #   ifdef CONFIG_RETAIL
                Flags |= SASM_SKIPVALIDATION;
        #   endif
        #else
            u32 Flags = SASM_DONOTOPTIMIZE;
        #endif
            HRESULT hr = XGAssembleShader(
                NULL,
                pShaderText,
                TotalBytes,
                Flags|SASM_SKIPPREPROCESSOR,
                NULL,
                &pMicrocode,
                &pErrorLog,
                NULL,
                NULL,
                NULL,
                (LPDWORD)&dwType
            );
            if( hr )
            {
                DisplayBuffer( (char*)pShaderText );
            __asm int 3
            }
        }

        // Create shader object ///////////////////////////////////////////////

        Result.shader::shader(
            dwType,
            *pMicrocode,
            NULL
        );
    }

#endif
}

#endif
