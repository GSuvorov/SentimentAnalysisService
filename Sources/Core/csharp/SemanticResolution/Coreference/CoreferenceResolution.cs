using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Xml.Linq;
using System.Globalization;
using Linguistics.Core;

namespace SemanticResolution
{
    /// <summary>
    /// класс применения кореферентности
    /// </summary>
    public class CoreferenceResolution
    {
        private const string ROOT_NAME = "ROOT";
        private const string NO_FULLNAME = "-";

        private XElement _CurrentNode;
        private XElement _CompareNode;
        private XElement _Text;

        private Dictionary<string, XElement> _LinqDictionary;

		private static BlockAttribute[] _PersonAttributes = new BlockAttribute[] { BlockAttribute.PROF, BlockAttribute.PRE,
			BlockAttribute.FN, BlockAttribute.MN, BlockAttribute.LN };
		private static BlockAttribute[] _UsualAttributes = new BlockAttribute[] { BlockAttribute.PROF, BlockAttribute.PRE };

		/// <summary>
		/// разрешение кореферентности
		/// </summary>
		/// <param name="xmlText">xml-текст</param>
		public void ResolveCoreference(ref XElement xmlText)
		{
			/*if (xmlText == null)
				return;
			_LinqDictionary = new Dictionary<string, XElement>();
			_CurrentNode = null;
			_CompareNode = null;
			_Text = new XElement(ROOT_NAME);

			List<XElement> list = new List<XElement>();
			foreach (XElement sentence in xmlText.Elements(NodeName.SENT))
			{
				if (sentence.Attribute(BlockAttribute.TYPE) != "fact")
					continue;
				foreach (XElement element in sentence.Elements(NodeName.ENTITY))
					if (GetCoreferenceType(element) != CoreferenceType.Default)
						_Text.Add(element);
			}
			list = _Text.Descendants().Where(_ => _.Name.IsEqual(NodeName.ENTITY)).ToList();
			Coreference(list);

			/// обновим xml
			int entityIndex = 0;
			List<XElement> newText = _Text.Elements().ToList();
			foreach (XElement sentence in xmlText.Elements())
			{
				List<XElement> nodes = sentence.Elements().ToList();
				for (int i = 0; i < nodes.Count; ++i)
				{
					if (entityIndex == newText.Count)
						break;
					if (nodes[i].Attribute(BlockAttribute.ID) ==
						newText[entityIndex].Attribute(BlockAttribute.ID))
					{
						nodes[i] = newText[entityIndex++];
					}
				}
				sentence.ReplaceNodes(nodes);
			}*/
		}

		#region [алгоритмы разрешения кореферентности]
		/// <summary>
		/// Алгоритм разрешения кореферентности
		/// </summary>
		/// <param name="element"></param>
		/// <returns></returns>
		private delegate bool CoreferenceAlgorithm(XElement element);

		private void Coreference(List<XElement> elements)
		{
			Dictionary<string, List<XElement>> UnknownList = new Dictionary<string, List<XElement>>();
			List<XElement> PersonList = new List<XElement>();
			List<XElement> OrganizationList = new List<XElement>();
			List<XElement> AbbreviationList = new List<XElement>();
			foreach (XElement node in elements)
			{
				switch (GetCoreferenceType(node))
				{
					case CoreferenceType.Person:
						if (PersonList.LastOrDefault() != node.Parent)
							PersonList.Add(node);
						break;
					case CoreferenceType.Organization:
					case CoreferenceType.WeakOrganization:
						OrganizationList.Add(node);
						break;
					case CoreferenceType.Abbreviation:
						AbbreviationList.Add(node);
						break;
					case CoreferenceType.Unknown:
						List<XElement> list;
						string value = node.AttributeUpper(BlockAttribute.PROF);
						if (UnknownList.TryGetValue(value, out list))
							list.Add(node);
						else
						{
							list = new List<XElement>();
							list.Add(node);
							UnknownList.Add(value, list);
						}
						break;
				}
			}

			/// сущности, которые разрешаем на данном этапе
			List<XElement> leafs = new List<XElement>();
			while (leafs != null)
			{
				GetLeafNodes(_Text, leafs);
				if (leafs.Any())
				{
					Coreference(OrganizationList, leafs, CoreferenceType.Organization);
					UnknownCoreference(UnknownList, leafs);
					Coreference(PersonList, leafs, CoreferenceType.Person);

					foreach (XElement leaf in leafs)
						if (!leaf.IsContainAttribute(BlockAttribute.FULLNAME))
							leaf.SetAttributeValue(BlockAttribute.FULLNAME, NO_FULLNAME);
					leafs.Clear();
				}
				else
					leafs = null;
			}
			Coreference(AbbreviationList, elements, CoreferenceType.Abbreviation, false);
			DeleteChildsNoFullname(_Text);

			foreach (XElement node in _Text.Elements())
			{
				string link = node.Attribute(BlockAttribute.LINK);
				if (!link.IsEmpty() && node.Parent.Name.IsEqual(ROOT_NAME))
				{
					XElement linkNode = null;
					if (GetLinkFromDictionary(link, out linkNode))
					{
						node.SetLinkToElement(linkNode);
						/*string fullname = linkNode.Attribute(BlockAttribute.FULLNAME);
                        if (fullname.IsEmpty())
                            node.SetAttributeValue(BlockAttribute.FULLNAME.ToString(), fullname);*/
					}
				}
			}
		}

