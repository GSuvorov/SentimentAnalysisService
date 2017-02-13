using System;
using System.Runtime.Serialization;

#if WITH_OM_TM
using Digest;
using TextMining.Core;
using TonalityMarking;

using EntityType = Lingvistics.EntityType;
#else
using Linguistics.Core;
#endif

namespace Lingvistics.Client
{
    /// <summary>
    /// Тема
    /// </summary>
    [Serializable]
    public sealed class ThemeItem
    {
        /// <summary>
        /// Идентификатор
        /// </summary>
        public int ID { get; set; }
        /// <summary>
        /// Нормализованное значение
        /// </summary>
        public string Name { get; set; }
        /// <summary>
        /// Исходная форма
        /// </summary>
        public string OriginalName { get; set; }
        /// <summary>
        /// Семантический тип
        /// </summary>
        public EntityType Type { get; set; }
        /// <summary>
        /// Частота в роли подлежащего
        /// </summary>
        public int FreqSubj { get; set; }
        /// <summary>
        /// Частота в роли дополнения
        /// </summary>
        public int FreqObj { get; set; }
        /// <summary>
        /// Частота в роли обстоятельства
        /// </summary>
        public int FreqAdj { get; set; }
        /// <summary>
        /// Частота внутренней цепочки
        /// </summary>
        public int FreqOther { get; set; }

        /// <summary>
        /// Суммарная частота
        /// </summary>
        public int Freq
        {
            get
            {
                return FreqSubj + FreqObj + FreqAdj + FreqOther;
            }
        }

        public override bool Equals( object obj )
        {
            if ( this == obj ) return true;
            ThemeItem other = (ThemeItem) obj;
            return this.Name.Equals( other.Name ) && this.Type.Equals( other.Type );
        }
        public override int GetHashCode()
        {
            return this.Name.GetHashCode() ^ this.Type.GetHashCode();
        }
    }

    /// <summary>
    /// Связь
    /// </summary>
    [Serializable]
    public sealed class LinkItem
    {
        /// <summary>
        /// Идентификатор первой темы
        /// </summary>
        public int SourceThemeID { get; set; }
        /// <summary>
        /// Идентификатор второй темы
        /// </summary>
        public int DestThemeID { get; set; }
        /// <summary>
        /// Тип связи
        /// </summary>
        public string Type { get; set; }
        /// <summary>
        /// Частота связи
        /// </summary>
        public int Freq { get; set; }

        public override bool Equals( object obj )
        {
            if ( this == obj ) return true;
            LinkItem other = (LinkItem) obj;
            return this.SourceThemeID.Equals( other.SourceThemeID ) && this.DestThemeID.Equals( other.DestThemeID ) &&
                this.Type.Equals( other.Type );
        }
        public override int GetHashCode()
        {
            return this.SourceThemeID.GetHashCode() ^ this.DestThemeID.GetHashCode() ^ this.Type.GetHashCode();
        }
    }

    /// <summary>
    /// Результат работы Лингвистики
    /// </summary>
    [Serializable]
    public sealed class LingvisticsResult
    {
        /// <summary>
        /// Тип результата
        /// </summary>
        public LingvisticsResultOptions Options { get; private set; }
        /// <summary>
        /// RDF
        /// </summary>
        public string RDF { get; private set; }
        /// <summary>
        /// Список тем
        /// </summary>
        public ThemeItem[] ThemeList { get; private set; }
        /// <summary>
        /// Список связей
        /// </summary>
        public LinkItem[] LinkList { get; private set; }

#if WITH_OM_TM
        public DigestOutputResult OpinionMiningWithTonalityResult { get; private set; }
        public TonalityMarkingOutputResult TonalityResult { get; private set; }        
#endif

        public LingvisticsResult( LingvisticsResultOptions options
                                , string rdf
                                , ThemeItem[] themeList
                                , LinkItem[]  linkList 
                                )
        {
            this.Options   = options;
            this.RDF       = rdf;
            this.ThemeList = themeList;
            this.LinkList  = linkList;
        }

#if WITH_OM_TM
        public LingvisticsResult( LingvisticsResultOptions options
                                , string rdf
                                , ThemeItem[] themeList
                                , LinkItem[]  linkList
                                , DigestOutputResult opinionMiningWithTonalityResult 
                                )
            : this( options, rdf, themeList, linkList )
        {
            OpinionMiningWithTonalityResult = opinionMiningWithTonalityResult;
        }

