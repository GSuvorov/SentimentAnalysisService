using System;
using System.Runtime.InteropServices;

namespace Lingvistics.Client
{
    /// <summary>
    /// 
    /// </summary>
    public sealed class WindowsEvent : IDisposable
    {
        public static int INFINITE = -1;

        private const uint EVENT_MODIFY_STATE = 0x2;
        private const uint EVENT_ALL_ACCESS = 0x001F0011;
        private static uint WAIT_OBJECT_0 = 0x00000000;
        private static uint WAIT_FAILED = 0xFFFFFFFF;

        [DllImport( "kernel32.dll" )]
        private static extern Int32 GetLastError();

        [DllImport( "kernel32.dll", SetLastError = true, CharSet = CharSet.Auto )]
        internal static extern IntPtr CreateEvent( IntPtr lpEventAttributes, bool bManualReset, bool bInitialState, string lpName );

        [DllImport( "kernel32.dll", SetLastError = true, CharSet = CharSet.Auto )]
        internal static extern IntPtr OpenEvent( uint dwDesiredAccess, bool bInheritHandle, string lpName );

        [DllImport( "kernel32.dll", SetLastError = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        internal static extern bool CloseHandle( IntPtr hObject );

        [DllImport( "kernel32.dll", SetLastError = true )]
        internal static extern bool SetEvent( IntPtr hEvent );

        [DllImport( "kernel32.dll", SetLastError = true )]
        internal static extern bool ResetEvent( IntPtr hEvent );

        [DllImport( "kernel32", SetLastError = true, ExactSpelling = true )]
        internal static extern uint WaitForSingleObject( IntPtr handle, Int32 milliseconds );

        private IntPtr _hEvent = IntPtr.Zero;
        private string _name;

        public WindowsEvent( string name )
        {
            _name = name;
            _hEvent = CreateEvent( IntPtr.Zero, true, false, name );
            if ( _hEvent == IntPtr.Zero )
            {
                throw new Exception( string.Format( "Ошибка открытия события {0}, код ошибки {1}",
                    _name, GetLastError() ) );
            }
        }
        public void Close()
        {
            if ( _hEvent != IntPtr.Zero )
            {
                CloseHandle( _hEvent );
                _hEvent = IntPtr.Zero;
            }
        }
        public void Set()
        {
            if ( !SetEvent( _hEvent ) )
            {
                throw new Exception( string.Format( "Ошибка установки события {0}, код ошибки {1}",
                    _name, GetLastError() ) );
            }
        }
        public void Reset()
        {
            if ( !ResetEvent( _hEvent ) )
            {
                throw new Exception( string.Format( "Ошибка сброса события {0}, код ошибки {1}",
                    _name, GetLastError() ) );
            }
        }
        public bool Wait( int milliseconds )
        {
            uint rc = WaitForSingleObject( _hEvent, milliseconds );
            if ( rc == WAIT_FAILED )
            {
                throw new Exception( string.Format( "Ошибка ожидания события {0}, код ошибки {1}",
                    _name, GetLastError() ) );
            }
            return (rc == WAIT_OBJECT_0);
        }
        public bool IsSet()
        {
            return Wait( 0 );
        }

        #region [.IDisposable.]
        public void Dispose()
        {
            Close();
        }
        #endregion
    }
}
