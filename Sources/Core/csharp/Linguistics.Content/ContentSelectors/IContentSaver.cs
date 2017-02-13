using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core.ContentSelectors
{
	/// <summary>
	/// Интерфейс для доступа к дочерним контентам
	/// </summary>
	interface IChildContentSaver
	{
		/// <summary>
		/// Сохранение заданного контента
		/// </summary>
		/// <param name="content">контент</param>
		/// <param name="position">позиция</param>
		void Save(ChildContent content, int position);

		/// <summary>
		/// Очистка текущего сдвига
		/// </summary>
		void ClearShift();

		/// <summary>
		/// Получение реальной позиции (с учетом всех выделенных и убранных контентов) для заданной позиции
		/// </summary>
		/// <param name="currentPosition">заданная позиция</param>
		/// <returns>реальная позиция</returns>
		int GetRealPosition(int currentPosition);

		/// <summary>
		/// Получение сдвига реальной позиции с учетом всех выделенных и убранных контентов
		/// </summary>
		/// <param name="realPosition">реальная позиция</param>
		/// <returns>сдвиг</returns>
		int GetRealPositionShift(int realPosition);
	}
}
