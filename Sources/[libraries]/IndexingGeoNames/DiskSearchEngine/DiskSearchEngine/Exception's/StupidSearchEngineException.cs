using System;
using System.Runtime.Serialization;

namespace DiskSearchEngine
{
    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public sealed class DiskSearchEngineException : Exception
    {
        public DiskSearchEngineException()
        {
        }
        public DiskSearchEngineException( string message )
            : base( message )
        {
        }
        protected DiskSearchEngineException( SerializationInfo info, StreamingContext context )
            : base( info, context )
        {
        }
        public DiskSearchEngineException( string message, Exception innerException )
            : base( message, innerException )
        {
        }
    }
}
