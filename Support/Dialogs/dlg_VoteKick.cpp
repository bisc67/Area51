//=========================================================================
//
//  dlg_vote_kick.cpp
//
//=========================================================================

#include "entropy.hpp"

#include "ui\ui_font.hpp"
#include "ui\ui_manager.hpp"
#include "ui\ui_control.hpp"
#include "ui\ui_playerlist.hpp"

#include "dlg_PopUp.hpp"
#include "dlg_VoteKick.hpp"

#include "StateMgr\StateMgr.hpp"
#include "stringmgr\stringmgr.hpp"
#include "Objects\Player.hpp"

//=========================================================================
//  Vote Map Dialog
//=========================================================================

enum controls
{
	IDC_VOTE_KICK_LISTBOX,
    IDC_VOTE_KICK_NAV_TEXT,
};


ui_manager::control_tem VoteKickControls[] = 
{
    // Frames.
    { IDC_VOTE_KICK_LISTBOX,   "IDS_VOTE_KICK",    "playerlist",  45,  60, 320, 238, 0, 0, 1, 1, ui_win::WF_VISIBLE | ui_win::WF_SCALE_XPOS | ui_win::WF_SCALE_XSIZE },
    { IDC_VOTE_KICK_NAV_TEXT,  "IDS_NULL",         "text",         0,   0,   0,   0, 0, 0, 0, 0, ui_win::WF_VISIBLE | ui_win::WF_SCALE_XPOS | ui_win::WF_SCALE_XSIZE },
};

ui_manager::dialog_tem VoteKickDialog =
{
    "IDS_VOTE_KICK_MENU",
    1, 9,
    sizeof(VoteKickControls)/sizeof(ui_manager::control_tem),
    &VoteKickControls[0],
    0
};

//=========================================================================
//  Defines
//=========================================================================

//=========================================================================
//  Structs
//=========================================================================

//=========================================================================
//  Data
//=========================================================================

//=========================================================================
//  Registration function
//=========================================================================

void dlg_vote_kick_register( ui_manager* pManager )
{
    pManager->RegisterDialogClass( "vote kick", &VoteKickDialog, &dlg_vote_kick_factory );
}

//=========================================================================
//  Factory function
//=========================================================================

ui_win* dlg_vote_kick_factory( s32 UserID, ui_manager* pManager, ui_manager::dialog_tem* pDialogTem, const irect& Position, ui_win* pParent, s32 Flags, void* pUserData )
{
    dlg_vote_kick* pDialog = new dlg_vote_kick;
    pDialog->Create( UserID, pManager, pDialogTem, Position, pParent, Flags, pUserData );

    return (ui_win*)pDialog;
}

//=========================================================================
//  dlg_level_select
//=========================================================================

dlg_vote_kick::dlg_vote_kick( void )
{
}

//=========================================================================

dlg_vote_kick::~dlg_vote_kick( void )
{
    Destroy();
}

//=========================================================================

xbool dlg_vote_kick::Create( s32                        UserID,
                             ui_manager*                pManager,
                             ui_manager::dialog_tem*    pDialogTem,
                             const irect&               Position,
                             ui_win*                    pParent,
                             s32                        Flags,
                             void*                      pUserData )
{
    xbool   Success = FALSE;

    (void)pUserData;

    ASSERT( pManager );

    // Do dialog creation
	Success = ui_dialog::Create( UserID, pManager, pDialogTem, Position, pParent, Flags );

    // find controls
    m_pPlayerList      = (ui_playerlist*) FindChildByID( IDC_VOTE_KICK_LISTBOX  );
    m_pNavText         = (ui_text*)       FindChildByID( IDC_VOTE_KICK_NAV_TEXT );

    // hide them
    m_pPlayerList     ->SetFlag(ui_win::WF_VISIBLE, FALSE);
    m_pNavText        ->SetFlag(ui_win::WF_VISIBLE, FALSE);

    // set up nav text
    xwstring navText(g_StringTableMgr( "ui", "IDS_NAV_SELECT" ));
    navText += g_StringTableMgr( "ui", "IDS_NAV_BACK" );
   
    m_pNavText->SetLabel( navText );
    m_pNavText->SetLabelFlags( ui_font::h_center|ui_font::v_top|ui_font::is_help_text );
    m_pNavText->UseSmallText(TRUE);

    // set up map list
    m_pPlayerList->SetFlag(ui_win::WF_SELECTED, TRUE);
    m_pPlayerList->SetBackgroundColor( xcolor (39,117,28,128) );
    m_pPlayerList->DisableFrame();
    m_pPlayerList->SetExitOnSelect(FALSE);
    m_pPlayerList->SetExitOnBack(TRUE);
    m_pPlayerList->EnableHeaderBar();
    m_pPlayerList->SetHeaderBarColor( xcolor(19,59,14,196) );
    m_pPlayerList->SetHeaderColor( xcolor(255,252,204,255) );
    m_pPlayerList->DeleteAllItems();

    // refresh player list
    RefreshPlayerList();

    // set initial focus
    m_CurrHL = 0;
    GotoControl( (ui_control*)m_pPlayerList );
    m_CurrentControl = IDC_VOTE_KICK_LISTBOX;
    m_PopUp = NULL;

    // initialize screen scaling
    InitScreenScaling( Position );

    // make the dialog active
    m_State = DIALOG_STATE_ACTIVE;

	// Return success code
    return Success;
}

