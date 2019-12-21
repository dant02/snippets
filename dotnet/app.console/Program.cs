using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using FirebirdSql.Data.FirebirdClient;

namespace app.console
{
    internal class Program
    {
        #region firebird

        private static readonly string fb2 = @"Database='c:\fbdata\netprovider_tests_default_false.fdb';DataSource=localhost;User=sysdba;Password=masterkey;Dialect=3;Charset=UTF8;Pooling=true;MinPoolSize=0;MaxPoolSize=100;Connection lifetime=30;";

        private static readonly string query = @"
EXECUTE BLOCK
RETURNS (
  c0 CHAR,
  c1 CHAR,
  c2 CHAR,
  c3 CHAR,
  s0 SMALLINT,
  s1 SMALLINT,
  s2 SMALLINT,
  s3 SMALLINT,
  i0 INTEGER,
  i1 INTEGER,
  i2 INTEGER,
  i3 INTEGER,
  l0 BIGINT,
  l1 BIGINT,
  l2 BIGINT,
  l3 BIGINT
)
AS
DECLARE VARIABLE x0 SMALLINT = -32768;
BEGIN
  WHILE (x0 < 32767) DO
  BEGIN
     s0 = x0;
     s1 = (x0 + 1) * -1;
     SUSPEND;
     x0 = x0 + 1;
  END
END ";

        // test app to show difference in performance, related to https://github.com/cincuranet/FirebirdSql.Data.FirebirdClient/pull/84
        private static void FBd()
        {
            string s = null;

            using (var ms = new MemoryStream())
            using (var sr = new StreamReader(ms, Encoding.Unicode))
                s = sr.ReadToEnd();

            var sw = new Stopwatch();

            using (var conn = new FbConnection(fb2))
            {
                conn.Open();

                using (var tr = conn.BeginTransaction())
                using (var cmd = new FbCommand(query, conn, tr))
                {
                    sw.Start();

                    for (int i = 0; i < 100; i++)
                    {
                        using (var rdr = cmd.ExecuteReader())
                            while (rdr.Read())
                            { }
                    }

                    sw.Stop();

                    tr.Commit();
                }
            }

            Console.WriteLine($"Elapsed: {sw.Elapsed}");
            Console.ReadLine();
        }

        #endregion firebird

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

            SendFileToPrinter("SEC001599B4FA0A", @"D:\elektlabs\masa\doc\System_Manuals\MASA_OData.pdf");

            //SendFileToPrinter("Microsoft Print to PDF", @"D:\elektlabs\masa\doc\System_Manuals\MASA_OData.pdf");

            //pq.AddJob("My job", pq.DefaultPrintTicket. @"D:\elektlabs\masa\doc\System_Manuals\MASA_OData.pdf", true);
        }

        #endregion spool print

        private static void Main(string[] args)
        {
            Print();
        }
    }
}