using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CodeProphetNet
{

    //This struct needs ti be kept in sync with
    struct FileHeaderV2
    {
	    fixed char id[4];
	    UInt32 version;
	    fixed char arch[8];
	    SYSTEMTIME time;
	    UInt64 timer_freq;
	    UInt64 base_addr;
	    UInt32 ptrlen;
	    UInt32 filetype;
    }

    class CSaveCallgraph
    {
	    
    	CSaveCallgraph()
        {
        }

        ~CSaveCallgraph()
        {
        }

        public void SaveAllCallTrees()
        {
        }

    	FileHeaderV2* fh;
    	FILE* savefile;
	    string filename;

        void CreateFileHeader()
        {

        }

        void SaveCallTree(TreeNode node)
        {
        }

        void SaveFileHeader()
        {
        }

        void GetFileName()
        {
        }

        void WriteNumThreads(UInt32 threads)
        {
        }

    }
}
