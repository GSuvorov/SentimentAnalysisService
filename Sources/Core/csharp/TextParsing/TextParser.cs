using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;
using System.Threading.Tasks;
using System.Collections.Concurrent;
using System.Threading;

namespace TextParsing
{
    /// <summary>
    /// Класс для парсинга текста
    /// </summary>
    public class TextParser
    {
        /// <summary>
        /// Флаг использования многопоточности
        /// </summary>
        public bool IsMultiThreaded { get; set; }

        /// <summary>
        /// выделитель параграфов
        /// </summary>
        private ParagraphSelector _paragraphSelector;
        /// <summary>
        /// выделитель предложений
        /// </summary>
        private SentenceSelector _sentenceSelector;

        #region [.ctor().]
        public TextParser( string path )
        {
            var tokenSelector = new TokenSelector( path );
            _paragraphSelector = new ParagraphSelector( tokenSelector );
            _sentenceSelector = new SentenceSelector( tokenSelector );
            IsMultiThreaded = true;
        }
        #endregion

        /// <summary>
        /// Пропарсить текст
        /// </summary>
        /// <param name="text">текст</param>
        /// <param name="isBlogText">флаг, что парсится блоговский текст</param>
        /// <returns>коллекция параграфов</returns>
        public List<Paragraph> ParseText( string text, bool isBlogText )
        {
            var paragraphList = _paragraphSelector.Select( text, isBlogText );
            if ( (Environment.ProcessorCount > 1) && IsMultiThreaded )
            {
                Parallel.ForEach( paragraphList, new ParallelOptions() { MaxDegreeOfParallelism = Environment.ProcessorCount },
                    paragraph => paragraph.SetSentences( _sentenceSelector.Select( paragraph.Text, isBlogText ) ) );
            }
            else
            /// однопоточный вариант
            {
                foreach ( Paragraph paragraph in paragraphList )
                {
                    paragraph.SetSentences( _sentenceSelector.Select( paragraph.Text, isBlogText ) );
                }
            }
            return paragraphList.Where( paragraph => paragraph.Sentences.Any() ).ToList();
        }
    }
}
