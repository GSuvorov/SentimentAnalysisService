using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

using Lingvistics;
using Linguistics.Core;
using Linguistics.Core.Morpho;
using Linguistics.Coreference;
using Lingvistics.Types;

namespace SemanticResolution
{
    /// <summary>
    /// класс построения семантической сети
    /// </summary>
    public class SemanticNetwork
    {
        private XElement _Text;

		/// <summary>
		/// пара "шифр связи(id1_id2_type)"-"связь"
		/// </summary>
		private Dictionary<string, LinkItem> _Links;
		/// <summary>
		/// пара "строка темы"-"тема"
		/// </summary>
		private Dictionary<string, ThemeItem> _Themes;
        private List<XElement> _SentenceEntities;
        private Dictionary<string, XElement> _Entities;
		/// <summary>
		/// Информация о разрешенной кореферентности
		/// </summary>
		private ICoreferenceInfo _coreferenceInfo;

		/// <summary>
		/// язык предложения
		/// </summary>
		private string _sentenceLanguage;

        private int _SubjectCount;

        private int _ThemeId;

		/// <summary>
		/// true - Надо генерировать все подтемы
		/// </summary>
		private bool _isAllSubThemes;

        /// <summary>
        /// построить семантическую сеть документа по xml
        /// </summary>
        /// <param name="xmlText">текст с выделенными сущностями и разбитый на предложения</param>
        /// <returns>сем. сеть в xml</returns>
		public Tuple<IEnumerable<ThemeItem>, IEnumerable<LinkItem>> BuildSemanticNetwork(XElement xmlText, ICoreferenceInfo coreferenceInfo, bool generateAllSubThemes)
        {
			_isAllSubThemes = generateAllSubThemes;
            _ThemeId = 1;
			_Links = new Dictionary<string, LinkItem>();
			_Themes = new Dictionary<string, ThemeItem>();
            _Entities = new Dictionary<string, XElement>();
			_coreferenceInfo = coreferenceInfo;
			if (xmlText != null)
			{
				_Text = xmlText;

				foreach (XElement sentence in xmlText.Elements(UnitTextType.SENT.ToString()))
				{
					_sentenceLanguage = sentence.Attribute(BlockAttribute.LANG);
					SentenceCycle(sentence);
				}
			}
			//var result = _Themes.Values.Where(_ => _.Name.IndexOf("MOLECULE") >= 0).ToList();
			return new Tuple<IEnumerable<ThemeItem>, IEnumerable<LinkItem>>(_Themes.Values, _Links.Values);
        }

		/// <summary>
		/// Проверка, что заданная сущность является темой
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <param name="coreferenceInfo">информация о корефернтности</param>
		/// <returns>результат проверки</returns>
		public static bool IsTheme(XElement entity, ICoreferenceInfo coreferenceInfo)
		{
			bool result = false;
			if (entity.Name.IsEqual(UnitTextType.ENTITY))
			{
				string type = entity.Attribute(BlockAttribute.TYPE);
				bool isNotTheme = entity.IsContainAttribute(BlockAttribute.NOT_THEME);
				isNotTheme = isNotTheme || !Enum.GetNames(typeof(EntityType)).Contains(type);
				isNotTheme = isNotTheme || (type.IsEqual(EntityType.Pronoun) && !entity.IsContainAttribute(BlockAttribute.LINK));
				isNotTheme = isNotTheme || type.IsEqual(EntityType.Adverb) || type.IsEqual(EntityType.Homogeneous);
				isNotTheme = isNotTheme || (entity.Name.IsEqual(UnitTextType.CET) && !type.IsEqual(EntityType.Conjunction));
				string masterName = string.Empty;
				XElement testEntity = entity;
				if (!isNotTheme)
				{
					if (coreferenceInfo != null)
					{
						var master = coreferenceInfo.TryGetMaster(entity);
						if (master != null)
						{
							testEntity = master.Item2;
							masterName = master.Item1;
						}
					}
					if (type.IsEqual(EntityType.PersonName))
					{
						bool isContainLastName = testEntity.IsContainAttribute(BlockAttribute.LN);
						bool isContainFirstName = testEntity.IsContainAttribute(BlockAttribute.FN);
						bool isContainMiddleName = testEntity.IsContainAttribute(BlockAttribute.MN);
						bool isFullName = isContainLastName && (isContainFirstName || isContainMiddleName);
                        isNotTheme |= !(isFullName ||
                                        testEntity.IsContainAttribute( BlockAttribute.PROF ) ||
                                        testEntity.IsContainAttribute( BlockAttribute.PRE ));
					}
				}
                string theme = GetThemeName( entity, masterName );
				isNotTheme |= theme.IsEmpty() || (theme.Length > 256);
				//result = result || (type.IsEqual(EntityType.Date) && (entity.Attribute(BlockAttribute.VALUE) == "0000-00-00/0000-00-00"));
				isNotTheme |= type.IsEqual(EntityType.Adjective) || type.IsEqual(EntityType.ShortAdjective) ||
                              type.IsEqual(EntityType.Comparative) || type.IsEqual(EntityType.Superlative);
				result = !isNotTheme;
			}
			return result;
		}

