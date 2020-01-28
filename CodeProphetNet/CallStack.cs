using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CodeProphetNet
{
    public class CallStack
    {
        private Stack<FunctionCall> ShadowCallStack;
        private TreeNode current_node;
        private UInt64 begintime;
        private UInt64 endtime;
        private TreeNode node0;
        private bool isprofiling;
        private string thread_name;


        CallStack()
        {
        }

        ~CallStack()
        {
        }

        void AddCall(Function address, UInt64 timestamp)
        {
        }

        void ReturnCall(Function address, UInt64 timestamp)
        {
        }

        TreeNode GetNode0()
        {
            return node0;
        }

        UInt64 GetTotalTime()
        {
            return endtime - begintime;
        }

        void CloseCallGraph()
        {
        }

        void SuspendProfiling()
        {
            isprofiling = false;
        }

        void ResumeProfiling()
        {
            isprofiling = true;
        }

        void SetThreadName(string name)
        {
            thread_name = name;
        }

        string GetThreadName()
        {
            return thread_name;
        }
    }
}
