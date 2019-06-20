/////////////////////////////////////////////////////////////////////
//  simple implementation for generic use
/////////////////////////////////////////////////////////////////////

using System;

namespace dant02.snippets.dotnet.lib
{
    public class ByteQueue : AbstractByteQueue
    {
        public ByteQueue(string name) : base(name)
        {
            this.buffer = new byte[allocatedSize];
        }

        public ByteQueue(string name, int size) : base(name)
        {
            allocatedSize = size;
            this.buffer = new byte[allocatedSize];
        }

        protected override void Resize(long length)
        {
            if (allocatedSize - writeIndex < length)
            {
                allocatedSize *= 2;
                var newBuffer = new byte[allocatedSize];

                Buffer.BlockCopy(this.buffer, 0, newBuffer, 0, this.buffer.Length);
                this.buffer = newBuffer;

                Resize(length);
            }
        }
    }
}