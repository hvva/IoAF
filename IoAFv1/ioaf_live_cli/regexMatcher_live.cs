using Microsoft.Win32;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Data.SQLite;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Xml;

namespace ioaf_live_cli
{
    class signCls
    {
        public string type;
        public float weight;
        public string regex;
        public int group;
        public Boolean isDB;
        public List<String> matchedList;

        public signCls(string sType, float weight)
        {
            this.type = sType;
            this.weight = weight;
            this.isDB = false;
            this.matchedList = new List<String>();
        }

    }
    class signs
    {
        public String tool;
        public String explain;
        public List<signCls> insSign;
        public List<signCls> runSign;
        public List<signCls> removeSign;
        public signs()
        {
            insSign = new List<signCls>();
            runSign = new List<signCls>();
            removeSign = new List<signCls>();
        }
    }
    class regexMatcher_live
    {
        ArrayList falsegrp;
        
        static void Main(string[] args)
        {

            new regexMatcher_live().DoMain(args[0], args[1]);
            //new regexMatcher_live().DoMain("eraser.xml", "live");
            }

        void DoMain( String xml, String output)
        {

            falsegrp = new ArrayList();

            Thread th1 = new Thread(unused => th_match( xml, output));

            th1.Start();

            while (th1.IsAlive)
            {
                Console.Write(".");
                Thread.Sleep(1000);
            }


        }