		/// <summary>
		/// Проверка, что заданная сущность является темой
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public static bool IsTheme(XElement entity)
		{
			return IsTheme(entity, null);
		}

        private void SentenceCycle(XElement sentence)
        {
            List<XElement> elements = sentence.Elements().Where(_ => _.Name.IsEqual(UnitTextType.SUB_SENT) || _.Name.IsEqual(UnitTextType.SENT)).ToList();
            for (int i = 0; i < elements.Count; ++i)
            {
                BuildSentenceNetwork(elements[i]);
                SentenceCycle(elements[i]);
            }
        }

        private void BuildSentenceNetwork(XElement sentence)
        {
            string sentType = sentence.Attribute(BlockAttribute.TYPE);
            /// переходность последнего глагола
            bool transitionalVerb = true;
            _SubjectCount = 0;
            _SentenceEntities = new List<XElement>();
            List<List<XElement>> chains = new List<List<XElement>>();
            List<int> verbIndexes = new List<int>();
            chains.Add(new List<XElement>());
            verbIndexes.Add(-1);
            int currentChain = 0;

            List<XElement> elements = sentence.Elements().ToList();
            for(int i = 0; i < elements.Count; ++i)
            /// запишем все сущности предложения в хранилище
            {
                bool updateVerb = false;
				if (elements[i].Name.IsEqual(UnitTextType.SENT))
				{

				}
				else if (elements[i].Name.IsEqual(UnitTextType.ENTITY))
				{
					int count = _SentenceEntities.Count;
					AddEntityChainToRepository(elements[i]);
					chains[currentChain].AddRange(
						_SentenceEntities.GetRange(count, _SentenceEntities.Count - count));
				}
				else if (elements[i].Name.IsEqual(UnitTextType.VERB))
				{
					if (!_sentenceLanguage.IsEqual("RU"))
						continue;

					if (sentType.IsEqual(SubSentenceType.DanglingParticiple) ||
						sentType.IsEqual(SubSentenceType.Participial))
					{
						updateVerb = true;
					}
					else
					{
						if (elements[i].Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Verb))
						{
							if (verbIndexes[currentChain] >= 0)
							{
								if (!transitionalVerb)
									chains[currentChain].RemoveAt(verbIndexes[currentChain]);
								else
								{
									chains.Add(new List<XElement>());
									chains[currentChain + 1].AddRange(chains[currentChain].GetRange(
										verbIndexes[currentChain] + 1,
										chains[currentChain].Count - verbIndexes[currentChain] - 1));
									verbIndexes.Add(-1);
									++currentChain;
								}
							}

							string[] information = elements[i].Attribute(BlockAttribute.MI).Split(ServiceFunction.MI_Sep);
							foreach (string info in information)
								if (info[MorphoInfo.Transitional].IsEqualCode(TransitionalType.NoTransitional))
									transitionalVerb = false;
							updateVerb = true;
						}
					}

					if (updateVerb)
					{
						verbIndexes[currentChain] = chains[currentChain].Count;
						chains[currentChain].Add(elements[i]);
					}
				}
            }

