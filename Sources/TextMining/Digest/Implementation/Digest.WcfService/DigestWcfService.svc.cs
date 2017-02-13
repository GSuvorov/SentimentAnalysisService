using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.ServiceModel;
using System.ServiceModel.Activation;
using System.Threading;
using System.Xml.Linq;

using log4net;
using Linguistics.Coreference;
using OpinionMining;
using TextMining.Core;
using TextMining.Service;
using TonalityMarking;
[assembly: log4net.Config.XmlConfigurator(ConfigFile = "DigestWcfService_log4net.xml", Watch = false)]

namespace Digest
{
    #region [.wcf-service attribute's.]   
    [AspNetCompatibilityRequirements(RequirementsMode = AspNetCompatibilityRequirementsMode.Allowed)]
    [ServiceBehavior
    (
               ConcurrencyMode = ConcurrencyMode.Multiple,
           InstanceContextMode = InstanceContextMode.Single, //.PerCall,
         MaxItemsInObjectGraph = int.MaxValue,
IncludeExceptionDetailInFaults = true
   //UseSynchronizationContext = false,
    )]
    #endregion
    public class DigestWcfService : IDigest
    {
        #region [.Static field's & properties.]
        internal static readonly ILog LOG = LogManager.GetLogger( typeof(DigestWcfService).Name );

        private static readonly TonalityMarkingWcfService _TonalityMarking;
        private static readonly OpinionMiningWcfService   _OpinionMining;
        #endregion

        #region [.ctor().]
        static DigestWcfService()
        {
            Config.ExecuteConfigure();

            _TonalityMarking = new TonalityMarkingWcfService();
            _OpinionMining   = new OpinionMiningWcfService();

            OperationContext.Current.AdjustBinding4AllEndpoints();
        }

        #region [.commented. delegate-BeginInvoke-variant.]
        /*
        public DigestWcfService()
        {
            _ExecuteDigestFunc = new Func< DigestInputParams, DigestOutputResult >( ExecuteDigest );
        }
        */
        #endregion
        #endregion

        #region [.ITonalityMarking.]
        public TonalityMarkingOutputResult ExecuteTonalityMarking( TonalityMarkingInputParams inputParams )
        {
            return (_TonalityMarking.ExecuteTonalityMarking( inputParams ));
        }
        
        public TonalityMarkingOutputResult4Blogs ExecuteTonalityMarking4Blogs( TonalityMarkingInputParams4Blogs inputParams )
        {
            return (_TonalityMarking.ExecuteTonalityMarking4Blogs( inputParams ));
        }
        public SocviewProcessingData ExecuteTonalityMarking4Socview( SocviewProcessingData socviewData )
        {
            return (_TonalityMarking.ExecuteTonalityMarking4Socview( socviewData ));
        }
        #endregion

        #region [.IOpinionMining.]
        public OpinionMiningOutputResult ExecuteOpinionMining( OpinionMiningInputParams inputParams )
        {
            return (_OpinionMining.ExecuteOpinionMining( inputParams ));
        }
        #endregion

        #region [.IDigest.]
        public DigestOutputResult ExecuteDigest( DigestInputParams inputParams )
        {
            try
            {
                #region [.check input.]
                inputParams.ThrowIfNull( "inputParams" );
                #endregion 

                #region [.process.]
                var stopwatch = Stopwatch.StartNew();
                var opinionMiningResult = this.ExecuteOpinionMining( inputParams.ToOpinionMiningInputParams() );

                var digestTuples = GenerateResult( opinionMiningResult, inputParams.ExecuteTonalityMarking, inputParams );

                #region commented
                /*var digestTuples = default(IEnumerable< DigestTuple >);
                if ( inputParams.ExecuteTonalityMarking )
                {
                    var digestTuplesList = new List< DigestTuple >( opinionMiningResult.Tuples.Count );
                    foreach ( var tuple in opinionMiningResult.Tuples )
                    {
                        if ( tuple.HasObject )
                        {
                            var tonalityMarkingResult = _TonalityMarking.ExecuteTonalityMarking4DigestWcfService( tuple.Sentence, inputParams );

                            digestTuplesList.Add( tuple.ToDigestTuple( tonalityMarkingResult ) );
                        }
                        else
                        {
                            digestTuplesList.Add( tuple.ToDigestTuple( inputParams ) );
                        }
                    }
                    digestTuples = digestTuplesList;
                }
                else
                {
                    digestTuples = from tuple in opinionMiningResult.Tuples
                                   select tuple.ToDigestTuple( inputParams );
                }*/
                #endregion
                stopwatch.Stop();
                #endregion

                #region [.write in log.]
                LOG.InfoEx( MethodBase.GetCurrentMethod().Name, WcfHelper.ClientRemoteAddress, stopwatch.Elapsed, inputParams );
                #endregion

                #region [.result.]
                return (new DigestOutputResult( digestTuples, opinionMiningResult.OperateRulesNames ));
                #endregion
            }
            catch ( Exception ex )
            {
                #region [.write error in log & rethrow.]
                LOG.ErrorEx( MethodBase.GetCurrentMethod().Name, ex, WcfHelper.ClientRemoteAddress, inputParams ); 
                throw; 
                #endregion
            }
        }

