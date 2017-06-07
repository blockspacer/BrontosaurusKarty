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
            this.panel1 = new System.Windows.Forms.Panel();
            this.SystemComboBox = new System.Windows.Forms.ComboBox();
            this.CreateSystemButton = new System.Windows.Forms.Button();
            this.panel2 = new System.Windows.Forms.Panel();
            this.MenuStrip.SuspendLayout();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
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
            // panel1
            // 
            this.panel1.Controls.Add(this.panel2);
            this.panel1.Location = new System.Drawing.Point(12, 27);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(980, 520);
            this.panel1.TabIndex = 3;
            // 
            // SystemComboBox
            // 
            this.SystemComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.SystemComboBox.FormattingEnabled = true;
            this.SystemComboBox.Location = new System.Drawing.Point(3, 3);
            this.SystemComboBox.Name = "SystemComboBox";
            this.SystemComboBox.Size = new System.Drawing.Size(202, 21);
            this.SystemComboBox.TabIndex = 0;
            // 
            // CreateSystemButton
            // 
            this.CreateSystemButton.Location = new System.Drawing.Point(211, 3);
            this.CreateSystemButton.Name = "CreateSystemButton";
            this.CreateSystemButton.Size = new System.Drawing.Size(75, 23);
            this.CreateSystemButton.TabIndex = 1;
            this.CreateSystemButton.Text = "Create";
            this.CreateSystemButton.UseVisualStyleBackColor = true;
            // 
            // panel2
            // 
            this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.panel2.Controls.Add(this.SystemComboBox);
            this.panel2.Controls.Add(this.CreateSystemButton);
            this.panel2.Location = new System.Drawing.Point(3, 3);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(290, 514);
            this.panel2.TabIndex = 2;
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1004, 572);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.StatusStrip);
            this.Controls.Add(this.MenuStrip);
            this.MainMenuStrip = this.MenuStrip;
            this.MinimumSize = new System.Drawing.Size(100, 100);
            this.Name = "MainWindow";
            this.Text = "Particle Editor";
            this.MenuStrip.ResumeLayout(false);
            this.MenuStrip.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
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
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button CreateSystemButton;
        private System.Windows.Forms.ComboBox SystemComboBox;
        private System.Windows.Forms.Panel panel2;
    }
}