            if (_SentenceEntities.Count > 1)
            /// надо устанавливать связи
            {
                XElement subject = GetSubject(sentence);
                
                if ((chains.Count == 1) && (verbIndexes[0] < 0))
                /// нет глагола
                {
                    AllToAll(chains[0]);
                }
                else
                {
                    for (int i = 0; i < chains.Count; ++i)
                    {
                        List<XElement> subjects = new List<XElement>();
                        XElement obj = null;
                        XElement verb = chains[i][verbIndexes[i]];
                        IEnumerable<XElement> range = null;

                        int subjIndex = GetSubjectIndex(chains[i], subject);
                        int objIndex = -1;
                        if (_SubjectCount > 0)
                        {
                            if (subjIndex < 0)
                            /// подлежащего нет
                            {
                                objIndex = GetObjectiveIndex(chains[i], 0, chains[i].Count,
                                    verbIndexes[i]);
                                if (objIndex >= 0)
                                    obj = chains[i][objIndex];
                            }
                            else
                            /// подлежащее есть
                            {
                                range = ConnectRange(chains[i], verbIndexes[i], subjIndex);
                                subjects.Add(chains[i][subjIndex]);
                                bool direction = (subjIndex < verbIndexes[i]);
                                for (int j = subjIndex - 1;
                                     (j >= 0) && (direction == (j < verbIndexes[i]));
                                     --j)
                                {
                                    if (chains[i][j].Attribute(BlockAttribute.ROLE).IsEqual(EntityRole.Subj))
                                        subjects.Add(chains[i][j]);
                                }
                            }
                        }
                        else if ((subject != null) && (i == 0))
                        /// подлежащее есть
                        {
                            range = ConnectRange(chains[i], verbIndexes[i], subjIndex);
                            subjects.Add(subject);
                        }
                        else
                        /// подлежащего нет
                        {
                            objIndex = GetObjectiveIndex(chains[i], 0, chains[i].Count, verbIndexes[i]);
                            if (objIndex >= 0)
                                obj = chains[i][objIndex];
                        }

                        if (!subjects.Any() && (obj == null))
                        /// есть только обстоятельства
                        {
                            chains[i].RemoveAt(verbIndexes[i]);
                            AllToAll(chains[i]);
                        }
                        else
                        {
                            if (subjects.Any())
                            /// есть подлежащее
                            {
                                SubjectToList(range, subjects, verb);
                            }
                            else
                            /// нет подлежащего, но есть дополнение
                            {
                                range = ConnectRange(chains[i], verbIndexes[i], objIndex);
                                ObjectiveToList(range, obj, verb);
                            }
                        }
                    }
                }
            }
        }

        private IEnumerable<XElement> ConnectRange(List<XElement> chains, int verbIndex, int index)
        {
            IEnumerable<XElement> range = null;
            if (index < verbIndex)
            {
                range = chains.GetRange(verbIndex + 1, chains.Count - verbIndex - 1);
                AllToAll(chains.GetRange(0, verbIndex));
            }
            else
            {
                range = chains.GetRange(0, verbIndex);
                AllToAll(chains.GetRange(verbIndex + 1, chains.Count - verbIndex - 1));
            }

            /// свяжем между собой обстоятельства 
            List<XElement> adjectives = new List<XElement>();
            foreach (XElement elem in range)
                if (elem.Attribute(BlockAttribute.ROLE).IsEqual(EntityRole.Adj))
                    adjectives.Add(elem);
            AllToAll(adjectives);
            
            return range;
        }

        private bool IsCompositeVerb(XElement verb)
        {
            bool composite = false;
            if (verb == null)
                return composite;

            XElement child = verb.Element(UnitTextType.VERB.ToString());
            if (child != null)
            {
                composite = IsCompositeVerb(child);
                if (!composite)
                {
					composite =
						(from morpho in verb.Attribute(BlockAttribute.MI).Split(ServiceFunction.MI_Sep)
						 where !morpho.IsEmpty() && morpho[MorphoInfo.Transitional].IsEqualCode(TransitionalType.Transitional)
						 select morpho).Any();
                }
            }
            return composite;
        }

        private XElement GetVerb(XElement verb, ref int levelFromEnd)
        {
            XElement curVerb = verb.Element(UnitTextType.VERB.ToString());
            if (curVerb != null)
            {
                curVerb = GetVerb(curVerb, ref levelFromEnd);
                if (levelFromEnd == -1)
                    curVerb = verb.Element(UnitTextType.VERB.ToString());
            }
            --levelFromEnd;
            return curVerb;
        }

        private void ChildsToList(IEnumerable<XElement> list, XElement parent)
        {
            foreach (XElement child in parent.Elements())
                if (child.Name.IsEqual(UnitTextType.ENTITY))
                {
                    string id = child.Attribute(BlockAttribute.ID);
                    if (_Entities.ContainsKey(id))
                    {
                        foreach (XElement theme in list)
                            AddLinkToSenteceRepository(child, theme, "");
                        ChildsToList(list, child);
                    }
                }
        }

