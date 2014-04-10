using System;
using System.Collections.Generic;
using System.Data.SQLite;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

/*
 * 
 *  args0 == image path
 *  args1 == offset
 *  args2 == dbname
 *  
 */ 

namespace extreg
{
    class extreg
    {
        static void Main(string[] args)
        {
            new extreg().DoMain(args[0], args[1], args[2]);
            //new extreg().DoMain("f:\\image\\eunjin-pc.e01", "206848", "dddd");
            //new extreg().extract_reg(args[1], args[0], args[2], "HKLMSAM", "testcase");
        }

        void DoMain(string imgpath, string offset, string dbname)
        {
            String sql = "Data Source="+dbname+"\\info.db";
            SQLiteConnection conn = new SQLiteConnection(sql);

            conn.Open();
            String HKLMSAM = @"select * from filelist where filelist.path match 'config/sam';";
            String HKLMSEC = @"select * from filelist where filelist.path match 'config/security';";
            String HKLMSOF = @"select * from filelist where filelist.path match 'config/software';";
            String HKLMSYS = @"select * from filelist where filelist.path match 'config/system';";
            String HKCU = @"select * from filelist where filelist.path match 'ntuser.dat';";
            SQLiteCommand cmd = new SQLiteCommand(HKLMSAM, conn);
            SQLiteDataReader r = cmd.ExecuteReader();
            int i = 0;
            if (r != null)
            {
                while (r.Read())
                {
                    if (!Regex.IsMatch(r["path"].ToString(), ".*?config/sam$", RegexOptions.IgnoreCase))
                        continue;
                    Console.WriteLine("LMSAM\\" + i++);
                    new Thread(unused => extract_reg(offset, imgpath, (string)r["inode"], "HKLMSAM", dbname)).Start();
                }
            }
            i = 0;
            r.Close();
            conn.Close();
            conn.Open();
            cmd = new SQLiteCommand(HKLMSEC, conn);
            SQLiteDataReader r2 = cmd.ExecuteReader();
            if (r2 != null)
            {
                while (r2.Read())
                {
                    if (!Regex.IsMatch(r2["path"].ToString(), ".*?config/security$", RegexOptions.IgnoreCase))
                        continue;
                    Console.WriteLine("LMSEC\\" + i++);
                    new Thread(unused => extract_reg(offset, imgpath, r2["inode"].ToString(), "HKLMSEC", dbname)).Start();
                }
            }
            i = 0;
            r2.Close();
            conn.Close();
            conn.Open();
            cmd = new SQLiteCommand(HKLMSOF, conn);
            SQLiteDataReader r3 = cmd.ExecuteReader();
            if (r3 != null)
            {
                while (r3.Read())
                {
                    if (!Regex.IsMatch(r3["path"].ToString(), ".*?config/software$", RegexOptions.IgnoreCase))
                        continue;
                    Console.WriteLine("LMSOF\\" + i++);
                    new Thread(unused => extract_reg(offset, imgpath, r3["inode"].ToString(), "HKLMSOFT", dbname)).Start();
                }
            }
            i = 0;
          /*  cmd = new SQLiteCommand(HKLMSYS, conn);
            r = cmd.ExecuteReader();
            while (r.Read())
            {
                Console.WriteLine("LMSYS\\"+i++);
                new Thread(unused => extract_reg(offset, imgpath, r["inode"].ToString(), "HKLMSYS", dbname)).Start();
            }
            i = 0;*/
            r3.Close();
            conn.Close();
            conn.Open();
            cmd = new SQLiteCommand(HKCU, conn);
            SQLiteDataReader r4 = cmd.ExecuteReader();
            if (r4 != null)
            {
                while (r4.Read())
                {
                    if (!Regex.IsMatch(r4["path"].ToString(), ".*?Users/.*?/ntuser.dat$", RegexOptions.IgnoreCase))
                        continue;
                    Console.WriteLine("CU\\" + i++);
                    new Thread(unused => extract_reg(offset, imgpath, r4["inode"].ToString(), "HKCU", dbname)).Start();
                }
            }
            conn.Close();
            
        }

       

        void extract_reg(string offset, string img, string inode, string fname, string dbname)
        {
            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo("cmd.exe");
            int i = 0;
            string fname2 = fname;
            while(File.Exists(dbname+"\\"+fname2+".reg"))
            {
                fname2 = fname + i++;
            }
            String arg = String.Format("/c .\\tsk\\icat.exe -o {0} {1} {2} > {3}\\{4}.reg", offset, img, inode, dbname, fname2);
            Console.WriteLine(arg);
            psi.Arguments = arg;
            psi.CreateNoWindow = true;
            psi.WindowStyle = ProcessWindowStyle.Hidden;
            p.StartInfo = psi;
            p.Start();
            p.WaitForExit();
          
        }
    
    }
}
