/////////////////////////////////////////////////////////////////////
//  implementation of ByteQueue utilizing BufferManager
/////////////////////////////////////////////////////////////////////

#if !NETSTANDARD1_6 && !NETSTANDARD2_0

using System;
using System.ServiceModel.Channels;

namespace dant02.snippets.dotnet.lib
{
    public class ManagedByteQueue : AbstractByteQueue, IDisposable
    {
        private readonly BufferManager bufferManager = null;

        public ManagedByteQueue(string name, BufferManager manager) : base(name)
        {
            this.bufferManager = manager;
            this.buffer = manager.TakeBuffer(allocatedSize);
        }

        public ManagedByteQueue(string name, BufferManager manager, int size) : base(name)
        {
            this.bufferManager = manager;
            this.allocatedSize = size;
            this.buffer = manager.TakeBuffer(allocatedSize);
        }

        public void Dispose()
        {
            this.bufferManager.ReturnBuffer(buffer);
        }

        protected override void Resize(long length)
        {
            if (allocatedSize - writeIndex < length)
                throw new NotImplementedException("Cannot resize managed byte queue");
        }
    }
}

#endif