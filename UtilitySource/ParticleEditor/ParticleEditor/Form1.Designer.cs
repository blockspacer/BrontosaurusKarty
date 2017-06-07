namespace ParticleEditor
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.MenuStrip = new System.Windows.Forms.MenuStrip();
            this.FileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.File_NewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.File_SaveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.File_SaveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.File_OpenToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.File_OpenQuitToolStripSeparator = new System.Windows.Forms.ToolStripSeparator();
            this.File_QuitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.StatusStrip = new System.Windows.Forms.StatusStrip();
            this.MainPanel = new System.Windows.Forms.Panel();
            this.SystemComboBox = new System.Windows.Forms.ComboBox();
            this.CreateSystemButton = new System.Windows.Forms.Button();
            this.panel2 = new System.Windows.Forms.Panel();
            this.CommonDataPanel = new System.Windows.Forms.Panel();
            this.SystemIdLabel = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.LoopCheckbox = new System.Windows.Forms.CheckBox();
            this.EmitRateField = new System.Windows.Forms.NumericUpDown();
            this.MaxParticleField = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.LifetimeField = new System.Windows.Forms.NumericUpDown();
            this.PersistantCheckbox = new System.Windows.Forms.CheckBox();
            this.label4 = new System.Windows.Forms.Label();
            this.MaxEmittersField = new System.Windows.Forms.NumericUpDown();
            this.SystemNameField = new System.Windows.Forms.TextBox();
            this.MenuStrip.SuspendLayout();
            this.MainPanel.SuspendLayout();
            this.panel2.SuspendLayout();
            this.CommonDataPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.EmitRateField)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.MaxParticleField)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.LifetimeField)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.MaxEmittersField)).BeginInit();
            this.SuspendLayout();
            // 
            // MenuStrip
            // 
            this.MenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.FileToolStripMenuItem});
            this.MenuStrip.Location = new System.Drawing.Point(0, 0);
            this.MenuStrip.Name = "MenuStrip";
            this.MenuStrip.Size = new System.Drawing.Size(1004, 24);
            this.MenuStrip.TabIndex = 0;
            this.MenuStrip.Text = "menuStrip1";
            // 
            // FileToolStripMenuItem
            // 
            this.FileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.File_NewToolStripMenuItem,
            this.File_OpenToolStripMenuItem,
            this.File_SaveToolStripMenuItem,
            this.File_SaveAsToolStripMenuItem,
            this.File_OpenQuitToolStripSeparator,
            this.File_QuitToolStripMenuItem});
            this.FileToolStripMenuItem.Name = "FileToolStripMenuItem";
            this.FileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.FileToolStripMenuItem.Text = "File";
            // 
            // File_NewToolStripMenuItem
            // 
            this.File_NewToolStripMenuItem.Name = "File_NewToolStripMenuItem";
            this.File_NewToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.File_NewToolStripMenuItem.Text = "New";
            this.File_NewToolStripMenuItem.Click += new System.EventHandler(this.File_NewToolStripMenuItem_Click);
            // 
            // File_SaveToolStripMenuItem
            // 
            this.File_SaveToolStripMenuItem.Name = "File_SaveToolStripMenuItem";
            this.File_SaveToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.File_SaveToolStripMenuItem.Text = "Save";
            // 
            // File_SaveAsToolStripMenuItem
            // 
            this.File_SaveAsToolStripMenuItem.Name = "File_SaveAsToolStripMenuItem";
            this.File_SaveAsToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.File_SaveAsToolStripMenuItem.Text = "Save as...";
            // 
            // File_OpenToolStripMenuItem
            // 
            this.File_OpenToolStripMenuItem.Name = "File_OpenToolStripMenuItem";
            this.File_OpenToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.File_OpenToolStripMenuItem.Text = "Open";
            this.File_OpenToolStripMenuItem.Click += new System.EventHandler(this.File_OpenToolStripMenuItem_Click);
            // 
            // File_OpenQuitToolStripSeparator
            // 
            this.File_OpenQuitToolStripSeparator.Name = "File_OpenQuitToolStripSeparator";
            this.File_OpenQuitToolStripSeparator.Size = new System.Drawing.Size(149, 6);
            // 
            // File_QuitToolStripMenuItem
            // 
            this.File_QuitToolStripMenuItem.Name = "File_QuitToolStripMenuItem";
            this.File_QuitToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.File_QuitToolStripMenuItem.Text = "Quit";
            // 
            // StatusStrip
            // 
            this.StatusStrip.Location = new System.Drawing.Point(0, 550);
            this.StatusStrip.Name = "StatusStrip";
            this.StatusStrip.Size = new System.Drawing.Size(1004, 22);
            this.StatusStrip.TabIndex = 2;
            this.StatusStrip.Text = "statusStrip1";
            // 
            // MainPanel
            // 
            this.MainPanel.Controls.Add(this.panel2);
            this.MainPanel.Location = new System.Drawing.Point(12, 27);
            this.MainPanel.Name = "MainPanel";
            this.MainPanel.Size = new System.Drawing.Size(980, 520);
            this.MainPanel.TabIndex = 3;
            // 
            // SystemComboBox
            // 
            this.SystemComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.SystemComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.SystemComboBox.FormattingEnabled = true;
            this.SystemComboBox.Location = new System.Drawing.Point(9, 3);
            this.SystemComboBox.Name = "SystemComboBox";
            this.SystemComboBox.Size = new System.Drawing.Size(262, 21);
            this.SystemComboBox.TabIndex = 0;
            this.SystemComboBox.SelectedIndexChanged += new System.EventHandler(this.SystemComboBox_SelectedIndexChanged);
            // 
            // CreateSystemButton
            // 
            this.CreateSystemButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.CreateSystemButton.Location = new System.Drawing.Point(277, 3);
            this.CreateSystemButton.Name = "CreateSystemButton";
            this.CreateSystemButton.Size = new System.Drawing.Size(75, 23);
            this.CreateSystemButton.TabIndex = 1;
            this.CreateSystemButton.Text = "Create";
            this.CreateSystemButton.UseVisualStyleBackColor = true;
            this.CreateSystemButton.Click += new System.EventHandler(this.CreateSystemButton_Click);
            // 
            // panel2
            // 
            this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.panel2.Controls.Add(this.CommonDataPanel);
            this.panel2.Controls.Add(this.SystemComboBox);
            this.panel2.Controls.Add(this.CreateSystemButton);
            this.panel2.Location = new System.Drawing.Point(3, 3);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(356, 514);
            this.panel2.TabIndex = 2;
            // 
            // CommonDataPanel
            // 
            this.CommonDataPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.CommonDataPanel.Controls.Add(this.SystemNameField);
            this.CommonDataPanel.Controls.Add(this.MaxEmittersField);
            this.CommonDataPanel.Controls.Add(this.label4);
            this.CommonDataPanel.Controls.Add(this.PersistantCheckbox);
            this.CommonDataPanel.Controls.Add(this.LifetimeField);
            this.CommonDataPanel.Controls.Add(this.label3);
            this.CommonDataPanel.Controls.Add(this.MaxParticleField);
            this.CommonDataPanel.Controls.Add(this.EmitRateField);
            this.CommonDataPanel.Controls.Add(this.LoopCheckbox);
            this.CommonDataPanel.Controls.Add(this.label2);
            this.CommonDataPanel.Controls.Add(this.label1);
            this.CommonDataPanel.Controls.Add(this.SystemIdLabel);
            this.CommonDataPanel.Location = new System.Drawing.Point(3, 32);
            this.CommonDataPanel.Name = "CommonDataPanel";
            this.CommonDataPanel.Size = new System.Drawing.Size(349, 479);
            this.CommonDataPanel.TabIndex = 2;
            // 
            // SystemIdLabel
            // 
            this.SystemIdLabel.AutoSize = true;
            this.SystemIdLabel.Location = new System.Drawing.Point(3, 0);
            this.SystemIdLabel.Name = "SystemIdLabel";
            this.SystemIdLabel.Size = new System.Drawing.Size(24, 13);
            this.SystemIdLabel.TabIndex = 0;
            this.SystemIdLabel.Text = "ID: ";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 48);
            this.label1.Margin = new System.Windows.Forms.Padding(3, 9, 3, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Emission Rate:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 70);
            this.label2.Margin = new System.Windows.Forms.Padding(3, 9, 3, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(125, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Max Number of Particles:";
            // 
            // LoopCheckbox
            // 
            this.LoopCheckbox.AutoSize = true;
            this.LoopCheckbox.Location = new System.Drawing.Point(6, 92);
            this.LoopCheckbox.Margin = new System.Windows.Forms.Padding(3, 9, 3, 3);
            this.LoopCheckbox.Name = "LoopCheckbox";
            this.LoopCheckbox.Size = new System.Drawing.Size(50, 17);
            this.LoopCheckbox.TabIndex = 3;
            this.LoopCheckbox.Text = "Loop";
            this.LoopCheckbox.UseVisualStyleBackColor = true;
            this.LoopCheckbox.CheckedChanged += new System.EventHandler(this.LoopCheckbox_CheckedChanged);
            // 
            // EmitRateField
            // 
            this.EmitRateField.Location = new System.Drawing.Point(86, 46);
            this.EmitRateField.Maximum = new decimal(new int[] {
            1000000000,
            0,
            0,
            0});
            this.EmitRateField.Name = "EmitRateField";
            this.EmitRateField.Size = new System.Drawing.Size(260, 20);
            this.EmitRateField.TabIndex = 4;
            this.EmitRateField.ValueChanged += new System.EventHandler(this.EmitRateField_ValueChanged);
            // 
            // MaxParticleField
            // 
            this.MaxParticleField.Location = new System.Drawing.Point(134, 68);
            this.MaxParticleField.Maximum = new decimal(new int[] {
            1000000000,
            0,
            0,
            0});
            this.MaxParticleField.Name = "MaxParticleField";
            this.MaxParticleField.Size = new System.Drawing.Size(212, 20);
            this.MaxParticleField.TabIndex = 5;
            this.MaxParticleField.ValueChanged += new System.EventHandler(this.MaxParticleField_ValueChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(83, 93);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(46, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Lifetime:";
            // 
            // LifetimeField
            // 
            this.LifetimeField.DecimalPlaces = 3;
            this.LifetimeField.Location = new System.Drawing.Point(134, 91);
            this.LifetimeField.Maximum = new decimal(new int[] {
            1000000,
            0,
            0,
            0});
            this.LifetimeField.Name = "LifetimeField";
            this.LifetimeField.Size = new System.Drawing.Size(212, 20);
            this.LifetimeField.TabIndex = 7;
            this.LifetimeField.ValueChanged += new System.EventHandler(this.LifetimeField_ValueChanged);
            // 
            // PersistantCheckbox
            // 
            this.PersistantCheckbox.AutoSize = true;
            this.PersistantCheckbox.Location = new System.Drawing.Point(6, 115);
            this.PersistantCheckbox.Name = "PersistantCheckbox";
            this.PersistantCheckbox.Size = new System.Drawing.Size(72, 17);
            this.PersistantCheckbox.TabIndex = 8;
            this.PersistantCheckbox.Text = "Persistant";
            this.PersistantCheckbox.UseVisualStyleBackColor = true;
            this.PersistantCheckbox.CheckedChanged += new System.EventHandler(this.PersistantCheckbox_CheckedChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(86, 115);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(70, 13);
            this.label4.TabIndex = 9;
            this.label4.Text = "Max Emitters:";
            // 
            // MaxEmittersField
            // 
            this.MaxEmittersField.Location = new System.Drawing.Point(162, 112);
            this.MaxEmittersField.Maximum = new decimal(new int[] {
            1000000,
            0,
            0,
            0});
            this.MaxEmittersField.Name = "MaxEmittersField";
            this.MaxEmittersField.Size = new System.Drawing.Size(184, 20);
            this.MaxEmittersField.TabIndex = 10;
            this.MaxEmittersField.ValueChanged += new System.EventHandler(this.MaxEmittersField_ValueChanged);
            // 
            // SystemNameField
            // 
            this.SystemNameField.Location = new System.Drawing.Point(3, 16);
            this.SystemNameField.Name = "SystemNameField";
            this.SystemNameField.Size = new System.Drawing.Size(340, 20);
            this.SystemNameField.TabIndex = 11;
            this.SystemNameField.TextChanged += new System.EventHandler(this.SystemNameField_TextChanged);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1004, 572);
            this.Controls.Add(this.MainPanel);
            this.Controls.Add(this.StatusStrip);
            this.Controls.Add(this.MenuStrip);
            this.MainMenuStrip = this.MenuStrip;
            this.MinimumSize = new System.Drawing.Size(100, 100);
            this.Name = "MainWindow";
            this.Text = "Particle Editor";
            this.MenuStrip.ResumeLayout(false);
            this.MenuStrip.PerformLayout();
            this.MainPanel.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.CommonDataPanel.ResumeLayout(false);
            this.CommonDataPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.EmitRateField)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.MaxParticleField)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.LifetimeField)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.MaxEmittersField)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip MenuStrip;
        private System.Windows.Forms.ToolStripMenuItem FileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem File_NewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem File_SaveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem File_SaveAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem File_OpenToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator File_OpenQuitToolStripSeparator;
        private System.Windows.Forms.ToolStripMenuItem File_QuitToolStripMenuItem;
        private System.Windows.Forms.StatusStrip StatusStrip;
        private System.Windows.Forms.Panel MainPanel;
        private System.Windows.Forms.Button CreateSystemButton;
        private System.Windows.Forms.ComboBox SystemComboBox;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel CommonDataPanel;
        private System.Windows.Forms.Label SystemIdLabel;
        private System.Windows.Forms.NumericUpDown MaxParticleField;
        private System.Windows.Forms.NumericUpDown EmitRateField;
        private System.Windows.Forms.CheckBox LoopCheckbox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown LifetimeField;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown MaxEmittersField;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.CheckBox PersistantCheckbox;
        private System.Windows.Forms.TextBox SystemNameField;
    }
}

