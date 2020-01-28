#include "stdafx.h"
#include "CodeProphetView.h"
#include "CodeProphetViewDoc.h"

IFunctionResolve* gbl_resolver;
unsigned int resolve_errors;
unsigned int file_ptr_len;

CCallTree::CCallTree()
{
	this->tot_time = 0;
}

CCallTree::~CCallTree()
{
	DeleteTrees(&this->node0);

	for(unsigned int i = 0; i < this->function_list.size(); i++)
		delete this->function_list.at(i);
}

void CCallTree::DeleteTrees(TreeNode* node)
{
	unsigned int num_child = node->children.size();

	while(num_child--)
		this->DeleteTrees( node->children.at(num_child) );

	if(node != &this->node0)
		delete node;
}


void CCallTree::BuildCallGraph(TreeNode* node, TreeNode* parent, CFile& file, DWORD64 timer_freq)
{
	unsigned int num_children;
	double child_times = 0;
	double tot_time_msec = 0;

	file.Read(&num_children,sizeof(unsigned int));

	node->prev = parent;
	node->num_child = num_children;

	{
		char addrstr[16+2+1] = {'0','x','\0'};
		node->func_addr_ptr = 0;
		file.Read(&node->func_addr_ptr,file_ptr_len);
		_i64toa_s(node->func_addr_ptr,&addrstr[2],17,16);
		node->func_addr = addrstr;
	}

	if(gbl_resolver && node->func_addr_ptr)
	{
		if (!gbl_resolver->ResolveAddress(node->func_addr_ptr, node->func_name))
			resolve_errors++;
		
		gbl_resolver->GetSourceFile(node->func_addr_ptr, node->src_file, node->src_line);
	}
	else
		node->src_line = 0;

	file.Read(&node->time,sizeof(DWORD64));
	file.Read(&node->min_time ,sizeof(DWORD64));
	file.Read(&node->max_time,sizeof(DWORD64));

	file.Read(&node->call_count,sizeof(unsigned int));

	(node->min_time)	?	node->min_msec = ((double)node->min_time/((double)timer_freq/1000.0f))	: node->min_msec = 0;
	(node->max_time)	?	node->max_msec = ((double)node->max_time/((double)timer_freq/1000.0f))	: node->max_msec = 0;

	(node->time)		?	node->tot_msec = ((double)node->time/((double)timer_freq/1000.0f))	: node->tot_msec = 0;
	(node->tot_msec)	?	node->avg_msec = (node->tot_msec/(double)node->call_count)		: node->avg_msec = 0;

	(node->time)		?	node->tot_pc = ((double)node->time / (double)tot_time)*100.0f		: node->tot_pc = 0;

	while (num_children--)
	{
		TreeNode* cnode = new TreeNode;

		node->children.push_back(cnode);

		BuildCallGraph(cnode, node, file, timer_freq);
	}

	//Tot-up all of the child times;
	num_children = node->num_child;
	while (num_children--)
	{
		TreeNode* cnode = node->children.at(num_children);
		child_times += cnode->tot_msec;
	}

	if(node->call_count)
		node->exc_msec = (node->tot_msec - child_times);
	else
		node->exc_msec = 0;

	(tot_time)			?	tot_time_msec = ((double)tot_time/((double)timer_freq/1000.0f))	: tot_time_msec = 0;
	(node->exc_msec)	?	node->exc_pc = (node->exc_msec / tot_time_msec)*100.0f		: node->exc_pc = 0;

	//Add to function list...
	/*
	1) If not node0, Look for address in the list.
	2) If not exist, create and copy current node.
	3) If it does exist, check the the address is not already 'accounted for' (recursive or circular calls) by searching
	   back up the tree.
	   -If it does exist, add the call_count, update the max and min times.
	   -If not, add the times to the existing in the list.
	4) Recaluate runtime% and max/min times etc.
	*/

	if(node != this->GetNode0() && node)
	{
		unsigned int num_func = function_list.size();
		bool has_found = false;
		bool is_recursive = false;
		TreeNode* test_node = NULL;

		//Look for current address in function list.
		for (unsigned int i = 0; i < num_func; i++)
		{
			test_node = function_list.at(i);
			if (test_node->func_addr_ptr == node->func_addr_ptr)
			{
				has_found = true;
				break;
			}
		}

		
		TreeNode* tnode = node->prev;

		while(tnode != this->GetNode0() && tnode)
		{
			if(tnode->func_addr_ptr == node->func_addr_ptr)
			{
				is_recursive = true; //Have found function higher up in the tree. It is recursive or circular.
				this->recursive_functions.push_back(tnode); //Add to list of recursive functions.
				break;
			}
			tnode = tnode->prev;
		}


		//If not found, create a new one, and add it to the list.
		if(!has_found)
		{
			test_node = new TreeNode;
			function_list.push_back(test_node);

			test_node->prev = NULL;
			test_node->num_child = 0;

			test_node->func_addr = node->func_addr;
			test_node->func_name = node->func_name;
			test_node->src_file = node->src_file;
			test_node->src_line = node->src_line;
			test_node->func_addr_ptr = node->func_addr_ptr;
			
			if(!is_recursive)
			{
				test_node->tot_msec = node->tot_msec;
				test_node->time = node->time;
			}
			else
			{
				test_node->tot_msec = 0;
				test_node->time = 0;
			}

			test_node->exc_msec = node->exc_msec;
			test_node->min_msec = node->min_msec;
			test_node->max_msec = node->max_msec;
			test_node->call_count = node->call_count;

			test_node->min_time = node->min_time;
			test_node->max_time = node->max_time;
		}
		else //Has found.
		{
			if(!is_recursive) //Check if it has been flagged as recursive. IE: It is the first in the recursive chain.
			{
				for(unsigned int i = 0; i < this->recursive_functions.size(); i++)
				{
					if(node == this->recursive_functions.at(i))
					{
						is_recursive = true;
						
						test_node->tot_msec += node->tot_msec; //We only add if we are at the top of the recusive call chain.
						test_node->time += node->time;
						
						break;
					}
				}
			}

			if(!is_recursive)
			{
				test_node->tot_msec += node->tot_msec;
				test_node->time += node->time;
			}

			test_node->exc_msec += node->exc_msec;
			test_node->call_count += node->call_count;

			if (test_node->min_msec > node->min_msec)
				test_node->min_msec = node->min_msec;

			if (test_node->max_msec < node->max_msec)
				test_node->max_msec = node->max_msec;

			if(test_node->min_time > node->min_time)
				test_node->min_time = node->min_time;
				
			if (test_node->max_time < node->max_time)
				test_node->max_time = node->max_time;
		}
		
		//Now we fixup percentages etc.

		(test_node->time) ? test_node->tot_pc = ((double)test_node->time / (double)tot_time)*100.0f : test_node->tot_pc = 0;
		(test_node->exc_msec) ? test_node->exc_pc = (test_node->exc_msec / tot_time_msec)*100.0f : test_node->exc_pc = 0;
		(test_node->tot_msec) ? test_node->avg_msec = (test_node->tot_msec/(double)test_node->call_count) : test_node->avg_msec = 0;
	}
}

