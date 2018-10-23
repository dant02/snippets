using System;

namespace dant02.snippets.dotnet.lib
{
    public class ByteQueue
    {
        private int actualSize = 4096;

        private byte[] buffer = null;
        private int rIndex = 0;
        private int wIndex = 0;

        public ByteQueue()
        {
            this.buffer = new byte[actualSize];
        }

        public ByteQueue(int size)
        {
            actualSize = size;
            this.buffer = new byte[actualSize];
        }

        public int Count { get { return wIndex - rIndex; } }

        public void Add(byte[] data, int count)
        {
            Resize(count);

            Buffer.BlockCopy(data, 0, buffer, wIndex, count);
            wIndex += count;
        }

        public int Get(byte[] data, int offset, int count)
        {
            int result = count;
            if (this.Count < result)
                result = this.Count;

            Buffer.BlockCopy(this.buffer, rIndex, data, offset, count);
            rIndex += result;

            if (rIndex == wIndex)
                rIndex = wIndex = 0;

            return result;
        }

        public Int32 GetInt32()
        {
            if (wIndex - rIndex < 4)
                throw new InvalidOperationException("Not enough data in buffer");

            Int32 result = BitConverter.ToInt32(this.buffer, rIndex);
            rIndex += 4;
            return result;
        }

        public Int64 GetInt64()
        {
            if (wIndex - rIndex < 8)
                throw new InvalidOperationException("Not enough data in buffer");

            Int64 result = BitConverter.ToInt64(this.buffer, rIndex);
            rIndex += 8;
            return result;
        }

        private void Resize(int length)
        {
            if (actualSize - wIndex < length)
            {
                actualSize *= 2;
                var newBuffer = new byte[actualSize];

                Buffer.BlockCopy(this.buffer, 0, newBuffer, 0, this.buffer.Length);
                this.buffer = newBuffer;

                Resize(length);
            }
        }
    }
}