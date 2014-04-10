using IOAF;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IOAF_GUI
{
    public partial class Form1 : Form
    {
        ArrayList partition = new ArrayList();
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            progressBar1.MarqueeAnimationSpeed = 0;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            imgOpenDialog.Filter = "Encase Image (E01)|*.e01|Raw Image (dd)|*.dd";
            
            if ( imgOpenDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK )
            {
                imgPath.Text = imgOpenDialog.FileName;
                RUNmmls();
            }
        }


        private void RUNmmls()
        {
            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo("tsk\\mmls.exe");

            StreamReader sout;

            psi.Arguments = imgPath.Text;
            psi.RedirectStandardOutput = true;
            psi.UseShellExecute = false;
            psi.CreateNoWindow = true;

            p.StartInfo = psi;
            p.Start();

            sout = p.StandardOutput;

            String s;
            
            partitionList.Items.Clear();
            partition.Clear();

            while ((s = sout.ReadLine()) != null)
            {
                Regex regex = new Regex(@"(?<start>[0-9]{10}).*?(?<end>[0-9]{10}).*?(?<len>[0-9]{10})\s{2,}(?<fs>.*?)$");

                Match m = regex.Match(s);

                if (!m.Success)
                    continue;

                if (m.Groups["fs"].Value.Contains("NTFS"))
                {
                    partition.Add(new DiskImgInfo(m.Groups["start"].Value,
                                              m.Groups["fs"].Value));
                    partitionList.Items.Add("Offset: " + m.Groups["start"].Value + "     Filesystem: " + m.Groups["fs"].Value);
                }
            }

            partitionList.SelectedIndex = 0;

        }

        void xmlMatcher(string dbname, string xmlName)
        {
            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo("regexMatcher.exe");

            psi.Arguments = String.Format("\"{0}\" \"{1}\"", dbname, xmlName);

            p.StartInfo = psi;
            p.Start();
            p.WaitForExit();


        }

        private void partitionList_SelectedIndexChanged(object sender, EventArgs e)
        {
            fls4root();
        }

        private void fls4root()
        {
            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo("tsk\\fls.exe");
            StreamReader stdout;

            DiskImgInfo im = (DiskImgInfo)partition[partitionList.SelectedIndex];

            psi.Arguments = "-o " + im.getOffset()+ " \"" + imgPath.Text+"\"";
            psi.RedirectStandardOutput = true;
            psi.UseShellExecute = false;
            psi.CreateNoWindow = true;

            p.StartInfo = psi;
            p.Start();

            stdout = p.StandardOutput;

            Preview.Text = stdout.ReadToEnd();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            if (imgPath.Text.Length == 0)
            {
                MessageBox.Show("디스크 이미지를 선택해 주세요.");
                return;
            }
            if(dbName.Text.Length == 0)
            {
                MessageBox.Show("DB Name을 입력해주세요.");
                return;
            }
            if(sXML.Text.Length == 0)
            {
                MessageBox.Show("Signature DB를 입력해주세요.");
                return;
            }

            progressBar1.MarqueeAnimationSpeed = 10;

            partitionList.Enabled = false;
            dbName.Enabled = false;
            sXML.Enabled = false;
            button1.Enabled = false;
            button2.Enabled = false;
            imgOpen.Enabled = false;

            string imagePath = imgPath.Text;
            int partno = partitionList.SelectedIndex;
            string db = dbName.Text;
            string xmlname = sXML.Text;
            selectPartition(imagePath, db, partno);
            extracgReg(imagePath, db, partno);
            insREG(db);
            xmlMatcher(db, xmlname);
            progressBar1.MarqueeAnimationSpeed = 0;
            partitionList.Enabled = true;
            dbName.Enabled = true;
            sXML.Enabled = true;
            button1.Enabled = true;
            button2.Enabled = true;
            imgOpen.Enabled = true;
        }

        void selectPartition(string imgpath, string dbname, int partno)
        {
            DiskImgInfo d = (DiskImgInfo)partition[partno];

            string dOffset = d.getOffset();

            Process p = new Process();
            ProcessStartInfo psi = new ProcessStartInfo("fls2db.exe");

            psi.Arguments = String.Format("\"{0}\" \"{1}\" \"{2}\"", imgpath, dOffset, dbname);

            p.StartInfo = psi;
            p.Start();
            p.WaitForExit();


        }

        void extracgReg(string imgpath, string dbname, int partno)
        {
            DiskImgInfo d = (DiskImgInfo)partition[partno];

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
            FileInfo[] file = dirInfo.GetFiles().Where(f => f.Extension.StartsWith(".reg")).ToArray();

            foreach (FileInfo f in file)
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

        private void button2_Click(object sender, EventArgs e)
        {
            xmlOpenDialog.Filter = "(Signature DB (xml)|*.xml";
            
            if ( xmlOpenDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK )
            {
                sXML.Text = xmlOpenDialog.FileName;
            }
        }
        

    }
}
