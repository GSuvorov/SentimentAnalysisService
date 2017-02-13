using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using Utils;

namespace TextParsing
{
    /// <summary>
    /// Схема параграфа, на которой отмечены концы предложений
    /// </summary>
    internal class ParagraphMap
    {
        /// <summary>
        /// Регистр
        /// </summary>
        private enum Register
        {
            /// <summary>
            /// только маленькие буквы
            /// </summary>
            AllLower,
            /// <summary>
            /// только большие буквы
            /// </summary>
            AllUpper,
            /// <summary>
            /// Первая заглавная, остальные - маленькие
            /// </summary>
            FirstUpper,
            /// <summary>
            /// смешанный регистр
            /// </summary>
            Mixed,
        }

        #region [.constants.]
        /// <summary>
        /// скобки
        /// </summary>
        private static readonly string Brackets = "([{}])";
        /// <summary>
        /// некоторые знаки препинания
        /// </summary>
        private static readonly string Punctuation = ",:;-–—+%";
        /// <summary>
        /// открывающие кавычки
        /// </summary>
        private static readonly string OpenQuotes = "\"“«" + (char) 0x201C;
        /// <summary>
        /// закрывающие кавычки
        /// </summary>
        private static readonly string CloseQuotes = "\"”»" + (char) 0x201D;
        /// <summary>
        /// максимальная длина инициала
        /// </summary>
        private static readonly int MaxInitalLength = 2;
        /// <summary>
        /// число цифр в году
        /// </summary>
        private static readonly int YearSize = 4;
        /// <summary>
        /// алфавит одиночных римских цифр
        /// </summary>
        private static readonly string SingleRomanNumberAlphabet = "IVX";
        /// <summary>
        /// алфавит римских цифр
        /// </summary>
        private static readonly string RomanNumberAlphabet = "IVXLC";
        /// <summary>
        /// Русский алфавит
        /// </summary>
        private static readonly string RussianUpperAlphabet = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
        #endregion

        /// <summary>
        /// Выделитель токенов
        /// </summary>
        private TokenSelector _tokenSelector;

        /// <summary>
        /// Текст
        /// </summary>
        public string Text { get; set; }
        /// <summary>
        /// Позиции концов предложений
        /// </summary>
        private List<PotencialSentenceEnd> _sentenceEndPositions;
        /// <summary>
        /// Точки при инициалах
        /// </summary>
        private List<int> _initialPoints;

        #region [.ctor().]
        public ParagraphMap( TokenSelector selector, string text, IEnumerable<PotencialSentenceEnd> sentenceEndPositions )
        {
            _tokenSelector = selector;
            Text = text;
            _sentenceEndPositions = new List<PotencialSentenceEnd>( sentenceEndPositions );
            _initialPoints = new List<int>();
        }
        #endregion

        /// <summary>
        /// Получение списка концов предложений
        /// </summary>
        /// <returns>список концов предложений</returns>
        public ReadOnlyCollection<PotencialSentenceEnd> SentenceEndPositions()
        {
            return _sentenceEndPositions.AsReadOnly();
        }

        #region [.filtering.]
        /// <summary>
        /// Фильтрация позиций концов предложений, попадающих в названия сервисов Яндекса
        /// </summary>
        public void FilterPositionsByYandexServices()
        {
            FilterPositions( IsYandexServicePoint );
        }

        /// <summary>
        /// Фильтрация позиций концов предложений по указанному типу токенов
        /// </summary>
        /// <param name="type">тип токенов для фильтрации</param>
        public void FilterPositionsByTokens( TokenType type )
        {
            FilterPositions( _tokenSelector.SelectTokens( Text, type ) );
        }

        /// <summary>
        /// Фильтрация позиций концов предложений по инициалам
        /// </summary>
        public void FilterPositionsByInitials()
        {
            FilterPositions( IsInitialPoint );
        }

        /// <summary>
        /// Фильтрация позиций концов предложений по многоточию
        /// </summary>
        public void FilterPositionsBySuspensionPoints()
        {
            FilterPositions( IsMathematicalSuspensionPoint );
            FilterPositions( IsScrappyCitation );
        }