        #region [.exec over isolate-appdomain-variant.]
        /*
        private DigestOutputResult ExecuteDigest__( DigestInputParams inputParams )
        {
            var workingAppDomain = default(AppDomain);
            if ( !_WorkingAppDomains.TryGetValue( Thread.CurrentThread.ManagedThreadId, out workingAppDomain ) )
            {
                workingAppDomain = AppDomain.CreateDomain( Thread.CurrentThread.ManagedThreadId.ToString() );
                _WorkingAppDomains.TryAdd( Thread.CurrentThread.ManagedThreadId, workingAppDomain );
            }
            
            workingAppDomain.SetData( "inputParams", inputParams );

            workingAppDomain.DoCallBack( new CrossAppDomainDelegate( ExecuteDigestInWorkingAppDomain ) );

            var bytes = (byte[]) workingAppDomain.GetData( "result" );
            
            using ( var ms = new MemoryStream( bytes ) )
            {
                var result = (DigestOutputResult) _BinaryFormatter.Deserialize( ms );

                return (result);
            }
        }
        
        private static readonly System.Collections.Concurrent.ConcurrentDictionary< int, AppDomain > _WorkingAppDomains = 
                            new System.Collections.Concurrent.ConcurrentDictionary< int, AppDomain >();
        private static readonly BinaryFormatter _BinaryFormatter = new BinaryFormatter();

        private static void ExecuteDigestInWorkingAppDomain()
        {
            var currentDomain = AppDomain.CurrentDomain;

            var inputParams = (DigestInputParams) currentDomain.GetData("inputParams");
            using ( var e = new AutoResetEvent( false ) )
            {
                var workingThread = new Thread( ThreadRoutine );
                workingThread.Priority = ThreadPriority.Highest;
                workingThread.TrySetApartmentState( ApartmentState.STA );                
                workingThread.DisableComObjectEagerCleanup();
                workingThread.Start( new object[] { e, inputParams } );

                e.WaitOne();
                workingThread.Join();
            }
        }
        private static void ThreadRoutine( object state )
        {
            var e = (AutoResetEvent) ((object[])state)[ 0 ];
            e.Set();

            var inputParams = (DigestInputParams)((object[])state)[ 1 ];
    
            var result = (new DigestWcfService()).ExecuteDigest__( inputParams );

            using ( var ms = new MemoryStream() )
            {
                _BinaryFormatter.Serialize( ms, result );

                var bytes = ms.GetBuffer();

                AppDomain.CurrentDomain.SetData( "result", bytes );
            }         
        }
        */
        #endregion

        #region [.commented. delegate-BeginInvoke-variant.]
        /*
        private readonly Func< DigestInputParams, DigestOutputResult > _ExecuteDigestFunc;
        */
        #endregion
        public IAsyncResult BeginExecuteDigestAsync( DigestInputParams inputParams, AsyncCallback callback, object state )
        {
            #region [.commented. task-variant.]
            /*
            var task = Task.Factory.StartNew< DigestOutputResult >( _ => ExecuteDigest( inputParams ), state, TaskCreationOptions.LongRunning );

            if ( callback != null ) 
            {
                task.ContinueWith( (res) => callback( task ) );
            }
            return (task);
            */
            #endregion

            #region [.commented. delegate-BeginInvoke-variant.]
            /*
            var result = _ExecuteDigestFunc.BeginInvoke( inputParams, callback, executeDigestFunc );            

            return (result);
            */
            #endregion

            return (new ExecuteDigestAsyncResult( this, inputParams, callback, state ));
        }
        public DigestOutputResult EndExecuteDigestAsync( IAsyncResult result )
        {
            #region [.commented. task-variant.]
            //return ((Task< DigestOutputResult >) result).Result; 
            #endregion

            #region [.commented. delegate-BeginInvoke-variant.]
            /*
            var _result = _ExecuteDigestFunc.EndInvoke( result );

            return (_result);
            */
            #endregion

            return (ExecuteDigestAsyncResult.End( result ));
        }
        #endregion

