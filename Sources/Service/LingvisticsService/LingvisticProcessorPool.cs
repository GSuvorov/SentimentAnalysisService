using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;

using Config = Lingvistics.Properties.Settings;
using ProcessingType = Lingvistics.Client.ProcessingType;
using ServerStatus = Lingvistics.Client.ServerStatus;

namespace Lingvistics
{
    /// <summary>
    /// 
    /// </summary>
	public sealed class LingvisticsProcessorPool
	{
        private static readonly log4net.ILog Logger = LOG.GetLogger();

		private readonly int _MaxInstanceInPool = Config.Default.MaxProcessorCount;

		private bool                         _CloseState;
		private ManualResetEvent             _FreeProcessorEvent;
		private Queue< int >                 _LightThreadQueue = new Queue< int >();
		private Queue< int >                 _HeavyThreadQueue = new Queue< int >();
		private Dictionary< int, DateTime >  _ThreadStartDateTimeDictionary = new Dictionary< int, DateTime >();
		private List< LingvisticsProcessor > _PoolItemList = new List<LingvisticsProcessor>();
		private Timer                        _MonitorTimer;

		public LingvisticsProcessorPool()
		{
            _MonitorTimer = new Timer( state => MonitorRoutine(), null, 1000, 1000 );
			_FreeProcessorEvent = new ManualResetEvent(false);
		}

		//Закрытие сервера
		public void Close()
		{
            _MonitorTimer.Dispose();
            lock ( _PoolItemList )
            {
                _CloseState = true;
                LingvisticsProcessor[] poolItemList = _PoolItemList.ToArray();
                foreach ( var poolItem in poolItemList )
                {
                    CloseItem( poolItem );
                }
                foreach ( var poolItem in poolItemList )
                {
                    while ( !poolItem.Closed )
                    {
                        Thread.Sleep( 10 );
                    }
                }
            }
		}

        //Статус загрузки сервера
        public ServerStatus GetLoadingStatus()
        {
            lock ( _PoolItemList )
            {
                return (new ServerStatus()
                {
                    MaxProcessorCount  = _MaxInstanceInPool,
                    BusyProcessorCount = _PoolItemList.Count( t => t.Busy ),
                    LightQueueLength   = _LightThreadQueue.Count,
                    HeavyQueueLength   = _HeavyThreadQueue.Count
                });
            }
        }

		//возвращение свободного процессора
        public LingvisticsProcessor GetItem( ProcessingType procType )
		{
			int threadID = Thread.CurrentThread.ManagedThreadId;

			//возвратить сразу или добавить в очередь
            lock ( _PoolItemList )
			{
                if ( _CloseState )
				{
					throw (new ApplicationException("Сервер останавливается"));
				}

                Logger.DebugFormat( "request for {0} fn", procType );

				//список свободных процессоров
                var freeItemList = _PoolItemList.Where( t => !t.Busy );

                //очереди пустые или процессоров больше 2 => вернуть процессор
                if ( freeItemList.Count() > Math.Min( 2, (_LightThreadQueue.Count + _HeavyThreadQueue.Count) ) )
                {
                    var poolItem = freeItemList.FirstOrDefault();
                    poolItem.Lock( procType );
                    //Если свободных процессоров нет, то сбросить событие
                    if ( !freeItemList.Skip( 1 ).Any() )
                    {
                        _FreeProcessorEvent.Reset();
                    }
                    Logger.DebugFormat( "proc({0}) lock w/o queue => {1}", poolItem.ProcessID, GetStat() );
                    return poolItem;
                }
                //Добавление в пул нового лингвистического процессора
                if ( _PoolItemList.Count < _MaxInstanceInPool )
                {
                    var poolItem = new LingvisticsProcessor();
                    _PoolItemList.Add( poolItem );
                    //если очереди пустые => вернуть результат
                    if ( _LightThreadQueue.Count == 0 && (procType == ProcessingType.Light || _HeavyThreadQueue.Count == 0) )
                    {
                        poolItem.Lock( procType );
                        _FreeProcessorEvent.Reset();
                        Logger.DebugFormat( "new proc({0}) lock => {1}", poolItem.ProcessID, GetStat() );
                        return (poolItem);
                    }
                }
                //Добавить текущий поток в очередь
                if ( procType == ProcessingType.Light )
                {
                    _LightThreadQueue.Enqueue( threadID );
                    Logger.DebugFormat( "add to LightQueue => {0}", GetStat() );
                }
                else //procType == ProcessingType.Heavy
                {
                    _HeavyThreadQueue.Enqueue( threadID );
                    Logger.DebugFormat( "add to HeavyQueue => {0}", GetStat() );
                }
                _ThreadStartDateTimeDictionary.Add( threadID, DateTime.Now );
			}

			var sw = Stopwatch.StartNew();

			//Цикл ожидания освобождения процессора
            for ( ; ; )
			{
                LingvisticsProcessor poolItem = null;
                //если сводных элементов нет, то переход в ожидание
                lock ( _PoolItemList )
                {
                    if ( _CloseState ) throw (new ApplicationException( "Сервер останавливается" ));

                    //запросы типа Light, идут раньше чем запросы типа Heavy
                    if ( procType == ProcessingType.Light || _LightThreadQueue.Count == 0 )
                    {
                        //текущий поток соответствует первому элементу очереди
                        Queue< int > queue = (procType == ProcessingType.Light) ? _LightThreadQueue : _HeavyThreadQueue;
                        if ( queue.Peek() == threadID )
                        {
                            poolItem = _PoolItemList.FirstOrDefault( t => !t.Busy );
                            //Добавление в пул нового лингвистического процессора
                            if ( poolItem == null && _PoolItemList.Count < _MaxInstanceInPool )
                            {
                                poolItem = new LingvisticsProcessor();
                                _PoolItemList.Add( poolItem );
                            }
                            if ( poolItem != null )
                            {
                                queue.Dequeue();
                                _ThreadStartDateTimeDictionary.Remove( threadID );
                                poolItem.Lock( procType );
                                //Если больше нет свободных процессоров, то сброс события ожидания
                                if ( !_PoolItemList.Any( t => !t.Busy ) )
                                {
                                    _FreeProcessorEvent.Reset();
                                }
                                sw.Stop();
                                Logger.DebugFormat( "proc({0}) lock (dT={1:F3}c) => {2}", poolItem.ProcessID, sw.Elapsed.TotalSeconds, GetStat() );

                                return (poolItem);
                            }
                        }
                    }
                } //end lock

                //если свободных процессоров нет и событие сброшено, то ждать освобождения процессора по событию
                if ( /*---ALWAYS TRUE--- poolItem == null &&*/ !_FreeProcessorEvent.WaitOne( 0 ) )
                {
                    Logger.Debug( "WaitFor FreeProcEvent" );
                    //При превышении таймаута => ошибка
                    var cur = DateTime.Now;
                    var end = _ThreadStartDateTimeDictionary[ threadID ].AddSeconds( Config.Default.ProcessorQueueTimeOutInSeconds );
                    if ( cur > end || !_FreeProcessorEvent.WaitOne( end - cur ) )
                    {
                        //удалить поток из очереди
                        lock ( _PoolItemList )
                        {
                            if ( _CloseState ) throw (new ApplicationException( "Сервер останавливается" ));

                            if ( procType == ProcessingType.Light )
                                _LightThreadQueue = new Queue< int >( _LightThreadQueue.Where( t => t != threadID ) );
                            else
                                _HeavyThreadQueue = new Queue< int >( _HeavyThreadQueue.Where( t => t != threadID ) );
                            _ThreadStartDateTimeDictionary.Remove( threadID );
                        }
                        Logger.DebugFormat( "present queue timeout: {0}", GetStat() );
                        throw (new ApplicationException( "Превышен таймаут на ожидание выполнения - недостаточно число процессов" ));
                    }
                }
				//иначе короткая пауза для прохода остальных потоков
				else
				{
                    //время ожидания расчитывается из числа потоков
                    Logger.DebugFormat( "SleepStart" );
                    Thread.Sleep( 1 );
                    Logger.DebugFormat( "SleepEnd" );
				}
			}
		}

