using System.Collections.Generic;
using System.IO;
using System;
using System.Diagnostics;
using System.Globalization;

namespace OffsetParser
{
	class Program
	{
		static void Main(string[] args)
		{
			File.WriteAllText(args[2], ParseTemplate(args[1], ParseFile(args[0])));
		}

		private static List<Interface> ParseFile(string file)
		{
			var interfaces = new List<Interface>();
			var currentInterface = new Interface("");

			string contents = File.ReadAllText(file);
			string[] parts = contents.Split('\r', '\n', '\t', ';', '(', ')', ',', ' ');
			for(var i = 0; i < parts.Length; i++)
			{
				var part = parts[i];
				if(String.IsNullOrEmpty(part)) continue;
				switch(part)
				{
					case "interface":
						Debug.Assert(parts[i+2] == "{");

						currentInterface = new Interface(parts[i + 1]);
						interfaces.Add(currentInterface);
						i += 2;
						break;
					case "function":
						// function `name`(`args`) is `option` returns `type` at `address`
						var function = new Function();

						function.Name = parts[i+1];
						function.Arguments = new List<KeyValuePair<string,string>>();
						int j = 2;
						for(; parts[i+j] != "is"; j+=2) {
							var type = parts[i+j];
							var argname = parts[i+j+1];
							if(!String.IsNullOrEmpty(type) && !String.IsNullOrEmpty(argname))
								function.Arguments.Add(new KeyValuePair<string,string>(type, argname));
							while(String.IsNullOrEmpty(parts[i+j+2])) j++;
						}
						function.Option = parts[i+j+1];
						function.Type = parts[i+j+3];
						function.Address = parts[i+j+5];

						currentInterface.Add(function);
						i += j + 5;
						break;
					case "global":
						// global `type` `name` is `option` at `address`
						Debug.Assert(parts[i+3] == "is" &&
									 parts[i+5] == "at");

						var global = new Global();

						global.Type = parts[i+1];
						global.Name = parts[i+2];
						global.Option = parts[i+4];
						global.Address = parts[i+6];

						currentInterface.Add(global);
						i += 6;
						break;
					case "hook":
						// hook `name` calls `name` at `address`
						Debug.Assert(parts[i+2] == "calls" &&
									 parts[i+4] == "at");

						var hook = new Hook();
						hook.Name = parts[i+1];
						hook.Callback = parts[i+3];
						hook.Address = parts[i+5];

						currentInterface.Add(hook);
						break;
					case "}":
						currentInterface = null;
						break;
				}
			}

			return interfaces;
		}

		private static string ParseTemplate(string template, List<Interface> interfaces)
		{
			var result = "";

			

			return result;
		}
	}

	public class Interface
	{
		private readonly List<Function> functions = new List<Function>();
		private readonly List<Global> globals = new List<Global>();
		private readonly List<Hook> hooks = new List<Hook>();

		public ICollection<Function> Modules { get { return functions; } }
		public ICollection<Global> Globals { get { return globals;  } }
		public ICollection<Hook> Hooks { get { return hooks; } }

		public string Name { get; set; }

		public Interface(string name) { Name = name; }
		public void Add(Function fn) { functions.Add(fn); }
		public void Add(Global global) { globals.Add(global); }
		public void Add(Hook hook) { hooks.Add(hook); }
	}

	public struct Function
	{
		public string Name;
		public List<KeyValuePair<string, string>> Arguments;
		public string Option;
		public string Type;
		public string Address;
	}
	public class Global
	{
		public string Name;
		public string Type;
		public string Option;
		public string Address;
	}
	public class Hook
	{
		public string Name;
		public string Callback;
		public string Address;
	}
}
