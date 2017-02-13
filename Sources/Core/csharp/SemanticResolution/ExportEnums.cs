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
	/// Атрибут цепочки
	/// </summary>
	public enum BlockAttribute
	{
		/// <summary>
		/// список прилагательных через запятую в той форме, в которой они встречаются в нормализованной цепочке
		/// </summary>
		ADJ,
		/// <summary>
		/// атрибут, указывающий на то, что данная сущность является кандидатом для разрешения анафории
		/// </summary>
		CANDIDAT,
		/// <summary>
		/// ссылка на сущность с разрешенными наследниками
		/// </summary>
		EI,
		/// <summary>
		/// новое собранное нормализованное значение
		/// </summary>
		FULLNAME,
		HASH,
		/// <summary>
		/// идентификатор
		/// </summary>
		ID,
		/// <summary>
		/// информация
		/// </summary>
		INFO,
		/// <summary>
		/// язык предложения
		/// </summary>
		LANG,
		/// <summary>
		/// в теге SENT - указание на то, что прямая речь синтаксически связана со следующей прямой речью
		/// в теге ENTITY - ссылка на разрешенную сущность
		/// </summary>
		LINK,
		/// <summary>
		/// морфологические характеристики цепочки
		/// </summary>
		MI,
		/// <summary>
		/// Наличие атрибута указывает на то, что сущность отфильтрована
		/// </summary>
		NOT_THEME,
		/// <summary>
		/// атрибут географии для OrgName
		/// </summary>
		PLACE,
		/// <summary>
		/// в теге SENT - смещение предложения от начала текста
		/// в остальных - смещение элемента от начала предложения
		/// </summary>
		POS,
		/// <summary>
		/// прилагательные
		/// </summary>
		PRE,
		/// <summary>
		/// профессия для персон, существительное дял остальных сущностей
		/// </summary>
		PROF,
		/// <summary>
		/// роль сущности в предложении
		/// </summary>
		ROLE,
		/// <summary>
		/// подлжеащее предложения
		/// </summary>
		SUBJ,
		/// <summary>
		/// нормализованное значение
		/// </summary>
		VALUE,
		/// <summary>
		/// нормализованное значение без стоп-прилагательных
		/// </summary>
		VALUE_THEME,
		/// <summary>
		/// тип
		/// </summary>
		TYPE,
		/// <summary>
		/// Имя
		/// </summary>
		FN,
		/// <summary>
		/// Фамилия
		/// </summary>
		LN,
		/// <summary>
		/// Отчество
		/// </summary>
		MN
	}

	/// <summary>
	/// Тип кореферентности
	/// </summary>
	public enum CoreferenceType
	{
		/// <summary>
		/// Не определен
		/// </summary>
		Default,
		/// <summary>
		/// Кореферентность персон
		/// </summary>
		Person,
		/// <summary>
		/// Кореферентность организаций
		/// </summary>
		Organization,
		/// <summary>
		/// Организации не типа OrgName
		/// </summary>
		WeakOrganization,
		/// <summary>
		/// Разрешение аббревиатур
		/// </summary>
		Abbreviation,
		/// <summary>
		/// Разрешение цепочек существительных
		/// </summary>
		Unknown
	}
}