        private void ObjectiveToList(IEnumerable<XElement> list, XElement obj, XElement verb)
        {
            int level = 0;
            if (IsCompositeVerb(verb))
                verb = GetVerb(verb, ref level);
            string value = verb.AttributeUpper(BlockAttribute.VALUE);
            foreach (XElement theme in list)
                AddLinkToSenteceRepository(obj, theme, value);
        }

        private void SubjectToList(IEnumerable<XElement> list, List<XElement> subjects, XElement verb)
        {
            int level = 1;
            bool easyConnect = !IsCompositeVerb(verb);
            XElement obj = null;
            int objIndex = GetObjectiveIndex(list, 0, list.Count(), -1);
            if (objIndex < 0)
            {
                if (!easyConnect)
                    verb = GetVerb(verb, ref level);
            }
            else
                obj = list.ElementAt(objIndex);

            if (objIndex < 0)
            /// нет обстоятельств
            {
                foreach (XElement subject in subjects)
                {
                    foreach (XElement theme in list)
                        AddLinkToSenteceRepository(subject, theme, "");
                    ChildsToList(list, subject);
                }
            }
            else
            {
				string value = verb.AttributeUpper(BlockAttribute.VALUE);
                foreach (XElement subject in subjects)
                {
                    AddLinkToSenteceRepository(subject, obj, value);
                    List<XElement> nodes = new List<XElement>();
                    nodes.Add(obj);
                    ChildsToList(nodes, subject);
                }
                ObjectiveToList(list, obj, verb);
            }
        }

        private void AllToAll(List<XElement> list)
        {
            for (int i = 0; i < list.Count; ++i)
            {
                bool miss = true;
                for (int j = i + 1; j < list.Count; ++j)
                {
                    if (!list[j].Attribute(BlockAttribute.ROLE).IsEqual(EntityRole.Other))
                        miss = false;
                    if (!miss && list[j].Name.IsEqual(UnitTextType.ENTITY))
                        AddLinkToSenteceRepository(list[i], list[j], "");
                }
            }
        }

        private int GetObjectiveIndex(IEnumerable<XElement> list, int start, int end, int verbIndex)
        {
            int index = -1;
            for (int i = start; (i < end) && (i < list.Count()); ++i)
            {
                if (list.ElementAt(i).Attribute(BlockAttribute.ROLE).IsEqual(EntityRole.Obj))
                {
                    if (i < verbIndex)
                        index = i;
                    else if ((i == verbIndex) && (index >= 0))
                        break;
                    else
                    {
                        index = i;
                        break;
                    }
                }
            }
            return index;
        }

        private int GetSubjectIndex(List<XElement> list, XElement subject)
        {
            int index = -1;
            if (subject != null)
            {
                for (int i = 0; i < list.Count; ++i)
                {
                    if (list[i].Attribute(BlockAttribute.ID) == subject.Attribute(BlockAttribute.ID))
                    {
                        index = i;
                        break;
                    }
                }
            }
            return index;
        }

        private XElement GetSubject(XElement sentence)
        {
            XElement subject = null;
			string subjID = sentence.Attribute(BlockAttribute.SUBJ);
			if (!subjID.IsEmpty())
			{
				if (_Entities.TryGetValue(subjID, out subject))
					if (!IsEntity(subject))
						subject = null;
			}
            return subject;
        }

        private bool IsEntity(XElement element)
        {
            bool isEntity = false;
            if (element != null)
            {
                string type = element.Attribute(BlockAttribute.TYPE);
                if (type.IsEqual(EntityType.Pronoun))
                {
                    string id = element.Attribute(BlockAttribute.ID);
                    XElement entity = null;
                    if (!id.IsEmpty() && _Entities.TryGetValue(id, out entity))
                        isEntity = IsEntity(entity);
                }
                else
                    isEntity = true;
            }
            return isEntity;
        }

        private void AddLeftSubThemesToSentenceRepository(XElement entity, string id)
        {
            List<XElement> subThemes = _Text.Elements("THEMES").ToList();
            for (int i = 0; i < subThemes.Count; ++i)
                if (subThemes[i].Attribute(BlockAttribute.ID) == id)
                {
                    foreach (XElement theme in subThemes[i].Elements())
                    {
                        theme.SetAttributeValue(BlockAttribute.ROLE, EntityRole.Other);
                        AddEntityToRepository(theme, entity);
                    }
                    subThemes[i].Remove();
                    break;
                }
        }

