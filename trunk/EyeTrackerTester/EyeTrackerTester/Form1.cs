using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace EyeTrackerTester
{
    public partial class Form1 : Form
    {
        Test thisTest;
        EyeTrackerTest currentTest;
        EyeTrackerTest currentResponse;
        bool testType2 = false;
        System.DateTime startTime;

        public Form1()
        {
            InitializeComponent();
        }

        private void StartTest_Click(object sender, EventArgs e)
        {
            testType2 = false;
            timer1.Enabled = true;
            thisTest = new Test(int.Parse(numTests.Text));

            //get and set next test
            currentResponse = EyeTrackerTest.None;
            
            currentTest = thisTest.NextTest();
            thisTest.tests.Add(currentTest);
            SetTest(currentTest);
        }

        private void StartTest2_Click(object sender, EventArgs e)
        {
            thisTest = new Test(0);
            testType2 = true;
            startTime = System.DateTime.Now;
            timer2.Interval = int.Parse(numTests.Text) * 1000;
            timer2.Enabled = true;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (currentResponse != EyeTrackerTest.None)
            {
                thisTest.responses.Add(currentResponse);
                //evaluate current test
                if (currentTest == currentResponse)
                {
                    currentResponse = EyeTrackerTest.None;
                    MessageBox.Show("correct");
                    thisTest.numCorrect++;
                }
                else
                {
                    currentResponse = EyeTrackerTest.None;
                    MessageBox.Show("incorrect");
                    thisTest.numWrong++;
                }
                thisTest.times.Add(System.DateTime.Now.Subtract(startTime).TotalMilliseconds);

                //reset
                currentTest = EyeTrackerTest.None;
                currentResponse = EyeTrackerTest.None;
                LookUp.BackColor = Color.Empty;
                LookLeft.BackColor = Color.Empty;
                LookRight.BackColor = Color.Empty;
                LookDown.BackColor = Color.Empty;
                Blink.BackColor = Color.Empty;
                WinkLeft.BackColor = Color.Empty;
                WinkRight.BackColor = Color.Empty;
                RaiseBrows.BackColor = Color.Empty;

                //check if test is done
                if (thisTest.numTestsLeft <= 0)
                {
                    timer1.Enabled = false;
                    SetTest(currentTest);
                    MessageBox.Show("test complete");
                    MessageBox.Show(thisTest.GetTestResults());
                    return;
                }
                //get and set next test
                while (thisTest.numTestsLeft > 0 && currentTest == EyeTrackerTest.None)
                    currentTest = thisTest.NextTest();
                SetTest(currentTest);
                thisTest.tests.Add(currentTest);
            }
        }

        private void SetTest(EyeTrackerTest currentTest)
        {
            switch (currentTest)
            {
                case EyeTrackerTest.LookUp:
                    LookUp.BackColor = Color.Yellow;
                    break;
                case EyeTrackerTest.LookLeft:
                    LookLeft.BackColor = Color.Yellow;
                    break;
                case EyeTrackerTest.LookRight:
                    LookRight.BackColor = Color.Yellow;
                    break;
                case EyeTrackerTest.LookDown:
                    LookDown.BackColor = Color.Yellow;
                    break;
                case EyeTrackerTest.Blink:
                    Blink.BackColor = Color.Yellow;
                    break;
                case EyeTrackerTest.WinkLeft:
                    WinkLeft.BackColor = Color.Yellow;
                    break;
                case EyeTrackerTest.WinkRight:
                    WinkRight.BackColor = Color.Yellow;
                    break;
                case EyeTrackerTest.RaiseBrows:
                    RaiseBrows.BackColor = Color.Yellow;
                    break;
                default:
                    break;
            }

            numTestsRemain.Text = thisTest.numTestsLeft.ToString();
            numTestsCorrect.Text = thisTest.numCorrect.ToString();
            numTestsIncorrect.Text = thisTest.numWrong.ToString();
            startTime = System.DateTime.Now;
        }

        private void LookUp_Click(object sender, EventArgs e)
        {
            currentResponse = EyeTrackerTest.LookUp;
            if (testType2)
            {
                thisTest.responses.Add(currentResponse);
                thisTest.times.Add(System.DateTime.Now.Subtract(startTime).TotalMilliseconds);
            }
        }

        private void LookLeft_Click(object sender, EventArgs e)
        {
            currentResponse = EyeTrackerTest.LookLeft;
            if (testType2)
            {
                thisTest.responses.Add(currentResponse);
                thisTest.times.Add(System.DateTime.Now.Subtract(startTime).TotalMilliseconds);
            }
        }

        private void LookRight_Click(object sender, EventArgs e)
        {
            currentResponse = EyeTrackerTest.LookRight;
            if (testType2)
            {
                thisTest.responses.Add(currentResponse);
                thisTest.times.Add(System.DateTime.Now.Subtract(startTime).TotalMilliseconds);
            }
        }

        private void LookDown_Click(object sender, EventArgs e)
        {
            currentResponse = EyeTrackerTest.LookDown;
            if (testType2)
            {
                thisTest.responses.Add(currentResponse);
                thisTest.times.Add(System.DateTime.Now.Subtract(startTime).TotalMilliseconds);
            }
        }

        private void WinkLeft_Click(object sender, EventArgs e)
        {
            currentResponse = EyeTrackerTest.WinkLeft;
            if (testType2)
            {
                thisTest.responses.Add(currentResponse);
                thisTest.times.Add(System.DateTime.Now.Subtract(startTime).TotalMilliseconds);
            }
        }

        private void Blink_Click(object sender, EventArgs e)
        {
            currentResponse = EyeTrackerTest.Blink;
            if (testType2)
            {
                thisTest.responses.Add(currentResponse);
                thisTest.times.Add(System.DateTime.Now.Subtract(startTime).TotalMilliseconds);
            }
        }

        private void WinkRight_Click(object sender, EventArgs e)
        {
            currentResponse = EyeTrackerTest.WinkRight;
            if (testType2)
            {
                thisTest.responses.Add(currentResponse);
                thisTest.times.Add(System.DateTime.Now.Subtract(startTime).TotalMilliseconds);
            }
        }

        private void RaiseBrows_Click(object sender, EventArgs e)
        {
            currentResponse = EyeTrackerTest.RaiseBrows;
            if (testType2)
            {
                thisTest.responses.Add(currentResponse);
                thisTest.times.Add(System.DateTime.Now.Subtract(startTime).TotalMilliseconds);
            }
        }

        private void KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.W + 32)
                LookUp_Click(sender, e);
            if (e.KeyChar == (char)Keys.A + 32)
                LookLeft_Click(sender, e);
            if (e.KeyChar == (char)Keys.D + 32)
                LookRight_Click(sender, e);
            if (e.KeyChar == (char)Keys.S + 32)
                LookDown_Click(sender, e);
            if (e.KeyChar == (char)Keys.Z + 32)
                WinkLeft_Click(sender, e);
            if (e.KeyChar == (char)Keys.X + 32)
                Blink_Click(sender, e);
            if (e.KeyChar == (char)Keys.C + 32)
                WinkRight_Click(sender, e);
            if (e.KeyChar == (char)Keys.Q + 32)
                RaiseBrows_Click(sender, e);
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            timer2.Enabled = false;
            testType2 = false;
            MessageBox.Show("test complete");
            MessageBox.Show(thisTest.GetTestResults2());
        }



    }
}
