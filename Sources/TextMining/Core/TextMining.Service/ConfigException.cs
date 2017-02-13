using System;
using System.Runtime.Serialization;

namespace TextMining.Service
{
    [Serializable]
    public class ConfigException : ApplicationException
    {
        public ConfigException(string message) : base (message) { }
        public ConfigException(string message, Exception innerException) : base(message, innerException) { }
        protected ConfigException(SerializationInfo info, StreamingContext context) : base(info, context) { }
    }
}