		private void Coreference(List<XElement> collection, List<XElement> entities, CoreferenceType type, bool isDownAnalyzing = true)
		{
			CoreferenceAlgorithm algorithm = null;
			switch (type)
			{
				case CoreferenceType.Abbreviation:
					algorithm = AbbreviationNodeCoreference;
					break;
				case CoreferenceType.Organization:
					algorithm = OrganizatioNodeCoreference;
					break;
				case CoreferenceType.Person:
					algorithm = PersonNodeCoreference;
					break;
			}
			if (algorithm == null)
				return;

			foreach (XElement entity in entities)
			{
				CoreferenceType corefType = GetCoreferenceType(entity);
				bool isGoodType = false;
				switch (type)
				{
					case CoreferenceType.Abbreviation:
						isGoodType = (corefType == CoreferenceType.Abbreviation);
						break;
					case CoreferenceType.Organization:
						isGoodType = (corefType == CoreferenceType.Organization) || (corefType == CoreferenceType.WeakOrganization);
						break;
					case CoreferenceType.Person:
						isGoodType = (corefType == CoreferenceType.Person);
						break;
				}
				if (isGoodType)
				{
					_CurrentNode = entity;
					string currentID = _CurrentNode.Attribute(BlockAttribute.ID);
					int index = collection.IndexOf(entity);
					bool find = false;
					for (int j = index - 1; !find && (j >= 0); --j)
						if (collection[j].Attribute(BlockAttribute.LINK) != currentID)
							find = algorithm(collection[j]);
					if (isDownAnalyzing && !find)
					/// вверху ничего не нашли
					{
						for (int j = index + 1; j < collection.Count; ++j)
							if (collection[j].Attribute(BlockAttribute.LINK) != currentID)
								algorithm(collection[j]);
					}
				}
			}
		}

		#region [Организации]
		private bool OrganizatioNodeCoreference(XElement second)
		{
			bool find = false;
			_CompareNode = second;

			string hash = _CurrentNode.AttributeUpper(BlockAttribute.HASH);
			if (!hash.IsEmpty() && hash.IsEqual(_CompareNode.AttributeUpper(BlockAttribute.HASH)))
				find = true;
			else
			{
				string nolessChilds = "";
				bool nolessInfo = true;
				/// противоречие
				bool contradiction = false;
				if (GetCoreferenceType(_CurrentNode) != CoreferenceType.Organization)
				{
					nolessInfo = false;
					contradiction = AnalizeAdjective(ref nolessInfo);
				}
				if (!contradiction)
				{
					bool nonemptyPair = false;
					AnalizeAttribute(BlockAttribute.PROF, ref contradiction, ref nonemptyPair, false);
				}
				if (!contradiction)
					contradiction = AnalizeChilds(ref nolessChilds);
				if (!contradiction && nolessInfo && !nolessChilds.IsEmpty())
					find = true;
			}
			if (find)
				UpdateCurrentNode();
			return find;
		}
		#endregion