        /// <summary>
        /// Фильтрация позиций концов предложений по единицам измерения
        /// </summary>
        public void FilterPositionsByMeasure()
        {
            var measureTokens = _tokenSelector.SelectTokens( Text, TokenType.Measure ).Select( _ =>
                {
                    if ( Char.IsUpper( Text.GetNextCharAfterSpace( _.Index + _.Length ) ) )
                        return new Token { Index = _.Index, Length = _.Length - 1 };
                    else
                        return _;
                } ).ToArray();
            FilterPositions( measureTokens );
        }

        /// <summary>
        /// Фильтрация позиций концов предложений по спискам сокращений
        /// </summary>
        /// <param name="reductionsBeforeProperOrNumber">сокращения, за которыми идут имена собственные или цифры</param>
        /// <param name="reductionsBeforeNoProper">сокращения, за которыми не идут имена собственные</param>
        public void FilterPositionsByReductions( IEnumerable<Token> reductionsBeforeProperOrNumber, IEnumerable<Token> reductionsBeforeNoProper )
        {
            List<Token> listBeforeProperOrNumber = reductionsBeforeProperOrNumber.ToList();
            List<Token> listBeforeNoProper = reductionsBeforeNoProper.ToList();
            ResolutionIntersection( listBeforeProperOrNumber, listBeforeNoProper );
            FilterPositionsInsideTokens( listBeforeProperOrNumber );
            FilterPositionsInsideTokens( listBeforeNoProper );

            FilterPositions( listBeforeProperOrNumber.Where( _ => IsNextProperOrNumber( _ ) ) );
            FilterPositions( listBeforeNoProper.Where( _ => IsNextNotProper( _ ) ) );
        }

        /// <summary>
        /// Фильтрация позиций концов предложений по спискам сокращений
        /// </summary>
        public void FilterPositionsByReductions()
        {
            FilterPositionsByReductions
                (
                    _tokenSelector.SelectTokensContainsEnd( Text, DictionaryType.BeforeProperOrNumber, _sentenceEndPositions ),
                    _tokenSelector.SelectTokensContainsEnd( Text, DictionaryType.BeforeNoProper, _sentenceEndPositions )
                );
        }

        /// <summary>
        /// Фильтрация позиций концов предложений, идущих подряд
        /// </summary>
        public void FilterPositionsInSuccesion()
        {
            for ( int i = 1; i < _sentenceEndPositions.Count; ++i )
            {
                PotencialSentenceEnd current = _sentenceEndPositions[ i ];
                PotencialSentenceEnd previous = _sentenceEndPositions[ i - 1 ];
                if ( string.IsNullOrEmpty( Text.Substring( previous.End, current.Index - previous.End ).Trim() ) )
                {
                    --i;
                    _sentenceEndPositions.RemoveAt( i );
                }
            }
        }

        /// <summary>
        /// Фильтрация концов предложений в кавычках
        /// </summary>
        public void FilterPositionsInQuotes()
        {
            FilterPositions( GetQuoteTexts() );
        }

        /// <summary>
        /// Фильтрация концов предложений по наличию домена
        /// </summary>
        public void FilterPositionsByDomen()
        {
            FilterPositions( IsDomenPoint );
        }
        #endregion

        #region [private members]
        /// <summary>
        /// Получение текстов в кавычках
        /// </summary>
        /// <returns>тексты в кавычках</returns>
        private Token[] GetQuoteTexts()
        {
            List<Token> result = new List<Token>();
            int closeQuoteCount = 0;
            int openQuoteIndex = -1;
            int quoteTypeIndex = -1;
            for ( int i = 0; i < Text.Length; ++i )
            {
                int quoteType = OpenQuotes.IndexOf( Text[ i ] );
                if ( quoteType > 0 )
                {
                    if ( closeQuoteCount == 0 )
                    {
                        openQuoteIndex = i;
                        quoteTypeIndex = quoteType;
                    }
                    if ( quoteType == quoteTypeIndex )
                        ++closeQuoteCount;
                }
                else if ( closeQuoteCount > 0 )
                {
                    quoteType = CloseQuotes.IndexOf( Text[ i ] );
                    if ( (quoteType > 0) && (quoteType == quoteTypeIndex) )
                        --closeQuoteCount;
                    if ( closeQuoteCount == 0 )
                        result.Add( new Token() { Index = openQuoteIndex, Length = i - openQuoteIndex } );
                }
            }
            return result.ToArray();
        }

