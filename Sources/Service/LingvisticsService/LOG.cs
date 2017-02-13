
namespace Lingvistics
{
    /// <summary>
    /// 
    /// </summary>
    internal static class LOG
    {
        public static log4net.ILog GetLogger()
        {
            return (log4net.LogManager.GetLogger( "LingvisticsServer" ));
        }
    }
}
