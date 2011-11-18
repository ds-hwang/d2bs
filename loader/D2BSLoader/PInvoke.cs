namespace PInvoke
{
	using System;
	using System.IO;
	using System.Diagnostics;
	using System.Runtime.InteropServices;
	using System.ComponentModel;

	[Flags]
	public enum ProcessAccessFlags : uint
	{
		All = 0x001F0FFF,
		Terminate = 0x00000001,
		CreateThread = 0x00000002,
		VMOperation = 0x00000008,
		VMRead = 0x00000010,
		VMWrite = 0x00000020,
		DupHandle = 0x00000040,
		SetInformation = 0x00000200,
		QueryInformation = 0x00000400,
		Synchronize = 0x00100000
	}
	[Flags]
	public enum AllocationType
	{
		WriteMatchFlagReset = 0x00000001, // Win98 only
		Commit = 0x00001000,
		Reserve = 0x00002000,
		CommitOrReserve = 0x00003000,
		Decommit = 0x00004000,
		Release = 0x00008000,
		Free = 0x00010000,
		Public = 0x00020000,
		Mapped = 0x00040000,
		Reset = 0x00080000, // Win2K only
		TopDown = 0x00100000,
		WriteWatch = 0x00200000, // Win98 only
		Physical = 0x00400000, // Win2K only
		//4MBPages    = 0x80000000, // ??
		SecImage = 0x01000000,
		Image = SecImage
	}
	[Flags]
	public enum PageAccessProtectionFlags
	{
		NoAccess = 0x001,
		ReadOnly = 0x002,
		ReadWrite = 0x004,
		WriteCopy = 0x008,
		Execute = 0x010,
		ExecuteRead = 0x020,
		ExecuteReadWrite = 0x040,
		ExecuteWriteCopy = 0x080,
		Guard = 0x100,
		NoCache = 0x200,
		WriteCombine = 0x400
	}
	[Flags]
	public enum CreateThreadFlags
	{
		RunImmediately = 0x0,
		CreateSuspended = 0x4,
		StackSizeParamIsAReservation = 0x10000,
        // NOT SUPPORTED IN WINDOWS
        UseNtCreateThreadEx = 0x800000
	}
    [Flags]
    public enum ProcessCreationFlags : uint
    {
        BreakawayFromJob = 0x01000000,
        DefaultErrorMode = 0x04000000,
        NewConsole = 0x00000010,
        NewProcessGroup = 0x00000200,
        NoWindow = 0x08000000,
        ProtectedProcess = 0x00040000,
        PreserveCodeAuthzLevel = 0x02000000,
        SeparateWowVdm = 0x00000800,
        SharedWowVdm = 0x00001000,
        Suspended = 0x00000004,
        UnicodeEnvironment = 0x00000400,
        DebugOnlyThisProcess = 0x00000002,
        DebugProcess = 0x00000001,
        DetachedProcess = 0x00000008,
        ExtendedStartupInfo = 0x00080000,
        InheritParentAffinity = 0x00010000
    }
    public enum ThreadAccessFlags : uint
    {
        Terminate = 0x0001,
        SuspendResume = 0x0002,
        GetContext = 0x0008,
        SetContext = 0x0010,
        SetInformation = 0x0020,
        QueryInformation = 0x0040,
        SetThreadToken = 0x0080,
        Impersonate = 0x0100,
        DirectImpersonate = 0x0200,
        SetLimitedInformation = 0x0400,
        QueryLimitedInformation = 0x0800,
    }
	[Flags]
	public enum LoadLibraryFlags : uint
	{
		LoadAsDataFile = 0x00000002,
		DontResolveReferences = 0x00000001,
		LoadWithAlteredSeachPath = 0x00000008,
		IgnoreCodeAuthzLevel = 0x00000010,
		LoadAsExclusiveDataFile = 0x00000040
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct PROCESS_BASIC_INFORMATION
	{
		public int ExitStatus;
		public int PebBaseAddress;
		public int AffinityMask;
		public int BasePriority;
		public uint UniqueProcessId;
		public uint InheritedFromUniqueProcessId;
	}

	public static class NTDll
	{
		[StructLayout(LayoutKind.Sequential)]
		private struct NtCreateThreadExBuffer
		{
			public int Size;
			public ulong Unknown1;
			public ulong Unknown2;
			public IntPtr Unknown3;
			public ulong Unknown4;
			public ulong Unknown5;
			public ulong Unknown6;
			public IntPtr Unknown7;
			public ulong Unknown8;
		};

		public static IntPtr CreateRemoteThread(IntPtr address, IntPtr param, IntPtr handle)
		{
			NtCreateThreadExBuffer buff = new NtCreateThreadExBuffer();
			buff.Size = Marshal.SizeOf(buff);
			buff.Unknown1 = 0x10003;
			buff.Unknown2 = 0x8;
			buff.Unknown3 = Marshal.AllocHGlobal(4);
			buff.Unknown4 = 0;
			buff.Unknown5 = 0x10004;
			buff.Unknown6 = 4;
			buff.Unknown7 = Marshal.AllocHGlobal(4);
			buff.Unknown8 = 0;

			IntPtr hthread = IntPtr.Zero;
			IntPtr ntstatus = NtCreateThreadEx(out hthread, 0x1FFFFF, IntPtr.Zero, handle, address, param, false, 0, 0, 0, out buff);

			if(hthread == IntPtr.Zero)
				throw new Win32Exception(Marshal.GetLastWin32Error());
			else
				return hthread;
		}

		[DllImport("ntdll.dll")]
		private static extern int NtQueryInformationProcess(IntPtr hProcess, int processInformationClass, ref PROCESS_BASIC_INFORMATION processBasicInformation, uint processInformationLength, out uint returnLength);
		public static bool ProcessIsChildOf(Process parent, Process child)
		{
			PROCESS_BASIC_INFORMATION pbi = new PROCESS_BASIC_INFORMATION();
			try {
				uint bytesWritten;
				NtQueryInformationProcess(child.Handle, 0, ref pbi, (uint)Marshal.SizeOf(pbi), out bytesWritten);
				if(pbi.InheritedFromUniqueProcessId == parent.Id)
					return true;
			} catch { return false; }
			return false;
		}

		[DllImport("ntdll.dll", SetLastError = true, ExactSpelling = true)]
		private static extern IntPtr NtCreateThreadEx(out IntPtr outhThread,
			int inlpvDesiredAccess, IntPtr lpObjectAttributes, IntPtr inhProcessHandle,
			IntPtr lpStartAddress,
			IntPtr lpParameter, bool inCreateSuspended, 
			ulong inStackZeroBits, ulong inSizeOfStackCommit, ulong inSizeOfStackReserve, 
			[MarshalAs(UnmanagedType.Struct)] out NtCreateThreadExBuffer outlpvBytesBuffer);
	   
	}

	public static class User32
	{
		[DllImport("user32.dll", CharSet = CharSet.Auto)]
		private static extern int GetClassName(IntPtr hWnd, System.Text.StringBuilder lpClassName, int nMaxCount);

		public static string GetClassNameFromProcess(Process p)
		{
			System.Text.StringBuilder classname = new System.Text.StringBuilder(100);
			GetClassName(p.MainWindowHandle, classname, classname.Capacity);
			return classname.ToString();
		}
	}

	public static class Kernel32
	{

        private struct ProcessInformation
        {
            public IntPtr Process;
            public IntPtr Thread;
            public uint ProcessId;
            public uint ThreadId;
        }

        private struct StartupInfo
        {
            public uint CB;
            public string Reserved;
            public string Desktop;
            public string Title;
            public uint X;
            public uint Y;
            public uint XSize;
            public uint YSize;
            public uint XCountChars;
            public uint YCountChars;
            public uint FillAttribute;
            public uint Flags;
            public short ShowWindow;
            public short Reserved2;
            public IntPtr lpReserved2;
            public IntPtr StdInput;
            public IntPtr StdOutput;
            public IntPtr StdError;
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool CreateProcess(string Application, string CommandLine, IntPtr ProcessAttributes,
                            IntPtr ThreadAttributes, bool InheritHandles, uint CreationFlags, IntPtr Environment,
                            string CurrentDirectory, ref StartupInfo StartupInfo, out ProcessInformation ProcessInformation);

        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        private static extern uint SuspendThread(IntPtr hThread);

        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        private static extern uint ResumeThread(IntPtr hThread);
        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool VirtualProtectEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, PageAccessProtectionFlags flags, out PageAccessProtectionFlags oldFlags);
        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        private static extern bool ReadProcessMemory(IntPtr hProcess, IntPtr baseAddress, [Out] byte[] buffer, uint size, out uint numBytesRead);

		[DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
		private static extern IntPtr OpenProcess(ProcessAccessFlags dwDesiredAccess, [MarshalAs(UnmanagedType.Bool)] bool bInheritHandle, uint dwProcessId);
		[DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
		private static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, IntPtr lpThreadId);
		[DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
		private static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, AllocationType flAllocationType, PageAccessProtectionFlags flProtect);
		[DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
		[return: MarshalAs(UnmanagedType.Bool)]
		private static extern bool VirtualFreeEx(IntPtr hProcess, IntPtr lpAddress, int dwSize, AllocationType dwFreeType);
		[DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
		[return: MarshalAs(UnmanagedType.Bool)]
		private static extern bool CloseHandle(IntPtr hObject);
		[DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
		private static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out int lpNumberOfBytesWritten);
		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);
		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern IntPtr GetModuleHandle(string lpModuleName);
		[DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
		private static extern UInt32 WaitForSingleObject(IntPtr hHandle, UInt32 dwMilliseconds);
		[DllImport("kernel32.dll", SetLastError = true)]
		static extern IntPtr LoadLibraryEx(string lpFileName, IntPtr hFile, LoadLibraryFlags dwFlags);
		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern IntPtr LoadLibrary(string library);
		[DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
		[return: MarshalAs(UnmanagedType.Bool)]
		private static extern bool FreeLibrary(IntPtr hHandle);
        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        private static extern IntPtr OpenThread(ThreadAccessFlags flags, bool bInheritHandle, uint threadId);

        public static bool LoadRemoteLibrary(Process p, string module) 
        {
            IntPtr moduleName = WriteObject(p, module);
            bool result = CallRemoteFunction(p, "kernel32.dll", "LoadLibraryA", moduleName);
            FreeObject(p, moduleName);
            return result;
        }
		public static bool UnloadRemoteLibrary(Process p, string module)
		{
			IntPtr moduleHandle = FindModuleHandle(p, module);
			IntPtr moduleName = WriteObject(p, moduleHandle);
			bool result = CallRemoteFunction(p, "kernel32.dll", "FreeLibrary", moduleName);
			FreeObject(p, moduleName);
			return result;
		}

        public static bool CallRemoteFunction(Process p, string module, string function, IntPtr param)
        {
            return CallRemoteFunction(p.Id, module, function, param);
        }
        public static bool CallRemoteFunction(int pid, string module, string function, IntPtr param)
        {
            IntPtr moduleHandle = LoadLibraryEx(module, LoadLibraryFlags.LoadAsDataFile);
            IntPtr address = GetProcAddress(moduleHandle, function);
            if (moduleHandle == IntPtr.Zero || address == IntPtr.Zero)
                return false;

            IntPtr result = CreateRemoteThread(pid, address, param, CreateThreadFlags.RunImmediately);
            if (result != IntPtr.Zero)
                WaitForSingleObject(result, UInt32.MaxValue);

            return result != IntPtr.Zero;
        }
        public static bool ReadProcessMemory(Process p, IntPtr address, ref byte[] buffer) 
        {
            IntPtr handle = PInvoke.Kernel32.GetProcessHandle(p, PInvoke.ProcessAccessFlags.VMRead);
            uint length = 0;
            PageAccessProtectionFlags flags, oldFlags;
            if (!VirtualProtect(new IntPtr(p.Id), address, (uint)buffer.Length, PageAccessProtectionFlags.ExecuteReadWrite, out flags))
                throw new Win32Exception(Marshal.GetLastWin32Error());
            if (!ReadProcessMemory(handle, address, buffer, (uint)buffer.Length, out length))
                throw new Win32Exception(Marshal.GetLastWin32Error());
            if (!VirtualProtect(new IntPtr(p.Id), address, (uint)buffer.Length, flags, out oldFlags))
                throw new Win32Exception(Marshal.GetLastWin32Error());
            CloseProcessHandle(handle);
            return (length == buffer.Length);
        }
        public static IntPtr WriteObject(Process p, object data) 
        {
            byte[] bytes = GetRawBytes(data);
            IntPtr address = VirtualAlloc(p, IntPtr.Zero, (uint)bytes.Length, AllocationType.Commit | AllocationType.Reserve, PageAccessProtectionFlags.ReadWrite);
            WriteProcessMemory(p, address, bytes);
            return address;
        }
        public static void FreeObject(Process p, IntPtr address)
        {
            VirtualFree(p, address);
        }
		public static IntPtr FindModuleHandle(Process p, string module)
		{
			p.WaitForInputIdle();
			p.Refresh();
			foreach(ProcessModule m in p.Modules)
			{
				if(Path.GetFileName(m.FileName).ToLowerInvariant() == Path.GetFileName(module).ToLowerInvariant())
					return m.BaseAddress;
			}
			return IntPtr.Zero;
		}
		public static IntPtr FindOffset(string moduleName, string function)
		{
			IntPtr localHandle = LoadLibraryEx(moduleName, LoadLibraryFlags.LoadAsDataFile);
			if(localHandle != IntPtr.Zero)
			{
				IntPtr address = GetProcAddress(localHandle, function);
				FreeLibrary(localHandle);
				return (IntPtr)(address.ToInt32() - localHandle.ToInt32());
			}
			return IntPtr.Zero;
		}
        [DebuggerHidden]
        public static bool VirtualProtect(IntPtr pid, IntPtr address, uint size, PageAccessProtectionFlags flags, out PageAccessProtectionFlags oldFlags)
        {
            IntPtr handle = GetProcessHandle(pid, ProcessAccessFlags.VMOperation);
            bool result = VirtualProtectEx(handle, address, size, flags, out oldFlags);
            CloseHandle(handle);
            return result;
        }
        [DebuggerHidden]
        public static IntPtr GetProcessHandle(IntPtr pid, ProcessAccessFlags flags)
        {
            IntPtr handle = OpenProcess(flags, false, (uint)pid);
            if (handle == IntPtr.Zero)
                throw new Win32Exception(Marshal.GetLastWin32Error());
            return handle;
        }
       [DebuggerHidden]
		public static IntPtr LoadLibraryEx(string module, LoadLibraryFlags flags)
		{
			return LoadLibraryEx(module, IntPtr.Zero, flags);
		}
		[DebuggerHidden]
		public static IntPtr GetProcessHandle(Process p, ProcessAccessFlags flags)
		{
			IntPtr handle = OpenProcess(flags, false, (uint)p.Id);
			if(handle == IntPtr.Zero)
				throw new Win32Exception(Marshal.GetLastWin32Error());
			return handle;
		}
		[DebuggerHidden]
		public static void CloseProcessHandle(IntPtr handle)
		{
			if(!CloseHandle(handle))
				throw new Win32Exception(Marshal.GetLastWin32Error());
		}
		[DebuggerHidden]
		public static IntPtr VirtualAlloc(Process p, IntPtr address, uint size, AllocationType type, PageAccessProtectionFlags flags)
		{
			IntPtr handle = GetProcessHandle(p, ProcessAccessFlags.VMOperation);
			IntPtr newaddress = VirtualAllocEx(handle, address, size, type, flags);
			if(newaddress == IntPtr.Zero)
				throw new Win32Exception(Marshal.GetLastWin32Error());
			CloseProcessHandle(handle);
			return newaddress;
		}
		[DebuggerHidden]
		public static void VirtualFree(Process p, IntPtr address)
		{
			IntPtr handle = GetProcessHandle(p, ProcessAccessFlags.VMOperation);
			bool success = VirtualFreeEx(handle, address, 0, AllocationType.Release);
			CloseProcessHandle(handle);
			if(!success)
				throw new Win32Exception(Marshal.GetLastWin32Error());
		}
		[DebuggerHidden]
		public static int WriteProcessMemory(Process p, IntPtr address, byte[] buffer)
		{
			IntPtr handle = GetProcessHandle(p, ProcessAccessFlags.VMWrite | ProcessAccessFlags.VMOperation);
			int bytes;
			if(!WriteProcessMemory(handle, address, buffer, (uint)buffer.Length, out bytes))
				throw new Win32Exception(Marshal.GetLastWin32Error());
			CloseProcessHandle(handle);
			return bytes;
		}
        [DebuggerHidden]
        public static IntPtr CreateRemoteThread(Process p, IntPtr address, IntPtr param, CreateThreadFlags flags) { return CreateRemoteThread(p.Id, address, param, flags); }
        [DebuggerHidden]
        public static IntPtr CreateRemoteThread(int pid, IntPtr address, IntPtr param, CreateThreadFlags flags)
        {
            IntPtr handle = GetProcessHandle(new IntPtr(pid), ProcessAccessFlags.CreateThread | ProcessAccessFlags.QueryInformation | ProcessAccessFlags.VMOperation | ProcessAccessFlags.VMRead | ProcessAccessFlags.VMWrite);
            IntPtr thread = CreateRemoteThread(handle, IntPtr.Zero, (uint)0, address, param, (uint)flags, IntPtr.Zero);
            if (thread == IntPtr.Zero)
                throw new Win32Exception(Marshal.GetLastWin32Error());
            CloseProcessHandle(handle);
            return thread;
        }
        [DebuggerHidden]
        public static void SuspendThread(int tid)
        {
            IntPtr handle = OpenThread(ThreadAccessFlags.SuspendResume, false, (uint)tid);
            SuspendThread(handle);
            CloseHandle(handle);
        }
        [DebuggerHidden]
        public static void ResumeThread(int tid)
        {
            IntPtr handle = OpenThread(ThreadAccessFlags.SuspendResume, false, (uint)tid);
            ResumeThread(handle);
            CloseHandle(handle);
        }
		[DebuggerHidden]
		public static byte[] GetRawBytes(object anything)
		{
			if(anything.GetType().Equals(typeof(string)))
			{
				return System.Text.ASCIIEncoding.ASCII.GetBytes((string)anything);
			}
			else
			{
				int structsize = Marshal.SizeOf(anything);
				IntPtr buffer = Marshal.AllocHGlobal(structsize);
				Marshal.StructureToPtr(anything, buffer, false);
				byte[] streamdatas = new byte[structsize];
				Marshal.Copy(buffer, streamdatas, 0, structsize);
				Marshal.FreeHGlobal(buffer);
				return streamdatas;
			}
		}

        public static uint StartProcess(string directory, string application, ProcessCreationFlags flags, params string[] parameters)
        {
            StartupInfo si = new StartupInfo();
            ProcessInformation pi = new ProcessInformation();
            if (CreateProcess(application, String.Concat(application, String.Concat(parameters)), IntPtr.Zero, IntPtr.Zero, false, (uint)flags, IntPtr.Zero, directory, ref si, out pi))
                return pi.ProcessId;
            return uint.MaxValue;
        }

        public static Process StartSuspended( Process process, ProcessStartInfo startInfo)
        {
            uint id = Kernel32.StartProcess(startInfo.WorkingDirectory, startInfo.FileName, ProcessCreationFlags.Suspended, startInfo.Arguments);
            return Process.GetProcessById((int)id);
        }
        public static void Suspend( Process process)
        {
            foreach (ProcessThread t in process.Threads)
                PInvoke.Kernel32.SuspendThread(new IntPtr( t.Id));
               
        }

        public static void Resume( Process process)
        {
            foreach (ProcessThread t in process.Threads)
                PInvoke.Kernel32.ResumeThread(t.Id);
        }

        public static void Suspend(ProcessThread thread)
        {
            PInvoke.Kernel32.SuspendThread((new IntPtr(thread.Id)));
        }
        public static void Resume( ProcessThread thread)
        {
            PInvoke.Kernel32.ResumeThread((new IntPtr(thread.Id)));
        }
		/* currently not working because of compiler problems */
		/*[DebuggerHidden]
		public static T ConvertTo<T>(byte[] rawdatas)
		{
			if(typeof(T).Equals(typeof(string)))
			{
				string str = BitConverter.ToString(rawdatas);
				return (T)str; // Cannot convert type 'string' to 'T'
			}
			else
			{
				int rawsize = Marshal.SizeOf(typeof(T));
				if (rawsize > rawdatas.Length) return default(T);
				IntPtr buffer = Marshal.AllocHGlobal(rawsize);
				Marshal.Copy(rawdatas, 0, buffer, rawsize);
				T retobj = (T)Marshal.PtrToStructure(buffer, typeof(T));
				Marshal.FreeHGlobal(buffer);
				return retobj;
			}
		}*/
	}
}