        #region [.4 use only from Linguistic-service. use as-inprocess build with Linguistic-service.]
        public static DigestOutputResult ExecuteDigestInprocWithLinguisticService( 
            XDocument rdf, ICoreferenceInfo coreferenceInfo, ObjectAllocateMethod objectAllocateMethod )
        {
                #region [.check input.]
                rdf.ThrowIfNull( "rdf" );
                #endregion 

                #region [.process.]
                rdf = Algorithms.PreprocessRdf( rdf );

                var opinionMiningResult = OpinionMiningWcfService.ExecuteOpinionMiningInprocWithLinguisticService( 
                                                            rdf, false, objectAllocateMethod, coreferenceInfo );

                var inputParams = new DigestInputParams( "[NEVER MIND]", InputTextFormat.LinguisticXml )
                {
                    ExecuteTonalityMarking = true,
                    ObjectAllocateMethod   = objectAllocateMethod,
                };

                var digestTuples = GenerateResult( opinionMiningResult, inputParams.ExecuteTonalityMarking, inputParams );
                #endregion

                #region [.result.]
                return (new DigestOutputResult( digestTuples, opinionMiningResult.OperateRulesNames ));
                #endregion
        }
        #endregion

        #region [.Private method's & properties.]
        private static IEnumerable< DigestTuple > GenerateResult( OpinionMiningOutputResult opinionMiningResult, bool executeTonalityMarking, InputParamsBase inputParams )
        {
            var digestTuples = default(IEnumerable< DigestTuple >);
            if ( executeTonalityMarking )
            {
                var digestTuplesList = new List< DigestTuple >( opinionMiningResult.Tuples.Count );
                foreach ( var tuple in opinionMiningResult.Tuples )
                {
                    if ( tuple.HasObject )
                    {
                        var tonalityMarkingResult = TonalityMarkingWcfService.ExecuteTonalityMarking4DigestWcfService( tuple.GetSentence(), inputParams );

                        digestTuplesList.Add( tuple.ToDigestTuple( tonalityMarkingResult, inputParams ) );
                    }
                    else
                    {
                        digestTuplesList.Add( tuple.ToDigestTuple( /*inputParams*/ ) );
                    }
                }
                digestTuples = digestTuplesList;
            }
            else
            {
                digestTuples = from tuple in opinionMiningResult.Tuples
                               select tuple.ToDigestTuple( /*inputParams*/ );
            }
            return (digestTuples);
        }
        #endregion
    }

    /// <summary>
    /// 
    /// </summary>
    internal abstract class AsyncResult : IAsyncResult
    {
        private static AsyncCallback _AsyncCompletionWrapperCallback;
        private AsyncCallback    _Callback;
        private bool             _EndCalled;
        private Exception        _Exception;
        private AsyncCompletion  _NextAsyncCompletion;
        private ManualResetEvent _ManualResetEvent;
        private object           _ThisLock;

        protected AsyncResult( AsyncCallback callback, object state )
        {
            this._Callback  = callback;
            this.AsyncState = state;
            this._ThisLock  = new object();
        }

        public object AsyncState
        {
            get;
            private set;
        }
        public WaitHandle AsyncWaitHandle
        {
            get
            {
                if ( _ManualResetEvent != null )
                {
                    return _ManualResetEvent;
                }

                lock ( _ThisLock )
                {
                    if ( _ManualResetEvent == null )
                    {
                        _ManualResetEvent = new ManualResetEvent( IsCompleted );
                    }
                }

                return (_ManualResetEvent);
            }
        }
        public bool CompletedSynchronously
        {
            get;
            private set;
        }
        public bool IsCompleted
        {
            get;
            private set;
        }
        public bool HasCallback
        {
            get { return (this._Callback != null); }
        }

        protected Action< Exception > OnCompleting { get; set; }
        protected Action< AsyncCallback, IAsyncResult > VirtualCallback { get; set; }