//=========================================================================

void dlg_vote_kick::Destroy( void )
{
    ui_dialog::Destroy();

    // kill screen wipe
    g_UiMgr->ResetScreenWipe();
}

//=========================================================================

void dlg_vote_kick::Render( s32 ox, s32 oy )
{
    static s32 offset   =  0;
    static s32 gap      =  9;
    static s32 width    =  4;

	irect rb;
    
    // render background filter
    s32 XRes, YRes;
    eng_GetRes(XRes, YRes);
#ifdef TARGET_PS2
    // Nasty hack to force PS2 to draw to rb.l = 0
    rb.Set( -1, 0, XRes, YRes );
#else
    rb.Set( 0, 0, XRes, YRes );
#endif
    g_UiMgr->RenderGouraudRect(rb, xcolor(0,0,0,180),
        xcolor(0,0,0,180),
        xcolor(0,0,0,180),
        xcolor(0,0,0,180),FALSE);

    // render transparent screen
    rb.l = m_CurrPos.l + 22;
    rb.t = m_CurrPos.t;
    rb.r = m_CurrPos.r - 23;
    rb.b = m_CurrPos.b;

    g_UiMgr->RenderGouraudRect(rb, xcolor(56,115,58,64),
                                   xcolor(56,115,58,64),
                                   xcolor(56,115,58,64),
                                   xcolor(56,115,58,64),FALSE);


    // render the screen bars
    s32 y = rb.t + offset;    

    while (y < rb.b)
    {
        irect bar;

        if ((y+width) > rb.b)
        {
            bar.Set(rb.l, y, rb.r, rb.b);
        }
        else
        {
            bar.Set(rb.l, y, rb.r, y+width);
        }

        // draw the bar
        g_UiMgr->RenderGouraudRect(bar, xcolor(56,115,58,30),
                                        xcolor(56,115,58,30),
                                        xcolor(56,115,58,30),
                                        xcolor(56,115,58,30),FALSE);

        y+=gap;
    }

    // increment the offset
    if (++offset > 9)
        offset = 0;

    // render the normal dialog stuff
    ui_dialog::Render( ox, oy );

    // render the glow bar
    g_UiMgr->RenderGlowBar();
}

//=========================================================================

void dlg_vote_kick::OnPadSelect( ui_win* pWin )
{
    (void)pWin;
    
    if ( m_State == DIALOG_STATE_ACTIVE )
    {
        // display confirmation popup
        irect r = g_UiMgr->GetUserBounds( g_UiUserID );
        m_PopUp = (dlg_popup*)g_UiMgr->OpenDialog(  m_UserID, "popup", r, NULL, ui_win::WF_VISIBLE|ui_win::WF_BORDER|ui_win::WF_DLG_CENTER|WF_INPUTMODAL|ui_win::WF_USE_ABSOLUTE );

        // set nav text
        xwstring navText(g_StringTableMgr( "ui", "IDS_NAV_YES" ));
        navText += g_StringTableMgr( "ui", "IDS_NAV_NO" );
        m_pNavText->SetFlag(ui_win::WF_VISIBLE, FALSE);
       
        m_PopUp->Configure( g_StringTableMgr( "ui", "IDS_VOTE_KICK" ), 
                            TRUE, 
                            TRUE, 
                            FALSE, 
                            g_StringTableMgr( "ui", "IDS_VOTE_KICK_MSG" ),
                            navText,
                            &m_PopUpResult );

        m_State = DIALOG_STATE_POPUP;
    }
}

//=========================================================================

void dlg_vote_kick::OnPadBack( ui_win* pWin )
{
    (void)pWin;

    if ( m_State == DIALOG_STATE_ACTIVE )
    {
        g_AudioMgr.Play("Backup");
        m_State = DIALOG_STATE_BACK;
    }
}

//=========================================================================