        public LingvisticsResult( LingvisticsResultOptions options
                                , string rdf
                                , ThemeItem[] themeList
                                , LinkItem[]  linkList
                                , TonalityMarkingOutputResult tonalityResult 
                                )
            : this( options, rdf, themeList, linkList )
        {
            TonalityResult = tonalityResult;
        }      
#endif
    }

    /// <summary>
    /// Результат работы Лингвистики для PTS
    /// </summary>
    [Serializable]
    public sealed class PTSResult
    {
        /// <summary>
        /// Терм-вектор и RDF
        /// </summary>
        public LingvisticsResult LingvisticResult { get; set; }
        /// <summary>
        /// Числовые интервалы
        /// </summary>
        public string TextRanges { get; set; }
    }

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public sealed class ServerStatus
    {
        public int MaxProcessorCount;
        public int BusyProcessorCount;
        public int HeavyQueueLength;
        public int LightQueueLength;

        public override string ToString()
        {
            return (string.Format( "BusyProc={0}({1}) HQ={2} LQ={3}", BusyProcessorCount, MaxProcessorCount, HeavyQueueLength, LightQueueLength ));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public enum ProcessingType
    {
        Light,
        Heavy
    }

    /// <summary>
    /// Типы возвращаемых результатов работы Лингвистики
    /// </summary>
    [Flags]
    public enum LingvisticsResultOptions
    {
        /// <summary>
        /// Без результата
        /// </summary>
        None = 0x0,
        /// <summary>
        /// Только RDF
        /// </summary>
        RDF = 0x1,
        /// <summary>
        /// RDF + список тем
        /// </summary>
        ThemeList = 0x2,
        /// <summary>
        /// RDF + Сем. сеть
        /// </summary>
        SemNet = 0x6, //0x2 + 0x4
#if WITH_OM_TM
        /// <summary>
        /// Только (OpinionMining + Tonality)
        /// </summary>
        OpinionMiningWithTonality = 0x8,
        /// <summary>
        /// Только (Tonality)
        /// </summary>
        Tonality = 0x10,
		/// <summary>
		/// Все результаты
		/// </summary>
		All = 0xF //0x1 + 0x2 + 0x4 + 0x8
#else
        All = 0x7 //0x1 + 0x2 + 0x4
#endif
    }

    /// <summary>
    /// Варианты выделения сущностей
    /// </summary>
    public enum SelectEntitiesMode
    {
        /// <summary>
        /// Полный анализ
        /// </summary>
        Full = 0,
        /// <summary>
        /// Упрощенный анализ
        /// </summary>
        Simple = 1,
    }

    /// <summary>
    /// 
    /// </summary>
    public interface ILingvisticsProcessor : ILingvisticsServer
    {
        /// <summary>
        /// Закрытие лингвистического процесса
        /// </summary>
        void Close();
    }

    /// <summary>
    /// 
    /// </summary>
    public interface ILingvisticsService : ILingvisticsServer
    {
        /// <summary>
        /// Получение степени загрузки очереди сервера
        /// </summary>
        /// <returns>Параметры состояния сервера</returns>
        ServerStatus GetLoadingStatus();
    }

    /// <summary>
    /// rdf-input Лингвистики
    /// </summary>
    [Serializable]
    public sealed class LingvisticsRDFInput //: ISerializable
    {
        public LingvisticsRDFInput()
        {
        }

        public string                   Rdf                  { get; set; }
        public LingvisticsResultOptions Options              { get; set; }
        public bool                     GenerateAllSubthemes { get; set; } //= false 
#if WITH_OM_TM
        /// <summary>
        /// use when {(Options & LingvisticsResultOptions.OpinionMiningWithTonality) == LingvisticsResultOptions.OpinionMiningWithTonality}
        ///       or {(Options & LingvisticsResultOptions.Tonality) == LingvisticsResultOptions.Tonality}
        /// </summary>
        public ObjectAllocateMethod     ObjectAllocateMethod { get; set; } //= ObjectAllocateMethod.FirstEntityWithTypePronoun
        /// <summary>
        /// use when only {(Options & LingvisticsResultOptions.Tonality) == LingvisticsResultOptions.Tonality}
        /// </summary>
        public TonalityMarkingInputParams4InProcess TonalityMarkingInput { get; set; }
#endif
        #region commented
        /*
        #region [.ISerializable.]
        private LingvisticRDFInput(SerializationInfo info, StreamingContext context)
        {
            SerializationInfoEnumerator enumerator = info.GetEnumerator();
            while (enumerator.MoveNext())
            {
                switch (enumerator.Name)
                {
                    case "Rdf":
                        this.Rdf = Convert.ToString(enumerator.Value);
                        break;
                    case "Options":
                        this.Options = (LingvisticResultOptions) enumerator.Value;
                        break;
                    case "GenerateAllSubthemes":
                        this.GenerateAllSubthemes = Convert.ToBoolean(enumerator.Value);
                        break;
#if WITH_OM_TM
                    case "ObjectAllocateMethod":
                        this.ObjectAllocateMethod = (ObjectAllocateMethod) enumerator.Value;
                        break;
#endif
                }
            }
        }
        void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Rdf", Rdf);
            info.AddValue("Options", (int) Options);
            info.AddValue("GenerateAllSubthemes", GenerateAllSubthemes);
#if WITH_OM_TM
            info.AddValue("ObjectAllocateMethod", (int) ObjectAllocateMethod);
#endif
        }
        #endregion
        */
        #endregion
    }

    /// <summary>
    /// text-input Лингвистики
    /// </summary>
    [Serializable]
    public sealed class LingvisticsTextInput //: ISerializable
    {
        public LingvisticsTextInput()
        {
        }
        public LingvisticsTextInput( LingvisticsTextInput other, string text )
        {
            Text = text;

            AfterSpellChecking = other.AfterSpellChecking;
            BaseDate = other.BaseDate;
            Options = other.Options;
            Mode = other.Mode;
            GenerateAllSubthemes = other.GenerateAllSubthemes;
#if WITH_OM_TM
            ObjectAllocateMethod = other.ObjectAllocateMethod;
#endif
        }

        public string                   Text                 { get; set; }
        public bool                     AfterSpellChecking   { get; set; }
        public DateTime                 BaseDate             { get; set; }
        public LingvisticsResultOptions Options              { get; set; } //= LingvisticResultOptions.All
        public SelectEntitiesMode       Mode                 { get; set; } //= SelectEntitiesMode.Full
        public bool                     GenerateAllSubthemes { get; set; } //= false 
#if WITH_OM_TM
        /// <summary>
        /// use when {(Options & LingvisticsResultOptions.OpinionMiningWithTonality) == LingvisticsResultOptions.OpinionMiningWithTonality}
        ///       or {(Options & LingvisticsResultOptions.Tonality) == LingvisticsResultOptions.Tonality}
        /// </summary>
        public ObjectAllocateMethod     ObjectAllocateMethod { get; set; } //= ObjectAllocateMethod.FirstEntityWithTypePronoun
        /// <summary>
        /// use when only {(Options & LingvisticsResultOptions.Tonality) == LingvisticsResultOptions.Tonality}
        /// </summary>
        public TonalityMarkingInputParams4InProcess TonalityMarkingInput { get; set; }
#endif
        #region commented
        /*
        #region [.ISerializable.]
        private LingvisticTextInput(SerializationInfo info, StreamingContext context)
        {
            SerializationInfoEnumerator enumerator = info.GetEnumerator();
            while (enumerator.MoveNext())
            {
                switch (enumerator.Name)
                {
                    case "Text":
                        this.Text = Convert.ToString(enumerator.Value);
                        break;
                    case "AfterSpellChecking":
                        this.AfterSpellChecking = Convert.ToBoolean(enumerator.Value);
                        break;
                    case "BaseDate":
                        this.BaseDate = Convert.ToDateTime(enumerator.Value);
                        break;
                    case "Options":
                        this.Options = (LingvisticResultOptions) enumerator.Value;
                        break;
                    case "Mode":
                        this.Mode = (SelectEntitiesMode) enumerator.Value;
                        break;
                    case "GenerateAllSubthemes":
                        this.GenerateAllSubthemes = Convert.ToBoolean(enumerator.Value);
                        break;
#if WITH_OM_TM
                    case "ObjectAllocateMethod":
                        this.ObjectAllocateMethod = (ObjectAllocateMethod) enumerator.Value;
                        break;
#endif
                }
            }
        }
        void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Text", Text);
            info.AddValue("AfterSpellChecking", AfterSpellChecking);
            info.AddValue("BaseDate", BaseDate);
            info.AddValue("Options", (int) Options);
            info.AddValue("Mode", (int) Mode);
            info.AddValue("GenerateAllSubthemes", GenerateAllSubthemes);
#if WITH_OM_TM
            info.AddValue("ObjectAllocateMethod", (int) ObjectAllocateMethod);
#endif
        }
        #endregion
        */
        #endregion
    }

    /// <summary>
    /// 
    /// </summary>
    public interface ILingvisticsServer
    {
        #region [.Heavy Function.]
        /// <summary>
        /// Лингвистическая обработка документа
        /// </summary>
        /// <param name="text">Текст документа или XML после орфокоррекции</param>
        /// <param name="afterSpellChecking">Флаг документа в виде XML после орфокоррекции</param>
        /// <param name="baseDate">Базовая дата для разрешения относительных дат</param>
        /// <param name="options">Набор флагов, определяющих состав выходного результата: RDF, SemNet, ThemeList</param>
        /// <param name="mode">Режим выделения сущностей: быстрый/полный</param>
        /// <param name="generateAllSubthemes">Флаг добавления в список подсущностей</param>
        /// <returns>Результат обработки текста</returns>
        LingvisticsResult ProcessText(
              string                   text,
              bool                     afterSpellChecking,
              DateTime                 baseDate,
              LingvisticsResultOptions options              = LingvisticsResultOptions.All,
              SelectEntitiesMode       mode                 = SelectEntitiesMode.Full,
              bool                     generateAllSubthemes = false );

        LingvisticsResult ProcessText( LingvisticsTextInput input );
        
        PTSResult ProcessPTS( string xml, bool buildSemanticNetwork, string language );
        #endregion

        #region [.Light Function.]
        /// <summary>
        /// Дополнительная обработка документа, представленного в виде RDF
        /// </summary>
        /// <param name="rdf">Документ в виде RDF</param>
        /// <param name="options">Набор флагов, определяющих состав выходного результата: SemNet, ThemeList</param>
        /// <param name="generateAllSubthemes">Флаг добавления в список подсущностей</param>
        /// <returns>Результат обработки RDF</returns>
        LingvisticsResult ProcessRDF( string rdf, LingvisticsResultOptions options, bool generateAllSubthemes = false );
        LingvisticsResult ProcessRDF( LingvisticsRDFInput input );

        /// <summary>
        /// Возвращает коллекцию словоформ для заданного слова
        /// </summary>
        /// <param name="word">слово</param>
        /// <returns>коллекция словоформ</returns>
        string[] GetAllWordForms( string word );

        /// <summary>
        /// Возвращает коллекцию словоформ для заданного слова и его части речи
        /// </summary>
        /// <param name="word">слово</param>
        /// <returns>коллекция словоформ и коллекция частей речи</returns>
        Tuple< string[], string[] > GetAllWordFormsWithPartsOfSpeech( string word );

        /// <summary>
        /// Возвращает нормальную форму слова
        /// </summary>
        /// <param name="word">слово</param>
        /// <returns>Нормальная форма слова</returns>
        string GetNormalWordForm( string word );

        /// <summary>
        /// Возвращает все возможные нормальные формы слова
        /// </summary>
        /// <param name="word">слово</param>
        /// <returns>список нормальных форм слова</returns>
        string[] GetAllNormalWordForm( string word );
        #endregion
    }
}