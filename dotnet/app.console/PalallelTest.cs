using System;

namespace app.console
{
    internal class PalallelTest
    {
        private void Calculate()
        {
            var r5 = SimulateCalculation.Calculate(5);
            var r6 = SimulateCalculation.Calculate(6);
            var r7 = SimulateCalculation.Calculate(7);

            Console.WriteLine(r5 + r6 + r7);
        }
    }

    internal class SimulateCalculation
    {
        public static double Calculate(double x)
        {
            var ticks = Environment.TickCount;
            for (int i = 0; i < 100_000_000; i++)
            {
                x = 1 + Math.Log10(x);
            }

            Console.WriteLine(Environment.TickCount - ticks + " calculated: " + x);
            return x;
        }
    }
}