        protected void Complete( bool completedSynchronously )
        {
            this.CompletedSynchronously = completedSynchronously;
            if ( OnCompleting != null )
            {
                try
                {
                    OnCompleting( this._Exception );
                }
                catch ( Exception exception )
                {
                    this._Exception = exception;
                }
            }

            if ( completedSynchronously )
            {
                this.IsCompleted = true;
            }
            else
            {
                lock ( _ThisLock )
                {
                    this.IsCompleted = true;
                    if ( this._ManualResetEvent != null )
                    {
                        this._ManualResetEvent.Set();
                    }
                }
            }

            if ( this._Callback != null )
            {
                if ( VirtualCallback != null )
                {
                    VirtualCallback( this._Callback, this );
                }
                else
                {
                    this._Callback( this );
                }
            }
        }
        protected void Complete( bool completedSynchronously, Exception exception )
        {
            this._Exception = exception;
            Complete( completedSynchronously );
        }

        private static void AsyncCompletionWrapperCallback( IAsyncResult result )
        {
            if ( result.CompletedSynchronously )
            {
                return;
            }

            var thisPtr = (AsyncResult) result.AsyncState;
            AsyncCompletion callback = thisPtr.GetNextCompletion();

            bool completeSelf = false;
            Exception completionException = null;
            try
            {
                completeSelf = callback( result );
            }
            catch ( Exception e )
            {
                completeSelf = true;
                completionException = e;
            }

            if ( completeSelf )
            {
                thisPtr.Complete( false, completionException );
            }
        }

        protected delegate bool AsyncCompletion( IAsyncResult result );
        protected AsyncCallback PrepareAsyncCompletion( AsyncCompletion callback )
        {
            this._NextAsyncCompletion = callback;
            if ( AsyncResult._AsyncCompletionWrapperCallback == null )
            {
                AsyncResult._AsyncCompletionWrapperCallback = new AsyncCallback( AsyncCompletionWrapperCallback );
            }

            return (AsyncResult._AsyncCompletionWrapperCallback);
        }
        private AsyncCompletion GetNextCompletion()
        {
            AsyncCompletion result = this._NextAsyncCompletion;
            this._NextAsyncCompletion = null;
            return (result);
        }

        protected static TAsyncResult End< TAsyncResult >( IAsyncResult result )
            where TAsyncResult : AsyncResult
        {
            if ( result == null )
            {
                throw (new ArgumentNullException( "result" ));
            }

            TAsyncResult asyncResult = result as TAsyncResult;

            if ( asyncResult == null )
            {
                throw (new ArgumentException( "result", "Invalid asyncResult" ));
            }

            if ( asyncResult._EndCalled )
            {
                throw (new InvalidOperationException( "AsyncResult has already ended" ));
            }

            asyncResult._EndCalled = true;

            if ( !asyncResult.IsCompleted )
            {
                asyncResult.AsyncWaitHandle.WaitOne();
            }

            if ( asyncResult._ManualResetEvent != null )
            {
                asyncResult._ManualResetEvent.Close();
            }

            if ( asyncResult._Exception != null )
            {
                // Rethrow the exception
                throw (asyncResult._Exception);
            }

            return (asyncResult);
        }        
    }

    /// <summary>
    /// 
    /// </summary>
    internal sealed class ExecuteDigestAsyncResult : AsyncResult
    {
        private IDigest            _IDigest;
        private DigestInputParams  _DigestInputParams;
        private DigestOutputResult _DigestOutputResult;

        public ExecuteDigestAsyncResult( IDigest idigest, DigestInputParams inputParams, AsyncCallback callback, object state )
            : base( callback, state )
        {
            this._IDigest           = idigest;
            this._DigestInputParams = inputParams;

            using ( var e = new AutoResetEvent( false ) )
            {
                var workingThread = new Thread( ThreadRoutine );
                workingThread.Start( e );

                e.WaitOne();
            }
        }

        private void ThreadRoutine( object obj )
        {
            try
            {
                var e = (AutoResetEvent) obj;
                e.Set();

                _DigestOutputResult = _IDigest.ExecuteDigest( _DigestInputParams );

                Complete( false );
            }
            catch ( Exception ex )
            {
                Complete( false, ex );
            }
        }

        #region [.End AsyncResult.]
        public static DigestOutputResult End( IAsyncResult result )
        {
            var thisObj = result as ExecuteDigestAsyncResult;
            AsyncResult.End< ExecuteDigestAsyncResult >( result );

            return (thisObj._DigestOutputResult);
        }
        #endregion
    }
}
