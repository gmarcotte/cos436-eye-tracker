using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace EyeTrackerTester
{
    enum EyeTrackerTest
    {
        LookUp,
        LookLeft,
        LookRight,
        LookDown,
        Blink,
        WinkLeft,
        WinkRight,
        RaiseBrows,
        None
    }

    class Test
    {
        int numLookUp;
        int numLookLeft;
        int numLookRight;
        int numLookDown;
        int numBlink;
        int numWinkLeft;
        int numWinkRight;
        int numRaiseBrows;
        int numTests;
        public int numTestsLeft;
        public int numCorrect;
        public int numWrong;
        Random rand;
        public List<EyeTrackerTest> tests;
        public List<EyeTrackerTest> responses;
        public List<double> times;

        public Test(int numTests)
        {
            this.numTests = numTests;
            numTestsLeft = numTests * 8;
            numLookUp = numTests;
            numLookLeft = numTests;
            numLookRight = numTests;
            numLookDown = numTests;
            numBlink = numTests;
            numWinkLeft = numTests;
            numWinkRight = numTests;
            numRaiseBrows = numTests;

            numCorrect = 0;
            numWrong = 0;
            rand = new Random();

            tests = new List<EyeTrackerTest>();
            responses = new List<EyeTrackerTest>();
            times = new List<double>();
        }

        public EyeTrackerTest NextTest()
        {
            //randomly pick action to test
            while (numTestsLeft > 0)
            {
                double tempRand = rand.NextDouble()*8;
                if ((tempRand < 1.0) && (numLookUp > 0))
                {
                    numTestsLeft--;
                    numLookUp--;
                    return EyeTrackerTest.LookUp;
                }
                else if ((tempRand < 2.0) && (numLookLeft > 0))
                {
                    numTestsLeft--;
                    numLookLeft--;
                    return EyeTrackerTest.LookLeft;
                }
                else if ((tempRand < 3.0) && (numLookRight > 0))
                {
                    numTestsLeft--;
                    numLookRight--;
                    return EyeTrackerTest.LookRight;
                }
                else if ((tempRand < 4.0) && (numLookDown > 0))
                {
                    numTestsLeft--;
                    numLookDown--;
                    return EyeTrackerTest.LookDown;
                }
                else if ((tempRand < 5.0) && (numBlink > 0))
                {
                    numTestsLeft--;
                    numBlink--;
                    return EyeTrackerTest.Blink;
                }
                else if ((tempRand < 6.0) && (numWinkLeft > 0))
                {
                    numTestsLeft--;
                    numWinkLeft--;
                    return EyeTrackerTest.WinkLeft;
                }
                else if ((tempRand < 7.0) && (numWinkRight > 0))
                {
                    numTestsLeft--;
                    numWinkRight--;
                    return EyeTrackerTest.WinkRight;
                }
                else if ((tempRand < 8.0) && (numRaiseBrows > 0))
                {
                    numTestsLeft--;
                    numRaiseBrows--;
                    return EyeTrackerTest.RaiseBrows;
                }
                else
                    return EyeTrackerTest.None;
            }
            return EyeTrackerTest.None;
        }

        public String GetTestResults()
        {
            StringBuilder results = new StringBuilder();
            for (int i = 0; i < tests.Count; i++)
            {
                results.Append(tests[i]);
                results.Append(", ");
                results.Append(responses[i]);
                results.Append(", ");
                results.Append(times[i]);
                results.Append("\n");
            }
            StreamWriter writer = new StreamWriter("c:\\eyetrackresults.txt");
            writer.Write(results.ToString());
            writer.Close();

            return results.ToString();
        }

        public String GetTestResults2()
        {
            StringBuilder results = new StringBuilder();
            for (int i = 0; i < responses.Count; i++)
            {
                results.Append(responses[i]);
                results.Append(", ");
                results.Append(times[i]);
                results.Append("\n");
            }
            StreamWriter writer = new StreamWriter("c:\\eyetrackresults.txt");
            writer.Write(results.ToString());
            writer.Close();

            return results.ToString();
        }
    }
}
