//==============================================================================
//
//  NetRocket.hpp
//
//==============================================================================

#ifndef NETROCKET_HPP
#define NETROCKET_HPP

//==============================================================================
//  INCLUDES
//==============================================================================

#include "NetObj.hpp"
#include "NetObjMgr.hpp"

//==============================================================================
//  DEFINES
//==============================================================================


//==============================================================================
//  TYPES
//==============================================================================

class net_rocket : public netobj
{

//------------------------------------------------------------------------------
//  Public Functions
//------------------------------------------------------------------------------

public:

//------------------------------------------------------------------------------
//  Public functions
//------------------------------------------------------------------------------

public:

virtual void        Activate        ( void );
virtual void        Deactivate      ( void );
                    
virtual void        CreateGameObj   ( void );
virtual void        DestroyGameObj  ( void );
                    
virtual void        OnAcceptUpdate  ( const bitstream& BitStream );
virtual void        OnProvideUpdate (       bitstream& BitStream, 
                                            u32&       DirtyBits, 
                                            s32        Client,
                                      const delta*     pDelta );

virtual void        Logic           ( void );

virtual void        DebugRender     ( void ) const;

        void        Setup           ( const vector3&   Position,
                                      const vector3&   Velocity,
                                            f32        FuseTime );

//------------------------------------------------------------------------------
//  Private Types
//------------------------------------------------------------------------------

    enum dirty_bits
    {
//      DEACTIVATE_BIT = 0x80000000,
//      ACTIVATE_BIT   = 0x40000000,
//      DELTA_DATA_BIT = 0x20000000,
    };

//------------------------------------------------------------------------------
//  Private Data
//------------------------------------------------------------------------------

protected:

        vector3         m_Position;
        vector3         m_Velocity;
        f32             m_FuseTime;

        vector3         m_BlendOffset;
        f32             m_BlendFactor;
};

//==============================================================================
#endif // NETROCKET_HPP
//==============================================================================
