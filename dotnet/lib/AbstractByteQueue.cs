/////////////////////////////////////////////////////////////////////
//  wrapper around byte array to simplify operations with it
/////////////////////////////////////////////////////////////////////

using System;
using System.IO;
using System.Threading.Tasks;

namespace dant02.snippets.dotnet.lib
{
    public abstract class AbstractByteQueue
    {
        protected int allocatedSize = 4096;
        protected byte[] buffer = null;
        protected int readIndex = 0;
        protected int writeIndex = 0;

        public AbstractByteQueue(string name)
        {
            this.Name = name;
        }

        public int Count { get { return writeIndex - readIndex; } }
        protected string Name { get; }

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
            if (this.Count < count)
                count = this.Count;

            Buffer.BlockCopy(this.buffer, readIndex, data, offset, count);
            readIndex += count;

            if (readIndex == writeIndex)
                readIndex = writeIndex = 0;

            return count;
        }

        public Int32 GetInt32()
        {
            if (this.Count < 4)
                throw new InvalidOperationException($"Not enough data in buffer, name: {this.Name}, GetInt32, rI:{readIndex}, wI:{writeIndex}");

            Int32 result = BitConverter.ToInt32(this.buffer, readIndex);
            readIndex += 4;

            if (readIndex == writeIndex)
                readIndex = writeIndex = 0;

            return result;
        }

        public Int64 GetInt64()
        {
            if (this.Count < 8)
                throw new InvalidOperationException($"Not enough data in buffer, name: {this.Name}, GetInt64, rI:{readIndex}, wI:{writeIndex}");

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

            var read = stream.Read(buffer, writeIndex, count);
            writeIndex += read;
            return read;
        }

        public async Task<int> ReadFromStreamAsync(Stream stream, int count)
        {
            var length = allocatedSize - writeIndex;

            if (count > length)
                Resize(count);

            var read = await stream.ReadAsync(buffer, writeIndex, count);

            writeIndex += read;
            return read;
        }

        public void WriteToStream(Stream stream)
        {
            var length = this.Count;
            stream.Write(buffer, readIndex, length);
            readIndex += length;

            if (readIndex == writeIndex)
                readIndex = writeIndex = 0;
        }

        protected abstract void Resize(long length);
    }
}