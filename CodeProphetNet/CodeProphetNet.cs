using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CodeProphetNet
{
    static public class CodeProphetNet
    {
        public static void OnFunctionEnter()
        {
        }

        public static void OnFunctionExit()
        {
        }

        public static void OnExit()
        {
        }
    }

    struct Function
    {
        string func_name;
    }

    struct FunctionCall
    {
        Function func;
        UInt64 timestamp;
    }
}