        void th_match( String xml, String output)
        {
            List<signs> signList = new List<signs>();
            Boolean isAndOpen = false;
            signs signDB = new signs();
            signCls nSign = null;
            String xName = "";
            String xProc = "";
            int i = 0, j = 0, k = 0;
            XmlTextReader reader = new XmlTextReader(xml);
            while (reader.Read())
            {
                switch (reader.NodeType)
                {
                    case XmlNodeType.Element: // The node is an element.
                        if (reader.Name == "ioaf")
                            signDB = new signs();
                        else if (reader.Name == "signs")
                            xProc = reader.GetAttribute("type");
                        else if (reader.Name == "sign")
                            nSign = new signCls(reader.GetAttribute("type"), float.Parse(reader.GetAttribute("weight")));
                        else if (reader.Name == "and")
                            isAndOpen = true;
                        xName = reader.Name;
                        break;
                    case XmlNodeType.Text: //Display the text in each element.
                        if (xName == "name" && signDB != null)
                            signDB.tool = reader.Value;
                        else if (xName == "description" && signDB != null)
                            signDB.explain = reader.Value.Replace("\t", "").TrimStart('\n');
                        else if (xName == "sign" && signDB != null)
                        {
                            nSign.regex = reader.Value;

                            if (xProc == "install")
                            {
                                nSign.group = i;
                                signDB.insSign.Add(nSign);
                                if (isAndOpen == false)
                                    i++;
                            }
                            else if (xProc == "run")
                            {
                                nSign.group = j;
                                signDB.runSign.Add(nSign);
                                if (isAndOpen == false)
                                    j++;
                            }
                            else if (xProc == "remove")
                            {
                                nSign.group = k;
                                signDB.removeSign.Add(nSign);
                                if (isAndOpen == false)
                                    k++;
                            }

                        }

                        break;
                    case XmlNodeType.EndElement: //Display the end of the element.
                        if (reader.Name == "and")
                        {
                            if (xProc == "install")
                                i++;
                            else if (xProc == "run")
                                j++;
                            else if (xProc == "remove")
                                k++;
                            isAndOpen = false;
                        }
                        else if (reader.Name == "ioaf")
                        {
                            signList.Add(signDB);
                            i = 0;
                            j = 0;
                            k = 0;
                        }

                        break;
                }
            }
           
            foreach (signs a in signList)
            {
                foreach (signCls b in a.insSign)
                {
                    b.isDB = isInDB( b.type, b.regex, b);
                }
                foreach (signCls b in a.insSign)
                {
                    if (falsegrp.Contains(b.group))
                        b.isDB = false;
                }
                falsegrp.Clear();
                foreach (signCls b in a.runSign)
                {
                    b.isDB = isInDB( b.type, b.regex, b);
                }
                foreach (signCls b in a.runSign)
                {
                    if (falsegrp.Contains(b.group))
                        b.isDB = false;
                }
                falsegrp.Clear();
                foreach (signCls b in a.removeSign)
                {
                    b.isDB = isInDB( b.type, b.regex, b);
                }
                foreach (signCls b in a.removeSign)
                {
                    if (falsegrp.Contains(b.group))
                        b.isDB = false;
                }
                falsegrp.Clear();
            }


            StreamWriter fs = new StreamWriter(output + ".html");
            fs.WriteLine("<!doctype html><html>");
            fs.WriteLine("<head>");
            fs.WriteLine("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>");
            fs.WriteLine("<link rel=\"stylesheet\" type=\"text/css\" href=\"./lib/style.css\" />");
            fs.WriteLine("</head>");
            fs.WriteLine("<body>");
            foreach (signs a in signList)
            {
                fs.WriteLine("<h1>" + a.tool + "</h1>");
                fs.WriteLine("<h4>" + a.explain.Replace("\n", "<br>") + "</h4>");
                fs.WriteLine("<table>");
                fs.WriteLine("<tr><th>Group</th><th>Regex</th><th>Result</th></tr><tr class=\"ec\" data-target=\"install\"><td colspan=\"3\">INSTALL +/-</td></tr>");
                foreach (signCls b in a.insSign)
                {
                    if (b.isDB)
                    {
                        fs.WriteLine("<tr class=\"install\"><td>" + b.group + "</td><td class=\"trueSign\">" + b.regex + "</td>");
                        fs.WriteLine("<td>");
                        foreach (string ms in b.matchedList)
                        {
                            Console.WriteLine(ms);
                            fs.WriteLine(ms + "<br>");
                        }
                        fs.WriteLine("</td></tr>");
                    }
                    else
                    {
                        fs.WriteLine("<tr class=\"install\"><td>" + b.group + "</td><td class=\"falseSign\">" + b.regex + "</td>");
                        fs.WriteLine("<td>");
                        foreach (string ms in b.matchedList)
                        {
                            Console.WriteLine(ms);
                            fs.WriteLine(ms + "<br>");
                        }
                        fs.WriteLine("</td></tr>");
                    }

                }
                fs.WriteLine("<tr class=\"ec\" data-target=\"run\"><td colspan=\"3\">RUN +/-</td></tr>");
                foreach (signCls b in a.runSign)
                {
                    if (b.isDB)
                    {
                        fs.WriteLine("<tr class=\"run\"><td>" + b.group + "</td><td class=\"trueSign\">" + b.regex + "</td>");
                        fs.WriteLine("<td>");
                        foreach (string ms in b.matchedList)
                        {
                            Console.WriteLine(ms);
                            fs.WriteLine(ms + "<br>");
                        }
                        fs.WriteLine("</td></tr>");
                    }
                    else
                    {
                        fs.WriteLine("<tr class=\"run\"><td>" + b.group + "</td><td class=\"falseSign\">" + b.regex + "</td>");
                        fs.WriteLine("<td>");
                        foreach (string ms in b.matchedList)
                        {
                            Console.WriteLine(ms);
                            fs.WriteLine(ms + "<br>");
                        }
                        fs.WriteLine("</td></tr>");
                    }

                }
                fs.WriteLine("<tr class=\"ec\" data-target=\"remov\"><td colspan=\"3\">REMOVE +/-</td></tr>");
                foreach (signCls b in a.removeSign)
                {
                    if (b.isDB)
                    {
                        fs.WriteLine("<tr class=\"remov\"><td>" + b.group + "</td><td class=\"trueSign\">" + b.regex + "</td>");
                        fs.WriteLine("<td>");
                        foreach (string ms in b.matchedList)
                        {
                            Console.WriteLine(ms);
                            fs.WriteLine(ms + "<br>");
                        }
                        fs.WriteLine("</td></tr>");
                    }
                    else
                    {
                        fs.WriteLine("<tr class=\"remov\"><td>" + b.group + "</td><td class=\"falseSign\">" + b.regex + "</td>");
                        fs.WriteLine("<td>");
                        foreach (string ms in b.matchedList)
                        {
                            Console.WriteLine(ms);
                            fs.WriteLine(ms + "<br>");
                        }
                        fs.WriteLine("</td></tr>");
                    }

                }
                fs.WriteLine("</table>");
            }
            fs.WriteLine("<script type=\"text/javascript\" src=\"./lib/jquery.js\" charset=\"utf-8\"></script>");

            fs.WriteLine("</body>");
            fs.WriteLine("</html>");
            fs.Close();
            
        }

