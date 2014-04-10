using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Data.SQLite;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Xml;

namespace regexMatcher
{
    class regmatcher
    {
        ArrayList falsegrp;
        SQLiteConnection conn;
        static void Main(string[] args)
        {
            
            new regmatcher().DoMain(args[0], args[1]);
            //new regmatcher().DoMain("test2", "sample.xml");
           
        }

        void DoMain(String dbname, String xml)
        {
            
            falsegrp = new ArrayList();

            Thread th1 = new Thread(unused => th_match(dbname, xml));

            th1.Start();

            while( th1.IsAlive )
            {
                Console.Write(".");
                Thread.Sleep(1000);
            }
            
            
        }

        
        void th_match(String dbname, String xml)
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
            String dbSource = @"Data Source=" + dbname + @"\info.db";
            conn = new SQLiteConnection(dbSource);
            conn.Open();
            foreach (signs a in signList)
            {
                foreach (signCls b in a.insSign)
                {
                    b.isDB = isInDB(dbname, b.type, b.regex, b);
                }
                foreach (signCls b in a.insSign)
                {
                    if (falsegrp.Contains(b.group))
                        b.isDB = false;
                }
                falsegrp.Clear();
                foreach (signCls b in a.runSign)
                {
                    b.isDB = isInDB(dbname, b.type, b.regex, b);
                }
                foreach (signCls b in a.runSign)
                {
                    if (falsegrp.Contains(b.group))
                        b.isDB = false;
                }
                falsegrp.Clear();
                foreach (signCls b in a.removeSign)
                {
                    b.isDB = isInDB(dbname, b.type, b.regex, b);
                }
                foreach (signCls b in a.removeSign)
                {
                    if (falsegrp.Contains(b.group))
                        b.isDB = false;
                }
                falsegrp.Clear();
            }


            StreamWriter fs = new StreamWriter(dbname + ".html");
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
                            fs.WriteLine(ms+"<br>");
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
            conn.Close();
        }

        Boolean isInDB(String dbname, String type, String regex, signCls s)
        {
            if (falsegrp.Contains(s.group))
                return false;

 

            SQLiteCommand cmd = conn.CreateCommand();
            cmd.CommandType = CommandType.Text;
            if (type.StartsWith("reg"))
                cmd.CommandText = "select * from reglist where reglist.path REGEXP @param1;";
            else if (type.StartsWith("file"))
                cmd.CommandText = "select * from filelist where filelist.path REGEXP @param1;";
            cmd.Parameters.Add(new SQLiteParameter("@param1", regex));

            SQLiteDataReader r = cmd.ExecuteReader();


            bool res = r.HasRows;
            if (res == false)
                falsegrp.Add(s.group);
            else
            {
                while(r.Read())
                {
                    s.matchedList.Add(r["path"].ToString());
                }
            }



            return res;
        }
    }
}
