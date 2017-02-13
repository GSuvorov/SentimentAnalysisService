using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Utils
{
    /// <summary>
    /// класс для хранения сдвигов с использованием ступенчатой таблицы
    /// </summary>
#if UTILS_INTERNAL
    internal class StairTableShift
#else
	public class StairTableShift
#endif
    {
        /// <summary>
        /// пара позиция-сдвиг
        /// </summary>
        private struct ShiftPositionPair
        {
            public int Position { get; set; }
            public int Shift { get; set; }
        }

        /// <summary>
        /// текущий сдвиг
        /// </summary>
        private int _currentShift;
        /// <summary>
        /// список пары позиция-сдвиг
        /// </summary>
        private List<ShiftPositionPair> _shifts;

        #region [.ctor().]
        public StairTableShift()
        {
            _shifts = new List<ShiftPositionPair>();
            Clear();
        }
        #endregion

        /// <summary>
        /// Обновление текущего сдвига заданными параметрами
        /// </summary>
        /// <param name="position">позиция</param>
        /// <param name="shift">сдвиг</param>
        public void UpdateShift( int position, int shift )
        {
            if ( shift != _currentShift )
            {
                _shifts.Add( new ShiftPositionPair() { Position = position, Shift = shift } );
                _currentShift = shift;
            }
        }

        /// <summary>
        /// получение сдвига для заданной позиции
        /// </summary>
        /// <param name="position">позиция</param>
        /// <returns>сдвиг</returns>
        public int GetShift( int position )
        {
            /// используем бинарный поиск
            int first = 0;
            int last = _shifts.Count;
            int middle = 0;
            if ( (_shifts.Count == 0) || (position < _shifts[ 0 ].Position) )
                return 0;
            else if ( _shifts.Last().Position <= position )
                return _shifts.Last().Shift;

            while ( first < last )
            {
                middle = first + (last - first) / 2;
                if ( position == _shifts[ middle ].Position )
                {
                    last = middle + 1;
                    first = last;
                }
                else if ( position < _shifts[ middle ].Position )
                    last = middle;
                else
                    first = middle + 1;
            }
            return _shifts[ last - 1 ].Shift;
        }

        /// <summary>
        /// Очистка
        /// </summary>
        public void Clear()
        {
            _currentShift = 0;
            _shifts.Clear();
        }
    }
}
