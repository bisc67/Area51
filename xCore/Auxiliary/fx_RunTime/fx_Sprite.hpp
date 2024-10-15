//==============================================================================
//
//  fx_Sprite.hpp
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================

#include "fx_Mgr.hpp"

//==============================================================================
//  TYPES
//==============================================================================

struct fx_edef_sprite : public fx_element_def
{
    s32     BitmapIndex;
    f32     ZBias;
}; 

//==============================================================================

class fx_sprite : public fx_element
{
public:
    void    AdvanceLogic    ( const fx_effect_base* pEffect, f32 DeltaTime );
    void    Render          ( const fx_effect_base* pEffect ) const;
};

//==============================================================================