void dlg_vote_kick::OnUpdate ( ui_win* pWin, f32 DeltaTime )
{
    (void)pWin;
    (void)DeltaTime;

    // scale window if necessary
    if( g_UiMgr->IsScreenScaling() )
    {
        if( UpdateScreenScaling( DeltaTime ) == FALSE )
        {
            // turn on the controls
            m_pPlayerList ->SetFlag(ui_win::WF_VISIBLE, TRUE);
            m_pNavText    ->SetFlag(ui_win::WF_VISIBLE, TRUE);
            GotoControl( (ui_control*)m_pPlayerList );
            g_UiMgr->SetScreenHighlight( m_pPlayerList->GetPosition() );
        }
    }

    // update the glow bar
    g_UiMgr->UpdateGlowBar(DeltaTime);

    // make sure a vote didn't start and we still have players to kick
    s32     NetSlot = g_NetworkMgr.GetLocalPlayerSlot(0);
    player* pPlayer = (player*)NetObjMgr.GetObjFromSlot( NetSlot );
    ASSERT( pPlayer );
    u32   KickMask     = pPlayer->GetVoteCanStartKick();
    xbool CanStartKick = !!(KickMask);

    if( CanStartKick == FALSE )
    {
        // kill any popups
        if( m_PopUp )
        {
            if ( m_PopUpResult != DLG_POPUP_IDLE )
            {
                ASSERT( g_UiMgr->GetTopmostDialog(m_UserID) == m_PopUp );
                g_UiMgr->EndDialog( m_UserID, TRUE );
                m_PopUp = NULL;
            }
        }

        // exit vote screen
        m_State = DIALOG_STATE_BACK;
        return;
    }

    if( m_PopUp &&  (m_PopUpResult != DLG_POPUP_IDLE) )
    {
        if ( m_PopUpResult == DLG_POPUP_YES )
        {
            // Initiate vote to kick!
            s32        Kick    = m_pPlayerList->GetSelectedItemData( 1 );
            s32        NetSlot = g_NetworkMgr.GetLocalPlayerSlot(0);
            player*    pPlayer = (player*)NetObjMgr.GetObjFromSlot( NetSlot );
            
            pPlayer->VoteStartKick( Kick );

            m_State = DIALOG_STATE_SELECT;
        }
        else
        {
            // re-enable dialog
            m_State = DIALOG_STATE_ACTIVE;
        }

        // clear popup 
        m_PopUp = NULL;

        // turn on nav text
        m_pNavText->SetFlag(ui_win::WF_VISIBLE, TRUE);
    }

    // refresh player list
    RefreshPlayerList();
}

//=========================================================================

static s32 sort_compare( const void* pItem1, const void* pItem2 )
{
    player_score* pScore1 = (player_score*)pItem1;
    player_score* pScore2 = (player_score*)pItem2;

    if( pScore1->Score > pScore2->Score )
    {
        return -1;
    }
    else if( pScore1->Score < pScore2->Score )
    {
        return 1;
    }
    else
    {
        // same score, sort on name
        return( x_stricmp( pScore1->NName, pScore2->NName ) );
    }
}

//=========================================================================

void dlg_vote_kick::RefreshPlayerList(void)
{
    s32     i;
    s32     Count   = 0;
    s32     NetSlot = g_NetworkMgr.GetLocalPlayerSlot(0);
    player* pPlayer = (player*)NetObjMgr.GetObjFromSlot( NetSlot );
    ASSERT( pPlayer );
    u32     KickMask = pPlayer->GetVoteCanStartKick();

    // store current selection
    s32 CurrentSelection = m_pPlayerList->GetSelection();
    m_pPlayerList->ClearSelection();

    // clear the player list
    m_pPlayerList->DeleteAllItems();

    // get player data
    const game_score& ScoreData = GameMgr.GetScore();

    // set score field mask
    m_pPlayerList->SetScoreFieldMask( ScoreData.ScoreFieldMask );

    // Clear the local list.
    x_memset( m_PlayerData, 0, sizeof(m_PlayerData) );

    // put the players in the list
    for( i=0 ; i<32 ; i++ )
    {
        if( KickMask & (1<<i) )
        {
            x_memcpy( &m_PlayerData[Count], 
                        &ScoreData.Player[i], 
                        sizeof(player_score) );
            m_PlayerData[Count].ClientIndex = i;
            Count++;
        }
    }

    // now we have the player data sort it
    if( Count )
    {
        // sort the one list
        x_qsort( &m_PlayerData[0], Count, sizeof(player_score), sort_compare );

        // finally add the sorted players to the player list
        for( i=0 ; i<Count ; i++ )
        {
            m_pPlayerList->AddItem( m_PlayerData[i].Name, 
                                   (s32)&m_PlayerData[i],
                                   m_PlayerData[i].ClientIndex );
        }

        // select the first entry
        if( (m_pPlayerList->GetItemCount() >= CurrentSelection) && (CurrentSelection != -1) )
        {
            m_pPlayerList->SetSelection( CurrentSelection );
        }
        else
        {
            m_pPlayerList->SetSelection( 0 );
        }
    }
}

//=========================================================================
