using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace TextParsing
{
    /// <summary>
    /// Класс для выделения параграфов из текста
    /// </summary>
    internal class ParagraphSelector
    {
        /// <summary>
        /// Граница параграфа
        /// </summary>
        private static readonly Regex _paragraphSeparator = new Regex( "[\n\r]+" );

        /// <summary>
        /// Нумерация
        /// </summary>
        private static readonly Regex _numbering = new Regex( "\\s*\\d+\\.\\s*[A-ZА-Я]" );

        /// <summary>
        /// Выделитель токенов
        /// </summary>
        private TokenSelector _tokenSelector;

        #region [.ctor().]
        static ParagraphSelector()
        {
        }

        public ParagraphSelector( TokenSelector selector )
        {
            _tokenSelector = selector;
        }
        #endregion

        /// <summary>
        /// Выделение параграфов из текста
        /// </summary>
        /// <param name="text">текст</param>
        /// <param name="isBlogText">флаг, что парсится блоговский текст</param>
        /// <returns>список параграфов</returns>
        public List<Paragraph> Select( string text, bool isBlogText )
        {
            /// начальная позиция для нового параграфа
            int newParagraphStartPos = 0;
            /// флаг, что между параграфами нет пустых строк
            bool isPossibleConcat = false;

            /// найденные разделители параграфов
            var paragraphSeparatorMatches = _paragraphSeparator.Matches( text );
            var result = new List<Paragraph>( paragraphSeparatorMatches.Count + 1 );
            foreach ( Match match in paragraphSeparatorMatches )
            {
                AddParagraph( result, new Paragraph( text, newParagraphStartPos, match.Index + match.Length ), !isBlogText && isPossibleConcat );
                newParagraphStartPos = match.Index + match.Length;
                isPossibleConcat = IsPossibleParagraphConcat( match.Value );
            }
            AddParagraph( result, new Paragraph( text, newParagraphStartPos, text.Length ), !isBlogText && isPossibleConcat );

            return result;
        }

        /// <summary>
        /// Проверка возможности объединения параграфов по их разделителю
        /// </summary>
        /// <param name="separator">разделитель</param>
        /// <returns>результат проверки</returns>
        private bool IsPossibleParagraphConcat( string separator )
        {
            return separator.Length <= 2;
        }

        /// <summary>
        /// Добавление параграфа в коллекцию
        /// </summary>
        /// <param name="paragraphs">список параграфов</param>
        /// <param name="newParagraph">новый параграф</param>
        /// <param name="isPossibleConcat">true - проверять на возможность объединения параграфов</param>
        private void AddParagraph( List<Paragraph> paragraphs, Paragraph newParagraph, bool isPossibleConcat )
        {
            bool isConcat = false;
            Paragraph last = paragraphs.LastOrDefault();
            if ( last != null )
            {
                isConcat = string.IsNullOrEmpty( last.Text.Trim() ) || string.IsNullOrEmpty( newParagraph.Text.Trim() );
                if ( isPossibleConcat && !isConcat )
                /// анализ возможности соединения параграфов
                {
                    if ( IsPotencialParagraphEnd( last ) )
                        isConcat = !IsParagraphEndWithSentenceEnd( last, newParagraph );
                    else
                        isConcat = TestParagraphConcatWithoutSentEnd( last, newParagraph );
                }
            }

            if ( isConcat )
                last.AppendNextPart( newParagraph );
            else
                paragraphs.Add( newParagraph );
        }

        /// <summary>
        /// Проверка, что параграф заканчивается на потенциальный конец предложения
        /// </summary>
        /// <param name="paragraph">параграф</param>
        /// <returns>результат проверки</returns>
        private bool IsPotencialParagraphEnd( Paragraph paragraph )
        {
            return PotencialSentenceEnd.Select( paragraph.Text.Trim().LastOrDefault().ToString() ).Any();
        }

        /// <summary>
        /// Проверка, что параграф заканчивается на конец предложения
        /// </summary>
        /// <param name="paragraph">проверяемый параграф</param>
        /// <param name="nextParagraph">параграф, следующий за проверяемым</param>
        /// <returns>результат проверки</returns>
        private bool IsParagraphEndWithSentenceEnd( Paragraph paragraph, Paragraph nextParagraph )
        {
            var match = _numbering.Match( nextParagraph.Text );
            if ( match.Success && (match.Index == 0) )
                return true;

            List<PotencialSentenceEnd> endPosition = new List<PotencialSentenceEnd>( 1 );
            string trimedText = paragraph.Text.TrimEnd();
            endPosition.Add( new PotencialSentenceEnd( trimedText.Length - 1, trimedText.Length ) );
            ParagraphMap map = new ParagraphMap( _tokenSelector, paragraph.Text + nextParagraph.Text, endPosition );
            var tokenBeforeNoProper = _tokenSelector.SelectTokenFromTextEnd( trimedText, DictionaryType.BeforeNoProper );
            map.FilterPositionsByReductions( ToArray( null ), ToArray( tokenBeforeNoProper ) );
            return map.SentenceEndPositions().Any();
        }

        /// <summary>
        /// Преобразование возможного токена в массив
        /// </summary>
        /// <param name="token">возможный токен</param>
        /// <returns>массив</returns>
        private Token[] ToArray( Token? token )
        {
            if ( token.HasValue )
                return new Token[] { token.Value };
            return new Token[] { };
        }

        /// <summary>
        /// Проверка, возможно ли соединение параграфов
        /// Считается, что разделителя предложений нет на границе
        /// </summary>
        /// <param name="previousParagraph">предыдущий параграф</param>
        /// <param name="currentParagraph">текущий параграф</param>
        /// <returns>результат проверки</returns>
        private bool TestParagraphConcatWithoutSentEnd( Paragraph previousParagraph, Paragraph currentParagraph )
        {
            char first = currentParagraph.Text.Trim().FirstOrDefault();
            char last = previousParagraph.Text.Trim().LastOrDefault();
            if ( last.IsTire() || (Char.IsDigit( first ) && !Char.IsDigit( last )) || first.IsLowerLetter() )
                return true;
            else if ( first.IsTire() )
            {
                char symbolAfterTire = currentParagraph.Text.Trim().Substring( 1 ).Trim().FirstOrDefault();
                return symbolAfterTire.IsLowerLetter();
            }
            else
                return false;
        }
    }
}
