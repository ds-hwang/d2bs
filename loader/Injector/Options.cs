using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Configuration;
using System.IO;

namespace Injector
{
	public partial class Options : Form
	{
		public Options()
		{
			InitializeComponent();
			textBox1.Text = Path.GetDirectoryName(ConfigurationManager.AppSettings["D2Exe"]);
			textBox2.Text = ConfigurationManager.AppSettings["D2Args"];
		}

		private void button1_Click(object sender, EventArgs e)
		{
			if(!File.Exists(textBox1.Text + Path.DirectorySeparatorChar + "diablo ii.exe") &&
				!File.Exists(textBox1.Text + Path.DirectorySeparatorChar + "game.exe"))
			{
				MessageBox.Show("Invalid path! Diablo II program files not found!", "D2BS");
				return;
			}
			Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
			config.AppSettings.Settings.Remove("D2Exe");
			config.AppSettings.Settings.Remove("D2Args");
			config.AppSettings.Settings.Add("D2Exe", textBox1.Text + Path.DirectorySeparatorChar + "game.exe");
			config.AppSettings.Settings.Add("D2Args", textBox2.Text);
			config.Save(ConfigurationSaveMode.Full);
			DialogResult = DialogResult.OK;
			Close();
		}

		private void button2_Click(object sender, EventArgs e)
		{
			DialogResult = DialogResult.Cancel;
			Close();
		}

		private void button3_Click(object sender, EventArgs e)
		{
			if(folderBrowserDialog1.ShowDialog() == DialogResult.OK)
				textBox1.Text = folderBrowserDialog1.SelectedPath;
		}
	}
}
