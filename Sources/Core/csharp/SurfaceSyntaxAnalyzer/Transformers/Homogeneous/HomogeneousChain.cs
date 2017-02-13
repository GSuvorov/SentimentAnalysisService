using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using Linguistics.Core.Morpho;
using Utils;

namespace SurfaceSyntaxAnalyzer.Homogeneous
{
	/// <summary>
	/// Цепочка однороднеых членов
	/// </summary>
	internal class HomogeneousChain
	{
		/// <summary>
		/// Формат, по которому выделилась цепочка
		/// </summary>
		private HomogeneousFormatBase _format;

		/// <summary>
		/// Однородные члены
		/// </summary>
		private List<UnitTextBase> _units;

		#region [.morpho properties.]
		/// <summary>
		/// Падежи цепочки
		/// </summary>
		private Case[] _cases;

		/// <summary>
		/// Лицо
		/// </summary>
		private Face _face;

		/// <summary>
		/// Одушевленность
		/// </summary>
		private AnimationType _animation;
		#endregion

		/// <summary>
		/// Однородные сущности
		/// </summary>
		private List<Entity> _entities;

		/// <summary>
		/// частица отрицания
		/// </summary>
		private string _negotiationParticle;

		/// <summary>
		/// сочинительный союз
		/// </summary>
		private string _conjunction;

		/// <summary>
		/// Предлог
		/// </summary>
		/// если предлог присутствует в цепочке однородных членов, то он стоит перед каждой сущностью
		private UnitTextBase _pretext;

		/// <summary>
		/// Проверка, что цепочка выделилась удачно
		/// </summary>
		public bool Success { get { return _units.Any(); } }

		#region [.ctor().]
		/// <summary>
		/// Конструктор
		/// </summary>
		/// <param name="unitCollection">коллекция юнитов, из которых строится цепочка</param>
		/// <param name="format">формат цепочки</param>
		public HomogeneousChain(IEnumerable<UnitTextBase> unitCollection, HomogeneousFormatBase format)
		{
			_format = format;
			_units = new List<UnitTextBase>();
			_entities = new List<Entity>();
			
			CreateChain(unitCollection);
		}
		#endregion

		/// <summary>
		/// Замена юнитов из цепочки однородных сущностей на саму сущность
		/// </summary>
		/// <param name="unitCollection">коллекция, содержащие юниты цепочки однородных сущностей</param>
		public void ReplaceUnits(IList<UnitTextBase> unitCollection)
		{
			if (_units.Any())
			{
				int index = unitCollection.IndexOf(_units.First());
				Entity homogeneous = new Entity(_units.GetText(), _units.First().PositionInfo.Start, new HomogeneousEntityInfo() { Pretext = _pretext });
				SimpleStringBuilder value = new SimpleStringBuilder();
				foreach (UnitTextBase unit in _units)
				{
					unitCollection.RemoveAt(index);
					if (unit.IsEntity)
					{
						Entity entity = (Entity)unit;
						homogeneous.AddChild(entity);
						value.Append(entity.Value);
					}
					else
						value.Append(unit.Text);
				}
				homogeneous.EntityInfo.Value = value.TrueValue;
				homogeneous.EntityInfo.Morpho = GetChainMorphoInformation();
				unitCollection.Insert(index, homogeneous);
			}
		}

		/// <summary>
		/// Получение коллекции морфологических свойств цепочки однородных сущностей
		/// </summary>
		/// <returns>коллекция морфологических свойств цепочки однородных сущностей</returns>
		private MorphoInformation[] GetChainMorphoInformation()
		{
			List<MorphoInformation> morpho = new List<MorphoInformation>(_cases.Length);
			foreach (var Case in _cases)
			{
				morpho.Add
				(
					new MorphoInformation()
					{
						Case = Case,
						Animation = _animation,
						Face = _face,
						Gender = Gender.Undefined,
						Number = Number.Plural
					}
				);
			}
			return morpho.ToArray();
		}

		/// <summary>
		/// Создание цепочки однородных из коллекции юнитов
		/// </summary>
		/// <param name="unitCollection">коллекция юнитов</param>
		private void CreateChain(IEnumerable<UnitTextBase> unitCollection)
		{
			List<UnitTextBase> part = new List<UnitTextBase>();

			InitializeChain();
			foreach (UnitTextBase unit in unitCollection)
			{
				if (!AddUnit(unit, part))
					break;
			}
			FinalCorrectChain();
		}

		/// <summary>
		/// Инициализация цепочки
		/// </summary>
		private void InitializeChain()
		{
			_format.InitializeChain();
			_cases = new Case[] { Case.Nominative, Case.Dative, Case.Genetive, Case.Accusative, Case.Instrumental, Case.Locative, Case.Undefined };
		}

