using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core.ContentSelectors
{
	/// <summary>
	/// Класс для выделения контента из текста
	/// </summary>
	internal abstract class ContentSelectorBase
	{
		/// <summary>
		/// Сохранитель контента
		/// </summary>
		protected IChildContentSaver _saver;

		/// <summary>
		/// Выделение контентов из заданного текста и сохранение их заданным сохранителем
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="selector">сохранитель</param>
		public void Select(ref string text, IChildContentSaver saver)
		{
			_saver = saver;
			_saver.ClearShift();
			Select(ref text);
			_saver = null;
		}

		/// <summary>
		/// Выделение контентов из заданного текста
		/// </summary>
		/// <param name="text">текст</param>
		protected abstract void Select(ref string text);
	}
}
