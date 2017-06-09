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
            FillFields();
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
                    MessageBox.Show("Attempted to open invalid file.", "Error",MessageBoxButtons.OK);
                    systems = new List<ParticleSystem>();
                    SetSystemComboBox();
                    FillFields();
                }
            }
        }

        private void FillFields()
        {
            int selectedSystem = SystemComboBox.SelectedIndex;

            if (selectedSystem >= 0 && selectedSystem < systems.Count)
            {
                FillFields(systems[selectedSystem]);
            }
            else
            {
                ClearFields();
            }
        }

        private void FillFields(ParticleSystem system)
        {
            SystemNameField.Text = system.Name;
            SystemIdLabel.Text = "ID: " + system.Id.ToString();
            EmitRateField.Value = system.EmissionRate;
            MaxParticleField.Value = system.MaxNumberOfParticles;
            LoopCheckbox.Checked = system.Loop;
            PersistantCheckbox.Checked = system.Persistant;
            LifetimeField.Value = (decimal)system.Lifetime;
            MaxEmittersField.Value = system.MaxEmitters;

            SetFieldsActiveRecursive(CommonDataPanel, true);
        }

        

        private void ClearFields()
        {
            SystemNameField.Text = "";
            SystemIdLabel.Text = "ID: ";
            EmitRateField.Value = 0;
            MaxParticleField.Value = 0;
            LoopCheckbox.Checked = true;
            PersistantCheckbox.Checked = true;
            LifetimeField.Value = 0;
            MaxEmittersField.Value = 0;
            SetFieldsActiveRecursive(CommonDataPanel, false);
        }

        private void SetFieldsActiveRecursive(Control control, bool enabled)
        {
            control.Enabled = enabled;

            foreach (var c in control.Controls.OfType<Control>())
            {
                SetFieldsActiveRecursive(c, enabled);
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

        private void SystemComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            FillFields();
        }

        private void File_NewToolStripMenuItem_Click(object sender, EventArgs e)
        {
            systems = new List<ParticleSystem>();
            SetSystemComboBox();
            FillFields();
        }

        private void CreateSystemButton_Click(object sender, EventArgs e)
        {
            systems.Add(new ParticleSystem());
            SetSystemComboBox();
            SystemComboBox.SelectedIndex = systems.Count - 1;
            FillFields();
        }

        private void SystemNameField_TextChanged(object sender, EventArgs e)
        {
            GetSelectedSystem().Name = (sender as TextBox).Text;
            SetSystemComboBox();
        }

        private ParticleSystem GetSelectedSystem()
        {
            ParticleSystem system;

            if(SystemComboBox.SelectedIndex >= 0 && SystemComboBox.SelectedIndex < systems.Count)
            {
                system = systems[SystemComboBox.SelectedIndex];
            }
            else
            {
                system = new ParticleSystem();
            }

            return system;
        }

        private void EmitRateField_ValueChanged(object sender, EventArgs e)
        {
            GetSelectedSystem().EmissionRate = (int)(sender as NumericUpDown).Value;
        }

        private void MaxParticleField_ValueChanged(object sender, EventArgs e)
        {
            GetSelectedSystem().MaxNumberOfParticles = (int) (sender as NumericUpDown).Value;
        }

        private void LifetimeField_ValueChanged(object sender, EventArgs e)
        {
            GetSelectedSystem().Lifetime = (float) (sender as NumericUpDown).Value;
        }

        private void MaxEmittersField_ValueChanged(object sender, EventArgs e)
        {
            GetSelectedSystem().MaxEmitters = (int) (sender as NumericUpDown).Value;
        }

        private void LoopCheckbox_CheckedChanged(object sender, EventArgs e)
        {
            GetSelectedSystem().Loop = (sender as CheckBox).Checked;
        }

        private void PersistantCheckbox_CheckedChanged(object sender, EventArgs e)
        {
            GetSelectedSystem().Persistant = (sender as CheckBox).Checked;
        }
    }
}
