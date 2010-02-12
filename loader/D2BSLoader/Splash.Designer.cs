namespace D2BSLoader
{
	partial class Splash
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
            System.Windows.Forms.TransparentLabel label1;
            VersionLabel = new System.Windows.Forms.TransparentLabel();
            label1 = new System.Windows.Forms.TransparentLabel();
            this.SuspendLayout();
            // 
            // label2
            // 
			VersionLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			VersionLabel.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(131)))), ((int)(((byte)(139)))), ((int)(((byte)(139)))));
			VersionLabel.Location = new System.Drawing.Point(251, 283);
			VersionLabel.Name = "label2";
			VersionLabel.Size = new System.Drawing.Size(175, 40);
			VersionLabel.TabIndex = 1;
			VersionLabel.TabStop = false;
            // 
            // label1
            // 
            label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            label1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(139)))), ((int)(((byte)(35)))), ((int)(((byte)(35)))));
            label1.Location = new System.Drawing.Point(230, 458);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(190, 18);
            label1.TabIndex = 0;
            label1.TabStop = false;
            label1.Text = "Copyright © 2008, The D2BS Team";
            // 
            // Splash
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Lime;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.ClientSize = new System.Drawing.Size(648, 488);
            this.ControlBox = false;
            this.Controls.Add(VersionLabel);
            this.Controls.Add(label1);
            this.DoubleBuffered = true;
            this.ForeColor = System.Drawing.SystemColors.ActiveCaption;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Splash";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Splash";
            this.TopMost = true;
            this.TransparencyKey = System.Drawing.Color.Black;
            this.Load += new System.EventHandler(this.Splash_Load);
            this.ResumeLayout(false);

		}

		private System.Windows.Forms.TransparentLabel VersionLabel;

		#endregion


	}
}