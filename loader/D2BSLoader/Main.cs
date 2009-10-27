using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Configuration;

namespace D2BSLoader
{
	public partial class Main : Form
	{
		private delegate void StatusCallback(string status, System.Drawing.Color color);
		private delegate void LoadAction(int pid);

		private string D2Path, D2Exe, D2Args;
		private BindingList<ProcessWrapper> processes = new BindingList<ProcessWrapper>();

		public bool Autoclosed { get; set; }

		public Main(string[] args)
		{
			// process command line args
			Dictionary<string, LoadAction> actions = new Dictionary<string, LoadAction>();
			actions.Add("inject", null);
			actions.Add("kill", null);
			actions.Add("start", null);
			string action = "";
			int pid = -1;
			for(int i = 0; i < args.Length; i++)
			{
				switch(args[i])
				{
					case "--pid": pid = Convert.ToInt32(args[i+1]); i++; break;
					default: action = args[i].Substring(2); break;
				}
			}

			if(action != "inject" && action != "kill")
			{
				ReloadSettings();
				if(String.IsNullOrEmpty(D2Path))
				{
					Options_Click(null, null);
					ReloadSettings();
				}
				if(!File.Exists(D2Path + Path.DirectorySeparatorChar + D2Exe))
				{
					MessageBox.Show("Diablo II Executable not found! Please click 'Options' and ensure that everything is correct.", "D2BS");
				}
			}

			if(!String.IsNullOrEmpty(action))
			{
				actions[action](pid);
				Autoclosed = true;
				Close();
				return;
			}

			InitializeComponent();

			processes.RaiseListChangedEvents = true;
			Processes.DataSource = processes;
			Processes.DisplayMember = "ProcessName";
			System.Threading.Thread t = new System.Threading.Thread(ListUpdateThread);

			Shown += delegate { Process.EnterDebugMode(); t.Start(); };
			FormClosing += delegate { t.Abort(); Process.LeaveDebugMode(); };
		}

		private void ListUpdateThread()
		{
			while(true)
			{
				foreach(ProcessWrapper p in processes)
					p.Process.Refresh();

				processes.RemoveAll(x => x.Process.HasExited);

				foreach(Process p in Process.GetProcesses())
				{
					if(processes.Exists(x => p.Id == x.Process.Id))
						continue;

					string moduleName = "";
					try {
						moduleName = Path.GetFileName(p.MainModule.FileName).ToLowerInvariant();
					} catch { }
					string classname = PInvoke.User32.GetClassNameFromProcess(p);
					if(!String.IsNullOrEmpty(classname) && classname == "Diablo II" &&
						(moduleName == "game.exe" || moduleName.Contains("d2loader")))
					{
						ProcessWrapper pw = new ProcessWrapper(p);
						processes.Add(pw);
						if(GetAutoload())
						{
							p.WaitForInputIdle();
							Attach(pw);
						}
					}
				}
				Processes.Invoke((MethodInvoker)delegate {
					((CurrencyManager)Processes.BindingContext[Processes.DataSource]).Refresh();
				});
				System.Threading.Thread.Sleep(400);
			}
		}

		public static void SaveSettings(string path, string exe, string args)
		{
			Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
			config.AppSettings.Settings.Remove("D2Path");
			config.AppSettings.Settings.Remove("D2Exe");
			config.AppSettings.Settings.Remove("D2Args");
			config.AppSettings.Settings.Add("D2Path", path);
			config.AppSettings.Settings.Add("D2Exe", exe);
			config.AppSettings.Settings.Add("D2Args", args);
			config.Save(ConfigurationSaveMode.Full);
		}

		private void ReloadSettings()
		{
			ConfigurationManager.RefreshSection("appSettings");
			D2Path = ConfigurationManager.AppSettings["D2Path"];
			D2Exe = ConfigurationManager.AppSettings["D2Exe"];
			D2Args = ConfigurationManager.AppSettings["D2Args"];
		}

		private bool GetAutoload()
		{
			bool autoload = false;
			if(Autoload.InvokeRequired)
				Autoload.Invoke((MethodInvoker)delegate { autoload = Autoload.Checked; });
			else
				autoload = Autoload.Checked;
			return autoload;
		}

		private void SetStatus(string status, Color color)
		{
			// ignore the call if we haven't init'd the window yet
			if(Status == null)
				return;

			if(Status.InvokeRequired)
			{
				StatusCallback cb = SetStatus;
				Status.Invoke(cb, status, color);
				return;
			}
			Status.ForeColor = color;
			Status.Text = status;
		}

		private Process GetProcessById(int pid)
		{
			try { return Process.GetProcessById(pid); }
			catch(ArgumentException) { return null; }
		}
		private void Start(int pid) { Start(); }
		private void Inject(int pid) { Process p = GetProcessById(pid); if(p != null) Attach(p); }
		private void Kill(int pid) { Process p = GetProcessById(pid); if(p != null) p.Kill(); }

		private bool Attach(Process p)
		{
			string path = Application.StartupPath + Path.DirectorySeparatorChar;
			return  File.Exists(path + "libnspr4.dll") &&
					File.Exists(path + "js32.dll") &&
					File.Exists(path + "cGuard.dll") &&
					PInvoke.Kernel32.LoadRemoteLibrary(p, path + "libnspr4.dll") &&
					PInvoke.Kernel32.LoadRemoteLibrary(p, path + "js32.dll") &&
					PInvoke.Kernel32.LoadRemoteLibrary(p, path + "cGuard.dll");
		}

		private void Attach(ProcessWrapper pw)
		{
			if(pw.Loaded)
				SetStatus("Already loaded!", Color.Blue);
			else if(Attach(pw.Process))
			{
				SetStatus("Success!", Color.Green);
				pw.Loaded = true;
			}
			else
				SetStatus("Failed!", Color.Red);
		}

		private void Start()
		{
			if(String.IsNullOrEmpty(D2Exe))
				return;

			ProcessStartInfo psi = new ProcessStartInfo(D2Path + Path.DirectorySeparatorChar + D2Exe, D2Args);
			psi.UseShellExecute = false;
			psi.WorkingDirectory = D2Path;
			Process p = Process.Start(psi);
			p.WaitForInputIdle();
			ProcessWrapper pw = new ProcessWrapper(p);
			processes.Add(pw);
			if(GetAutoload())
				Attach(pw);
		}

		private void Load_Click(object sender, EventArgs e)
		{
			if(Processes.SelectedIndex == -1)
			{
				SetStatus("No process selected!", Color.Blue);
				return;
			}
			Attach(Processes.SelectedItem as ProcessWrapper);
		}

		private void StartNew_Click(object sender, EventArgs e)
		{
			Start();
		}

		private void Options_Click(object sender, EventArgs e)
		{
			Options o = new Options();
			o.ShowDialog();
			ReloadSettings();
		}
	}

	internal class ProcessWrapper
	{
		public bool Loaded { get; set; }
		public Process Process { get; internal set; }
		public string ProcessName { get { return Process.MainWindowTitle + " [" + Process.Id + "]" + (Loaded ? " *" : ""); } }
		public ProcessWrapper(Process p) { Process = p; }
	}

	public static class BindingListExtensions
	{
		public static void RemoveAll<T>(this BindingList<T> list, Predicate<T> pred)
		{
			for(int i = 0; i < list.Count; i++)
			{
				if(pred(list[i]))
					list.RemoveAt(i);
			}
		}
		public static bool Exists<T>(this BindingList<T> list, Predicate<T> pred)
		{
			foreach(T obj in list)
				if(pred(obj))
					return true;
			return false;
		}
	}

}