DWORD64 CCallTree::GetTotalTime()
{
	return this->tot_time;
}

void CCallTree::SetTotalTime(DWORD64 tot_time)
{
	this->tot_time = tot_time;
}

TreeNode* CCallTree::GetNode0()
{
	return &this->node0;
}

CCallGraph::CCallGraph(void)
{
	this->arch = undefined;
	this->prof_time = 0;
	this->timer_freq = 0;
	this->version = 0;
}

CCallGraph::~CCallGraph(void)
{
	unsigned int num_trees = this->g_treelist.size();

	while(num_trees--)
		delete this->g_treelist.at(num_trees);
}

int CCallGraph::LoadFromFile(CFile& file, CNewProjectSettings& proj_set, IFunctionResolve* resolver)
{
	FileHeaderV2 fh;

	if (file.Read(&fh,sizeof(FileHeaderV2)) < sizeof(FileHeaderV2))
		return FALSE;

	if (strncmp(fh.id, "CPG", 3))
		return FALSE;

	gbl_resolver = resolver;

	if(!gbl_resolver->SetBaseAddress(fh.base_addr))
	{
		CString error_msg;
		error_msg.Format(_T("Unable to find marker symbol in image file.\nError Code: %d"), GetLastError() );
		AfxMessageBox(error_msg, NULL, MB_OK | MB_ICONWARNING);
	}

	this->prof_time = fh.time;

	if( !strcmp(fh.arch, "PC_X86"))
		this->arch = x86_pc;
	else if (!strcmp(fh.arch, "PC_X64"))
		this->arch = x64_pc;
	else if (!strcmp(fh.arch, "WM_ARM"))
		this->arch = wm5_arm;
	else
		this->arch = undefined;

	this->timer_freq = fh.timer_freq;
	this->version = fh.version;
	file_ptr_len = this->ptrlen = fh.ptrlen;

	if (file.Read(&this->num_threads,sizeof(unsigned int)) < sizeof(unsigned int))
		return FALSE;

	return BuildCallTrees(this->num_threads, file);
}

