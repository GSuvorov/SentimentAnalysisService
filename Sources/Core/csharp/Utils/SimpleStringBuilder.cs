using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Utils
{
    /// <summary>
    /// Класс для построения строк в нижнем регистре и без повторяющихся пробелов
    /// </summary>
#if UTILS_INTERNAL
    internal class SimpleStringBuilder
#else
	public class SimpleStringBuilder
#endif
    {
        /// <summary>
        /// Строитель
        /// </summary>
        private StringBuilder _builder;

        /// <summary>
        /// Таблица сдвигов
        /// </summary>
        private StairTableShift _tableShift;

        /// <summary>
        /// Сдвиг
        /// </summary>
        private int _shift;

        /// <summary>
        /// Встретился пробел
        /// </summary>
        private bool _isSpace;

        #region [.ctor().]
        public SimpleStringBuilder()
        {
            Initialization();
        }

        public SimpleStringBuilder( string text )
        {
            Initialization();
            Append( text );
        }

        private void Initialization()
        {
            _builder = new StringBuilder();
            _tableShift = new StairTableShift();
            _isSpace = true;
        }
        #endregion

        /// <summary>
        /// Значение в нижнем регистре
        /// </summary>
        public string LowerValue { get { return _builder.ToString().ToLower(); } }

        /// <summary>
        /// Действительное значение
        /// </summary>
        public string TrueValue { get { return _builder.ToString(); } }

        /// <summary>
        /// Длина
        /// </summary>
        public int Length { get { return _builder.Length + ((_builder.Length != 0) && _isSpace ? 1 : 0); } }

        /// <summary>
        /// Добавление заданной строки
        /// </summary>
        /// <param name="value">строка</param>
        /// <returns>обновленный объект</returns>
        public SimpleStringBuilder Append( string value )
        {
            foreach ( char symbol in value )
                Append( symbol );
            return this;
        }

        /// <summary>
        /// Добавление заданного символа
        /// </summary>
        /// <param name="value">символ</param>
        /// <returns>обновленный объект</returns>
        public void Append( char symbol )
        {
            if ( Char.IsWhiteSpace( symbol ) )
            {
                if ( _isSpace )
                    ++_shift;
                _isSpace = true;
            }
            else
            {
                if ( _isSpace && (_builder.Length != 0) )
                    _builder.Append( " " );
                _tableShift.UpdateShift( _builder.Length, _shift );
                _builder.Append( symbol );
                _isSpace = false;
            }
        }

        /// <summary>
        /// Очистка строки
        /// </summary>
        public void Clear()
        {
            _tableShift.Clear();
            _builder.Clear();
            _isSpace = false;
        }

        /// <summary>
        /// Получение реальной позиции для заданной
        /// </summary>
        /// <param name="position">заданная позиция</param>
        /// <returns>реальная позиция</returns>
        public int GetRealPosition( int position )
        {
            return position + _tableShift.GetShift( position );
        }
    }
}
