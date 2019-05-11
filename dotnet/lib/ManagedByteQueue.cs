/////////////////////////////////////////////////////////////////////
//  implementation using utilizing BufferManager
/////////////////////////////////////////////////////////////////////

#if !NETSTANDARD1_6 && !NETSTANDARD2_0

using System;
using System.ServiceModel.Channels;

namespace dant02.snippets.dotnet.lib
{
    public class ManagedByteQueue : AbstractByteQueue, IDisposable
    {
        private readonly BufferManager bufferManager = null;

        public ManagedByteQueue(BufferManager manager)
        {
            this.bufferManager = manager;
            this.buffer = manager.TakeBuffer(allocatedSize);
        }

        public ManagedByteQueue(BufferManager manager, int size)
        {
            this.bufferManager = manager;
            this.allocatedSize = size;
            this.buffer = manager.TakeBuffer(allocatedSize);
        }

        public void Dispose()
        {
            this.bufferManager.ReturnBuffer(buffer);
        }

        protected override void Resize(int length)
        {
            if (allocatedSize - writeIndex < length)
                throw new NotImplementedException("Cannot resize managed byte queue");
        }
    }
}

#endif