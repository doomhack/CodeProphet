
// CodeProphetViewDoc.cpp : implementation of the CCodeProphetViewDoc class
//

#include "stdafx.h"
#include "CodeProphetView.h"

#include "CodeProphetViewDoc.h"

#include "NewProject.h"
#include "PDBFile.h"
#include "NoResolve.h"
#include "LoadProgress.h"

#include "MyWizPage.h"
#include "Pages.h"
#include "NewProjWiz.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCodeProphetViewDoc

IMPLEMENT_DYNCREATE(CCodeProphetViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CCodeProphetViewDoc, CDocument)
END_MESSAGE_MAP()


// CCodeProphetViewDoc construction/destruction

CCodeProphetViewDoc::CCodeProphetViewDoc()
{
	// TODO: add one-time construction code here

}

CCodeProphetViewDoc::~CCodeProphetViewDoc()
{
}

BOOL CCodeProphetViewDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CNewProjWiz npw;
	npw.AddPages();
	npw.DoModal();

	CFile cpg_file;
	IFunctionResolve *resolver = NULL;

	CNewProject new_doc_dlg;
	new_doc_dlg.SetProjSettings(m_proj_set);

	if(new_doc_dlg.DoModal() != IDOK)
		return FALSE;

	if(!cpg_file.Open(m_proj_set.cpgfile, CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(_T("Failed to open file."),NULL,MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	switch(m_proj_set.symbol_res)
	{
		case symres_pdb:
			resolver = new CPDBFile(m_proj_set.symres_file);
			break;
		case symres_none:
			resolver = new CNoResolve;
			break;
		default:
			AfxMessageBox(_T("Unknown symbol resolver specified."),NULL,MB_OK | MB_ICONSTOP);
	}

	if(!resolver)
	{
		AfxMessageBox(_T("Alloc new symbol resolver failed."),NULL,MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(!resolver->ParseFile(m_proj_set.undec))
	{
		AfxMessageBox(_T("Failed to start symbol resolver."),NULL,MB_OK | MB_ICONSTOP);
		resolver->UnloadSymbols();
		delete resolver;
		return FALSE;
	}

	CLoadProgress* prog = new CLoadProgress();

	if(!this->m_callgraph.LoadFromFile(cpg_file, m_proj_set, resolver ))
	{
		AfxMessageBox(_T("Failed to load file."),NULL,MB_OK | MB_ICONSTOP);
		resolver->UnloadSymbols();
		delete resolver;
		return FALSE;
	}

	delete prog;

	cpg_file.Close();

	resolver->UnloadSymbols();
	delete resolver;
	resolver = NULL;

	this->SetModifiedFlag();

	return TRUE;

}




// CCodeProphetViewDoc serialization

void CCodeProphetViewDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CString id = _T("CPV");
		ar << id;
		ar << this->m_callgraph.arch;
		ar << this->m_callgraph.num_threads;
		ar << this->m_callgraph.prof_time;
		ar << this->m_callgraph.ptrlen;
		ar << this->m_callgraph.timer_freq;
		ar << this->m_callgraph.version;

		int i = m_callgraph.num_threads;

		while(i--)
		{
			CCallTree* thistree = this->m_callgraph.g_treelist.at(i);

			SaveCallGraph(ar, thistree->GetNode0());

			
			ar << thistree->function_list.size();
			unsigned int c = 0;

			while(c < thistree->function_list.size())
			{
				TreeNode* thisnode = thistree->function_list.at(c);
			
				ar << thisnode->avg_msec;
				ar << thisnode->call_count;
				ar << thisnode->exc_msec;
				ar << thisnode->exc_pc;
				ar << thisnode->func_addr;
				ar << thisnode->func_addr_ptr;
				ar << thisnode->func_name;
				ar << thisnode->max_msec;
				ar << thisnode->max_time;
				ar << thisnode->min_msec;
				ar << thisnode->min_time;
				ar << thisnode->src_file;
				ar << thisnode->src_line;
				ar << thisnode->time;
				ar << thisnode->tot_msec;
				ar << thisnode->tot_pc;

				c++;
			}
		}
	}
	else
	{
		CString id;
		ar >> id;

		int p = 0;
		ar >> p;
		this->m_callgraph.arch = (EPlatformArch)p;

		ar >> this->m_callgraph.num_threads;
		ar >> this->m_callgraph.prof_time;
		ar >> this->m_callgraph.ptrlen;
		ar >> this->m_callgraph.timer_freq;
		ar >> this->m_callgraph.version;
		
		int i = m_callgraph.num_threads;

		while(i--)
		{
			CCallTree* thistree = new CCallTree();

			this->m_callgraph.g_treelist.push_front(thistree);

			LoadCallGraph(ar, thistree->GetNode0(), NULL);

			int c = 0;
			ar >> c;

			while(c--)
			{
				TreeNode* thisnode = new TreeNode();
			
				thistree->function_list.push_back(thisnode);

				ar >> thisnode->avg_msec;
				ar >> thisnode->call_count;
				ar >> thisnode->exc_msec;
				ar >> thisnode->exc_pc;
				ar >> thisnode->func_addr;
				ar >> thisnode->func_addr_ptr;
				ar >> thisnode->func_name;
				ar >> thisnode->max_msec;
				ar >> thisnode->max_time;
				ar >> thisnode->min_msec;
				ar >> thisnode->min_time;
				ar >> thisnode->src_file;
				ar >> thisnode->src_line;
				ar >> thisnode->time;
				ar >> thisnode->tot_msec;
				ar >> thisnode->tot_pc;
			}
		}
	}
}

void CCodeProphetViewDoc::SaveCallGraph(CArchive& ar, TreeNode* thisnode)
{
	ar << thisnode->avg_msec;
	ar << thisnode->call_count;
	ar << thisnode->exc_msec;
	ar << thisnode->exc_pc;
	ar << thisnode->func_addr;
	ar << thisnode->func_addr_ptr;
	ar << thisnode->func_name;
	ar << thisnode->max_msec;
	ar << thisnode->max_time;
	ar << thisnode->min_msec;
	ar << thisnode->min_time;
	ar << thisnode->num_child;
	ar << thisnode->src_file;
	ar << thisnode->src_line;
	ar << thisnode->time;
	ar << thisnode->tot_msec;
	ar << thisnode->tot_pc;

	unsigned int i = 0;

	while(i < thisnode->num_child)
	{
		SaveCallGraph(ar, thisnode->children.at(i));
		i++;
	}
}

void CCodeProphetViewDoc::LoadCallGraph(CArchive& ar, TreeNode* thisnode, TreeNode* parent)
{
	ar >> thisnode->avg_msec;
	ar >> thisnode->call_count;
	ar >> thisnode->exc_msec;
	ar >> thisnode->exc_pc;
	ar >> thisnode->func_addr;
	ar >> thisnode->func_addr_ptr;
	ar >> thisnode->func_name;
	ar >> thisnode->max_msec;
	ar >> thisnode->max_time;
	ar >> thisnode->min_msec;
	ar >> thisnode->min_time;
	ar >> thisnode->num_child;
	ar >> thisnode->src_file;
	ar >> thisnode->src_line;
	ar >> thisnode->time;
	ar >> thisnode->tot_msec;
	ar >> thisnode->tot_pc;

	thisnode->prev = parent;

	int i = thisnode->num_child;

	while(i--)
	{
		TreeNode* next = new TreeNode();
		thisnode->children.push_back(next);

		LoadCallGraph(ar, next, thisnode);
	}
}

// CCodeProphetViewDoc diagnostics

#ifdef _DEBUG
void CCodeProphetViewDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCodeProphetViewDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCodeProphetViewDoc commands
