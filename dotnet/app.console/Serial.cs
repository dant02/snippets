using System;
using System.IO.Ports;

namespace app.console
{
    internal class Serial
    {
        // Create the serial port with basic settings
        private static SerialPort port;

        public void Go(string[] args)
        {
            if (args.Length == 0)
            {
                throw new ArgumentNullException("missing 1st argument COM port name");
            }

            port = new SerialPort(args[0],
                9600, Parity.None, 8, StopBits.One);

            // Instatiate this
            SerialPortProgram();
        }

        private static void port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            // Show all the incoming data in the port's buffer
            Console.WriteLine(port.ReadExisting());
        }

        private static void SerialPortProgram()
        {
            Console.WriteLine("Incoming Data:");
            // Attach a method to be called when there
            // is data waiting in the port's buffer
            port.DataReceived += new SerialDataReceivedEventHandler(port_DataReceived);
            // Begin communications
            port.Open();

            // Enter an application loop to keep this thread alive
            while (true)
            {
                var txt = Console.ReadLine();

                if (txt == "quit")
                {
                    break;
                }

                port.WriteLine(txt);
            }
        }
    }
}