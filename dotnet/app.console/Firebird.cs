﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FirebirdSql.Data.FirebirdClient;

namespace app.console
{
    internal class Firebird
    {
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

    }
}
