﻿using System.Runtime.InteropServices;

namespace app.console
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public class DOCINFOA
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string pDocName;

        [MarshalAs(UnmanagedType.LPStr)]
        public string pOutputFile;

        [MarshalAs(UnmanagedType.LPStr)]
        public string pDataType;
    }
}