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
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace ParticleEditor
{
    public partial class MainWindow : Form
    {
        private List<ParticleSystem> systems = new List<ParticleSystem>();
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

                try
                {
                    string json = File.ReadAllText(currentDocument);
                    systems = LoadSystems(json);

                    SetSystemComboBox();
                }
                catch (Exception exception)
                {
                    Console.WriteLine(exception);
                    throw;
                }
            }
        }

        private void SetSystemComboBox()
        {
            int prevSel = SystemComboBox.SelectedIndex;
            
            SystemComboBox.Items.Clear();

            foreach (ParticleSystem particleSystem in systems)
            {
                SystemComboBox.Items.Add(particleSystem.Name);
            }

            if (SystemComboBox.Items.Count > 0)
            {
                SystemComboBox.SelectedIndex = (prevSel < SystemComboBox.Items.Count
                    ? (prevSel != -1 ? prevSel : 0)
                    : SystemComboBox.Items.Count - 1);
            }
           
        }

        private List<ParticleSystem> LoadSystems(string json)
        {
            List<ParticleSystem> sys = new List<ParticleSystem>();
            
            JObject data = JObject.Parse(json);

            JArray arrdata = (JArray)data["systems"];

            foreach (JToken jToken in arrdata)
            {
                ParticleSystem s = new ParticleSystem();

                s.Deserialize(jToken);

                sys.Add(s);
            }

            return sys;
        }
    }
}
