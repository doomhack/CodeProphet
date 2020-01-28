using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CodeProphetNet
{
    public class TreeNode
    {
        TreeNode prev;
        Function func;

        UInt64 time;
        UInt64 mintime;
        UInt64 maxtime;

        UInt32 call_count;
        Stack<TreeNode> children;
    }
}
