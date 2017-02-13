<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
                version="1.0"
                xmlns:uf="uf"
								exclude-result-prefixes="uf">
  <xsl:output method="xml" 
              indent="no"
              omit-xml-declaration="yes" />

  <xsl:template match="/">
    <xsl:element name="NER">
      <xsl:apply-templates />
    </xsl:element>
  </xsl:template>

  <xsl:template match="ENTITY[ node() and (@TYPE='PersonName') and
                               not(ancestor::ENTITY[ @TYPE='PersonName' ]) 
                               and uf:HasRuOrEnUpperChar2( . ) ]">
    <xsl:element name="NAME">  
      <xsl:apply-templates select="node()" />
    </xsl:element>
  </xsl:template>

  <xsl:template match="ENTITY[ node() and (@TYPE='OrgName') and
                               not(ancestor::ENTITY[ @TYPE='OrgName' or @TYPE='OrgNoInfo' ])
                               and uf:HasRuOrEnUpperChar2( . ) ]">
    <xsl:element name="ORG">
      <xsl:apply-templates select="node()" />
    </xsl:element>
  </xsl:template>
  <xsl:template match="ENTITY[ node() and (@TYPE='OrgNoInfo') and
                               ((ancestor::SENT//ENTITY)[ 1 ] != self::ENTITY) and 
                               not(ancestor::ENTITY[ @TYPE='OrgName' or @TYPE='OrgNoInfo' ])
                               and uf:HasRuOrEnUpperChar2( . ) ]">
    <xsl:element name="ORG">
      <xsl:apply-templates select="node()" />
    </xsl:element>
  </xsl:template>

  <xsl:template match="ENTITY[ node() and (@TYPE='PlaceName' or @TYPE='PseudoGeo') and
                               not(ancestor::ENTITY[ @TYPE='PlaceName' or @TYPE='PseudoGeo' ])
                               and uf:HasRuOrEnUpperChar2( . ) ]">
    <xsl:element name="GEO">
      <xsl:apply-templates select="node()" />
    </xsl:element>
  </xsl:template>

  <xsl:template match="ENTITY[ node() and (@TYPE='Event') and
                               not(ancestor::ENTITY[ @TYPE='Event' ])
                               and uf:HasRuOrEnUpperChar2( . ) ]">
    <xsl:element name="ENTR">
      <xsl:apply-templates select="node()" />
    </xsl:element>
  </xsl:template>

  <xsl:template match="ENTITY[ node() and (@TYPE='Product') and
                               not(ancestor::ENTITY[ @TYPE='Product' ])
                               and uf:HasRuOrEnUpperChar2( . ) ]">
    <xsl:element name="PROD">
      <xsl:apply-templates select="node()" />
    </xsl:element>    
  </xsl:template>

  <!--Разбивка на предложения-->
  <!--
  <xsl:template match="SENT[ node() and not(ancestor::SUB_SENT[@TYPE='ApproximatingForceps']) ]">
    <xsl:apply-templates select="node()" />
    <xsl:text>&#x0D;&#x0A;</xsl:text>
  </xsl:template>
  -->
  
  <xsl:template match="text()">
    <xsl:variable name="txt" select="normalize-space()" />
    
    <xsl:choose>
      <xsl:when test="parent::U">
        <xsl:value-of select="." />
      </xsl:when>
      <xsl:when test="$txt != ''">
        <xsl:if test="not(starts-with($txt, ',')) and 
                      not(starts-with($txt, ':')) and 
                      not(starts-with($txt, '.')) and 
                      not(starts-with($txt, '-')) and 
                      not(starts-with($txt, ';')) and 
                      not(starts-with($txt, '[')) and 
                      not(starts-with($txt, ']')) and 
                      not(starts-with($txt, '(')) and 
                      not(starts-with($txt, ')'))">
          <xsl:text> </xsl:text>
        </xsl:if>
        <xsl:value-of select="$txt" />        
      </xsl:when>
    </xsl:choose>    
  </xsl:template>

  <ms:script implements-prefix="uf" xmlns:ms="urn:schemas-microsoft-com:xslt" language="C#">
    <ms:using namespace="System"/>
    <ms:using namespace="System.Xml.XPath"/>
    <![CDATA[
    public static bool HasRuOrEnUpperChar2( XPathNodeIterator iterator )
    {
      if ( iterator.MoveNext() )
      {			  
				XPathNavigator nav = iterator.Current;
        var attr_val = nav.GetAttribute("FN", string.Empty);
        if ( !string.IsNullOrEmpty(attr_val) ) return (true);
        attr_val = nav.GetAttribute("LN", string.Empty);
        if ( !string.IsNullOrEmpty(attr_val) ) return (true);
        attr_val = nav.GetAttribute("MN", string.Empty);
        if ( !string.IsNullOrEmpty(attr_val) ) return (true);
        attr_val = nav.GetAttribute("NAME", string.Empty);
        if ( !string.IsNullOrEmpty(attr_val) ) return (true);
        
        var attr_pre  = nav.GetAttribute("PRE", string.Empty);
        var attr_prof = nav.GetAttribute("PROF", string.Empty);
        if ( !string.IsNullOrEmpty(attr_pre) && !string.IsNullOrEmpty(attr_prof) ) 
          return (HasRuOrEnUpperChar(attr_pre + attr_prof));        
        if ( !string.IsNullOrEmpty(attr_prof) ) 
          return (HasRuOrEnUpperChar(attr_prof));
        
        attr_val = nav.GetAttribute("VALUE", string.Empty);
        return (HasRuOrEnUpperChar(attr_val));
      }
      return (false);
    }    
    public static bool HasRuOrEnUpperChar( string value )
    {
        for ( int i = 0, len = value.Length; i < len; i++ )
        {
            var ch = value[ i ];
            if ( ('A' <= ch && ch <= 'Z') ||
                 ('А' <= ch && ch <= 'Я')) {
                return (true);
            }
        }
        return (false);
    }
    public bool HasWordsStartWithUpperChars( string value )
    {
      var array = value.Split( new[] { ' ', '\t', '\n', '\r' }, StringSplitOptions.RemoveEmptyEntries );
      for ( var j = 0; j < array.Length; j++ )
      {
          var a = array[ j ];
          var i = 0;
          for ( i = 0; i < a.Length; i++ )
              if ( !char.IsPunctuation( a, i ) )
                  break;
          if ( i < a.Length && char.IsUpper( a, i ) )
              return (true);
      }
      return (false);
    }]]>
  </ms:script>
  
</xsl:stylesheet>
