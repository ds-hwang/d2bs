using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace D2BSLoader
{
	public partial class Splash : Form
	{
		public Splash()
		{
			InitializeComponent();
			Timer t = new Timer();
			t.Interval = 3000;
			t.Tick += delegate { t.Stop(); Close(); };
			t.Start();
		}
	}
}
