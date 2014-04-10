namespace IOAF_GUI
{
    partial class Form1
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다.
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            this.imgOpen = new System.Windows.Forms.Button();
            this.imgPath = new System.Windows.Forms.TextBox();
            this.partitionList = new System.Windows.Forms.ComboBox();
            this.Preview = new System.Windows.Forms.TextBox();
            this.imgOpenDialog = new System.Windows.Forms.OpenFileDialog();
            this.label1 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.label2 = new System.Windows.Forms.Label();
            this.dbName = new System.Windows.Forms.TextBox();
            this.sXML = new System.Windows.Forms.TextBox();
            this.xmlOpenDialog = new System.Windows.Forms.OpenFileDialog();
            this.button2 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // imgOpen
            // 
            this.imgOpen.Location = new System.Drawing.Point(12, 12);
            this.imgOpen.Name = "imgOpen";
            this.imgOpen.Size = new System.Drawing.Size(102, 23);
            this.imgOpen.TabIndex = 0;
            this.imgOpen.Text = "Image Open";
            this.imgOpen.UseVisualStyleBackColor = true;
            this.imgOpen.Click += new System.EventHandler(this.button1_Click);
            // 
            // imgPath
            // 
            this.imgPath.Location = new System.Drawing.Point(120, 12);
            this.imgPath.Name = "imgPath";
            this.imgPath.ReadOnly = true;
            this.imgPath.Size = new System.Drawing.Size(422, 21);
            this.imgPath.TabIndex = 1;
            // 
            // partitionList
            // 
            this.partitionList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.partitionList.FormattingEnabled = true;
            this.partitionList.Location = new System.Drawing.Point(12, 41);
            this.partitionList.Name = "partitionList";
            this.partitionList.Size = new System.Drawing.Size(530, 20);
            this.partitionList.TabIndex = 2;
            this.partitionList.SelectedIndexChanged += new System.EventHandler(this.partitionList_SelectedIndexChanged);
            // 
            // Preview
            // 
            this.Preview.Location = new System.Drawing.Point(12, 106);
            this.Preview.Multiline = true;
            this.Preview.Name = "Preview";
            this.Preview.Size = new System.Drawing.Size(698, 315);
            this.Preview.TabIndex = 3;
            // 
            // imgOpenDialog
            // 
            this.imgOpenDialog.FileName = "openFileDialog1";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 91);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(88, 12);
            this.label1.TabIndex = 4;
            this.label1.Text = "Root Directory:";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(548, 12);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(162, 49);
            this.button1.TabIndex = 5;
            this.button1.Text = "Start";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(14, 427);
            this.progressBar1.MarqueeAnimationSpeed = 10;
            this.progressBar1.Maximum = 10;
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(696, 23);
            this.progressBar1.Style = System.Windows.Forms.ProgressBarStyle.Marquee;
            this.progressBar1.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 70);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 12);
            this.label2.TabIndex = 7;
            this.label2.Text = "DBName";
            // 
            // dbName
            // 
            this.dbName.Location = new System.Drawing.Point(73, 67);
            this.dbName.Name = "dbName";
            this.dbName.Size = new System.Drawing.Size(125, 21);
            this.dbName.TabIndex = 8;
            // 
            // sXML
            // 
            this.sXML.Location = new System.Drawing.Point(329, 67);
            this.sXML.Name = "sXML";
            this.sXML.ReadOnly = true;
            this.sXML.Size = new System.Drawing.Size(381, 21);
            this.sXML.TabIndex = 10;
            // 
            // xmlOpenDialog
            // 
            this.xmlOpenDialog.FileName = "openFileDialog1";
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(204, 67);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(119, 23);
            this.button2.TabIndex = 11;
            this.button2.Text = "Signature DB";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(722, 462);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.sXML);
            this.Controls.Add(this.dbName);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.Preview);
            this.Controls.Add(this.partitionList);
            this.Controls.Add(this.imgPath);
            this.Controls.Add(this.imgOpen);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "Form1";
            this.Text = "IOAF_gui";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button imgOpen;
        private System.Windows.Forms.TextBox imgPath;
        private System.Windows.Forms.ComboBox partitionList;
        private System.Windows.Forms.TextBox Preview;
        private System.Windows.Forms.OpenFileDialog imgOpenDialog;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox dbName;
        private System.Windows.Forms.TextBox sXML;
        private System.Windows.Forms.OpenFileDialog xmlOpenDialog;
        private System.Windows.Forms.Button button2;
    }
}

