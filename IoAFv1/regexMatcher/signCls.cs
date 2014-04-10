using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace regexMatcher
{
    class signCls
    {
        public string type;
        public float weight;
        public string regex;
        public int group;
        public Boolean isDB;
        public List<String> matchedList;

        public signCls(string sType, float weight) {
            this.type = sType;
            this.weight = weight;
            this.isDB = false;
            this.matchedList = new List<String>();
        }

    }
}
