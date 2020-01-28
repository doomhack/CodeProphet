
// CodeProphetViewDoc.h : interface of the CCodeProphetViewDoc class
//


#pragma once

#include "CallGraph.h"

class CCodeProphetViewDoc : public CDocument
{
protected: // create from serialization only
	CCodeProphetViewDoc();
	DECLARE_DYNCREATE(CCodeProphetViewDoc)

	void SaveCallGraph(CArchive& ar, TreeNode* thisnode);
	void LoadCallGraph(CArchive& ar, TreeNode* thisnode, TreeNode* parent);
// Attributes
public:
	CNewProjectSettings m_proj_set;
	CCallGraph m_callgraph;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CCodeProphetViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


