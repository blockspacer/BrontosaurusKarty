using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ParticleEditor
{
    public partial class MainWindow : Form
    {
        private string currentDocument = "";
        public MainWindow()
        {
            InitializeComponent();
        }

        private void File_OpenToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.Multiselect = false;

            dialog.Filter = "JSON | *.json";
            try
            {
                dialog.InitialDirectory = Path.GetDirectoryName(currentDocument);
            }
            catch (Exception exception)
            {
            }

            if (dialog.ShowDialog() == DialogResult.OK)
            {
                currentDocument = dialog.FileName;
            }
        }
    }
}