		/// <summary>
		/// добавляет цепочку сущности (включая ее наследников) в хранилище предложения
		/// </summary>
		/// <param name="entity"></param>
        private void AddEntityChainToRepository(XElement entity)
        {
			AddEntityToSentenceRepository(entity);
			foreach (XElement child in entity.Descendants())
				if (child.Name.IsEqual(UnitTextType.ENTITY))
					AddEntityToRepository(child, child.Parent);
			if (_isAllSubThemes && entity.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.PersonName))
			{
				XElement child = new XElement(entity.Name);
				child.SetType(EntityType.PersonName);
				child.SetRole(EntityRole.Other);
				child.SetAttributeValue(BlockAttribute.VALUE, GetPersonName(entity));
				if (entity.IsContainAttribute(BlockAttribute.LN))
					child.SetAttributeValue(BlockAttribute.LN, entity.Attribute(BlockAttribute.LN));
				if (entity.IsContainAttribute(BlockAttribute.FN))
					child.SetAttributeValue(BlockAttribute.FN, entity.Attribute(BlockAttribute.FN));
				if (entity.IsContainAttribute(BlockAttribute.MN))
					child.SetAttributeValue(BlockAttribute.MN, entity.Attribute(BlockAttribute.MN));
				AddEntityToSentenceRepository(child);
			}
        }

		private string GetPersonName(XElement person)
		{
			StringBuilder builder = new StringBuilder();
			BlockAttribute[] attributeOrder = new BlockAttribute[] { BlockAttribute.LN, BlockAttribute.FN, BlockAttribute.MN };
			foreach (BlockAttribute attribute in attributeOrder)
			{
				string value = person.Attribute(attribute);
				if (!value.IsEmpty())
				{
					if (!builder.IsEmpty())
						builder.Append(" ");
					builder.Append(value);
				}
			}
			return builder.ToString();
		}

		/// <summary>
		/// добавляет сущность в хранилище
		/// </summary>
		/// <param name="entity"></param>
		/// <param name="parent"></param>
		private void AddEntityToRepository(XElement entity, XElement parent)
		{
			if (_isAllSubThemes || (GetTypeLevel(entity) <= GetTypeLevel(parent)))
				AddEntityToSentenceRepository(entity);
		}

		/// <summary>
		/// возвращает уровень типа сущности
		/// </summary>
		/// <param name="entity"></param>
		/// <returns></returns>
        private int GetTypeLevel(XElement entity)
        {
            string type = EntityType.Default.ToString();
            if ((entity != null) && entity.Name.IsEqual(UnitTextType.ENTITY))
                type = entity.Attribute(BlockAttribute.TYPE);
			int level = 4;
			if (type.IsEqual(EntityType.PersonName) || type.IsEqual(EntityType.PersonInfo))
				level = 1;
			else if (type.IsEqual(EntityType.OrgName) || type.IsEqual(EntityType.OrgAbbr))
				level = 2;
			else if (type.IsEqual(EntityType.PlaceName))
				level = 3;
			return level;
        }

        private ThemeItem FindTheme(XElement entity)
        {
			ThemeItem finded = null;
			if (entity != null)
				_Themes.TryGetValue(GetValue(entity), out finded);
            return finded;
        }

		/// <summary>
		/// добавление сущности в хранилище
		/// </summary>
		/// <param name="entity"></param>
        private void AddEntityToSentenceRepository(XElement entity)
        {
			if (!IsTheme(entity))
				return;
            
			ThemeItem theme = FindTheme(entity);
            if (theme == null)
            {
                ThemeItem newTheme = new ThemeItem();
				newTheme.ID = _ThemeId;
				newTheme.Type = entity.Attribute(BlockAttribute.TYPE);
				newTheme.Name = GetThemeName(entity, GetMasterName(entity));
				newTheme.OriginalName = GetOriginalName(entity);
                ++_ThemeId;
				newTheme.FreqSubj = 0;
				newTheme.FreqObj = 0;
				newTheme.FreqAdj = 0;
				newTheme.FreqOther = 0;
				_Themes.Add(GetValue(entity), newTheme);
                theme = newTheme;
            }
            IncrementFrequency(theme, entity);

            string id = entity.Attribute(BlockAttribute.ID);
			entity = new XElement(entity);
			if(!_Entities.ContainsKey(id))
				_Entities.Add(id, entity);
            entity.SetID(theme.ID.ToString());
            _SentenceEntities.Add(entity);

            /// добавим подтемы с правым отсечением
            AddLeftSubThemesToSentenceRepository(entity, id);
        }

        private LinkItem AddLinkToSenteceRepository(XElement first, XElement second, string type)
        {
            LinkItem finded = null;
			ThemeItem firstTheme = FindTheme(first);
			ThemeItem secondTheme = FindTheme(second);
			if ((firstTheme == null) || (secondTheme == null))
				return null;

            string role1 = first.Attribute(BlockAttribute.ROLE);
            string role2 = second.Attribute(BlockAttribute.ROLE);
			int id1 = firstTheme.ID;
            int id2 = secondTheme.ID;
            if ((id1 == id2) || (role1.IsEqual(EntityRole.Other) && (role1 == role2)))
				return null;

            if (first.Name.IsEqual(UnitTextType.VERB) || second.Name.IsEqual(UnitTextType.VERB))
				return null;

            if (role1.IsEqual(EntityRole.Other) || role2.IsEqual(EntityRole.Other))
                type = string.Empty;
            bool direction = type.IsEmpty();
            LinkItem link = new LinkItem();
			link.SourceThemeID = id1;
			link.DestThemeID = id2;
			link.Freq = 0;
			link.Type = type.Trim().ToUpper();

			string linkCode = string.Format("{0}_{1}_{2}", id1, id2, type);
			if (!_Links.TryGetValue(linkCode, out finded) && !direction)
			/// связь не найдена - при отсутствии направления поменяем местами темы
			{
				linkCode = string.Format("{0}_{1}_{2}", id2, id1, type);
				_Links.TryGetValue(linkCode, out finded);
			}
            if (finded == null)
            {
                _Links.Add(linkCode, link);
                finded = link;
            }
			++finded.Freq;
            return finded;
        }

        private void IncrementFrequency(ThemeItem theme, XElement entity)
        {
            string str = entity.Attribute(BlockAttribute.ROLE);
            EntityRole role = EntityRole.Other;
            if(!str.IsEmpty())
                role = (EntityRole)Enum.Parse(typeof(EntityRole), str);
            switch (role)
            {
                case EntityRole.Adj:
					++theme.FreqAdj;
                    break;
                case EntityRole.Obj:
					++theme.FreqObj;
                    break;
                case EntityRole.Other:
					++theme.FreqOther;
                    break;
                case EntityRole.Subj:
					++theme.FreqSubj;
                    ++_SubjectCount;
                    break;
            }
        }

		private string GetValue(XElement entity)
		{
			return string.Format("{0}:{1}", entity.Attribute(BlockAttribute.TYPE), GetThemeName(entity, GetMasterName(entity)));
		}

		private string GetMasterName(XElement entity)
		{
			string result = string.Empty;
			if (_coreferenceInfo != null)
			{
				var master = _coreferenceInfo.TryGetMaster(entity);
				if (master != null)
					result = master.Item1;
			}
			return result;
		}

		private static string GetThemeName(XElement entity, string masterName)
		{
			if (entity.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Date))
				return GetDateValue(entity);
			else if (entity.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.URL))
				return entity.Attribute(BlockAttribute.VALUE).ToUpper();
			else
			{
				string value = masterName;
				if (value.IsEmpty())
				{
					if (entity.IsContainAttribute(BlockAttribute.VALUE_THEME))
						value = entity.Attribute(BlockAttribute.VALUE_THEME);
					else
						value = entity.Attribute(BlockAttribute.VALUE);
				}
				return EntityNormalizer.Normalize(value).ToUpper();
			}
		}

		private static string GetDateValue(XElement entity)
		{
			string value = entity.Attribute(BlockAttribute.VALUE);
			var arr = value.Split('/');
			if (!arr[0].StartsWith("0000"))
				return arr[0];
			else if (arr.Length > 1 && !arr[1].StartsWith("0000"))
				return arr[1];
			else
				return value;
		}

		private string GetOriginalName(XElement entity)
		{
			if (_isAllSubThemes)
			{
				string result = "";
				if (entity.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Date))
					result = entity.Value;
				else if (entity.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.URL))
					return entity.Attribute(BlockAttribute.VALUE).ToUpper();
				else if (entity.IsContainAttribute(BlockAttribute.FULLNAME))
					result = entity.Attribute(BlockAttribute.VALUE);
				return EntityNormalizer.Normalize(result).ToUpper();
			}
			return null;
		}
    }
}