		#region [обычные цепочки]
		private void UnknownCoreference(Dictionary<string, List<XElement>> UnknownList, List<XElement> leafs)
		{
			foreach (XElement element in leafs)
			{
				if ((GetCoreferenceType(element) != CoreferenceType.Unknown))
					continue;

				_CurrentNode = element;
				string PROFvalue = _CurrentNode.AttributeUpper(BlockAttribute.PROF);
				List<XElement> smallDict;
				if (UnknownList.TryGetValue(PROFvalue, out smallDict))
				{
					int index = -1;
					for (int i = 0; i < smallDict.Count; ++i)
					/// найдем текущую сущность в словаре
					{
						if (smallDict[i] == _CurrentNode)
						{
							index = i;
							break;
						}
					}

					bool find = false;
					for (int i = index - 1; !find && (i >= 0); --i)
						find = UnknownNodeCoreference(smallDict[i]);
					/*if (!find)
					/// в начале текста ничего не нашли
					{
						for (int i = index + 1; i < smallDict.Count; ++i)
							UnknownNodeCoreference(smallDict[i]);
					}*/
				}
			}
		}

		private bool UnknownNodeCoreference(XElement second)
		{
			bool find = false;
			_CompareNode = second;

			/// больше прилагательных
			bool nolessInfo = false;
			/// nolessChilds != "", если у m_CompareNode детей не меньше, чем у m_CurrentNode
			string nolessChilds = "";
			/// противоречие
			bool contradiction = AnalizeAdjective(ref nolessInfo);
			if (!contradiction && nolessInfo)
				contradiction = AnalizeChilds(ref nolessChilds);

			if (!contradiction && nolessInfo && (nolessChilds != ""))
			{
				string PROFvalue = GetNodeFullnameUpper(_CurrentNode);
				string dictPROFvalue = GetNodeFullnameUpper(_CompareNode);
				if (PROFvalue.Length < dictPROFvalue.Length)
				{
					UpdateCurrentNode();
					find = true;
				}
			}
			return find;
		}
		#endregion

		#region [Персоны]
		private bool PersonNodeCoreference(XElement second)
		{
			bool find = false;
			_CompareNode = second;

			/// наличие непустой совпавшей пары
			bool nonemptyPair = false;
			/// противоречие
			bool contradiction = false;
			bool nolessInfo = false;
			AnalizeAttribute(BlockAttribute.LN, ref contradiction, ref nonemptyPair, false);
			AnalizeAttribute(BlockAttribute.FN, ref contradiction, ref nonemptyPair, true);
			AnalizeAttribute(BlockAttribute.MN, ref contradiction, ref nonemptyPair, true);
			AnalizeAttribute(BlockAttribute.PROF, ref contradiction, ref nonemptyPair, false);
			if (nonemptyPair)
			{
				string nolessChilds = "";
				if (!contradiction)
					contradiction = AnalizeChilds(ref nolessChilds);
				if (!contradiction)
					contradiction = AnalizeAdjective(ref nolessInfo);

				if (!contradiction)
				{
					UpdateCurrentNode();
					find = true;
				}
			}
			return find;
		}
		#endregion

		#region [Аббревиатуры]
		private bool AbbreviationNodeCoreference(XElement second)
		{
			bool abbrText = false;
			_CompareNode = second;
			if (!_CompareNode.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.OrgAbbr))
			{
				string abbr = _CurrentNode.AttributeUpper(BlockAttribute.VALUE);
				string nodeValue = _CompareNode.AttributeUpper(BlockAttribute.VALUE);
				if (!nodeValue.IsEmpty() && (nodeValue[0] == abbr[0]))
				{
					abbrText = true;
					List<string> words = new List<string>(nodeValue.Split(' '));
					int i = 0;
					int j = 0;
					bool process = true;
					while (process)
					{
						if (abbr[i] == words[j][0])
						{
							++i;
							++j;
						}
						else if ((i + 1 < abbr.Length) && (abbr[i + 1] == words[j][0]))
						{
							if ((j > 0) && (words[j - 1].IndexOf(abbr[i], 1) > 0))
							{
								i += 2;
								++j;
							}
							else
								abbrText = false;
						}
						else
							abbrText = false;
						if ((i >= abbr.Length) || (j >= words.Count))
						{
							if ((j >= words.Count) && (i == abbr.Length - 1))
							/// осталась последняя буква аббревиатуры
							{
								if ((j > 0) && (words[j - 1].IndexOf(abbr[i], 1) < 0))
									abbrText = false;
							}
							else if (i < abbr.Length)
								abbrText = false;
							process = false;
						}
						if (!abbrText)
							process = false;
					}
					if (abbrText)
						UpdateCurrentNode();
				}
			}
			return abbrText;
		}
		#endregion