int CCallGraph::BuildCallTrees(unsigned int num_trees, CFile& file)
{
	while(num_trees--)
	{
		char tmrk[5]; //Thread marker.
		file.Read(tmrk,4);
		tmrk[4] = '\0';

		if (strncmp(tmrk, "TMRK", 4))
			return FALSE; //We've lost the plot somewhere!

		CCallTree* thistree = new CCallTree;
		
		{
			DWORD64 temp_time;
			file.Read(&temp_time,sizeof(DWORD64));

			thistree->SetTotalTime(temp_time);
		}

		g_treelist.push_front(thistree);

		thistree->BuildCallGraph(thistree->GetNode0(), NULL, file, this->timer_freq);
		
		//Now we build the classview.
		int num_func = thistree->function_list.size();
		
		while(num_func--)
		{
			TreeNode* thisfunc = thistree->function_list.at(num_func);
			CString tempname;
			//Remove any template names. eg:	std::vector<std::char>::push_back
			//									std::vector::push_back
			int sep_start = thisfunc->func_name.Find(_T("<"));
			if(sep_start > -1)
			{
				tempname = thisfunc->func_name.Left(sep_start);

				int sep_level = 0;

				while(sep_start < thisfunc->func_name.GetLength() )
				{
					CString c = thisfunc->func_name.Mid(sep_start, 1);

					if (c == _T("<"))
						sep_level++;
					else if(c == _T(">"))
						sep_level--;
					else if(!sep_level)
						tempname+=c;

					sep_start++;
				}
			}
			else
				tempname = thisfunc->func_name;
			
			int sep = -2;

			Class* thisclass = NULL;

			while(sep = tempname.Find(_T("::"), sep+2))
			{
				if(sep == -1)
					break;

				CString classname = tempname.Left(sep);
				//Look for class in list.

				int numclass = thistree->classes.size();

				while(numclass--)
				{
					if(thistree->classes.at(numclass)->class_name == classname)
					{
						thisclass = thistree->classes.at(numclass);
						//break;
					}
				}

				if(!thisclass)
				{
					thisclass = new Class;
					thisclass->class_name = classname;
					thistree->classes.push_back(thisclass);
				}

				thisclass->members.push_back(thisfunc);
			}
		}
	}

	if(resolve_errors)
	{
		TCHAR err_msg[64];
		wsprintf(err_msg, _T("Failed to resolve %d symbol(s)"),resolve_errors);
		AfxMessageBox(err_msg, NULL, MB_OK | MB_ICONEXCLAMATION);
	}

	resolve_errors = 0;
	gbl_resolver = NULL;

	return TRUE;
}