        private bool checkKey(string root, string path)
        {
            try
            {
                RegistryKey rk = null;
                if (root == "HKLM")
                    rk = Registry.LocalMachine.OpenSubKey(path, false);
                else if (root == "HKCU")
                    rk = Registry.CurrentUser.OpenSubKey(path, false);
                else if (root == "HKCC")
                    rk = Registry.CurrentConfig.OpenSubKey(path, false);
                else if (root == "HKU")
                    rk = Registry.Users.OpenSubKey(path, false);
                else if (root == "HKCR")
                    rk = Registry.ClassesRoot.OpenSubKey(path, false);

                if (rk == null) return false;

                rk.Close();
                return true;
            }
            catch
            {
                return true;
            }
        }

        private bool checkVal(string root, string path, string key)
        {
            try
            {
                RegistryKey rk = null;
                if (root == "HKLM")
                    rk = Registry.LocalMachine.OpenSubKey(path, false);
                else if (root == "HKCU")
                    rk = Registry.CurrentUser.OpenSubKey(path, false);
                else if (root == "HKCC")
                    rk = Registry.CurrentConfig.OpenSubKey(path, false);
                else if (root == "HKU")
                    rk = Registry.Users.OpenSubKey(path, false);
                else if (root == "HKCR")
                    rk = Registry.ClassesRoot.OpenSubKey(path, false);

                if (rk == null)
                {

                    return false;
                }

                string regStr = rk.GetValue(key) as string;
                if (regStr == null)
                    return false;

                rk.Close();

                return true;
            }
            catch
            {
                return true;
            }


        }


        Boolean isInDB( String type, String regex, signCls s)
        {
            if (falsegrp.Contains(s.group))
                return false;

            
            
            if (type.StartsWith("reg"))
            {
                regex = regex.Replace("/", "\\");
                if (regex[0] == '^')
                    regex = regex.Substring(1);

                if (regex[regex.Length - 1] == '$')
                    regex = regex.Substring(0, regex.Length - 1);

                int r = regex.LastIndexOf('\\');
                            int len = r - 5;
                            if (len < 0) len = 0;
                int i = 5;
                            if (i > regex.Length)
                                i = regex.Length;
                            if (checkVal(regex.Substring(0, 4), regex.Substring(5, len), regex.Substring(r + 1))
                                || checkKey(regex.Substring(0, 4), regex.Substring(i)))
                            {
                                s.matchedList.Add(regex.ToString());
                                return true;
                            }
                            else
                            {
                                falsegrp.Add(s.group);
                                return false;
                            }
            }
            else if (type.StartsWith("file"))
            {
                regex = regex.Replace("?", String.Empty);
                regex = regex.Replace("/", "\\\\");

                Process fls = new Process();
                ProcessStartInfo psi = new ProcessStartInfo("cmd.exe");
                String arg = String.Format("/c dir /as /s /b c:\\ | findstr /i \"{0}\"",regex);
                StreamReader fout;

                
                psi.Arguments = arg;
                psi.CreateNoWindow = true;
                psi.UseShellExecute = false;
                psi.RedirectStandardOutput = true;

                fls.StartInfo = psi;
                fls.Start();

                fout = fls.StandardOutput;




                String dr;
                while (true)
                {
                    dr = fout.ReadLine();
                    
                    if( dr != null)
                        s.matchedList.Add(dr);
                    else
                        if (fout.EndOfStream && fls.HasExited)
                            break;
                    
                }

                if (s.matchedList.Count > 0)
                    return true;
                else
                    return false;


            }
            return false;
                
        }
    }
}