		#endregion

		#region [служебные функции для разрешения кореферености]
		/// <summary>
		/// Анализ прилагательных
		/// </summary>
		/// <param name="nolessInfo"></param>
		/// <returns>true, если было найдено противоречие</returns>
		private bool AnalizeAdjective(ref bool nolessInfo)
		{
			bool contradiction = false;
			nolessInfo = false;
			string currentValue = _CurrentNode.Attribute(BlockAttribute.PRE);
			string compareValue = _CompareNode.Attribute(BlockAttribute.PRE);

			List<string> current = new List<string>();
			List<string> compare = new List<string>();

			if (!currentValue.IsEmpty())
				current.AddRange(currentValue.Split(' '));
			if (!compareValue.IsEmpty())
				compare.AddRange(compareValue.Split(' '));

			List<string> small = compare;
			List<string> big = current;
			if (current.Count <= compare.Count)
			{
				small = current;
				big = compare;
				nolessInfo = true;
			}

			int numEqual = small.Count(adj => big.Any(_ => _ == adj));
			/// используются не все прилагательные короткой цепочки
			if (numEqual != small.Count)
				contradiction = true;
			return contradiction;
		}

		/// <summary>
		/// Анализ совпадения значений атрибута
		/// </summary>
		/// <param name="attribute">атрибут</param>
		/// <param name="contradiction">флаг нахождения противоречия</param>
		/// <param name="nonemptyPair">флаг, что проверяемые значения не пустые</param>
		/// <param name="applyFirstLetter">допускается проверка по первому символу</param>
		private void AnalizeAttribute(BlockAttribute attribute, ref bool contradiction,
									  ref bool nonemptyPair, bool applyFirstLetter)
		{
			if (contradiction)
				return;

			string current = _CurrentNode.AttributeUpper(attribute);
			string compare = _CompareNode.AttributeUpper(attribute);
			if (current != compare)
			{
				if (!(current.IsEmpty() || compare.IsEmpty()))
				{
					if (applyFirstLetter && ((current.Length == 1) || (compare.Length == 1)))
					/// указан только инициал
					{
						if (current.FirstOrDefault() != compare.FirstOrDefault())
							contradiction = true;
						else
							nonemptyPair = true;
					}
					else
						contradiction = true;
				}
			}
			else if (!current.IsEmpty())
				nonemptyPair = true;
		}
		#endregion

		private bool DeleteChildsNoFullname(XElement parent)
        {
            bool deleted = true;
            foreach (XElement node in parent.Elements(UnitTextType.ENTITY))
            {
                deleted = DeleteChildsNoFullname(node);
                string fullname = NO_FULLNAME;
                if (node.IsContainAttribute(BlockAttribute.FULLNAME))
					fullname = node.AttributeUpper(BlockAttribute.FULLNAME);

                if (fullname == NO_FULLNAME)
                {
                    string newValue = null;
                    if (!deleted)
                        newValue = "";
                    node.SetAttributeValue(BlockAttribute.FULLNAME, newValue);
                }
                else
                    deleted = false;
            }
            return deleted;
        }

        /// возвращает все листовые теги
        /// под листовыми тегами понимаются такие, у которых все дети прошли разрешение кореферентности
        private bool GetLeafNodes(XElement parent, List<XElement> list)
        {
            bool isLeaf = true;
            foreach (XElement node in parent.Elements(UnitTextType.ENTITY))
            {
                if (node.IsContainAttribute(BlockAttribute.FULLNAME))
                    continue;
                
                List<XElement> childrenLeaf = new List<XElement>();
                isLeaf = GetLeafNodes(node, childrenLeaf);
                CoreferenceType type = GetCoreferenceType(node);
                foreach (XElement elem in childrenLeaf)
                {
					CoreferenceType elemType = GetCoreferenceType(elem);
					if (((type == CoreferenceType.Person) && (elemType == type)) ||
						((type == CoreferenceType.Unknown) && (elemType != type)) ||
						(type == CoreferenceType.Default))
                    {
                        continue;
                    }
                    list.Add(elem);
                    isLeaf = false;
                }

                if(isLeaf)
                    list.Add(node);
            }
            return isLeaf;
        }

