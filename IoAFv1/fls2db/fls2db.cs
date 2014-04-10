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
 *
 *  args0 == image path
 *  args1 == offset
 *  args2 == dbname
 * 
 */

namespace fls2db
{
    class fls2db
    {
        Boolean isFLSComplete;
        Queue<String> flsQ;
        Thread[] th;
        private object lockObject; // criticalsection
        String dbname;
        String dbSource;
        int i = 0;
    
        static void Main(string[] args)
        {
            new fls2db().DoMain(args[0], args[1], args[2]);
            //new fls2db().DoMain("E:\\image\\vm.e01", "206848", "test");
        }

        private void createDB()
        {
            if (File.Exists(dbname + @"\info.db"))
                File.Delete(dbname + @"\info.db");

            SQLiteConnection conn = new SQLiteConnection(dbSource);

            conn.Open();
            String sqlt = "create virtual table if not exists filelist using fts3( path string, inode string);";
           // String sqlt2 = "create table if not exists fileinfo ( inode string, filesize string);";
            String sqlt2 = "create virtual table if not exists reglist using fts3( path string, inode string);";
            SQLiteCommand cmd = new SQLiteCommand(sqlt, conn);
            cmd.ExecuteNonQuery();
            SQLiteCommand cmd2 = new SQLiteCommand(sqlt2, conn);
            cmd2.ExecuteNonQuery();
            conn.Close();
        }
        private void insertFile(String path, String inode)
        {
            SQLiteConnection conn = new SQLiteConnection(dbSource);

            conn.Open();

            //String sqli = "insert into filelist values ('" + path + "', '" + inode + "');";
            SQLiteCommand cmd = conn.CreateCommand();
            cmd.CommandType = CommandType.Text;
            cmd.CommandText = "insert into filelist values (@param1, @param2);";
            cmd.Parameters.Add(new SQLiteParameter("@param1", path));
            cmd.Parameters.Add(new SQLiteParameter("@param2", inode));
            //SQLiteCommand cmd = new SQLiteCommand(sqli, conn);
            cmd.ExecuteNonQuery();
            conn.Close();
        }

        void DoMain(string imgpath, string offset, string db)
        {
            dbname = db;
            dbSource = @"Data Source=" + dbname + "\\info.db";
            System.IO.Directory.CreateDirectory(dbname);
            flsQ = new Queue<string>();
            lockObject = new object();
            createDB();
            Thread thth = new Thread(unused => RunFLS(imgpath, offset));
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
                    insertFile(path, inode);

                }
            }
            

        }
        
        public void RunFLS(string img, string offset)
        {
            Process fls = new Process();
            ProcessStartInfo psi = new ProcessStartInfo(".\\tsk\\fls.exe");
            String arg = String.Format("-rp -o {0} {1}", offset, img);
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
            while(true)
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

                lock ( lockObject )
                    flsQ.Enqueue(s);
             }

            


        }
    }
}
