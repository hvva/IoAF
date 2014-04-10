using System;
using System.Collections.Generic;
using System.Data;
using System.Data.SQLite;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

/*
 * args0 == img
 * args1 == dbname
 * args2 == prefix
 * args3 == regoffset
 *
 */

namespace flsreg2db
{
    class regfls2db
    {
        Boolean isFLSComplete;
        Queue<String> flsQ;
        Thread[] th;
        private object lockObject; // criticalsection
        String dbname;
        String dbSource;
        int i = 0;
        string prefix;
        static void Main(string[] args)
        {
            string regoffset = "4128";
            string pre = args[2];
            if (args.Length == 4)
                regoffset = args[3];
            new regfls2db().DoMain(args[1],args[0], regoffset, pre);
            //new fls2db().DoMain("E:\\image\\vm.e01", "206848", "test");
        }

        private void createDB()
        {
           
            SQLiteConnection conn = new SQLiteConnection(dbSource);

            conn.Open();
            String sqlt = "create table if not exists reglist ( path string, inode string);";
            
            SQLiteCommand cmd = new SQLiteCommand(sqlt, conn);
            cmd.ExecuteNonQuery();
        
            conn.Close();
        }
        private void insertReg(String path, String inode)
        {
            SQLiteConnection conn = new SQLiteConnection(dbSource);

            conn.Open();

           // String sqli = "insert into reglist values ('" + prefix + "\\" + path + "', '" + inode + "');";
            //System.Data.SQLite.SQLiteCommand cmd = new SQLiteCommand(sqli, conn);

            SQLiteCommand cmd = conn.CreateCommand();
            cmd.CommandType = CommandType.Text;
            cmd.CommandText = "insert into reglist values (@param1, @param2);";
            cmd.Parameters.Add(new SQLiteParameter("@param1", prefix + "/" + path));
            cmd.Parameters.Add(new SQLiteParameter("@param2", inode));

            cmd.ExecuteNonQuery();
            conn.Close();
        }

        void DoMain(string db, string imgpath, string regoffset, string pre)
        {
            dbname = db;
            prefix = pre;
            dbSource = @"Data Source=" + dbname + "\\info.db";
            if(!Directory.Exists(".\\"+dbname))
                System.IO.Directory.CreateDirectory(dbname);
            flsQ = new Queue<string>();
            lockObject = new object();
            createDB();
            Thread thth = new Thread(unused => RunFLS(imgpath, regoffset));
            thth.Start();

            th = new Thread[4] {
                    new Thread(inputDB),
                    new Thread(inputDB),
                    new Thread(inputDB),
                    new Thread(inputDB)
                };

            foreach (Thread t in th)
                t.Start();


            foreach (Thread t in th)
                t.Join();
        }

        void inputDB() //thread function
        {
            String s;
            while (true)
            {
         
                lock (lockObject)
                {
                    if (flsQ.Count == 0)
                        if (isFLSComplete == false)
                            continue;
                        else
                            break;
                    s = flsQ.Dequeue();
                    Console.WriteLine(i++);
                
                    Regex reg = new Regex(@"(?<type>...) (?<inode>.*?):\s+(?<path>.*?)$");

                    Match matches = reg.Match(s);

                    String type = matches.Groups["type"].Value;
                    String inode = matches.Groups["inode"].Value;
                    String path = matches.Groups["path"].Value;
                    insertReg(path, inode);
                }

            }


        }

        public void RunFLS(string img, string rootoffset)
        {
            Process fls = new Process();
            ProcessStartInfo psi = new ProcessStartInfo(".\\tsk\\fls.exe");
            String arg = String.Format("-rp -f reg {0} {1}", img, rootoffset );
            Console.WriteLine(arg);
            StreamReader fout;

            isFLSComplete = false;

            psi.Arguments = arg;
            psi.CreateNoWindow = true;
            psi.UseShellExecute = false;
            psi.RedirectStandardOutput = true;

            fls.StartInfo = psi;
            fls.Start();

            fout = fls.StandardOutput;




            String s;
            while (true)
            {
                s = fout.ReadLine();

                if (s == null)
                    if (fls.HasExited) // 종료가 되고 읽어온게 없을경우 break;
                    {
                        isFLSComplete = true;
                        break;
                    }
                    else
                        continue; // 그 이외엔 continue

                lock (lockObject)
                    flsQ.Enqueue(s);
            }




        }
    }
}
