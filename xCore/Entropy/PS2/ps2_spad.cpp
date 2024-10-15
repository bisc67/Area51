//=========================================================================
//
// PS2_SPAD.CPP
//
//=========================================================================

#include "x_debug.hpp"
#include "x_string.hpp"
#include "ps2_spad.hpp"

//=========================================================================
// GLOBALS
//=========================================================================

scratchpad SPAD;

//=========================================================================
// IMPLEMENTATION
//=========================================================================

scratchpad::scratchpad( void )
{
#if defined(X_ASSERT)
    m_bLocked = FALSE;
#endif
}
//=========================================================================

scratchpad::~scratchpad( void )
{
    ASSERT( !m_bLocked );
}

//=========================================================================
