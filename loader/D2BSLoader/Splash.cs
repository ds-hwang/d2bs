using System.Windows.Forms;
using System.Drawing.Imaging;
using System.Drawing;

namespace D2BSLoader
{
	public partial class Splash : Form
	{
		public Splash(Form parent)
		{
			InitializeComponent();
			parent.Opacity = 0f;
			Timer t = new Timer();
			t.Interval = 3000;
			t.Tick += delegate { Opacity = 0f; t.Stop(); Close(); parent.Opacity = 100f; };
			t.Start();
		}
        protected override void OnPaintBackground(PaintEventArgs e)
		{
			e.Graphics.DrawImage(global::D2BSLoader.Properties.Resources.splash, e.ClipRectangle);
		}
        protected override void OnPaint(PaintEventArgs e)
        {
            
        }

        private void Splash_Load(object sender, System.EventArgs e)
        {

        }
	}
}
