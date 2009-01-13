namespace EyeTrackerTester
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.LookUp = new System.Windows.Forms.Button();
            this.LookRight = new System.Windows.Forms.Button();
            this.LookLeft = new System.Windows.Forms.Button();
            this.LookDown = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.numTests = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.StartTest = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.numTestsRemain = new System.Windows.Forms.Label();
            this.numTestsCorrect = new System.Windows.Forms.Label();
            this.numTestsIncorrect = new System.Windows.Forms.Label();
            this.Blink = new System.Windows.Forms.Button();
            this.WinkLeft = new System.Windows.Forms.Button();
            this.WinkRight = new System.Windows.Forms.Button();
            this.RaiseBrows = new System.Windows.Forms.Button();
            this.StartTest2 = new System.Windows.Forms.Button();
            this.timer2 = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // LookUp
            // 
            this.LookUp.AccessibleName = "";
            this.LookUp.Location = new System.Drawing.Point(100, 50);
            this.LookUp.Name = "LookUp";
            this.LookUp.Size = new System.Drawing.Size(50, 50);
            this.LookUp.TabIndex = 0;
            this.LookUp.Text = "Up";
            this.LookUp.UseVisualStyleBackColor = true;
            this.LookUp.Click += new System.EventHandler(this.LookUp_Click);
            this.LookUp.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPress);
            // 
            // LookRight
            // 
            this.LookRight.AccessibleName = "";
            this.LookRight.Location = new System.Drawing.Point(160, 110);
            this.LookRight.Name = "LookRight";
            this.LookRight.Size = new System.Drawing.Size(50, 50);
            this.LookRight.TabIndex = 1;
            this.LookRight.Text = "Right";
            this.LookRight.UseVisualStyleBackColor = true;
            this.LookRight.Click += new System.EventHandler(this.LookRight_Click);
            this.LookRight.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPress);
            // 
            // LookLeft
            // 
            this.LookLeft.AccessibleName = "";
            this.LookLeft.Location = new System.Drawing.Point(40, 110);
            this.LookLeft.Name = "LookLeft";
            this.LookLeft.Size = new System.Drawing.Size(50, 50);
            this.LookLeft.TabIndex = 2;
            this.LookLeft.Text = "Left";
            this.LookLeft.UseVisualStyleBackColor = true;
            this.LookLeft.Click += new System.EventHandler(this.LookLeft_Click);
            this.LookLeft.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPress);
            // 
            // LookDown
            // 
            this.LookDown.AccessibleName = "";
            this.LookDown.Location = new System.Drawing.Point(100, 170);
            this.LookDown.Name = "LookDown";
            this.LookDown.Size = new System.Drawing.Size(50, 50);
            this.LookDown.TabIndex = 3;
            this.LookDown.Text = "Down";
            this.LookDown.UseVisualStyleBackColor = true;
            this.LookDown.Click += new System.EventHandler(this.LookDown_Click);
            this.LookDown.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPress);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(260, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(182, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Number of Tests / Length of Test (s):";
            // 
            // numTests
            // 
            this.numTests.Location = new System.Drawing.Point(448, 32);
            this.numTests.Name = "numTests";
            this.numTests.Size = new System.Drawing.Size(34, 20);
            this.numTests.TabIndex = 5;
            this.numTests.Text = "10";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(260, 55);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(141, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Number of Tests Remaining:";
            // 
            // StartTest
            // 
            this.StartTest.AccessibleName = "";
            this.StartTest.Location = new System.Drawing.Point(263, 211);
            this.StartTest.Name = "StartTest";
            this.StartTest.Size = new System.Drawing.Size(121, 50);
            this.StartTest.TabIndex = 7;
            this.StartTest.Text = "Start Test";
            this.StartTest.UseVisualStyleBackColor = true;
            this.StartTest.Click += new System.EventHandler(this.StartTest_Click);
            this.StartTest.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPress);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(260, 70);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(84, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Number Correct:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(260, 85);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(92, 13);
            this.label4.TabIndex = 9;
            this.label4.Text = "Number Incorrect:";
            // 
            // numTestsRemain
            // 
            this.numTestsRemain.AutoSize = true;
            this.numTestsRemain.Location = new System.Drawing.Point(407, 55);
            this.numTestsRemain.Name = "numTestsRemain";
            this.numTestsRemain.Size = new System.Drawing.Size(10, 13);
            this.numTestsRemain.TabIndex = 10;
            this.numTestsRemain.Text = "-";
            // 
            // numTestsCorrect
            // 
            this.numTestsCorrect.AutoSize = true;
            this.numTestsCorrect.Location = new System.Drawing.Point(407, 70);
            this.numTestsCorrect.Name = "numTestsCorrect";
            this.numTestsCorrect.Size = new System.Drawing.Size(10, 13);
            this.numTestsCorrect.TabIndex = 11;
            this.numTestsCorrect.Text = "-";
            // 
            // numTestsIncorrect
            // 
            this.numTestsIncorrect.AutoSize = true;
            this.numTestsIncorrect.Location = new System.Drawing.Point(407, 85);
            this.numTestsIncorrect.Name = "numTestsIncorrect";
            this.numTestsIncorrect.Size = new System.Drawing.Size(10, 13);
            this.numTestsIncorrect.TabIndex = 12;
            this.numTestsIncorrect.Text = "-";
            // 
            // Blink
            // 
            this.Blink.AccessibleName = "";
            this.Blink.Location = new System.Drawing.Point(100, 230);
            this.Blink.Name = "Blink";
            this.Blink.Size = new System.Drawing.Size(50, 50);
            this.Blink.TabIndex = 14;
            this.Blink.Text = "Blink";
            this.Blink.UseVisualStyleBackColor = true;
            this.Blink.Click += new System.EventHandler(this.Blink_Click);
            this.Blink.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPress);
            // 
            // WinkLeft
            // 
            this.WinkLeft.AccessibleName = "";
            this.WinkLeft.Location = new System.Drawing.Point(40, 230);
            this.WinkLeft.Name = "WinkLeft";
            this.WinkLeft.Size = new System.Drawing.Size(50, 50);
            this.WinkLeft.TabIndex = 13;
            this.WinkLeft.Text = "Wink Left";
            this.WinkLeft.UseVisualStyleBackColor = true;
            this.WinkLeft.Click += new System.EventHandler(this.WinkLeft_Click);
            this.WinkLeft.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPress);
            // 
            // WinkRight
            // 
            this.WinkRight.AccessibleName = "";
            this.WinkRight.Location = new System.Drawing.Point(160, 230);
            this.WinkRight.Name = "WinkRight";
            this.WinkRight.Size = new System.Drawing.Size(50, 50);
            this.WinkRight.TabIndex = 15;
            this.WinkRight.Text = "Wink Right";
            this.WinkRight.UseVisualStyleBackColor = true;
            this.WinkRight.Click += new System.EventHandler(this.WinkRight_Click);
            this.WinkRight.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPress);
            // 
            // RaiseBrows
            // 
            this.RaiseBrows.AccessibleName = "";
            this.RaiseBrows.Location = new System.Drawing.Point(100, 290);
            this.RaiseBrows.Name = "RaiseBrows";
            this.RaiseBrows.Size = new System.Drawing.Size(50, 50);
            this.RaiseBrows.TabIndex = 16;
            this.RaiseBrows.Text = "Raise Brows";
            this.RaiseBrows.UseVisualStyleBackColor = true;
            this.RaiseBrows.Click += new System.EventHandler(this.RaiseBrows_Click);
            this.RaiseBrows.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPress);
            // 
            // StartTest2
            // 
            this.StartTest2.AccessibleName = "";
            this.StartTest2.Location = new System.Drawing.Point(263, 278);
            this.StartTest2.Name = "StartTest2";
            this.StartTest2.Size = new System.Drawing.Size(121, 50);
            this.StartTest2.TabIndex = 17;
            this.StartTest2.Text = "Start Test 2";
            this.StartTest2.UseVisualStyleBackColor = true;
            this.StartTest2.Click += new System.EventHandler(this.StartTest2_Click);
            this.StartTest2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPress);
            // 
            // timer2
            // 
            this.timer2.Tick += new System.EventHandler(this.timer2_Tick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(508, 398);
            this.Controls.Add(this.StartTest2);
            this.Controls.Add(this.RaiseBrows);
            this.Controls.Add(this.WinkRight);
            this.Controls.Add(this.Blink);
            this.Controls.Add(this.WinkLeft);
            this.Controls.Add(this.numTestsIncorrect);
            this.Controls.Add(this.numTestsCorrect);
            this.Controls.Add(this.numTestsRemain);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.StartTest);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.numTests);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.LookDown);
            this.Controls.Add(this.LookLeft);
            this.Controls.Add(this.LookRight);
            this.Controls.Add(this.LookUp);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button LookUp;
        private System.Windows.Forms.Button LookRight;
        private System.Windows.Forms.Button LookLeft;
        private System.Windows.Forms.Button LookDown;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox numTests;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button StartTest;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label numTestsRemain;
        private System.Windows.Forms.Label numTestsCorrect;
        private System.Windows.Forms.Label numTestsIncorrect;
        private System.Windows.Forms.Button Blink;
        private System.Windows.Forms.Button WinkLeft;
        private System.Windows.Forms.Button WinkRight;
        private System.Windows.Forms.Button RaiseBrows;
        private System.Windows.Forms.Button StartTest2;
        private System.Windows.Forms.Timer timer2;

    }
}