		/// <summary>
		/// Закрытие лингв. процесса и удаление его из пула
		/// лингв. процесса должен предварительно залочен, например через вызов GetItem
		/// </summary>
		/// <param name="poolItem">Закрываемый объект</param>
        public void CloseItem( LingvisticsProcessor poolItem )
		{
            lock ( _PoolItemList )
            {
                _PoolItemList.Remove( poolItem );
                _FreeProcessorEvent.Set();
                Logger.InfoFormat( "proc({0}) close => {1}", poolItem.ProcessID, GetStat() );
            }
            poolItem.Close();
		}

		/// <summary>
		/// Выставить состояние процессора в "свободен" и выставить событие - "есть свободный процессор"
		/// </summary>
		/// <param name="poolItem"></param>
        public void FreeItem( LingvisticsProcessor poolItem )
        {
            lock ( _PoolItemList )
            {
                poolItem.Free();
                _FreeProcessorEvent.Set();
                Logger.DebugFormat( "proc({0}) free => {1}", poolItem.ProcessID, GetStat() );
            }
        }

        private string GetStat()
        {
            var sb = new StringBuilder();
            sb.AppendFormat( "TotalProc={0} FreeProc={1}", _PoolItemList.Count, _PoolItemList.Count( t => !t.Busy ) );
            if ( _LightThreadQueue.Count > 0 )
                sb.AppendFormat( " LightQueue={0}[{1}]", _LightThreadQueue.Count, string.Join( ",", _LightThreadQueue ) );
            if ( _HeavyThreadQueue.Count > 0 )
                sb.AppendFormat( " HeavyQueue={0}[{1}]", _HeavyThreadQueue.Count, string.Join( ",", _HeavyThreadQueue ) );
            return (sb.ToString());
        }

		//Проверка процессоров на непредвиденное закрытие
		//а также закрытие процессоров в случае длительного простоя
        private void MonitorRoutine()
        {
            for ( ; ; )
            {
                lock ( _PoolItemList )
                {
                    //Если процессор упал
                    var poolItem = _PoolItemList.FirstOrDefault( t => t.Closed );
                    if ( poolItem != null )
                    {
                        Logger.InfoFormat( "Monitor: Proc({0}) is stopped", poolItem.ProcessID );
                    }
                    //Если превышено время простоя
                    if ( poolItem == null && _PoolItemList.Count > Config.Default.StartupProcessorCount )
                    {
                        poolItem = _PoolItemList.FirstOrDefault( t => !t.Busy &&
                            t.LastDateTimeAction.AddSeconds( Config.Default.ProcessorIdleTimeOutInSeconds ) < DateTime.Now );
                        if ( poolItem != null )
                        {
                            Logger.InfoFormat( "Monitor: Proc({0}) idle timeout happens", poolItem.ProcessID );
                        }
                    }
                    if ( poolItem == null ) break;
                    CloseItem( poolItem );
                }
            }
        }
	}

}
