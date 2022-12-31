using System;
using System.Drawing.Printing;
using System.IO;
using System.Runtime.InteropServices;

namespace app.console
{
    internal class Prints
    {
        #region spool print

        #region imports

        [DllImport("winspool.drv", EntryPoint = "ClosePrinter", SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        public static extern bool ClosePrinter(IntPtr hPrinter);

        [DllImport("winspool.drv", EntryPoint = "EndDocPrinter", SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        public static extern bool EndDocPrinter(IntPtr hPrinter);

        [DllImport("winspool.drv", EntryPoint = "EndPagePrinter", SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        public static extern bool EndPagePrinter(IntPtr hPrinter);

        [DllImport("winspool.drv", EntryPoint = "OpenPrinterA", SetLastError = true, CharSet = CharSet.Ansi, ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        public static extern bool OpenPrinter([MarshalAs(UnmanagedType.LPStr)] string szPrinter, out IntPtr hPrinter, IntPtr pd);

        [DllImport("winspool.drv", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern Boolean SetDefaultPrinter(String name);

        [DllImport("winspool.drv", EntryPoint = "StartDocPrinterA", SetLastError = true, CharSet = CharSet.Ansi, ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        public static extern bool StartDocPrinter(IntPtr hPrinter, Int32 level, [In, MarshalAs(UnmanagedType.LPStruct)] DOCINFOA di);

        [DllImport("winspool.drv", EntryPoint = "StartPagePrinter", SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        public static extern bool StartPagePrinter(IntPtr hPrinter);

        [DllImport("winspool.drv", EntryPoint = "WritePrinter", SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        public static extern bool WritePrinter(IntPtr hPrinter, IntPtr pBytes, Int32 dwCount, out Int32 dwWritten);

        #endregion imports

        public static bool SendBytesToPrinter(string szPrinterName, IntPtr pBytes, Int32 dwCount)
        {
            Int32 dwError = 0, dwWritten = 0;
            IntPtr hPrinter = new IntPtr(0);
            DOCINFOA di = new DOCINFOA();
            bool bSuccess = false;

            di.pDocName = "Document";
            di.pDataType = "RAW";

            if (OpenPrinter(szPrinterName.Normalize(), out hPrinter, IntPtr.Zero))
            {
                if (hPrinter.ToInt32() == 0)
                    return false; //Printer not found!!

                if (StartDocPrinter(hPrinter, 1, di))
                {
                    if (StartPagePrinter(hPrinter))
                    {
                        bSuccess = WritePrinter(hPrinter, pBytes, dwCount, out dwWritten);
                        EndPagePrinter(hPrinter);
                    }
                    EndDocPrinter(hPrinter);
                }
                ClosePrinter(hPrinter);
            }

            if (bSuccess == false)
            {
                dwError = Marshal.GetLastWin32Error();
            }
            return bSuccess;
        }

        public static bool SendFileToPrinter(string szPrinterName, string szFileName)
        {
            bool bSuccess = false;
            try
            {
                FileStream fs = new FileStream(szFileName, FileMode.Open);

                BinaryReader br = new BinaryReader(fs);

                Byte[] bytes = new Byte[fs.Length];

                IntPtr pUnmanagedBytes = new IntPtr(0);
                int nLength;

                nLength = Convert.ToInt32(fs.Length);

                bytes = br.ReadBytes(nLength);

                pUnmanagedBytes = Marshal.AllocCoTaskMem(nLength);

                Marshal.Copy(bytes, 0, pUnmanagedBytes, nLength);

                bSuccess = SendBytesToPrinter(szPrinterName, pUnmanagedBytes, nLength);

                Marshal.FreeCoTaskMem(pUnmanagedBytes);

                br.Close();
                fs.Close();
            }
            catch (Exception ex)
            {
                string s = ex.Message.ToString();
            }
            return bSuccess;
        }

        private static void Print()
        {
            //var ps = new PrintServer(@"\\HP-ZBOOK-TRUBAC");
            //var pq = ps.GetPrintQueue("SEC001599B4FA0A");
            //var printDialog = new PrintDialog();
            //printDialog.PrintQueue = pq

            SendFileToPrinter("Microsoft XPS Document Writer", @"c:\elektlabs\masa\doc\System_Manuals\MASA_OData.pdf");

            //SendFileToPrinter("Microsoft Print to PDF", @"D:\elektlabs\masa\doc\System_Manuals\MASA_OData.pdf");

            //pq.AddJob("My job", pq.DefaultPrintTicket. @"D:\elektlabs\masa\doc\System_Manuals\MASA_OData.pdf", true);
        }

        #endregion spool print

        #region iron pdf print

        private static void PrintIron()
        {
            // Create a new PDF and print it
            IronPdf.HtmlToPdf Renderer = new IronPdf.HtmlToPdf();

            var pdf = IronPdf.PdfDocument.FromFile(@"c:\elektlabs\masa\doc\System_Manuals\MASA_OData.pdf");

            //PdfDocument Pdf = Renderer.RenderUrlAsPdf("https://www.nuget.org/packages/IronPdf");
            // Send the PDF to the default printer to print // .Print();
            using (var doc = pdf.GetPrintDocument())
            {
                var controller = new StandardPrintController();

                doc.PrinterSettings.PrinterName = "Microsoft XPS Document Writer";
                doc.PrinterSettings.Copies = 1;
                doc.PrintController = controller;
                doc.Print();
            }

            //For advanced silent real-world printing options, use  PdfDocument.GetPrintDocument
            //Remember to add an assembly reference to System.Drawing.dll
            //  System.Drawing.Printing.PrintDocument PrintDocYouCanWorkWith = pdf.GetPrintDocument();
        }

        #endregion iron pdf print
    }
}