using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace mft2db
{
    class Program
    {
        static void Main(string[] args)
        {
            new Program().DoMain("ddd", "mft");
        }

        void DoMain(String dbname, String mftn)
        {
            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo(".\\tsk\\mftparser\\analyzeMFT.exe");
            psi.Arguments = String.Format("-f \"{0}\" -o \"{1}\" -p -a", mftn, "tmp" + dbname);
            p.StartInfo = psi;
            p.Start();

            p.WaitForExit();

            StreamReader sr = new StreamReader("tmp" + dbname);

            String s;
            while (true)
            {
                s = sr.ReadLine();
                if (sr.EndOfStream)
                    break;
                if (s == null)
                    continue;
                string[] m = s.Split(',');
                s = m[7].Replace("\"", String.Empty);
                
                Console.WriteLine(s);


            }
        }
    }
}
