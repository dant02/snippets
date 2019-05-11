/////////////////////////////////////////////////////////////////////
//  wrapper around byte array to simplify operations with it
/////////////////////////////////////////////////////////////////////

using System;
using System.IO;

namespace dant02.snippets.dotnet.lib
{
    public abstract class AbstractByteQueue
    {
        protected int allocatedSize = 4096;
        protected byte[] buffer = null;
        protected int readIndex = 0;
        protected int writeIndex = 0;

        public int Count { get { return writeIndex - readIndex; } }

        public void Add(byte data)
        {
            Resize(1);

            buffer[writeIndex] = data;
            writeIndex += 1;
        }

        public void Add(byte[] data, int count)
        {
            this.Add(data, count, 0);
        }

        public void Add(byte[] data, int count, int offset)
        {
            Resize(count);

            Buffer.BlockCopy(data, offset, buffer, writeIndex, count);
            writeIndex += count;
        }

        public void Clear()
        {
            this.readIndex = this.writeIndex = 0;
        }

        public int Get(byte[] data, int offset, int count)
        {
            int result = count;
            if (this.Count < result)
                result = this.Count;

            Buffer.BlockCopy(this.buffer, readIndex, data, offset, count);
            readIndex += result;

            if (readIndex == writeIndex)
                readIndex = writeIndex = 0;

            return result;
        }

        public Int32 GetInt32()
        {
            if (writeIndex - readIndex < 4)
                throw new InvalidOperationException("Not enough data in buffer");

            Int32 result = BitConverter.ToInt32(this.buffer, readIndex);
            readIndex += 4;

            if (readIndex == writeIndex)
                readIndex = writeIndex = 0;

            return result;
        }

        public Int64 GetInt64()
        {
            if (writeIndex - readIndex < 8)
                throw new InvalidOperationException("Not enough data in buffer");

            Int64 result = BitConverter.ToInt64(this.buffer, readIndex);
            readIndex += 8;

            if (readIndex == writeIndex)
                readIndex = writeIndex = 0;

            return result;
        }

        public int ReadFromStream(Stream stream, int count)
        {
            var length = allocatedSize - writeIndex;

            if (count > length)
                Resize(count);

            var read = stream.Read(buffer, writeIndex, length);
            writeIndex += read;
            return read;
        }

        public void WriteToStream(Stream stream)
        {
            var length = writeIndex - readIndex;
            stream.Write(buffer, readIndex, length);
        }

        protected abstract void Resize(int length);
    }
}