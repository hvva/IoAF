using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace regexMatcher
{
    class signs
    {
       public  String tool;
       public String explain;
       public List<signCls> insSign;
       public List<signCls> runSign;
       public List<signCls> removeSign;
       public signs() {
           insSign = new List<signCls>();
           runSign = new List<signCls>();
           removeSign = new List<signCls>();
       }
    }
}
