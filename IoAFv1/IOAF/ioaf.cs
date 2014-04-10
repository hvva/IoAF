using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace IOAF
{
    class ioaf
    {
        List<DiskImgInfo> partInfo = null;
        static void Main(string[] args)
        {
            Console.Clear();

            if (args.Length == 0)
            {
                Console.WriteLine("usage : ./ioaf <imagePATH>");
                return;
            }

            new ioaf().DoMain(args[0]);
        }

        void DoMain(String imagePATH)
        {
            partInfo = new List<DiskImgInfo>();
            RUNmmls(imagePATH);
        }

        private void RUNmmls(String imgPATH)
        {
            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo(".\\tsk\\mmls.exe");

            StreamReader sout;

            psi.Arguments = imgPATH;
            psi.RedirectStandardOutput = true;
            psi.UseShellExecute = false;
            psi.CreateNoWindow = true;

            p.StartInfo = psi;
            p.Start();

            sout = p.StandardOutput;

            String s;
            int i = 0;

            Console.WriteLine("\n\n\t No \t  Offset\tFilesystem");

            while ((s = sout.ReadLine()) != null)
            {
                Regex regex = new Regex(@"(?<start>[0-9]{10}).*?(?<end>[0-9]{10}).*?(?<len>[0-9]{10})\s{2,}(?<fs>.*?)$");

                Match m = regex.Match(s);

                if (!m.Success)
                    continue;

                if (m.Groups["fs"].Value.Contains("NTFS"))
                {
                    string offset = m.Groups["start"].Value;
                    string fs = m.Groups["fs"].Value;
                    partInfo.Add(new DiskImgInfo(offset, fs));
                   
                }
            }

            bool isLoop = true;
            int partno = 0;
            while(isLoop)
            {
                i = 0;
                foreach(DiskImgInfo e in partInfo)
                {
                    string fmt = String.Format("\t{0,4:D4}\t{1}\t{2}", i++, e.getOffset(), e.getFS());
                    Console.WriteLine(fmt);
                }
                Console.WriteLine("\n\n  v : View Filelist partition    s : Select Partition    x : Exit");
                ConsoleKeyInfo k = Console.ReadKey(true);
                //Console.WriteLine(k.KeyChar);
                switch(k.KeyChar)
                {
                    case 'v':
                        Console.Write("\n  View: Partition No (0-"+(i-1)+") >");
                        partno = Convert.ToInt32(Console.ReadLine());
                        fls4root(imgPATH, partInfo[partno]);
                        break;
                    case 's':
                        Console.Write("\n  Select: Partition No (0-"+(i-1)+") >");
                        partno = Convert.ToInt32(Console.ReadLine());
                        isLoop = false;
                        break;
                    case 'x':
                        isLoop = false;
                        Environment.Exit(0);
                        return;

                    default:
                        Console.Clear();
                        break;
                }
            }
            Console.Write("DBName >");
            string dbname = Console.ReadLine();
            Console.Write("Signature DB >");
            string xmlname = Console.ReadLine();
            selectPartition(imgPATH, dbname, partno);
            extracgReg(imgPATH, dbname, partno);
            insREG(dbname);
            xmlMatcher(dbname, xmlname);

        }
        void xmlMatcher(string dbname, string xmlName)
        {
            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo("regexMatcher.exe");

            psi.Arguments = String.Format("\"{0}\" \"{1}\"",  dbname, xmlName);

            p.StartInfo = psi;
            p.Start();
            p.WaitForExit();


        }

        void selectPartition( string imgpath, string dbname,  int partno )
        {
            DiskImgInfo d = partInfo[partno];

            string dOffset = d.getOffset();

            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo("fls2db.exe");

            psi.Arguments = String.Format("\"{0}\" \"{1}\" \"{2}\"", imgpath, dOffset, dbname);

            p.StartInfo = psi;
            p.Start();
            p.WaitForExit();


        }

        void extracgReg(string imgpath, string dbname, int partno )
        {
            DiskImgInfo d = partInfo[partno];

            string dOffset = d.getOffset();

            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo("extreg.exe");

            psi.Arguments = String.Format("\"{0}\" \"{1}\" \"{2}\"", imgpath, dOffset, dbname);

            p.StartInfo = psi;
            p.Start();
            p.WaitForExit();
        }

        void insREG(string dbname)
        {
            DirectoryInfo dirInfo = new DirectoryInfo(".\\" + dbname);
            FileInfo[] file  = dirInfo.GetFiles().Where(f => f.Extension.StartsWith(".reg")).ToArray();
            
            foreach(FileInfo f in file)
            {
               // Console.WriteLine(f.FullName);
                int len = f.Name.Length;
                string chive = f.Name.Substring(4, len - 8);
                string rhive = f.Name.Substring(0, 4);
                //Console.WriteLine(rhive);
                //Console.WriteLine(chive);
                string prefix = rhive;
                if (chive.StartsWith("SAM"))
                    prefix += "/SAM";
                else if (chive.StartsWith("SEC"))
                    prefix += "/SECURITY";
                else if (chive.StartsWith("SOFT"))
                    prefix += "/SOFTWARE";
                else if (chive.StartsWith("SYS"))
                    prefix += "/SYSTEM";

                //string dOffset = d.getOffset();

                Process p = new Process();
                ProcessStartInfo psi = new ProcessStartInfo("flsreg2db.exe");

                psi.Arguments = String.Format("\"{0}\" \"{1}\" \"{2}\"", f.FullName, dbname, prefix);
                Console.WriteLine(psi.Arguments);
                p.StartInfo = psi;
                p.Start();
                p.WaitForExit();

            }
        }

        private void fls4root(string imgPATH, DiskImgInfo part)
        {
            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo(".\\tsk\\fls.exe");


            Console.Clear();

            psi.Arguments = "-rp -o " + part.getOffset() + " \"" + imgPATH + "\"";
            psi.RedirectStandardOutput = true;
            psi.UseShellExecute = false;
            
            psi.CreateNoWindow = true;

            p.StartInfo = psi;
            p.EnableRaisingEvents = true;
            p.OutputDataReceived += p_OutputDataReceived;
            p.Start();
            p.BeginOutputReadLine();
            p.WaitForExit();
            Console.WriteLine("\n");
            
        }

        void p_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if(!String.IsNullOrEmpty(e.Data))
                Console.WriteLine("    "+ e.Data);
            //throw new NotImplementedException();
        }

    }
}
