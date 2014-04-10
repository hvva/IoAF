using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IOAF
{
    class DiskImgInfo
    {
        string offset;
        string fs;
    
        public string getOffset()
        {
            return this.offset;
        }

        public string getFS()
        {
            return this.fs;
        }

        public DiskImgInfo(string offset, string fs)
        {
            // TODO: Complete member initialization
            this.offset = offset;
            this.fs = fs;
        }
    }
}
