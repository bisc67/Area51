#if !defined(AFX_CFxEditor_View_H__FFF83B58_8521_4D74_AA19_39732C752244__INCLUDED_)
#define AFX_CFxEditor_View_H__FFF83B58_8521_4D74_AA19_39732C752244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CFxEditor_View.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFxEditor_View view
#include "View3d.h"
#include "Grid3d.hpp"
#include "Axis3d.hpp"

class CFxEditor_View : public CView3D
{
    grid3d              m_Grid;
    axis3d              m_Axis;

protected:
	CFxEditor_View();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFxEditor_View)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFxEditor_View)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFxEditor_View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFxEditor_View)
    afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFxEditor_View_H__FFF83B58_8521_4D74_AA19_39732C752244__INCLUDED_)