		/// <summary>
		/// Финальная корректировка цепочки
		/// </summary>
		private void FinalCorrectChain()
		{
			if (_entities.Count == 1)
			{
				_units.Clear();
				_entities.Clear();
			}
			else
				_format.FinalCorrectChain(_units);
		}

		/// <summary>
		/// Добавление юнита в цепочку
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <param name="part">часть цепочки до первой встреченной сущности</param>
		/// <returns>флаг допустимости юнита для цепочки</returns>
		private bool AddUnit(UnitTextBase unit, List<UnitTextBase> part)
		{
			bool result = true;
			if (unit.UnitTextType == UnitTextType.ENTITY)
			{
				Entity entity = (Entity)unit;
				if (_format.IsHomogeneousItem(entity))
				{
					result = AddEntity(entity, part);
					part.Clear();
				}
				else
					part.Add(unit);
			}
			else
			{
				if (DictionaryResource.IsHomogeneousConjunction(unit.Text))
					result = unit.Text.IsSameValue(ref _conjunction);
				else if (DictionaryResource.IsNegotiationParticle(unit.Text))
					result = unit.Text.IsSameValue(ref _negotiationParticle);
				else if (DictionaryResource.IsOtherEnumeration(unit.Text))
				{
					_units.AddRange(part);
					part.Clear();
					result = false;
				}
				else
				{
					result = _format.IsAvailable(unit);
					if (unit.IsPretext())
						_pretext = unit;
				}
				part.Add(unit);
			}
			return result;
		}

		/// <summary>
		/// Добавление сущности в цепочку
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <param name="part">часть цепочки до первой встреченной сущности</param>
		/// <returns>флаг допустимости сущности для цепочки</returns>
		private bool AddEntity(Entity entity, List<UnitTextBase> part)
		{
			Case[] cases = GetCasesIntersection(entity.EntityInfo.Morpho);
			if (cases.Any())
			{
				InsertEntityInChain(entity, part, cases);
				return true;
			}
			return false;
		}

		/// <summary>
		/// Вставка сущности в цепочку:
		/// 1. обновление морфологических свойств цепочки
		/// 2. вставка части цепочки до первой встреченной сущности
		/// 3. вставка самой сущности
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <param name="part">часть цепочки до первой встреченной сущности</param>
		/// <param name="cases">коллекция падежей для цепочки</param>
		private void InsertEntityInChain(Entity entity, List<UnitTextBase> part, Case[] cases)
		{
			UpdateChainMorpho(entity.EntityInfo.Morpho, cases);
			_units.AddRange(part);
			_units.Add(entity);
			_entities.Add(entity);
		}

		/// <summary>
		/// Обновление морфологических свойств цепочки
		/// </summary>
		/// <param name="morpho">коллекция морфологических свойств сущности</param>
		/// <param name="cases">коллекция падежей для цепочки</param>
		private void UpdateChainMorpho(IEnumerable<MorphoInformation> morpho, Case[] cases)
		{
			/// флаг неинициализированности одушевленности
			bool isAnimationInitialized = _entities.Any();

			_cases = cases;
			if (morpho != null)
			{
				foreach (MorphoInformation info in morpho)
				{
					if (GetFaceLevel(info.Face) < GetFaceLevel(_face))
						_face = info.Face;

					if (!isAnimationInitialized)
						_animation = info.Animation;
					else if (_animation != info.Animation)
						_animation = AnimationType.Undefined;
					isAnimationInitialized = true;
				}
			}
		}

		/// <summary>
		/// получение приоритета лица
		/// </summary>
		/// <param name="face">лицо</param>
		/// <returns>приоритет</returns>
		private int GetFaceLevel(Face face)
		{
			int result = -1;
			switch (face)
			{
				case Face.First:
					result = 1;
					break;
				case Face.Second:
					result = 2;
					break;
				case Face.Third:
					result = 3;
					break;
				case Face.Undefined:
					result = 4;
					break;
				default:
					throw new ArgumentException("wrong Face");
			}
			return result;
		}

		/// <summary>
		/// Получение пересечения текущих возможных падежей цепочки с падежами из коллеции морфологических свойств цепочки
		/// </summary>
		/// <param name="morpho">коллеция морфологических свойств цепочки</param>
		/// <returns>коллекция пересечения падежей</returns>
		private Case[] GetCasesIntersection(IEnumerable<MorphoInformation> morpho)
		{
			HashSet<Case> cases = new HashSet<Case>();
			if ((morpho != null) && morpho.Any())
			{
				foreach (MorphoInformation info in morpho)
					if (_cases.Contains(info.Case) && !cases.Contains(info.Case))
						cases.Add(info.Case);
			}
			else
				cases.Add(Case.Undefined);
			return cases.ToArray();
		}
	}
}