        /// <summary>
        /// Проверка, что после токена идет имя собственное или число: буква, цифра, кавычка или скобка
        /// </summary>
        /// <param name="token">токен для проверки</param>
        /// <returns>результат проверки</returns>
        private bool IsNextProperOrNumber( Token token )
        {
            char nextChar = Text.GetNextCharAfterSpace( token.Index + token.Length - 1 );
            bool isNextLetter = Char.IsLetter( nextChar );
            if ( IsReductionForNumber( token ) && isNextLetter && Char.IsUpper( nextChar ) )
                return false;
            return isNextLetter || Char.IsDigit( nextChar ) || IsQuote( nextChar ) || IsBracket( nextChar ) || IsPunct( nextChar );
        }

        /// <summary>
        /// Проверка, что заданный токен - сокращение для числа
        /// </summary>
        /// <param name="token">токен</param>
        /// <returns>результат проверки</returns>
        private bool IsReductionForNumber( Token token )
        {
            string value = Text.Substring( token.Index, token.Length ).ToLower();
            return IsMeter( value, token.Index ) || IsYear( value, token.Index ) || IsSecond( value, token.Index );
        }

        /// <summary>
        /// Проверка, что заданное слово - сокращение для метров
        /// </summary>
        /// <param name="value">слово</param>
        /// <param name="position">позиция</param>
        /// <returns>результат проверки</returns>
        private bool IsMeter( string value, int position )
        {
            return (value == "м.") && Char.IsDigit( Text.GetPreviousCharAfterSpace( position ) );
        }

        /// <summary>
        /// Проверка, что заданное слово - сокращение для года
        /// </summary>
        /// <param name="value">слово</param>
        /// <param name="position">позиция</param>
        /// <returns>результат проверки</returns>
        private bool IsYear( string value, int position )
        {
            string previousWord = Text.GetWordBeforeCurrentPosition( position, true );
            bool isWord = previousWord.Any( _ => Char.IsLetter( _ ) );
            return (value == "г.") && !isWord && Char.IsDigit( Text.GetPreviousCharAfterSpace( position ) );
        }

        /// <summary>
        /// Проверка, что заданное слово - сокращение для секунд
        /// </summary>
        /// <param name="value">слово</param>
        /// <param name="position">позиция</param>
        /// <returns>результат проверки</returns>
        private bool IsSecond( string value, int position )
        {
            if ( value == "с." )
            {
                string word = Text.GetWordBeforeCurrentPosition( position, true );
                return !word.Any( _ => !Char.IsDigit( _ ) ) && (word.Length != YearSize);
            }
            return false;
        }

        /// <summary>
        /// Проверка, что после токена идет не имя собственное: маленькая буква, цифра или скобка
        /// </summary>
        /// <param name="token">токен для проверки</param>
        /// <returns>результат проверки</returns>
        private bool IsNextNotProper( Token token )
        {
            char nextChar = Text.GetNextCharAfterSpace( token.Index + token.Length - 1 );
            return (Char.IsLetter( nextChar ) && Char.IsLower( nextChar )) || Char.IsDigit( nextChar ) || IsBracket( nextChar ) || IsPunct( nextChar );
        }

        /// <summary>
        /// Разрешение пересечения двух коллекций.
        /// Из пересекающихся элементов выбирается тот, что длиннее
        /// </summary>
        /// <param name="first">первая коллекция</param>
        /// <param name="second">вторая коллекция</param>
        private void ResolutionIntersection( List<Token> first, List<Token> second )
        {
            int firstIndex = 0;
            int secondIndex = 0;
            for ( ; (firstIndex < first.Count) && (secondIndex < second.Count); )
            {
                Token firstToken = first[ firstIndex ];
                Token secondToken = second[ secondIndex ];
                if ( firstToken.Index > secondToken.Index + secondToken.Length )
                    ++secondIndex;
                else if ( (firstToken.Index > secondToken.Index) || (firstToken.Index + secondToken.Length > secondToken.Index) )
                /// пересечение
                {
                    if ( firstToken.Length >= secondToken.Length )
                        second.RemoveAt( secondIndex );
                    else
                        first.RemoveAt( firstIndex );
                }
                else
                    ++firstIndex;
            }
        }