        private void SpecialUnknownProperTest(XElement node, ref EntityType type)
        {
            if (type == EntityType.UnknownProper)
            /// проверим на однословность
            {
				string value = node.Attribute(BlockAttribute.VALUE);
                if (value.IndexOf(" ") < 0)
                    type = EntityType.PersonName;
            }
        }

        /// <summary>
        /// возвращает тип кореферентности
        /// </summary>
        /// <param name="node">сущность</param>
        /// <returns>тип кореферентности</returns>
        public CoreferenceType GetCoreferenceType(XElement node)
        {
            EntityType entityType = EntityType.Default;
            entityType = (EntityType)Enum.Parse(typeof(EntityType),
                node.Attribute(BlockAttribute.TYPE.ToString()).Value);

            SpecialUnknownProperTest(node, ref entityType);

            CoreferenceType type = CoreferenceType.Default;
            switch (entityType)
            {
                case EntityType.PersonName:
                case EntityType.PersonInfo:
                case EntityType.PersonNoInfo:
                    type = CoreferenceType.Person;
                    break;
                case EntityType.OrgName:
                    type = CoreferenceType.Organization;
                    break;
                case EntityType.OrgAbbr:
                    type = CoreferenceType.Abbreviation;
                    break;
                case EntityType.OrgNoInfo:
                    type = CoreferenceType.WeakOrganization;
                    break;
                case EntityType.Unknown:
                case EntityType.UnknownProper:
                case EntityType.NounVerb:
                    type = CoreferenceType.Unknown;
                    break;
            }
            return type;
        }

        private bool AnalizeChilds(ref string id)
        {
            bool contradiction = false;
            string currentValue = GetNodeChildTextUpper(_CurrentNode);
            string compareValue = GetNodeChildTextUpper(_CompareNode);

            if (compareValue.Length >= currentValue.Length)
            {
                if (compareValue.IndexOf(currentValue, StringComparison.InvariantCulture) < 0)
                    contradiction = true;
                else
                {
					id = _CompareNode.AttributeUpper(BlockAttribute.ID);
                    if (!_LinqDictionary.ContainsKey(id))
                        _LinqDictionary.Add(id, _CompareNode);
                }
            }

            return contradiction;
        }

        private List<XElement> GetNodeChilds(XElement node, List<string> prevLink, bool withoutLinks)
        {
            List<XElement> childs = new List<XElement>();
            string link = "";
            if (!withoutLinks)
                link = GetNodeLink(node);
            if (link == "")
            {
                CoreferenceType parentType = GetCoreferenceType(node);
                foreach (XElement child in node.Elements(UnitTextType.ENTITY))
                {
                    CoreferenceType type = GetCoreferenceType(child);
                    if ((type == CoreferenceType.Organization) ||
                        (type == CoreferenceType.Unknown) ||
                        (type != parentType))
                    {
                        childs.Add(child);
                    }
                    else
                        childs.AddRange(GetNodeChilds(child, prevLink, withoutLinks));
                }
            }
            else
            {
                if (prevLink.Contains(link))
                {
                    List<XElement> childText = new List<XElement>();
                    foreach (string id in prevLink)
                    {
                        XElement linkNode;
                        if (GetLinkFromDictionary(id, out linkNode))
                        {
                            List<XElement> fullname = GetNodeChilds(linkNode, prevLink, true);
                            if (fullname.Count > childText.Count)
                                childText = fullname;
                        }
                    }
                    childs.AddRange(childText);
                }
                else
                {
                    prevLink.Add(link);
                    XElement linkNode;
                    if (GetLinkFromDictionary(link, out linkNode))
                        childs.AddRange(GetNodeChilds(linkNode, prevLink, withoutLinks));
                }
            }
            return childs;
        }

