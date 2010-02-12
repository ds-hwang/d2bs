using System.Windows.Forms;
using System.Drawing.Imaging;
using System.Drawing;
using System.Diagnostics;

namespace D2BSLoader
{
	public partial class Splash : Form
	{
		Bitmap splashImage = D2BSLoader.Properties.Resources.splash;
		public Splash(Form parent)
		{
			InitializeComponent();
			try {
				FileVersionInfo ver = FileVersionInfo.GetVersionInfo(System.IO.Path.Combine(Application.StartupPath, "D2BS.bin"));
				VersionLabel.Text = "version " + ver.ProductMajorPart + "." + ver.ProductMinorPart +
								(ver.ProductBuildPart != 0 ? "." + ver.ProductBuildPart : "");
			} catch {}
			parent.Opacity = 0f;
			Timer t = new Timer();
			t.Interval = 3000;
			t.Tick += delegate { Opacity = 0f; t.Stop(); Close(); parent.Opacity = 100f; };
			t.Start();
		}
		protected override void OnPaintBackground(PaintEventArgs e)
		{
			e.Graphics.DrawImage(splashImage, e.ClipRectangle);
		}
        protected override void OnPaint(PaintEventArgs e) { }
        private void Splash_Load(object sender, System.EventArgs e) { }
	}
}