        /// <summary>
        /// Выбор из входной коллекции только тех значений, предыдущий символ для которых не буква
        /// </summary>
        /// <param name="tokens">входная коллекция</param>
        /// <returns>результирующая коллекция</returns>
        private IEnumerable<Token> FilterByPreviousCharNotLetter( IEnumerable<Token> tokens )
        {
            return tokens.Where( _ => !Char.IsLetter( Text.GetPreviousChar( _.Index ) ) );
        }

        /// <summary>
        /// Проверка, что заданный потенциальный конец предложения является многоточием в математическом выражении
        /// </summary>
        /// <param name="end">потенциальный конец предложения</param>
        /// <returns>результат проверки</returns>
        private bool IsMathematicalSuspensionPoint( PotencialSentenceEnd end )
        {
            string value = Text.Substring( end.Index, end.End - end.Index );
            if ( (value == "...") || (value == "…") )
            {
                char previousChar = Text.GetPreviousCharAfterSpace( end.Index );
                if ( Char.IsDigit( previousChar ) || previousChar.EqualsAny( ',', '+', '*', '-', '=', '±', '≥', '≤' ) )
                    return true;
            }
            return false;
        }

        /// <summary>
        /// Проверка, что заданный потенциальный конец предложения относится к обрывочной ситате
        /// </summary>
        /// <param name="end">потенциальный конец предложения</param>
        /// <returns>результат проверки</returns>
        private bool IsScrappyCitation( PotencialSentenceEnd end )
        {
            string endText = Text.Substring( end.Index, end.End - end.Index );
            return ((endText == "…") || (endText == "...")) &&
                (OpenQuotes.Contains( Text.GetPreviousCharAfterSpace( end.Index ) ) ||
                CloseQuotes.Contains( Text.GetNextCharAfterSpace( end.End - 1 ) ));
        }

