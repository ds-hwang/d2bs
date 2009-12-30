using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace D2BSLoader
{
	public partial class Options : Form
	{
		public Options(string path, string exe, string args, string dll)
		{
			InitializeComponent();

			Path.Text = path;
			if(String.IsNullOrEmpty(Path.Text))
			{
				Microsoft.Win32.RegistryKey key = Microsoft.Win32.Registry.CurrentUser;
				if(key != null)
					key = key.OpenSubKey("Software");
				if(key != null)
					key = key.OpenSubKey("Blizzard Entertainment");
				if(key != null)
					key = key.OpenSubKey("Diablo II");
				if(key != null)
					Path.Text = key.GetValue("InstallPath", String.Empty).ToString();
			}
			Exe.Text = exe;
			Args.Text = args;
			DLL.Text = String.IsNullOrEmpty(dll) ? "cGuard.dll" : dll;
		}

		private void OK_Click(object sender, EventArgs e)
		{
			if(File.Exists(System.IO.Path.Combine(Path.Text, Exe.Text)))
			{
				Main.SaveSettings(Path.Text, Exe.Text, Args.Text, DLL.Text);
				Close();
			}
			else
				MessageBox.Show("Invalid path and/or executable!", "D2BS");
		}

		private void Cancel_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void FindPath_Click(object sender, EventArgs e)
		{
			FolderBrowserDialog fbd = new FolderBrowserDialog();
			fbd.ShowNewFolderButton = false;
			fbd.RootFolder = Environment.SpecialFolder.MyComputer;
			if(DialogResult.OK == fbd.ShowDialog())
				Path.Text = fbd.SelectedPath;
		}

		private void FindExe_Click(object sender, EventArgs e)
		{
			OpenFileDialog ofd = new OpenFileDialog();
			ofd.Filter = "Executable Files|*.exe";
			ofd.Multiselect = false;
			ofd.InitialDirectory = Path.Text;
			if(DialogResult.OK == ofd.ShowDialog())
				Exe.Text = ofd.SafeFileName;
		}

		private void FindDll_Click(object sender, EventArgs e)
		{
			OpenFileDialog ofd = new OpenFileDialog();
			ofd.Filter = "DLL Files|*.dll";
			ofd.Multiselect = false;
			ofd.InitialDirectory = Application.StartupPath;
			if(DialogResult.OK == ofd.ShowDialog())
				DLL.Text = ofd.SafeFileName;
		}
	}
}
