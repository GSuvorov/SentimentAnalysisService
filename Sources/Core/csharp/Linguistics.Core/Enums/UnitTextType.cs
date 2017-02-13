using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if WITH_OM_TM
namespace Lingvistics
#else
namespace Linguistics.Core
#endif
{
	/// <summary>
	/// Имя узла в xml
	/// </summary
	#if WITH_OM_TM
	public enum NodeName
	#else
	public enum UnitTextType
	#endif
	{
		/// <summary>
		/// Вспомогательные сущности: предлог, наречие, предикатив
		/// </summary>
		CET,
		/// <summary>
		/// Сущность
		/// </summary>
		ENTITY,
		/// <summary>
		/// Предложение
		/// </summary>
		SENT,
		/// <summary>
		/// Подпредложение
		/// </summary>
		SUB_SENT,
		/// <summary>
		/// внутренний элемент сущностей
		/// </summary>
		T,
		/// <summary>
		/// весь текст
		/// </summary>
		TEXT,
		/// <summary>
		/// Подтема
		/// </summary>
		THEME,
		/// <summary>
		/// Формы глагола: глагол, причастие и деепричастие
		/// </summary>
		VERB,
		/// <summary>
		/// Неразмеченный текст
		/// </summary>
		U
	}
}