        /// <summary>
        /// Проверка, что заданный потенциальный конец предложения является точкой при домене
        /// </summary>
        /// <param name="end">потенциальный конец предложения</param>
        /// <returns>результат проверки</returns>
        private bool IsDomenPoint( PotencialSentenceEnd end )
        {
            if ( IsPoint( end ) )
            {
                string previousWord = Text.GetWordBeforeCurrentPosition( end.Index, true );
                string nextWord = Text.GetWordAfterCurrentPosition( end.Index, true );
                Register previousWordRegister = GetWordRegister( previousWord );
                Register nextWordRegister = GetWordRegister( nextWord );
                if ( IsPointCloseToPreviousWord( end.Index, previousWord ) && IsPointCloseToNextWord( end.End, nextWord ) &&
                    (previousWordRegister == nextWordRegister) && (nextWordRegister != Register.Mixed) &&
                    _tokenSelector.Dictionaries.GetDictionaryElements( DictionaryType.Domens ).Contains( nextWord.ToLower() ) )
                {
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// Получение ресгитра слова
        /// </summary>
        /// <param name="word">слово</param>
        /// <returns>регистр</returns>
        private Register GetWordRegister( string word )
        {
            Register result = Register.Mixed;
            for ( int i = 0; i < word.Length; ++i )
            {
                if ( Char.IsUpper( word[ i ] ) )
                {
                    if ( (result == Register.AllLower) || (result == Register.FirstUpper) )
                    {
                        result = Register.Mixed;
                        break;
                    }
                    else
                        result = Register.AllUpper;
                }
                else if ( Char.IsLower( word[ i ] ) )
                {
                    if ( (result == Register.AllUpper) && (i == 1) )
                        result = Register.FirstUpper;
                    else if ( result == Register.AllUpper )
                    {
                        result = Register.Mixed;
                        break;
                    }
                    else if ( result != Register.FirstUpper )
                        result = Register.AllLower;
                }
            }
            return result;
        }

        /// <summary>
        /// Проверка, что заданный потенциальный конец предложения является точкой при инициале
        /// </summary>
        /// <param name="end">потенциальный конец предложения</param>
        /// <returns>результат проверки</returns>
        private bool IsInitialPoint( PotencialSentenceEnd end )
        {
            string initial;
            if ( IsPoint( end ) && GetWordWithFirstUpperLetterBeforePoint( end.Index, MaxInitalLength, out initial ) )
            {
                bool isDegree = IsDegree( end.Index, Text.GetWordBeforeCurrentPosition( end.Index, true ) );
                bool isInterjection = _tokenSelector.Dictionaries.GetDictionaryElements( DictionaryType.Interjections ).Contains( initial );
                char nextChar = Text.GetNextCharAfterSpace( end.End - 1 );
                if ( !isInterjection && !(IsRomanNumber( initial ) && RussianUpperAlphabet.Contains( nextChar )) && !(isDegree && Char.IsUpper( nextChar )) )
                {
                    _initialPoints.Add( end.Index );
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// Проверка, что заданное слово - римское число
        /// </summary>
        /// <param name="word">слово</param>
        /// <returns>результат проверки</returns>
        private bool IsRomanNumber( string word )
        {
            if ( word.Length == 1 )
                return SingleRomanNumberAlphabet.Contains( word[ 0 ] );
            else
            {
                foreach ( char symbol in word )
                    if ( !RomanNumberAlphabet.Contains( symbol ) )
                        return false;
                return true;
            }
        }

        /// <summary>
        /// Проверка, что заданный поетнциальный конец предложения - точка
        /// </summary>
        /// <param name="potencialSentenceEnd">потенциальный конец предложения</param>
        /// <returns>результат проверки</returns>
        private bool IsPoint( PotencialSentenceEnd potencialSentenceEnd )
        {
            return (potencialSentenceEnd.End - potencialSentenceEnd.Index == 1) && Text[ potencialSentenceEnd.Index ].IsPoint();
        }

        /// <summary>
        /// Проверка, что точка стоит перед именем собственным
        /// </summary>
        /// <param name="pointIndex">индекс точки</param>
        /// <returns>результат проверки</returns>
        private bool IsPointBeforeProper( int pointIndex )
        {
            SimpleStringBuilder builder = new SimpleStringBuilder();
            for ( int i = pointIndex + 1; i < Text.Length; ++i )
            {
                char symbol = Text[ i ];
                if ( Char.IsLetter( symbol ) )
                {
                    if ( Char.IsUpper( symbol ) )
                    {
                        string value = builder.LowerValue.Trim();
                        return string.IsNullOrEmpty( value ) || _tokenSelector.Dictionaries.GetDictionaryElements( DictionaryType.FamilyPrefixes ).Contains( value );
                    }
                }
                else if ( !symbol.IsSpace() )
                    break;
                builder.Append( symbol );
            }
            return false;
        }

        /// <summary>
        /// Получение слова перед точкой с проверкой, что оно не более заданной длины и начинается с большой буквы
        /// </summary>
        /// <param name="pointIndex">индекс точки</param>
        /// <param name="maxWordLength">максимальная длина слова</param>
        /// <param name="word">полученное слова</param>
        /// <returns>
        /// true - слово получено
        /// false - слово не удовлетворяет критериям
        /// </returns>
        private bool GetWordWithFirstUpperLetterBeforePoint( int pointIndex, int maxWordLength, out string word )
        {
            word = Text.GetWordBeforeCurrentPosition( pointIndex, true );
            if ( IsPointCloseToPreviousWord( pointIndex, word ) )
            {
                int startIndex = pointIndex - word.Length;
                if ( word.StartsWith( "-" ) && Text.GetPreviousChar( startIndex ).IsPoint() && _initialPoints.Contains( startIndex - 1 ) )
                    word = word.Substring( 1 );
                bool isGoodLength = (word.Length <= maxWordLength) && (word.Length > 0);
                return isGoodLength && Char.IsUpper( word[ 0 ] ) && word.Skip( 1 ).All( _ => Char.IsLower( _ ) );
            }
            return false;
        }

        /// <summary>
        /// Проверка, что точка стоит вплотную к предыдущему слову
        /// </summary>
        /// <param name="pointPosition">позиция точки</param>
        /// <param name="previousWord">предыдущее слово</param>
        /// <returns>результат проверки</returns>
        private bool IsPointCloseToPreviousWord( int pointPosition, string previousWord )
        {
            #region [.defense.]
            if ( previousWord == null )
                throw new ArgumentNullException( "previousWord" );
            #endregion
            return Text.Substring( pointPosition - previousWord.Length, previousWord.Length ) == previousWord;
        }

        /// <summary>
        /// Проверка, что точка стоит вплотную к следующему слову
        /// </summary>
        /// <param name="pointPosition">позиция точки</param>
        /// <param name="nextWord">следующее слово</param>
        /// <returns>результат проверки</returns>
        private bool IsPointCloseToNextWord( int pointPosition, string nextWord )
        {
            #region [.defense.]
            if ( nextWord == null )
                throw new ArgumentNullException( "nextWord" );
            #endregion
            return Text.Substring( pointPosition, nextWord.Length ) == nextWord;
        }

        /// <summary>
        /// Проверка, что заданный потенциальный конец предложения является точкой при названии сервиса Яндекса
        /// </summary>
        /// <param name="end">потенциальный конец предложения</param>
        /// <returns>результат проверки</returns>
        private bool IsYandexServicePoint( PotencialSentenceEnd end )
        {
            if ( IsPoint( end ) )
            {
                string previousWord = Text.GetWordBeforeCurrentPosition( end.Index, true ).ToUpper();
                return !Char.IsWhiteSpace( Text.GetPreviousChar( end.Index ) ) && ((previousWord == "ЯНДЕКС") || (previousWord == "YANDEX")) &&
                    Char.IsLetter( Text.GetNextChar( end.End - 1 ) );
            }
            return false;
        }

        /// <summary>
        /// Проверка слова перед точкой на наличие символа °
        /// </summary>
        /// <param name="pointIndex"></param>
        /// <param name="wordBeforePoint"></param>
        /// <returns></returns>
        private bool IsDegree( int pointIndex, string wordBeforePoint )
        {
            int index = pointIndex;
            char result = Text.GetPreviousChar( index );
            while ( result.IsSpace() )
                result = Text.GetPreviousChar( --index );
            int potencialDegreeSymbolPosition = index - wordBeforePoint.Length;
            return (potencialDegreeSymbolPosition >= 0) && (Text.GetPreviousCharAfterSpace( potencialDegreeSymbolPosition ) == '°');
        }

        /// <summary>
        /// Фильтрация позиций концов предложений, которые лежат внутри указанных токенов
        /// </summary>
        /// <param name="collection">список токенов</param>
        private void FilterPositionsInsideTokens( List<Token> collection )
        {
            FilterPositions( collection.Select( _ => new Token() { Index = _.Index, Length = _.Length - 1 } ) );
        }

        /// <summary>
        /// Фильтрация позиций концов предложений по предикату
        /// </summary>
        /// <param name="predicate">предикат для отбора фильтруемых концов предложений</param>
        private void FilterPositions( Func<PotencialSentenceEnd, bool> predicate )
        {
            if ( predicate != null )
                FilterPositions( _sentenceEndPositions.Where( _ => predicate( _ ) ).Select( _ => new Token() { Index = _.Index, Length = _.End - _.Index } ).ToArray() );
        }

        /// <summary>
        /// Фильтрация позиций концов предложений, которые пересекаются с указанными токенами
        /// </summary>
        /// <param name="tokens">список токенов</param>
        private void FilterPositions( IEnumerable<Token> tokens )
        {
            int index = 0;
            int length = tokens.Count();
            for ( int i = 0; i < length; ++i )
            {
                var current = tokens.ElementAt( i );
                while ( index < _sentenceEndPositions.Count )
                {
                    if ( _sentenceEndPositions[ index ].End > current.Index + current.Length )
                        break;
                    else if ( _sentenceEndPositions[ index ].Index >= current.Index )
                        _sentenceEndPositions.RemoveAt( index );
                    else
                        ++index;
                }
            }
        }

        /// <summary>
        /// Проверка, что заданный символ - скобка
        /// </summary>
        /// <param name="symbol">символ</param>
        /// <returns>результат проверки</returns>
        private bool IsBracket( char symbol )
        {
            return Brackets.Contains( symbol );
        }

        /// <summary>
        /// Проверка, что заданный символ - допустимый знак препинания
        /// </summary>
        /// <param name="symbol">символ</param>
        /// <returns>результат проверки</returns>
        private bool IsPunct( char symbol )
        {
            return Punctuation.Contains( symbol );
        }

        /// <summary>
        /// Проверка, что заданный символ - кавычка
        /// </summary>
        /// <param name="symbol">символ</param>
        /// <returns>результат проверки</returns>
        private bool IsQuote( char symbol )
        {
            return OpenQuotes.Contains( symbol ) || CloseQuotes.Contains( symbol );
        }
        #endregion
    }
}
