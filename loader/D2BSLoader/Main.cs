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
		private delegate int LoadAction(int pid);

		private static string D2Path = String.Empty,
							  D2Exe = String.Empty,
							  D2Args = String.Empty,
							  D2BSDLL = String.Empty;
		private static int LoadDelay = 1000;
		private static Dictionary<string, LoadAction> actions = new Dictionary<string, LoadAction>() {
				{"inject", Inject},
				{"kill", Kill},
				{"start", Start},
				{"save", Save}
			};
		private BindingList<ProcessWrapper> processes = new BindingList<ProcessWrapper>();

		public bool Autoclosed { get; set; }
		private static bool IsInDebug { get; set; }

		public Main(string[] args)
		{
			if(args.Length > 0)
				ProcessCmdArgs(args);
			else
			{
				Splash s = new Splash(this);
				s.FormClosed += delegate {
					LoadSettings(true);
					if(String.IsNullOrEmpty(D2Path) || String.IsNullOrEmpty(D2Exe) ||
					   !File.Exists(Path.Combine(D2Path, D2Exe)) ||
					   String.IsNullOrEmpty(D2BSDLL) ||
					   !File.Exists(Path.Combine(Application.StartupPath, D2BSDLL)))
						Options_Click(null, null);
				};
				s.Show();

				InitializeComponent();

				processes.RaiseListChangedEvents = true;
				Processes.DataSource = processes;
				Processes.DisplayMember = "ProcessName";
				System.Threading.Thread t = new System.Threading.Thread(ListUpdateThread);

				Shown += delegate { t.Start(); Process.EnterDebugMode(); IsInDebug = true; };
				FormClosing += delegate { t.Abort(); Process.LeaveDebugMode(); IsInDebug = false; };
			}
		}

		private void ProcessCmdArgs(string[] args)
		{
			// process command line args
			string action = String.Empty,
				   path = String.Empty,
				   exe = String.Empty,
				   param = String.Empty,
				   dll = "cGuard.dll";

			int pid = -1;

			for(int i = 0; i < args.Length; i++)
			{
				switch(args[i])
				{
					case "--pid": pid = Convert.ToInt32(args[i + 1]); i++; break;
					case "--dll": dll = args[i + 1]; i++; break;
					case "--path": path = args[i + 1]; i++; break;
					case "--exe": exe = args[i + 1]; i++; break;
					case "--params":
						// treat the rest of the command line as if it were params directly to d2
						string[] args2 = new string[args.Length - i - 1];
						Array.Copy(args, i + 1, args2, 0, args.Length - i - 1);
						param = " " + String.Join(" ", args2);
						i = args.Length;
						break;
					default: action = args[i].Substring(2); break;
				}
			}

			// copy over the specified path, exe, and dll if necessary
			if(!String.IsNullOrEmpty(path))
				D2Path = path;
			if(!String.IsNullOrEmpty(exe))
				D2Exe = exe;
			if(!String.IsNullOrEmpty(dll))
				D2BSDLL = dll;

			// merge the specified args with the official args
			D2Args = String.Join(" ", new string[] { D2Args, param });

			if((action == "start" && (String.IsNullOrEmpty(D2Path) || String.IsNullOrEmpty(D2Exe))))
			{
				// if the path or exe is empty, load settings
				if(String.IsNullOrEmpty(D2Path) || String.IsNullOrEmpty(D2Exe))
					LoadSettings(false);
				// if the path is still empty, die
				if(String.IsNullOrEmpty(D2Path))
				{
					Autoclosed = true;
					Console.WriteLine("-1");
					Close();
					return;
				}
			}

			if(!String.IsNullOrEmpty(action))
			{
				Autoclosed = true;
				int res = actions[action](pid);
				if(action == "start")
					Console.WriteLine(res);
				Close();
				return;
			}
		}

		private void ListUpdateThread()
		{
			while(true)
			{
				foreach(ProcessWrapper p in processes)
					p.Process.Refresh();

				foreach(Process p in Process.GetProcesses())
				{
					if(processes.Exists(x => p.Id == x.Process.Id))
						continue;

					if(IsD2Window(p))
					{
						ProcessWrapper pw = new ProcessWrapper(p);
						processes.Add(pw);
						p.Exited += delegate { processes.Remove(pw); };
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

		private static bool IsD2Window(Process p)
		{
			if(p == null)
				return false;
			string classname = GetLCClassName(p);
			string moduleName = "";
			try { moduleName = Path.GetFileName(p.MainModule.FileName).ToLowerInvariant(); } catch { }
			return !String.IsNullOrEmpty(classname) && classname == "diablo ii" &&
						(moduleName == "game.exe" || moduleName.Contains("d2loader") ||
						 moduleName.Contains("d2launcher"));
		}

		public static void SaveSettings(string path, string exe, string args, string dll)
		{
			string exeName = Path.Combine(Application.StartupPath, "D2BS.exe");
			Configuration config = ConfigurationManager.OpenExeConfiguration(exeName);
			config.AppSettings.Settings.Remove("D2Path");
			config.AppSettings.Settings.Remove("D2Exe");
			config.AppSettings.Settings.Remove("D2Args");
			config.AppSettings.Settings.Remove("D2BSDLL");
			config.AppSettings.Settings.Add("D2Path", path);
			config.AppSettings.Settings.Add("D2Exe", exe);
			config.AppSettings.Settings.Add("D2Args", args);
			config.AppSettings.Settings.Add("D2BSDLL", dll);
			config.Save(ConfigurationSaveMode.Full);
		}

		public static void LoadSettings(bool showError)
		{
			try
			{
				string exeName = Path.Combine(Application.StartupPath, "D2BS.exe");
				Configuration config = ConfigurationManager.OpenExeConfiguration(exeName);
				D2Path = config.AppSettings.Settings["D2Path"].Value;
				D2Exe = config.AppSettings.Settings["D2Exe"].Value;
				D2Args = config.AppSettings.Settings["D2Args"].Value;
				D2BSDLL = config.AppSettings.Settings["D2BSDLL"].Value;
				try {
					LoadDelay = Convert.ToInt32(config.AppSettings.Settings["LoadDelay"].Value);
				} catch { LoadDelay = 1000; }
			} catch {
				if(showError)
					MessageBox.Show("Configuration not found.", "D2BS");
			}
		}

		public static void SetSettings(string path, string exe, string args, string dll)
		{
			if(!String.IsNullOrEmpty(path))
				D2Path = path;
			if(!String.IsNullOrEmpty(exe))
				D2Exe = exe;
			if(!String.IsNullOrEmpty(args))
				D2Args = args;
			if(!String.IsNullOrEmpty(dll))
				D2BSDLL = dll;
		}

		public static void GetSettings(out string path, out string exe, out string args, out string dll)
		{
			try {
				string exeName = Path.Combine(Application.StartupPath, "D2BS.exe");
				Configuration config = ConfigurationManager.OpenExeConfiguration(exeName);
				path = config.AppSettings.Settings["D2Path"].Value;
				exe = config.AppSettings.Settings["D2Exe"].Value;
				args = config.AppSettings.Settings["D2Args"].Value;
				dll = config.AppSettings.Settings["D2BSDLL"].Value;
			} catch {
				path = exe = args = dll = null;
			}
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

		private static string GetLCClassName(Process p) { return PInvoke.User32.GetClassNameFromProcess(p).ToLowerInvariant(); }
		private static Process GetProcessById(int pid)
		{
			try { return Process.GetProcessById(pid); }
			catch(ArgumentException) { return null; }
		}

		private static int Start(int pid)
		{
			int id = Start();
			Inject(id);
			return id;
		}
		public static int Inject(int pid)
		{
			Process p = GetProcessById(pid);
			if(IsD2Window(p))
			{
				Attach(p);
				return pid;
			}
			return -1;
		}
		public static int Kill(int pid)
		{
			Process p = GetProcessById(pid);
			if(IsD2Window(p))
			{
				p.Kill();
				return pid;
			}
			return -1;
		}
		private static int Save(int pid)
		{
			SaveSettings(D2Path, D2Exe, D2Args, D2BSDLL);
			return -1;
		}

		private static bool Attach(Process p)
		{
			if(!IsInDebug)
				Process.EnterDebugMode();
			string js32 = Path.Combine(Application.StartupPath, "mozjs.dll"),
				   libnspr = Path.Combine(Application.StartupPath, "nspr4.dll"),
				   d2bs = Path.Combine(Application.StartupPath, D2BSDLL);
			return  File.Exists(libnspr) && File.Exists(js32) && File.Exists(d2bs) &&
					PInvoke.Kernel32.LoadRemoteLibrary(p, libnspr) &&
					PInvoke.Kernel32.LoadRemoteLibrary(p, js32) &&
					PInvoke.Kernel32.LoadRemoteLibrary(p, d2bs);
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

		public static int Start(string path, string exe, string param, string dll)
		{
			D2Path = path;
			D2Exe = exe;
			D2Args = param;
			D2BSDLL = dll;
			return Start();
		}

		public static int Start(params string[] args)
		{
			if(String.IsNullOrEmpty(D2Path) ||
			   String.IsNullOrEmpty(D2Exe) ||
			   String.IsNullOrEmpty(D2BSDLL))
				LoadSettings(false);
			D2Args += String.Join(" ", args);
			return Start();
		}

		private static int Start()
		{
			if(!File.Exists(Path.Combine(D2Path, D2Exe)) ||
			   !File.Exists(Path.Combine(Application.StartupPath, D2BSDLL)))
				return -1;

			ProcessStartInfo psi = new ProcessStartInfo(Path.Combine(D2Path, D2Exe), D2Args);
			psi.UseShellExecute = false;
			psi.WorkingDirectory = D2Path;
			Process p = Process.Start(psi);
			System.Threading.Thread.Sleep(LoadDelay);
			Process[] children = p.GetChildProcesses();
			if(children.Length > 0)
			{
				foreach(Process child in children)
					if(IsD2Window(child))
					{
						child.WaitForInputIdle();
						return child.Id;
					}
			}
			return p.Id;
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
			Options o = new Options(D2Path, D2Exe, D2Args, D2BSDLL);
			o.ShowDialog();
			LoadSettings(false);
		}

        private void Main_Load(object sender, EventArgs e)
        {

        }
	}

	class ProcessWrapper
	{
		public bool Loaded { get; set; }
		public Process Process { get; internal set; }
		public string ProcessName {
			get {
				bool exited = false;
				try { exited = Process.HasExited; } catch {}
				if(exited)
					return "Exited...";
				return Process.MainWindowTitle + " [" + Process.Id + "]" + (Loaded ? " *" : "");
			}
		}
		public ProcessWrapper(Process p) { Process = p; }
	}

	static class BindingListExtensions
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
	static class ProcessExtensions
	{
		public static Process[] GetChildProcesses(this Process process)
		{
			List<Process> children = new List<Process>();
			Process[] processes = Process.GetProcesses();
			foreach(Process p in processes)
			{
				if(PInvoke.NTDll.ProcessIsChildOf(process, p))
					children.Add(p);
			}
			return children.ToArray();
		}
	}
}
