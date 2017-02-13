using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TextParsing
{
    /// <summary>
    /// Класс дял выделения предложений из параграфа
    /// </summary>
    internal class SentenceSelector
    {
        /// <summary>
        /// Выделитель токенов
        /// </summary>
        private TokenSelector _tokenSelector;

        #region [.ctor().]
        public SentenceSelector( TokenSelector selector )
        {
            _tokenSelector = selector;
        }
        #endregion

        /// <summary>
        /// Выделение предложений из текста параграфа
        /// </summary>
        /// <param name="text">текст параграфа</param>
        /// <param name="isBlogText">флаг, что парсится блоговский текст</param>
        /// <returns>список предложений</returns>
        public List<Sentence> Select( string paragraph, bool isBlogText )
        {
            var paragraphMap = new ParagraphMap( _tokenSelector, paragraph, PotencialSentenceEnd.Select( paragraph ) );
            Filter( paragraphMap );
            var sentenceEnds = paragraphMap.SentenceEndPositions().ToList();
            if ( isBlogText )
            {
                sentenceEnds = sentenceEnds
                    .Union( _tokenSelector.SelectTokens( paragraph, TokenType.Smile ).Select( _ => new PotencialSentenceEnd( _.Index, _.Index + _.Length ) ) )
                    .OrderBy( _ => _.Index )
                    .ToList();
            }
            return CreateSentences( paragraphMap.Text, sentenceEnds );
        }

        /// <summary>
        /// Создание предложений по разделителям в схеме параграфа
        /// </summary>
        /// <returns>список предложений</returns>
        private List<Sentence> CreateSentences( string text, IEnumerable<PotencialSentenceEnd> sentenceEnds )
        {
            List<Sentence> result = new List<Sentence>();

            int sentenceStartPosition = 0;
            foreach ( var endPosition in sentenceEnds )
            {
                AddSentence( result, new Sentence( text, sentenceStartPosition, endPosition.End ) );
                sentenceStartPosition = endPosition.End;
            }
            AddSentence( result, new Sentence( text, sentenceStartPosition, text.Length ) );

            return result;
        }

        /// <summary>
        /// Добавление предложения в коллекцию
        /// </summary>
        /// <param name="sentences">список предложений</param>
        /// <param name="newSentence">новое предложение</param>
        private void AddSentence( List<Sentence> sentences, Sentence newSentence )
        {
            if ( sentences.Any() )
            {
                Sentence last = sentences.LastOrDefault();
                if ( string.IsNullOrEmpty( newSentence.Text.Trim() ) || string.IsNullOrEmpty( last.Text.Trim() ) )
                {
                    last.AppendNextPart( newSentence );
                    return;
                }
            }
            sentences.Add( newSentence );
        }

        /// <summary>
        /// Фильтрация схемы параграфа
        /// </summary>
        private void Filter( ParagraphMap paragraphMap )
        {
            paragraphMap.FilterPositionsByTokens( TokenType.Email );
            paragraphMap.FilterPositionsByTokens( TokenType.URL );
            paragraphMap.FilterPositionsByTokens( TokenType.NumberSeparator );
            paragraphMap.FilterPositionsByMeasure();
            paragraphMap.FilterPositionsByDomen();
            //paragraphMap.FilterPositionsInQuotes();
            paragraphMap.FilterPositionsBySuspensionPoints();
            paragraphMap.FilterPositionsByInitials();
            paragraphMap.FilterPositionsByReductions();
            paragraphMap.FilterPositionsByYandexServices();
            paragraphMap.FilterPositionsInSuccesion();
        }
    }
}