        public List<XElement> GetNodeChilds(XElement node)
        {
            List<string> prevLink = new List<string>();
            return GetNodeChilds(node, prevLink, false);
        }

        public string GetNodeChildText(XElement node)
        {
            StringBuilder text = new StringBuilder();
            List<XElement> childs = GetNodeChilds(node);
            foreach (XElement child in childs)
            {
                text.Append(GetNodeFullname(child));
                text.Append(" ");
            }
            return text.ToString().Trim();
        }

        private string GetNodeChildTextUpper(XElement node)
        {
            return GetNodeChildText(node).ToUpper(CultureInfo.InvariantCulture);
        }

        private bool GetLinkFromDictionary(string link, out XElement linkNode)
        {
            return _LinqDictionary.TryGetValue(link, out linkNode);
        }

        private string GetNodeLink(XElement node)
        {
            string link = "";
            int index = 0;
			BlockAttribute[] atrribates = new BlockAttribute[] { BlockAttribute.LINK, BlockAttribute.EI };
            while ((link == "") && (index < atrribates.Length))
            {
				link = node.Attribute(atrribates[index]);
                ++index;
            }
            return link;
        }

        private string GetNodeFullname(XElement node)
        {
            string value = "";
            if (node.IsContainAttribute(BlockAttribute.EI))
            {
                XElement fullname;
                if (GetLinkFromDictionary(node.Attribute(BlockAttribute.EI), out fullname))
                    value = fullname.Attribute(BlockAttribute.VALUE);
            }
            else if (node.IsContainAttribute(BlockAttribute.LINK))
            {
                XElement fullname;
                if (GetLinkFromDictionary(node.Attribute(BlockAttribute.LINK), out fullname))
                    value = fullname.Attribute(BlockAttribute.FULLNAME);
            }
            else
                value = node.Attribute(BlockAttribute.VALUE);
            return value.Trim();
        }

        private string GetNodeFullnameUpper(XElement node)
        {
            return GetNodeFullname(node).ToUpper(CultureInfo.InvariantCulture);
        }

        private void UpdateCurrentNode()
        {
            string id = "";
            if (!NeedUpdateNode())
                return;

            switch (GetCoreferenceType(_CurrentNode))
            {
                case CoreferenceType.Person:
                    bool equalAllAttrib = true;
					foreach (BlockAttribute attribute in _PersonAttributes)
                    /// установка новых атрибутов
                    {
						string currentValue = _CurrentNode.AttributeUpper(attribute);
						string compareValue = _CompareNode.AttributeUpper(attribute);
                        if (currentValue.Length < compareValue.Length)
                        {
                            if (!compareValue.IsEmpty())
                                compareValue = _CompareNode.Attribute(attribute);
                            _CurrentNode.SetAttributeValue(attribute, compareValue);
                        }
                        else if (currentValue != compareValue)
                            equalAllAttrib = false;
                    }
                    bool equalChild = false;
                    /// надо обновить детей
                    bool needUpdateChild = NeedUpdateField(GetNodeChildTextUpper(_CurrentNode),
                                                           GetNodeChildTextUpper(_CompareNode),
                                                           ref equalChild);
					id = _CompareNode.Attribute(BlockAttribute.LINK);
					if (id.IsEmpty())
						id = _CompareNode.Attribute(BlockAttribute.ID);
                    if (_CurrentNode.Parent.Name.IsEqual(ROOT_NAME) && (equalChild || needUpdateChild) &&
                        (equalAllAttrib))
                    /// m_CurrentNode полностью повторяет m_CompareNode
                    {
                        _CurrentNode.SetAttributeValue(BlockAttribute.LINK, id);
                        _CurrentNode.SetAttributeValue(BlockAttribute.TYPE, _CompareNode.Attribute(BlockAttribute.TYPE));
                    }
                    else
                    {
						_CurrentNode.SetAttributeValue(BlockAttribute.LINK, id);
                        /*if (needUpdateChild)
                            _CurrentNode.SetAttributeValue(BlockAttribute.EI.ToString(), id);*/
                    }
                    break;
                case CoreferenceType.Unknown:
                case CoreferenceType.Organization:
                case CoreferenceType.WeakOrganization:
                    id = GetNodeLink(_CompareNode);
					if (id.IsEmpty())
						id = _CompareNode.AttributeUpper(BlockAttribute.ID);
                    _CurrentNode.SetAttributeValue(BlockAttribute.LINK, id);
                    _CurrentNode.SetAttributeValue(BlockAttribute.TYPE, _CompareNode.Attribute(BlockAttribute.TYPE));
                    break;
                case CoreferenceType.Abbreviation:
					id = _CompareNode.AttributeUpper(BlockAttribute.ID);
                    _CurrentNode.SetAttributeValue(BlockAttribute.LINK, id);
                    _CurrentNode.SetAttributeValue(BlockAttribute.TYPE, _CompareNode.Attribute(BlockAttribute.TYPE));
                    break;
            }
            _CurrentNode.SetAttributeValue(BlockAttribute.FULLNAME, "");
            if (!_LinqDictionary.ContainsKey(id))
                _LinqDictionary.Add(id, _CompareNode);
        }

