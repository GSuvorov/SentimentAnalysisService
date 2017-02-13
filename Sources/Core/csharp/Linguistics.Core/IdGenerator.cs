using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Генератор идентификаторов
	/// </summary>
	internal static class IdGenerator
	{
		/// <summary>
		/// текущий идентификатор сущности
		/// </summary>
		private static int _currentEntityID;
		/// <summary>
		/// текущий идентификатор подпредложения
		/// </summary>
		private static int _currentSubSentenceID;

		/// <summary>
		/// словарь идентификаторов сущностей
		/// </summary>
		private static Dictionary<Entity, int> _entityID = new Dictionary<Entity,int>();

		/// <summary>
		/// Получение идентификатора заданной сущности
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>идентификатор сущности</returns>
		public static int GetID(Entity entity)
		{
			int result;
			if (!GetExistedID(entity, out result))
			{
				_entityID.Add(entity, ++_currentEntityID);
				result = _currentEntityID;
			}
			return result;
		}

		/// <summary>
		/// Получение идентификатора заданного подпредложения
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		/// <returns>идентификатор подпредложения</returns>
		public static int GetID(SubSentence subSentence)
		{
			return ++_currentSubSentenceID;
		}

		/// <summary>
		/// Получение идентификатора сущности, которая присутствует в словаре идентификаторов
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <param name="id">идентификатор</param>
		/// <returns>флаг присутствует наличия сущности в словаре идентификаторов</returns>
		public static bool GetExistedID(Entity entity, out int id)
		{
			return _entityID.TryGetValue(entity, out id);
		}

		/// <summary>
		/// Сброс текущего идентификатора
		/// </summary>
		public static void ReleaseID()
		{
			_currentEntityID = 0;
			_currentSubSentenceID = 0;
			_entityID.Clear();
		}

		/// <summary>
		/// Получение всех проидентифицированных сущностей
		/// </summary>
		/// <returns></returns>
		public static Entity[] GetAllEntities()
		{
			return _entityID.Keys.ToArray();
		}
	}
}
