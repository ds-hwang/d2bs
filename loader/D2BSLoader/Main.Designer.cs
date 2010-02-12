namespace D2BSLoader
{
	partial class Main
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if(disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            System.Windows.Forms.Label label1;
            System.Windows.Forms.Button Load;
            System.Windows.Forms.Button StartNew;
            System.Windows.Forms.Label label2;
            System.Windows.Forms.Button Options;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Main));
            this.Processes = new System.Windows.Forms.ListBox();
            this.Status = new System.Windows.Forms.Label();
            this.Autoload = new System.Windows.Forms.CheckBox();
            label1 = new System.Windows.Forms.Label();
            Load = new System.Windows.Forms.Button();
            StartNew = new System.Windows.Forms.Button();
            label2 = new System.Windows.Forms.Label();
            Options = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new System.Drawing.Point(12, 9);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(141, 13);
            label1.TabIndex = 5;
            label1.Text = "Available Diablo II Instances";
            // 
            // Load
            // 
            Load.Location = new System.Drawing.Point(12, 152);
            Load.Name = "Load";
            Load.Size = new System.Drawing.Size(75, 23);
            Load.TabIndex = 2;
            Load.Text = "Load";
            Load.UseVisualStyleBackColor = true;
            Load.Click += new System.EventHandler(this.Load_Click);
            // 
            // StartNew
            // 
            StartNew.Location = new System.Drawing.Point(109, 152);
            StartNew.Name = "StartNew";
            StartNew.Size = new System.Drawing.Size(75, 23);
            StartNew.TabIndex = 3;
            StartNew.Text = "Start New";
            StartNew.UseVisualStyleBackColor = true;
            StartNew.Click += new System.EventHandler(this.StartNew_Click);
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new System.Drawing.Point(12, 123);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(87, 13);
            label2.TabIndex = 6;
            label2.Text = "Last Load Status";
            // 
            // Options
            // 
            Options.Location = new System.Drawing.Point(154, 1);
            Options.Name = "Options";
            Options.Size = new System.Drawing.Size(56, 21);
            Options.TabIndex = 4;
            Options.Text = "Options";
            Options.UseVisualStyleBackColor = true;
            Options.Click += new System.EventHandler(this.Options_Click);
            // 
            // Processes
            // 
            this.Processes.FormattingEnabled = true;
            this.Processes.Location = new System.Drawing.Point(12, 25);
            this.Processes.Name = "Processes";
            this.Processes.Size = new System.Drawing.Size(198, 95);
            this.Processes.TabIndex = 0;
            // 
            // Status
            // 
            this.Status.AutoSize = true;
            this.Status.ForeColor = System.Drawing.SystemColors.InactiveCaption;
            this.Status.Location = new System.Drawing.Point(28, 136);
            this.Status.Name = "Status";
            this.Status.Size = new System.Drawing.Size(33, 13);
            this.Status.TabIndex = 7;
            this.Status.Text = "None";
            // 
            // Autoload
            // 
            this.Autoload.AutoSize = true;
            this.Autoload.Location = new System.Drawing.Point(142, 123);
            this.Autoload.Name = "Autoload";
            this.Autoload.Size = new System.Drawing.Size(68, 17);
            this.Autoload.TabIndex = 1;
            this.Autoload.Text = "Autoload";
            this.Autoload.UseVisualStyleBackColor = true;
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(222, 187);
            this.Controls.Add(Options);
            this.Controls.Add(this.Autoload);
            this.Controls.Add(this.Status);
            this.Controls.Add(label2);
            this.Controls.Add(StartNew);
            this.Controls.Add(Load);
            this.Controls.Add(label1);
            this.Controls.Add(this.Processes);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "Main";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "D2BS Loader";
            this.Load += new System.EventHandler(this.Main_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ListBox Processes;
		private System.Windows.Forms.Label Status;
		private System.Windows.Forms.CheckBox Autoload;
	}
}