        private bool NeedUpdateNode()
        {
            bool allEqual = true;
            Array attributes = null;
            CoreferenceType type = GetCoreferenceType(_CurrentNode);
            switch (type)
            {
                case CoreferenceType.Person:
					attributes = _PersonAttributes;
                    break;
                case CoreferenceType.Unknown:
                case CoreferenceType.Organization:
                case CoreferenceType.WeakOrganization:
					attributes = _UsualAttributes;
                    break;
            }

            if (attributes != null)
            {
				foreach (BlockAttribute attribute in attributes)
                {
					string currentValue = _CurrentNode.AttributeUpper(attribute);
					string compareValue = _CompareNode.AttributeUpper(attribute);
                    if (currentValue.IsEmpty() && compareValue.IsEmpty())
                        allEqual = false;
                }
            }
            if (allEqual)
            {
                allEqual = !NeedUpdateField(GetNodeChildTextUpper(_CurrentNode),
                                            GetNodeChildTextUpper(_CompareNode),
                                            ref allEqual);
            }

            if (!allEqual)
            {
                if (_CurrentNode.Descendants().Contains(_CompareNode) ||
                    _CompareNode.Descendants().Contains(_CurrentNode))
                {
                    allEqual = true;
                }
                else if (_CompareNode.IsContainAttribute(BlockAttribute.LINK))
                {
					string link = _CompareNode.Attribute(BlockAttribute.LINK);
                    foreach (XElement parent in _CurrentNode.DescendantsAndSelf())
                    {
                        if (link.IsEqual(parent.Attribute(BlockAttribute.ID)) ||
							link.IsEqual(parent.Attribute(BlockAttribute.LINK)))
                        {
                            allEqual = true;
                            break;
                        }
                    }
                }
            }

            if (!allEqual)
            {
                XElement currentFirstParent = GetFirstParent(_CurrentNode);
                XElement compareFirstParent = GetFirstParent(_CompareNode);
                if (currentFirstParent.Descendants().Count() == compareFirstParent.Descendants().Count())
                {
                    bool allExist = true;
                    foreach (XElement node in currentFirstParent.DescendantsAndSelf(UnitTextType.ENTITY.ToString()))
                    {
                        int result =
                            (from child in compareFirstParent.DescendantsAndSelf()
                            where child.Name.IsEqual(UnitTextType.ENTITY) &&
								node.AttributeUpper(BlockAttribute.PROF).IsEqual(child.AttributeUpper(BlockAttribute.PROF)) &&
								node.AttributeUpper(BlockAttribute.PRE).IsEqual(child.AttributeUpper(BlockAttribute.PRE))
                            select child).Count();
                        if (result == 0)
                        {
                            allExist = false;
                            break;
                        }
                    }
                    if (allExist)
                        allEqual = true;
                }
            }
            return !allEqual;
        }

        private XElement GetFirstParent(XElement node)
        {
            XElement parent = node;
            while (parent.Parent.Name.IsEqual(UnitTextType.ENTITY))
                parent = parent.Parent;
            return parent;
        }

        private bool NeedUpdateField(string first, string second, ref bool equal)
        {
            bool needUpdate = false;
            equal = false;
            if (first.Length < second.Length)
                needUpdate = true;
            else if (first == second)
                equal = true;
            return needUpdate;
        }
    }
}