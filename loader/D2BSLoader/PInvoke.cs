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
	public enum VirtualAllocExTypes
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
		StackSizeParamIsAReservation = 0x10000
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
		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern IntPtr OpenProcess(ProcessAccessFlags dwDesiredAccess, [MarshalAs(UnmanagedType.Bool)] bool bInheritHandle, uint dwProcessId);
		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, IntPtr lpThreadId);
		[DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
		private static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, VirtualAllocExTypes flAllocationType, PageAccessProtectionFlags flProtect);
		[DllImport("kernel32.dll", SetLastError = true)]
		[return: MarshalAs(UnmanagedType.Bool)]
		private static extern bool CloseHandle(IntPtr hObject);
		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out int lpNumberOfBytesWritten);
		[DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Ansi, ExactSpelling = true)]
		private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);
		[DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
		private static extern IntPtr GetModuleHandle(string lpModuleName);
		[DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
		private static extern UInt32 WaitForSingleObject(IntPtr hHandle, UInt32 dwMilliseconds);
		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern IntPtr LoadLibrary(string library);
		[DllImport("kernel32.dll", SetLastError = true)]
		[return: MarshalAs(UnmanagedType.Bool)]
		private static extern bool FreeLibrary(IntPtr hHandle);

		public static bool CallRemoteFunction(Process p, string module, string function, IntPtr param)
		{
			IntPtr moduleHandle = FindModuleHandle(p, module);
			if(moduleHandle == IntPtr.Zero)
				return false;

			IntPtr localHandle = LoadLibrary(module);
			IntPtr address = (IntPtr)(moduleHandle.ToInt32() + (GetProcAddress(localHandle, function).ToInt32() - localHandle.ToInt32()));
			FreeLibrary(localHandle);
			if(address != IntPtr.Zero)
			{
				IntPtr result = CreateRemoteThread(p, address, param, CreateThreadFlags.RunImmediately);
				if(result != IntPtr.Zero)
					WaitForSingleObject(result, UInt32.MaxValue);
				return result != IntPtr.Zero;
			}
			return false;
		}
		public static bool LoadRemoteLibrary(Process p, string module)
		{
			IntPtr moduleName = WriteObject(p, module);
			IntPtr address = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
			if(address != IntPtr.Zero)
			{
				IntPtr result = CreateRemoteThread(p, address, moduleName, CreateThreadFlags.RunImmediately);
				if(result != IntPtr.Zero)
					WaitForSingleObject(result, UInt32.MaxValue);
				return result != IntPtr.Zero;
			}
			return false;
			//return CallRemoteFunction(p, "kernel32.dll", "LoadLibraryA", moduleName);
		}
		public static bool UnloadRemoteLibrary(Process p, string module)
		{
			IntPtr moduleHandle = FindModuleHandle(p, module);
			IntPtr moduleName = WriteObject(p, moduleHandle);
			return CallRemoteFunction(p, "kernel32.dll", "FreeLibrary", moduleName);
		}
		public static IntPtr WriteObject(Process p, object data)
		{
			byte[] bytes = GetRawBytes(data);
			IntPtr address = VirtualAlloc(p, IntPtr.Zero, (uint)bytes.Length, VirtualAllocExTypes.Commit | VirtualAllocExTypes.Reserve, PageAccessProtectionFlags.ReadWrite);
			WriteProcessMemory(p, address, bytes);
			return address;
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
		public static IntPtr VirtualAlloc(Process p, IntPtr address, uint size, VirtualAllocExTypes type, PageAccessProtectionFlags flags)
		{
			IntPtr handle = GetProcessHandle(p, ProcessAccessFlags.VMOperation);
			IntPtr newaddress = VirtualAllocEx(handle, address, size, type, flags);
			if(newaddress == IntPtr.Zero)
				throw new Win32Exception(Marshal.GetLastWin32Error());
			CloseProcessHandle(handle);
			return newaddress;
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
		public static IntPtr CreateRemoteThread(Process p, IntPtr address, IntPtr param, CreateThreadFlags flags)
		{
			IntPtr handle = GetProcessHandle(p, ProcessAccessFlags.CreateThread | ProcessAccessFlags.QueryInformation | ProcessAccessFlags.VMOperation | ProcessAccessFlags.VMRead | ProcessAccessFlags.VMWrite);
			IntPtr thread = CreateRemoteThread(handle, IntPtr.Zero, (uint)0, address, param, (uint)flags, IntPtr.Zero);
			if(thread == IntPtr.Zero)
				throw new Win32Exception(Marshal.GetLastWin32Error());
			CloseProcessHandle(handle);
			return thread;
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