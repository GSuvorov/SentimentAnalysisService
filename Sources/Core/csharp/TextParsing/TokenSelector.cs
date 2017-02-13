using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace TextParsing
{
    /// <summary>
    /// Токен
    /// </summary>
    internal struct Token
    {
        /// <summary>
        /// Позиция
        /// </summary>
        public int Index;
        /// <summary>
        /// Длина
        /// </summary>
        public int Length;
    }

    /// <summary>
    /// Тип токена
    /// </summary>
    internal enum TokenType
    {
        /// <summary>
        /// email
        /// </summary>
        Email,
        /// <summary>
        /// Разделитель чисел
        /// </summary>
        NumberSeparator,
        /// <summary>
        /// Единица измерения
        /// </summary>
        Measure,
        /// <summary>
        /// Смайлик
        /// </summary>
        Smile,
        /// <summary>
        /// url
        /// </summary>
        URL
    }

    /// <summary>
    /// Класс для выделения токенов в тексте
    /// </summary>
    internal class TokenSelector
    {
        /// <summary>
        /// Словари-ресурсы
        /// </summary>
        public DictionaryResources Dictionaries { get; private set; }

        /// <summary>
        /// регулярные выражения для выделения токенов
        /// </summary>
        private Dictionary<TokenType, Regex> _tokenRegex;

        #region [.ctor().]
        public TokenSelector( string dictionaryResourcesPath )
        {
            Dictionaries = new DictionaryResources( dictionaryResourcesPath );
            _tokenRegex = new Dictionary<TokenType, Regex>();
            InitializeTokenRegex();
        }

        /// <summary>
        /// Инициализация регулярных выражений
        /// </summary>
        private void InitializeTokenRegex()
        {
            string domensRegex = GetRegexStringForCollection( DictionaryType.Domens );
            AddNewRegex( TokenType.Email, string.Format( "[A-Za-z][-A-Za-z_0-9\\. ]+@[A-Za-z ]+\\.[ ]*({0})\\b", domensRegex ) );
            AddNewRegex( TokenType.URL, string.Format( "([-\\.A-Za-z0-9:\\/\\\\]+[\\.][ ]*({0})(\\S*[A-Za-z\\/0-9%])*)|([-А-Яа-я0-9]+)[\\.][ ]*({0})\\b", domensRegex ) );
            AddNewRegex( TokenType.Measure, string.Format( "(({0}\\.[ ]*\\/{0})|({1}\\.[ ]*\\/{1}))\\.", "[A-Za-z]+", "[А-Яа-я]+" ) );
            AddNewRegex( TokenType.NumberSeparator, "[0-9][\\.,:][0-9]" );
            AddNewRegex( TokenType.Smile, GetRegexStringForCollection( DictionaryType.Smiles, SmileConverter ) );
        }

        #region [.regex converters.]
        /// <summary>
        /// Конвертер смайлика
        /// </summary>
        /// <param name="value">смайлик</param>
        /// <returns>сконвертированный смайлик</returns>
        private string SmileConverter( string smile )
        {
            return smile + "+";
        }

        /// <summary>
        /// Конвертер сокращений
        /// </summary>
        /// <param name="value">сокращение</param>
        /// <returns>сконвертированное сокращение</returns>
        private string ReductionConverter( string reduction )
        {
            if ( reduction.EndsWith( "." ) )
                return reduction.Insert( reduction.Length - ("\\.").Length, "[ ]*" );
            return reduction;
        }
        #endregion

        /// <summary>
        /// Добавление нового регулярного выражения
        /// </summary>
        /// <param name="type">тип токена</param>
        /// <param name="regexString">текст регулярного выражения</param>
        private void AddNewRegex( TokenType type, string regexString )
        {
            _tokenRegex.Add( type, new Regex( regexString ) );
        }

        /// <summary>
        /// Получение регулярного выражению по словарю
        /// </summary>
        /// <param name="type">тип словаря</param>
        /// <param name="converter">конвертер для преобразования каждого элемента коллекции</param>
        /// <returns>регулярное выражение</returns>
        private string GetRegexStringForCollection( DictionaryType type, Utils.RegexHelper.StringToRegexConverter converter = null )
        {
            return Utils.RegexHelper.GetVariantsFromCollection( Dictionaries.GetDictionaryElements( type ), converter );
        }
        #endregion

        /// <summary>
        /// Выделение токенов заданного типа из текста
        /// </summary>
        /// <param name="text">текст</param>
        /// <param name="type">тип токенов</param>
        /// <returns>выделенные токены</returns>
        public Token[] SelectTokens( string text, TokenType type )
        {
            return _tokenRegex[ type ]
                .Matches( text )
                .Cast<Match>()
                .Select( _ => new Token() { Index = _.Index, Length = _.Value.TrimEnd().Length } ).ToArray();
        }

        /// <summary>
        /// Получение токенов, содержащих заданные точки, из словаря заданного типа
        /// </summary>
        /// <param name="text">текст</param>
        /// <param name="type">тип словаря</param>
        /// <param name="ends">точки</param>
        /// <returns>выделеннеы токены</returns>
        public Token[] SelectTokensContainsEnd( string text, DictionaryType type, IEnumerable<PotencialSentenceEnd> ends )
        {
            List<Token> result = new List<Token>();
            foreach ( var end in ends )
            {
                string previousWord = text.GetWordBeforeCurrentPosition( end.Index );
                if ( string.IsNullOrEmpty( previousWord ) )
                    continue;
                string reduction = previousWord + text.Substring( end.Index, end.End - end.Index );
                string[] reductionsFromDictionary = Filter( Dictionaries.GetDictionaryElements( type ), reduction );
                if ( reductionsFromDictionary.Any() )
                {
                    string previousText = text.GetPreviousTextToSpace( end.End - reduction.Length );
                    Regex regex = new Regex( Utils.RegexHelper.GetVariantsFromCollection( reductionsFromDictionary, ReductionConverter ) );
                    Match match = regex.Match( text, end.End - reduction.Length - previousText.Length );
                    if ( match.Success && (match.Index < end.Index) )
                        result.Add( new Token() { Index = match.Index, Length = match.Value.TrimEnd().Length } );
                }
            }
            return result.ToArray();
        }

        /// <summary>
        /// Выделение токена в конце текста
        /// </summary>
        /// <param name="text">текст</param>
        /// <param name="type">тип словаря</param>
        /// <returns>токен</returns>
        public Token? SelectTokenFromTextEnd( string text, DictionaryType type )
        {
            if ( string.IsNullOrEmpty( text ) )
                return null;
            StringBuilder reduction = new StringBuilder( text.GetWordBeforeCurrentPosition( text.Length - 1 ) );
            reduction.Append( text.Last() );

            bool isSpace = false;
            int index = text.Length - reduction.Length;
            Token? result = null;
            List<string> possibleReductions = Dictionaries.GetDictionaryElements( type ).ToList();
            while ( possibleReductions.Any() && (index > 1) )
            {
                if ( !isSpace )
                {
                    string fullReduction = Filter( possibleReductions, reduction.ToString() );
                    if ( !string.IsNullOrEmpty( fullReduction ) )
                        result = new Token() { Index = index, Length = text.Length - index };
                }
                isSpace = false;
                --index;
                if ( text[ index ].IsSpace() )
                    isSpace = true;
                else
                    reduction.Insert( 0, text[ index ] );
            }
            return result;
        }

        /// <summary>
        /// Фильтрация коллекции по фильтру
        /// </summary>
        /// <param name="source">коллекция</param>
        /// <param name="filter">фильтр</param>
        /// <returns>результат</returns>
        private string Filter( List<string> source, string filter )
        {
            string result = null;
            for ( int i = 0; i < source.Count; ++i )
            {
                if ( !source[ i ].EndsWith( filter.ToString() ) )
                {
                    source.RemoveAt( i );
                    --i;
                }
                else if ( source[ i ].Length == filter.Length )
                {
                    result = source[ i ];
                    source.RemoveAt( i );
                    --i;
                }
            }
            return result;
        }

        /// <summary>
        /// Фильтрация исходной коллекции: каждый элемент коллекции должен начинаться с заданной строки
        /// </summary>
        /// <param name="source">коллекции</param>
        /// <param name="filter">строка-фильтр</param>
        /// <returns>отфильтрованная коллекция</returns>
        private string[] Filter( IEnumerable<string> source, string filter )
        {
            return source.Where( _ => _.Contains( filter ) ).ToArray();
        }
    }
}
